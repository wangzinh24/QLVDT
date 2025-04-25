#include "user.h"
#include "wallet.h"
#include "send_internal_mail.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <ctime>
#include <cstdlib>
#include <thread>
#include <chrono>
#include <cstdlib>
#include <ctime>
#include <string>
using namespace std;
// Danh sách người dùng trong hệ thống (dùng cho toàn bộ chương trình)
unordered_map<string, User> users;
// Hàm sinh mã OTP ngẫu nhiên gồm 6 chữ số
// Trả về: chuỗi số OTP dùng để xác thực khi đổi thông tin
string generateOTP2() {
    srand(time(0));
    string otp = "";
    for (int i = 0; i < 6; ++i)
        otp += '0' + rand() % 10;
    return otp;
}
// Mảng chứa mã màu ANSI để in dòng nhấp nháy (hiệu ứng)
string colors[] = {
    "\033[31m", // Red
    "\033[32m", // Green
    "\033[33m", // Yellow
    "\033[34m", // Blue
    "\033[35m", // Magenta
    "\033[36m", // Cyan
};
// Hàm hiệu ứng in dòng nhấp nháy nhiều màu
// line: dòng cần in
// times: số lần nhấp nháy
// delay: độ trễ giữa các lần nháy (ms)
void blinkLine(const string& line, int times = 4, int delay = 200) {
    for (int i = 0; i < times; ++i) {
        cout << colors[i % 6] << line << "\033[0m\r" << flush;
        std::this_thread::sleep_for(std::chrono::milliseconds(delay));
        cout << string(line.length(), ' ') << "\r" << flush;
        std::this_thread::sleep_for(std::chrono::milliseconds(delay));
    }
    cout << colors[times % 6] << line << "\033[0m" << endl;
}
// Hàm băm mật khẩu thành chuỗi mã hóa (không lưu mật khẩu gốc)
string hashPassword(const string& password) {
    hash<string> hasher;
    size_t hashed = hasher(password);
    return to_string(hashed);
}
// Lưu toàn bộ dữ liệu người dùng ra file "users.txt"
void saveUsersToFile() {
    ofstream file("users.txt");
    for (const auto& pair : users) {
        const User& u = pair.second;
        file << u.username << "," << u.hashedPassword << "," << u.role << ","
        << u.balance << "," << u.fullName << "," << u.dob << "," << u.email << ","
        << u.requirePasswordChange << "\n";
    }
    file.close();
}
// Cho phép người dùng chỉnh sửa thông tin cá nhân với OTP xác thực
// INPUT: người dùng đang đăng nhập
// OUTPUT: cập nhật thông tin, ghi file nếu OTP đúng
void editProfile(User* user) {
    int choice;
    do {
        cout << "\n======== EDIT PROFILES ========\n";
        cout << "1. Change Full Name\n";
        cout << "2. Change Date of Birth\n";
        cout << "3. Change Email\n";
        cout << "0. Back\n";
        cout << "Choose: ";
        cin >> choice;
        cin.ignore();
        if (choice == 0) {
            cout << "Returning to menu...\n";
            break;
        }
        // Gửi OTP xác nhận thay đổi thông tin
        string otp = generateOTP2();
        string message = "You requested to update personal info.\nYour OTP code is: " + otp;
        if (!sendInternalMail(user->email, message)) {
            cout << "Failed to send OTP. Please try again.\n";
            return;
        }
        string inputOtp;
        cout << "Enter OTP sent to your internal mailbox: ";
        getline(cin, inputOtp);

        if (inputOtp != otp) {
            cout << "Invalid OTP. Aborting update.\n";
            return;
        }
         // Cập nhật thông tin theo lựa chọn
        switch (choice) {
            case 1:
                cout << "New full name: ";
                getline(cin, user->fullName);
                break;
            case 2:
                cout << "New date of birth (YYYY-MM-DD): ";
                getline(cin, user->dob);
                break;
            case 3:
                cout << "New email: ";
                getline(cin, user->email);
                break;
            default:
                cout << "Invalid choice.\n";
        }

        saveUsersToFile();
        cout << "Information updated successfully.\n";

    } while (true);
}
// Tải dữ liệu người dùng từ file "users.txt" vào bộ nhớ (unordered_map)
void loadUsersFromFile() {
    ifstream file("users.txt");
    if (!file) return;
    string line;
    while (getline(file, line)) {
        stringstream ss(line);
        string username, hashedPassword, role, balanceStr, fullName, dob, email;
        getline(ss, username, ',');
        getline(ss, hashedPassword, ',');
        getline(ss, role, ',');
        getline(ss, balanceStr, ',');
        getline(ss, fullName, ',');
        getline(ss, dob, ',');
        getline(ss, email, ',');
        string requireChangeStr;
        getline(ss, requireChangeStr, ',');
        bool requireChange = (requireChangeStr == "1" || requireChangeStr == "true");
        double balance = stod(balanceStr);
        users[username] = {username, hashedPassword, role, balance, fullName, dob, email, requireChange};
    }

    file.close();
}
// Đăng ký tài khoản người dùng mới
// INPUT: nhập từ bàn phím thông tin cá nhân
// OUTPUT: lưu người dùng mới vào map và file
bool registerUser() {
    string username, password, role, fullName, dob, email;
    bool autoGenerated = false;
    cout << "Enter username: ";
    cin >> username;
    if (users.count(username)) {
        cout << "❌ Account already exists.\n";
        return false;
    }
    cin.ignore();
    cout << "Enter full name: ";
    getline(cin, fullName);
    cout << "Enter date of birth (YYYY-MM-DD): ";
    getline(cin, dob);
    cout << "Enter email: ";
    getline(cin, email);
    cout << "Enter password (or leave empty to auto-generate): ";
    getline(cin, password);
     // Nếu không nhập mật khẩu, sẽ tự động tạo
    if (password.empty()) {
        srand(time(0));
        password = "pass" + to_string(rand() % 10000);
        cout << "Auto-generated password: " << password << "\n";
        autoGenerated = true;
    }
      // Kiểm tra vai trò hợp lệ
    do {
        cout << "Enter role (user/admin): ";
        cin >> role;
        if (role != "user" && role != "admin") {
            cout << "Invalid role. Please enter again.\n";
        }
    } while (role != "user" && role != "admin");
      // Tạo tài khoản mới
    string hashed = hashPassword(password);
    users[username] = {username, hashed, role, 1000.0, fullName, dob, email, autoGenerated};
    saveUsersToFile();
    cout << "✅ Account created successfully!\n";
    cout << "🔑 Your password: " << password << "\n";
    return true;
}
// Đăng nhập người dùng và xử lý thay đổi mật khẩu bắt buộc nếu cần
User* loginUser() {
    string username, password;
    cout << "Username: ";
    cin >> username;
    cout << "Password: ";
    cin >> password;
    if (!users.count(username)) {
        cout << "❌ Incorrect username.\n";
        return nullptr;
    }
    string hashed = hashPassword(password);
    if (users[username].hashedPassword != hashed) {
        cout << "❌ Incorrect password.\n";
        return nullptr;
    }
    User* u = &users[username];
    if (u->requirePasswordChange) {
    cout << "🔐 You must change your password before continuing.\n";
    string newPass;
    cout << "Enter new password: ";
    cin >> newPass;
    u->hashedPassword = hashPassword(newPass);
    u->requirePasswordChange = false;
    saveUsersToFile();
    cout << "✅ Password changed successfully. You may now continue.\n";
    }
    cout << "✅ Login successful!\n";
    return &users[username];
}
// Xem thông tin cụ thể của một người dùng theo username
void viewUserInfo() {
    string username;
    cout << "Enter username to view: ";
    cin >> username;
    if (!users.count(username)) {
        cout << "User not found.\n";
        return;
    }
    User& u = users[username];
    cout << "\n========= User Infor ==========\n";
    cout << "Username: " << u.username << "\n";
    cout << "Full Name: " << u.fullName << "\n";
    cout << "Date of Birth: " << u.dob << "\n";
    cout << "Email: " << u.email << "\n";
    cout << "Role: " << u.role << "\n";
    cout << "Balance: " << u.balance << " points\n";
}
// Duyệt và in toàn bộ người dùng trong hệ thống
void viewAllUsers() {
    cout << "\n========== User List ==========\n";
    if (users.empty()) {
        cout << "⚠️ No users found.\n";
        return;
    }

    for (const auto& pair : users) {
        const User& u = pair.second;
        cout << "👤 Username: " << u.username << "\n";
        cout << "   Name: " << u.fullName << "\n";
        cout << "   Role: " << u.role << "\n";
        cout << "   Balance: " << u.balance << "\n";
        cout << "   DOB: " << u.dob << "\n";
        cout << "   Email: " << u.email << "\n";
        cout << "-------------------------\n";
    }
}
// Hiển thị thông tin cá nhân của người đang đăng nhập
void viewMyInfo(User* user) {
    blinkLine("========= YOUR PROFILE ========");
   // cout << "\n========= YOUR PROFILE ========\n";
    cout << "👤 Full Name: " << user->fullName << "\n";
    cout << "🔑 Username: " << user->username << "\n";
    cout << "🎂 Date of Birth: " <<user->dob << "\n";
    cout << "📧 Email: " <<user->email << "\n";
    cout << "🧾 Role    : " << user->role << "\n";
    cout << "💰 Balance : " << user->balance << " points\n";
}
// Menu chính dành cho người dùng (với quyền user/admin)
void userMenu(User* user) {
    int choice;
    do {
        cout << "\n========== USER MENU ==========\n";
        cout << "Welcome, " << user->fullName << " (" << user->username << ", " << user->role << ")\n";
        cout << "1. View balance\n";
        cout << "2. Change my password\n";
        cout << "3. Edit Profile \n";
        cout << "4. Transfer points\n";
        cout << "5. View transaction history\n";
        cout << "6. View my profile\n";
        if (user->role == "admin") {
            cout << "7. Create new account\n";
            cout << "8. Reset another user's password\n";
            cout <<"9. Find user information\n";
            cout <<"10. View All User\n";
        }
        cout << "0. Logout\n";
        cout << "Select an option: ";
        cin >> choice;

        switch (choice) {
            case 1:
                cout << "💰 Your current balance: " << user->balance << " points\n";
                break;

            case 2: {
                string newPass;
                cout << "Enter new password: ";
                cin >> newPass;
                user->hashedPassword = hashPassword(newPass);
                saveUsersToFile();
                cout << "🔐 Password updated successfully!\n";
                break;
            }
            case 3:
                editProfile(user);
                break;
            case 4:
                 transferPoints(user);
                 break;
            case 5:
                 viewTransactionHistory(user);
                 break;
            case 6:
                 viewMyInfo(user);
                 break;
            case 7:
                if (user->role == "admin") {
                    registerUser();
                } else {
                    cout << "❌ Permission denied.\n";
                }
                break;

            case 8:
                if (user->role == "admin") {
                    string targetUsername;
                    cout << "Enter the username to reset password: ";
                    cin >> targetUsername;

                    if (users.count(targetUsername)) {
                        string newPass;
                        cout << "Enter new password for user '" << targetUsername << "': ";
                        cin >> newPass;
                        users[targetUsername].hashedPassword = hashPassword(newPass);
                        saveUsersToFile();
                        cout << "🔁 Password reset successfully.\n";
                    } else {
                        cout << "❌ User not found.\n";
                    }
                } else {
                    cout << "❌ Permission denied.\n";
                }
                break;
            case 9: 
                 if (user->role == "admin") {
                    viewUserInfo();
            }
            break;
            case 10:
                 if (user->role == "admin") viewAllUsers();
                 else cout << "❌ Permission denied.\n";
                 break;
            case 0:
                cout << "👋 Logging out...\n";
                break;

            default:
                cout << "⚠️ Invalid option. Please try again.\n";
        }
    } while (choice != 0);
}
