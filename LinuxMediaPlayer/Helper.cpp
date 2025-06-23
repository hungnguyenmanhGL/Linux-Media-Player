#include "Helper.h"

Helper::Helper()
{
}

Helper::~Helper()
{
}

int Helper::InputInt(int minInclu, int maxInclu = -999) {
	int val = minInclu - 1;
	if (minInclu >= maxInclu) {
		do {
			printf("Input value (%d <= value): ", minInclu);
			cin >> val;

			if (cin.fail()) {
				cin.clear();
				std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
				cout << "Invalid input for int. Try again.\n";
			}
			if (val < minInclu) {
				cout << "Value out of range.\n";
			}
		} while (cin.fail() || val < minInclu);
	}
	else {
		do {
			printf("Input value (%d <= value <= %d): ", minInclu, maxInclu);
			cin >> val;

			if (cin.fail()) {
				cin.clear();
				std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
				cout << "Invalid input for int. Try again.\n";
			}
			if (val < minInclu || val > maxInclu) {
				cout << "Value out of range.\n";
			}
		} while (cin.fail() || val < minInclu || val > maxInclu);
	}
	return val;
}