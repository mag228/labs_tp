#define _CRT_SECURE_NO_WARNINGS
#include "my_dll.h"
#include <iostream>
#include <string>
#include <ctime>
using namespace std;

string hello_message = "This program determines: the entered year is a leap year?";
string error_input_message = "Invalid input!!!";
string message_true = " year - it is a leap year";
string message_false = " year - it is a not leap year";
string goodbye_message = "Goodbye!!!";

int get_int(int left, int right, istream&is, ostream&os) {
	int res;
	string str;
	bool f = true;
	while (f) {
		os << "Enter an integer ["<< left<<", "<< right << "] ('exit' for exit)> ";
		is >> str;
		if (str == "exit") return -1;
		res = atoi(str.c_str());
		is.ignore(is.rdbuf()->in_avail());
		if (to_string(res) != str || res < left || res>right) cout << error_input_message << endl;
		else f = false;
	};
	return res;
}

void main() {
	cout << hello_message << endl;
	
	time_t t;
	time(&t);
	t = 1900 + localtime(&t)->tm_year;
	cout << "Year now: " << t << endl;
	cout << t << (is_leap_year(t) ? message_true : message_false) << endl;

	bool f;
	while (true) {
		t = get_int(0, 10000, cin, cout);
		if (t == -1) {
			cout << goodbye_message << endl;
			break;
		}
		cout << t << (is_leap_year(t) ? message_true : message_false) << endl;
	}
	system("pause");
}