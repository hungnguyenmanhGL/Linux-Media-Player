# CMakeList.txt : CMake project for LinuxMediaPlayer, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("LinuxMediaPlayer")

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(ZLIB)
find_package(TagLib 2.0.0 REQUIRED)
find_package(SDL2 REQUIRED)
find_package(PkgConfig REQUIRED)
find_package(SDL2_ttf REQUIRED)
find_package(SDL2_image REQUIRED)

set(CMAKE_SKIP_BUILD_RPATH FALSE)
set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
set(CMAKE_INSTALL_RPATH $ORIGIN)

find_library(ALSA_LIBRARY NAMES asound REQUIRED)
find_library(PULSE_LIBRARY NAMES pulse REQUIRED)
find_library(SAMPLERATE_LIBRARY NAMES samplerate REQUIRED)
find_library(XSS_LIBRARY NAMES Xss REQUIRED)
find_library(DECOR_LIBRARY NAMES decor-0 REQUIRED)
find_library(WEBP_DEMUX_LIBRARY NAMES webpdemux REQUIRED)

pkg_check_modules(LIBAV REQUIRED IMPORTED_TARGET
    libavformat
    libavcodec
    libswresample
    libswscale
    libavutil
)

# fetch sdl_fontcache for better performance when rendering text
include(FetchContent)
FetchContent_Declare(
    sdl_fontcache
    GIT_REPOSITORY https://github.com/hungnguyenmanhGL/SDL_FontCache
    GIT_TAG master    
)
FetchContent_MakeAvailable(sdl_fontcache)

# Add source to this project's executable.
add_executable (LinuxMediaPlayer 
    "LinuxMediaPlayer.cpp" 
    "LinuxMediaPlayer.h" 
      
    "Helper.h"  "Helper.cpp"
    "model/Playlist.h" "model/Playlist.cpp"
    "model/MediaFile.h" "model/MediaFile.cpp" 
    "model/AudioFile.h" "model/AudioFile.cpp" 
    "model/VideoFile.h" "model/VideoFile.cpp" 
    "model/MediaManager.h" "model/MediaManager.cpp"
    "ui/ConsoleView.h" "ui/ConsoleView.cpp"
    "controller/Controller.h" "controller/Controller.cpp" 
    #"controller/SDLController.h" "controller/SDLController.cpp"
    "controller/AudioProcessor.h" "controller/AudioProcessor.cpp"
)

set(ASSETS_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/assets")
set(ASSETS_BUILD_DIR "${CMAKE_BINARY_DIR}/assets")
add_custom_command(
    TARGET LinuxMediaPlayer  
    PRE_BUILD              
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${ASSETS_SOURCE_DIR}" 
            "${ASSETS_BUILD_DIR}"  
    COMMENT "Copying assets directory to build directory..."
)

target_include_directories(LinuxMediaPlayer 
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
    PUBLIC model
    PUBLIC ui
    PUBLIC controller
    PUBLIC ${LIBAV_INCLUDE_DIRS}
    $<BUILD_INTERFACE:${sdl_fontcache_SOURCE_DIR}>
)

target_link_libraries(LinuxMediaPlayer TagLib::tag)
target_link_libraries(LinuxMediaPlayer 
    SDL2::SDL2-static
    SDL2_ttf::SDL2_ttf-static  
    SDL2_image::SDL2_image-static
    FontCache
)
target_link_libraries(LinuxMediaPlayer PkgConfig::LIBAV)

target_link_libraries(LinuxMediaPlayer
    asound        
    pulse        
    samplerate  
    Xss        
    decor-0      
    webpdemux    
)


if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET LinuxMediaPlayer PROPERTY CXX_STANDARD 20)
endif()

