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

private:
	MediaManager manager;
	ConsoleView console;
};