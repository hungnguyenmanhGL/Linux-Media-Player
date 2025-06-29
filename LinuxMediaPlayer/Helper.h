#pragma once
#include <iostream>
#include <string>
#include <stdio.h>
#include <limits>
#include <functional>
using namespace std;

class Helper
{
public:
	Helper();
	~Helper();

	static int InputInt(int minInclu, int maxInclu = -999);

	//input string with a function pointer to check validity, invalid string msg should be printed by check function
	static string InputString(const string& inputMsg, function<bool(const string&)> checkFunc);

	static string GetAudioDurationString(int secs);

private:

};
