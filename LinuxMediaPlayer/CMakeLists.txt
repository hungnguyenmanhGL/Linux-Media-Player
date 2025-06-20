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

# Add source to this project's executable.
add_executable (LinuxMediaPlayer 
    "LinuxMediaPlayer.cpp" 
    "LinuxMediaPlayer.h" 
    "Manager.h"  
    "Helper.h"  
    "model/Playlist.h" "model/Playlist.cpp"
    "model/MediaFile.h" "model/MediaFile.cpp" 
    "model/AudioFile.h" "model/AudioFile.cpp" 
    "model/VideoFile.h" "model/VideoFile.cpp" 
    "model/MediaManager.h" "model/MediaManager.cpp"
    "ui/ConsoleView.h" "ui/ConsoleView.cpp"
    "controller/Controller.h" "controller/Controller.cpp")

target_include_directories(LinuxMediaPlayer 
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
    PUBLIC model
    PUBLIC ui
    PUBLIC controller
)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET LinuxMediaPlayer PROPERTY CXX_STANDARD 20)
endif()


# TODO: Add tests and install targets if needed.
