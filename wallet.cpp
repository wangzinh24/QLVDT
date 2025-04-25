#include "wallet.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <ctime>
#include <limits>
#include <filesystem>
#include "send_internal_mail.h"
#include <cstdlib>
#include <ctime>
#include <algorithm>
using namespace std;
vector<Transaction> transactions; // Danh sách các giao dịch đã thực hiện
// Hàm sinh mã OTP gồm 6 chữ số ngẫu nhiên
// Input: không có
// Output: chuỗi 6 chữ số ngẫu nhiên (dạng string)
string generateOTP() {
    srand(time(0));
    string otp = "";
    for (int i = 0; i < 6; ++i) {
        otp += '0' + rand() % 10;
    }
    return otp;
}
// Hàm lưu toàn bộ danh sách giao dịch vào file "transactions.txt"
// Input: không có
// Output: không có (ghi trực tiếp ra file)
void saveTransactionsToFile() {
    ofstream file("transactions.txt");
    for (const auto& tx : transactions) {
        file << tx.sender << "," << tx.receiver << "," << tx.amount << "," << tx.timestamp << "," << tx.note << "\n";
    }
    file.close();
}
// Hàm tải dữ liệu giao dịch từ file vào vector transactions
// Input: không có
// Output: không có (gán giá trị vào biến toàn cục transactions)
void loadTransactionsFromFile() {
    ifstream file("transactions.txt");
    string line;
    while (getline(file, line)) {
        stringstream ss(line);
        string sender, receiver, timestamp, note, amountStr;
        double amount;
        getline(ss, sender, ',');
        getline(ss, receiver, ',');
        getline(ss, amountStr, ',');
        getline(ss, timestamp, ',');
        getline(ss, note);
        amount = stod(amountStr);
        transactions.push_back({sender, receiver, amount, timestamp, note});
    }
    file.close();
}
// Hàm sao lưu dữ liệu người dùng và giao dịch ra thư mục backups/
// Input: không có
// Output: không có (tạo các file backup trong thư mục backups/)
void backupData() {
    string backupDir = "backups";
    if (!filesystem::exists(backupDir)) {
        filesystem::create_directory(backupDir);
    }
    auto now = chrono::system_clock::now();
    time_t now_c = chrono::system_clock::to_time_t(now);
    tm* timeinfo = localtime(&now_c);
    stringstream ss;
    ss << put_time(timeinfo, "%Y-%m-%d_%H-%M-%S");
    string timestamp = ss.str();
    string userBackup = backupDir + "/users_" + timestamp + ".txt";
    string txBackup = backupDir + "/transactions_" + timestamp + ".txt";
    try {
        filesystem::copy_file("users.txt", userBackup, filesystem::copy_options::overwrite_existing);
        filesystem::copy_file("transactions.txt", txBackup, filesystem::copy_options::overwrite_existing);
    } catch (const exception& e) {
        cerr << "❌ Backup failed: " << e.what() << "\n";
        return;
    }
    // Giữ lại 2 file backup gần nhất, xóa các file cũ hơn
    vector<filesystem::directory_entry> userBackups;
    for (const auto& entry : filesystem::directory_iterator(backupDir)) {
        if (entry.path().filename().string().find("users_") == 0)
            userBackups.push_back(entry);
    }
    sort(userBackups.begin(), userBackups.end(), [](const auto& a, const auto& b) {
        return filesystem::last_write_time(a) > filesystem::last_write_time(b);
    });
    while (userBackups.size() > 2) {
        try {
            filesystem::remove(userBackups.back());
            userBackups.pop_back();
        } catch (...) {
            cerr << "❌ Failed to delete old user backup.\n";
            break;
        }
    }
    vector<filesystem::directory_entry> txBackups;
    for (const auto& entry : filesystem::directory_iterator(backupDir)) {
        if (entry.path().filename().string().find("transactions_") == 0)
            txBackups.push_back(entry);
    }
    sort(txBackups.begin(), txBackups.end(), [](const auto& a, const auto& b) {
        return filesystem::last_write_time(a) > filesystem::last_write_time(b);
    });
    while (txBackups.size() > 2) {
        try {
            filesystem::remove(txBackups.back());
            txBackups.pop_back();
        } catch (...) {
            cerr << "❌ Failed to delete old transaction backup.\n";
            break;
        }
    }
}
// Hàm thực hiện chuyển điểm từ người dùng hiện tại sang một người dùng khác
// Input: con trỏ đến User đang đăng nhập
// Output: thực hiện giao dịch và lưu vào file (nếu OTP đúng)
void transferPoints(User* sender) {
    string recipientUsername;
    cout << "Enter recipient's username: ";
    cin >> recipientUsername;
    if (users.find(recipientUsername) == users.end()) {
        cout << "❌ Recipient not found.\n";
        return;
    }
    if (recipientUsername == sender->username) {
        cout << "❌ You cannot transfer points to yourself.\n";
        return;
    }
    User& recipient = users[recipientUsername];
    double amount;
    while (true) {
        cout << "Enter amount to transfer: ";
        cin >> amount;

        if (cin.fail() || amount <= 0 || amount > sender->balance) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "❌ Invalid amount. Please enter a valid number and ensure you have enough balance.\n";
        } else {
            break;
        }
    }
    cin.ignore();
    string note;
    cout << "Enter transaction note (optional): ";
    getline(cin, note);
    // Gửi OTP qua mail
    string otp = generateOTP();
    string message = "You are performing a POINT TRANSFER transaction.\nThe OTP verification code is: " + otp;
    if (!sendInternalMail(sender->email, message)) {
        cout << "❌ Unable to send OTP. Transaction canceled.\n";
        return;
    }
    string inputOtp;
    cout << "Enter the OTP sent to your internal email: ";
    cin >> inputOtp;
    if (inputOtp != otp) {
        cout << "❌ Incorrect OTP code. Transaction canceled.\n";
        return;
    }
    // Tiến hành chuyển điểm nếu OTP hợp lệ
    try {
        sender->balance -= amount;
        recipient.balance += amount;
        time_t now = time(0);
        string timestamp = ctime(&now);
        timestamp.pop_back();
        transactions.push_back({sender->username, recipient.username, amount, timestamp, note});
        saveTransactionsToFile();
        saveUsersToFile();
        backupData();
        cout << "✅ Transfer successful!\n";
    } catch (...) {
         // Trong trường hợp lỗi xảy ra, khôi phục lại số dư
        sender->balance += amount;
        recipient.balance -= amount;
        cout << "❌ Transfer failed. Transaction rolled back.\n";
    }
}
// Hàm hiển thị lịch sử giao dịch của người dùng
// Input: con trỏ đến User cần xem lịch sử
// Output: in ra danh sách giao dịch liên quan đến user đó
void viewTransactionHistory(User* user) {
    cout << "\n===== TRANSACTION HISTORY =====\n";
    for (const auto& tx : transactions) {
        if (tx.sender == user->username || tx.receiver == user->username) {
            cout << "[" << tx.timestamp << "] ";
            cout << tx.sender << " → " << tx.receiver << ": " << tx.amount << " points";
            if (!tx.note.empty()) cout << " (" << tx.note << ")";
            cout << "\n";
        }
    }
}
