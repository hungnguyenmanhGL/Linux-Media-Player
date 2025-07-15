#include "Helper.h"

Helper::Helper()
{
}

Helper::~Helper()
{
}

int Helper::InputInt(int minInclu, int maxInclu) {
	int val = minInclu;
	if (minInclu >= maxInclu) {
		do {
			printf("Input value (%d <= value): ", minInclu);
			cin >> val;

			if (cin.fail()) {
				cin.clear();
				cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
				cout << "Invalid input for int. Try again.\n";
			}
			else {
				cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
				if (val < minInclu) cout << "Input is smaller than minimum value.\n";
			}
		} while (cin.fail() || val < minInclu);
	}
	else {
		do {
			printf("Input value (%d <= value <= %d): ", minInclu, maxInclu);
			cin >> val;

			if (cin.fail()) {
				cin.clear();
				cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
				cout << "Invalid input for int. Try again.\n";
			}
			else {
				cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
				if (val < minInclu || val > maxInclu) cout << "Input is out of valid range.\n";
			}
		} while (cin.fail() || val < minInclu || val > maxInclu);
	}
	return val;
}

string Helper::InputString(const string& inputMsg, function<bool(const string&)> checkFunc) {
	string res;
	bool allowed = false;
	do {
		/*std::cout << "  Good bit (no errors): " << (std::cin.good() ? "true" : "false") << std::endl;
		std::cout << "  EOF bit (end of file): " << (std::cin.eof() ? "true" : "false") << std::endl;
		std::cout << "  Fail bit (read error): " << (std::cin.fail() ? "true" : "false") << std::endl;
		std::cout << "  Bad bit (stream corrupted): " << (std::cin.bad() ? "true" : "false") << std::endl;
		std::cout << "-------------------------------------\n" << std::endl;*/

		cout << inputMsg;
		getline(cin, res);
		if (checkFunc != nullptr) allowed = checkFunc(res);
		else allowed = true;
	} while (!allowed);
	return res;
}

char Helper::GetFirstCharInput() {
	string input;
	getline(cin, input);
	if (input.size() == 0) return '*';
	return input.at(0);
}

string Helper::GetAudioDurationString(int secs) {
	int hour, minute;
	const int hourInSecond = 3600;
	const int minInSecond = 60;
	string hh, mm, ss;

	if (secs >= hourInSecond) {
		hour = secs / hourInSecond;
		secs -= hour * hourInSecond;
		hh = to_string(hour);
		if (hour < 10) hh = '0' + hh;

		minute = secs / minInSecond;
		secs -= minInSecond * minute;
		mm = to_string(minute);
		if (minute < 10) mm = '0' + mm;

		ss = to_string(secs);
		if (secs < 10) ss = '0' + ss;
		return hh + ":" + mm + ":" + ss;
	}
	else {
		minute = secs / minInSecond;
		secs -= minInSecond * minute;
		mm = to_string(minute);
		if (minute < 10) mm = '0' + mm;

		ss = to_string(secs);
		if (secs < 10) ss = '0' + ss;
		return mm + ":" + ss;
	}
}