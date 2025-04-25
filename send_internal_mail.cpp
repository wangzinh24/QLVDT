#include "send_internal_mail.h"
#include <fstream>
#include <string>
#include <iomanip>
#include <ctime>
using namespace std;
string getCurrentTime() {
    time_t now = time(0);
    tm* ltm = localtime(&now);
    stringstream ss;
    ss << 1900 + ltm->tm_year << "-"
       << setfill('0') << setw(2) << 1 + ltm->tm_mon << "-"
       << setfill('0') << setw(2) << ltm->tm_mday << " "
       << setfill('0') << setw(2) << ltm->tm_hour << ":"
       << setfill('0') << setw(2) << ltm->tm_min << ":"
       << setfill('0') << setw(2) << ltm->tm_sec;
    return ss.str();
}

bool sendInternalMail(const string& recipient, const string& message) {
    ofstream inbox("mailapp/mail_box/" + recipient + ".txt", ios::app);
    if (!inbox.is_open()) return false;

    inbox << "From: admin@gmail.com\n";
    inbox << "Time: " << getCurrentTime() << "\n";
    inbox << "Message: " << message << "\n\n";
    return true;
}
