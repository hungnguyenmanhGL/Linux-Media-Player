// LinuxMediaPlayer.cpp : Defines the entry point for the application.
#include "LinuxMediaPlayer.h"
using namespace std;

int main()
{
	SDL_Init(0);
	Controller control;
	control.MainLoop();
	SDL_Quit();
	return 0;
}