#ifndef BANNER_H
#define BANNER_H
#include <iostream>
#include <thread>
#include <chrono>
using namespace std;
void showBanner() {
    const string green = "\033[1;32m";
    const string reset = "\033[0m";

    cout << green;
    cout << "+=============================+\n";
    cout << "+       MAC QUANG VINH        +\n";
    cout << "+                             +\n";
    cout << "+       MSV: K24DTCN249       +\n";
    cout << "+=============================+\n";
    cout << reset;
}
#endif