// LinuxMediaPlayer.h : Include file for standard system include files,
// or project specific include files.

#pragma once

#include <iostream>
#include <dirent.h>
#include <filesystem>
#include <stdio.h>
#include <vector>
#include <queue>
#include <unordered_set>
#include <sys/stat.h>

/*ncurses has to be installed on target machine ? \
ncurses isn't on Windows, so this will show as error but compile normal if use WSL or on Linux*/
#include <ncurses.h> 
