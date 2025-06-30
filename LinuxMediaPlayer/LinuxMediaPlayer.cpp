// LinuxMediaPlayer.cpp : Defines the entry point for the application.
#include "LinuxMediaPlayer.h"
using namespace std;

int main()
{
	Controller control;
	control.InitSDL();
	control.MainLoop();
	control.TerminateThreads();
	control.QuitSDL();
	return 0;
}