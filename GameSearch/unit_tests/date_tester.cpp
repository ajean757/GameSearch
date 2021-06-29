#include <iostream>
#include <string>
#include <vector>
#include "../Date.h"

using namespace std;


void leap_year_tester() {
    for(unsigned int i = 1000; i < 3000; ++i) {
        Date tmp_date(i, 1, 1);
        cout << "is " << i << " a leap year? " << tmp_date.is_leap_year(i) << endl;
    }
}

void increment_and_decrement_tester() {
    Date today("2021-06-21");
    
    for(int i = 0; i < 1000; ++i) {
        cout << today.to_string() << endl;
        --today;
    }

    
    for(int i = 0; i < 777; ++i) {
        cout << today.to_string() << endl;
        ++today;
    }
}
int main() {

    increment_and_decrement_tester();
    
}