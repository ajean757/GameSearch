#pragma once
#include <string>
#include <exception>
#include <iostream>

using std::cout;
using std::endl;
using std::exception;
using std::string;



class NoSuchDate : public exception {
	const char* msg;
public:
	NoSuchDate(const char* msg) : msg(msg) {}

	const char* what() const noexcept {
		return msg;
	}
};


//define helper function
string trim_entries(string line, char delimiter, size_t n);

class Date {
	unsigned int year;
	unsigned int month;
	unsigned int day;

public:
	Date() : year(0), month(0), day(0) {};
	
	Date(unsigned int y, unsigned int m, unsigned int d)  {
		if (!is_valid_date(y, m, d)) {
			throw NoSuchDate("Date isn't valid");
		}
		else {
			year = y;
			month = m;
			day = d;
		}
	};

	// String constructor: proper format is "yyyy-mm-dd"
	Date(const string& date) {  
		*this = std::move(to_date(date));
	}
	
	Date& operator++() {
		if (end_of_year()) {
			++year;
			month = 1;
			day = 1;
		}
		else if (end_of_month()) {
			++month;
			day = 1;
		}
		else {
			++day;
		}

		return *this;
	}

	// Decrement day (less common operation, only used for finding maximum bound
	Date& operator--() { 
		if (month == 1 && day == 1) {
			--year;
			month = 12;
			day = 31;
		}
		else if (day == 1) {
			--month;
			day = get_end_of_month();
		}
		else {
			--day;
		}

		return *this;
	}
	
	bool operator<(const Date& rhs) const noexcept {
		return (year < rhs.year) || (year == rhs.year && month < rhs.month) || (year == rhs.year && month == rhs.month && day < rhs.day);
	}

	bool operator>(const Date& rhs) const noexcept {
		return (year > rhs.year) || (year == rhs.year && month > rhs.month) || (year == rhs.year && month == rhs.month && day > rhs.day);
	}

	bool operator==(const Date& rhs) const noexcept {
		return (day == rhs.day) && (month == rhs.month) && (year == rhs.year);
	}

	bool operator!=(const Date& rhs) const noexcept {
		return !(*this == rhs);
	}
	string to_string() const {
		return std::to_string(year) + "-" + std::to_string(month) + "-" + std::to_string(day);
	}

	

private:

	static Date to_date(string s_date) {
		unsigned int y;
		unsigned int m;
		unsigned int d;
	
		y = stoi(s_date.substr(0, s_date.find('-')));
		s_date = std::move(trim_entries(s_date, '-', 1));
		m = stoi(s_date.substr(0, s_date.find('-')));
		s_date = std::move(trim_entries(s_date, '-', 1));
		d = stoi(s_date);
		return Date(y, m, d);
		
	}

	bool is_valid_date(unsigned int y, unsigned int m, unsigned int d) {
		if (d > 31 || m > 12) { // catches obvious erroneous dates
			return false;
		}
		else if ((m % 2 == 1 && m <= 7) || (m % 2 == 0 && m > 7)) {  // checks for proper months that end with 31 days
			return !(d > 31);
		}
		else if (m == 2) { // checks for imporper leap years
			return (is_leap_year(y)) ? !(d > 29) : !(d > 28);
		}
		else { // ensures all other days are within 30 days
			return !(d > 30);
		}
	}

	bool is_leap_year(unsigned int y) {
		return ((y % 4 == 0) && !(y % 100 == 0)) || (y % 400 == 0);
	}

	bool end_of_month() {
		if ((month % 2 == 1 && month < 7) || (month % 2 == 0 && month > 7)) {
			return day == 31;
		}
		else if (month == 2) {
			return (is_leap_year(year)) ? day == 29 : day == 28;
		}
		else {
			return day == 30;
		}
	}

	int get_end_of_month() {
		if ((month % 2 == 1 && month < 7) || (month % 2 == 0 && month > 7)) {
			return 31;
		}
		else if (month == 2) {
			return (is_leap_year(year)) ? 29 : 28;
		}
		else {
			return 30;
		}
	}

	bool end_of_year() {
		return month == 12 && day == 31;
	}
};
// Helper function
string trim_entries(string line, char delimiter, size_t n) {
	if (n == 0) {
		return line;
	}

	return trim_entries(line.substr(line.find(delimiter) + 1), delimiter, n - 1);
}
