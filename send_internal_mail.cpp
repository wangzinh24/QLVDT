#include "send_internal_mail.h"
#include <fstream>
#include <string>
#include <iomanip>
#include <ctime>
using namespace std;

// Hàm getCurrentTime
//  Trả về chuỗi thời gian hiện tại dưới định dạng "YYYY-MM-DD HH:MM:SS"
//  Dùng để ghi lại thời điểm gửi mail vào hộp thư
string getCurrentTime() {
    time_t now = time(0);                 // Lấy thời gian hiện tại (timestamp)
    tm* ltm = localtime(&now);            // Chuyển sang định dạng thời gian địa phương

    stringstream ss;
    ss << 1900 + ltm->tm_year << "-"      // Năm (tm_year tính từ 1900)
       << setfill('0') << setw(2) << 1 + ltm->tm_mon << "-"  // Tháng (tm_mon từ 0–11)
       << setfill('0') << setw(2) << ltm->tm_mday << " "
       << setfill('0') << setw(2) << ltm->tm_hour << ":"
       << setfill('0') << setw(2) << ltm->tm_min << ":"
       << setfill('0') << setw(2) << ltm->tm_sec;
    return ss.str();
}

// Hàm sendInternalMail
//  Gửi tin nhắn nội bộ từ "admin@gmail.com" đến người dùng cụ thể
// Tham số:
//   - recipient: tên người nhận (được dùng làm tên file hộp thư)
//   - message: nội dung tin nhắn cần gửi (thường là OTP hoặc thông báo)
// Trả về:
//   - true nếu gửi thành công (file mở được)
//   - false nếu không thể mở file hộp thư của người nhận
bool sendInternalMail(const string& recipient, const string& message) {
    ofstream inbox("mailapp/mail_box/" + recipient + ".txt", ios::app);  // Ghi tiếp vào file hộp thư
    if (!inbox.is_open()) return false;  // Nếu không mở được file, báo lỗi

    inbox << "From: admin@gmail.com\n";            // Ghi người gửi
    inbox << "Time: " << getCurrentTime() << "\n"; // Ghi thời gian gửi
    inbox << "Message: " << message << "\n\n";     // Ghi nội dung tin nhắn
    return true;
}
