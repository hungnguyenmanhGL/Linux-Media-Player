#pragma once
#include "MediaManager.h"
#include "ConsoleView.h"
#include "Helper.h"

class Controller
{
public:
	Controller();
	~Controller();

	void MainLoop();

	bool PlaylistLoop();

	void ContentLoop(const int& plIndex);

private:
	MediaManager manager;
	ConsoleView console;
};