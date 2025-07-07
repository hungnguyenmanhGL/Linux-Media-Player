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

set_target_properties(LinuxMediaPlayer PROPERTIES BUILD_WITH_INSTALL_RPATH TRUE)
set_target_properties(LinuxMediaPlayer PROPERTIES INSTALL_RPATH "$ORIGIN/libs")

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

set(BUNDLE_SCRIPT_SOURCE "${CMAKE_CURRENT_SOURCE_DIR}/bundle_deps.py")

# Copy the script to the build directory.
# This happens during the 'cmake -S . -B build' configuration step.
file(COPY "${BUNDLE_SCRIPT_SOURCE}" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}")

# Optional: Add a custom command to make it executable right after copying.
# This ensures it's ready to run when you try it from the console.
add_custom_command(
    OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/bundle_deps.py" # This custom command's output
    COMMAND chmod +x "${CMAKE_CURRENT_BINARY_DIR}/bundle_deps.py"
    DEPENDS "${BUNDLE_SCRIPT_SOURCE}" # If source changes, re-run this.
    COMMENT "Making bundle_deps.sh executable in build directory."
)

# TODO: Add tests and install targets if needed.
