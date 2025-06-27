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

pkg_check_modules(LIBAV REQUIRED IMPORTED_TARGET
    libavformat
    libavcodec
    libswresample
    libswscale
    libavutil
)

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
    "controller/SDLController.h")

target_include_directories(LinuxMediaPlayer 
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
    PUBLIC model
    PUBLIC ui
    PUBLIC controller
    PUBLIC ${LIBAV_INCLUDE_DIRS}
)

target_link_libraries(LinuxMediaPlayer TagLib::tag)
target_link_libraries(LinuxMediaPlayer SDL2::SDL2 SDL2_ttf::SDL2_ttf)
target_link_libraries(LinuxMediaPlayer PkgConfig::LIBAV)


if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET LinuxMediaPlayer PROPERTY CXX_STANDARD 20)
endif()


# TODO: Add tests and install targets if needed.
