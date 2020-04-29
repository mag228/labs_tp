#include "pch.h"
#include "my_dll.h"

bool is_leap_year(const unsigned year) {
	return !(year % 400) || !(year % 4) && (year % 100);
}