# function to avoid copying symlink instead of the real library
function(resolve_symlink_to_real_path input_path output_var)
    if(EXISTS "${input_path}")
        execute_process(
            COMMAND realpath "${input_path}"
            OUTPUT_VARIABLE _resolved_path
            OUTPUT_STRIP_TRAILING_WHITESPACE
            RESULT_VARIABLE _real_path_result
            # Set WORKING_DIRECTORY to ensure realpath works correctly relative to original location if needed
            WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
        )
        if(_real_path_result EQUAL 0)
            # If realpath succeeded, use its output
            set(${output_var} "${_resolved_path}" PARENT_SCOPE)
        else()
            # If realpath failed (e.g., not a symlink, or file doesn't exist, though it should for libraries)
            # as a fallback, just use the original path. This might lead to issues later if it's a symlink.
            message(WARNING "realpath failed for ${input_path}. Copying original path. Result: ${_real_path_result}")
            set(${output_var} "${input_path}" PARENT_SCOPE)
        endif()
    else()
        # If input path does not exist, log a warning and skip, or use original path as fallback
        message(WARNING "Library path does not exist: ${input_path}")
        set(${output_var} "${input_path}" PARENT_SCOPE) # Still append to list, might fail later
    endif()
endfunction()


set(LIBS_DEST_DIR "$<TARGET_FILE_DIR:LinuxMediaPlayer>")

get_target_property(LIBAV_LINK_LIBS PkgConfig::LIBAV INTERFACE_LINK_LIBRARIES)

set(SHARED_LIBS_TO_COPY "")

foreach(lib ${LIBAV_LINK_LIBS})
    if(EXISTS "${lib}" AND "${lib}" MATCHES "\\.so$")
        resolve_symlink_to_real_path("${lib}" REAL_LIB_PATH)
        list(APPEND SHARED_LIBS_TO_COPY "${REAL_LIB_PATH}")
    endif()
endforeach()

if (ALSA_LIBRARY)
    resolve_symlink_to_real_path("${ALSA_LIBRARY}" REAL_ALSA_LIB_PATH)
    list(APPEND SHARED_LIBS_TO_COPY "${REAL_ALSA_LIB_PATH}")
endif()
if (PULSE_LIBRARY)
    resolve_symlink_to_real_path("${PULSE_LIBRARY}" REAL_PULSE_LIB_PATH)
    list(APPEND SHARED_LIBS_TO_COPY "${REAL_PULSE_LIB_PATH}")
endif()
if (SAMPLERATE_LIBRARY)
    resolve_symlink_to_real_path("${SAMPLERATE_LIBRARY}" REAL_SAMPLERATE_LIB_PATH)
    list(APPEND SHARED_LIBS_TO_COPY "${REAL_SAMPLERATE_LIB_PATH}")
endif()
if (XSS_LIBRARY)
    resolve_symlink_to_real_path("${XSS_LIBRARY}" REAL_XSS_LIB_PATH)
    list(APPEND SHARED_LIBS_TO_COPY "${REAL_XSS_LIB_PATH}")
endif()
if (DECOR_LIBRARY)
    resolve_symlink_to_real_path("${DECOR_LIBRARY}" REAL_DECOR_LIB_PATH)
    list(APPEND SHARED_LIBS_TO_COPY "${REAL_DECOR_LIB_PATH}")
endif()
if (WEBP_DEMUX_LIBRARY)
    resolve_symlink_to_real_path("${WEBP_DEMUX_LIBRARY}" REAL_WEBP_DEMUX_LIB_PATH)
    list(APPEND SHARED_LIBS_TO_COPY "${REAL_WEBP_DEMUX_LIB_PATH}")
endif()

list(REMOVE_DUPLICATES SHARED_LIBS_TO_COPY)


add_custom_command(
    TARGET LinuxMediaPlayer POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:LinuxMediaPlayer>"
    COMMAND ${CMAKE_COMMAND} -E echo "Copying required shared libraries to $<TARGET_FILE_DIR:LinuxMediaPlayer>..."
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${SHARED_LIBS_TO_COPY} "$<TARGET_FILE_DIR:LinuxMediaPlayer>"

    # COMMAND ${CMAKE_COMMAND} -E chmod +x "${PATCHELF_SCRIPT_PATH}"
    # COMMAND "${PATCHELF_SCRIPT_PATH}"
    # COMMENT "Executing RPATH patching script."
)
# TODO: Add tests and install targets if needed.
