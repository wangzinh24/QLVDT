
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <iomanip>
#include <ctime>
#include <filesystem>
#include <regex>
#include <openssl/sha.h>

using namespace std;
namespace fs = std::filesystem;

const string CREDENTIAL_FILE = "user.txt";
const string MAILBOX_FOLDER = "mail_box/";
// Hàm in banner
void showBanner() {
        string rainbow[] = {
            "\033[91m", // đỏ
            "\033[93m", // vàng
            "\033[92m", // xanh lá
            "\033[96m", // cyan
            "\033[94m", // xanh dương
            "\033[95m"  // tím
        };
        string reset = "\033[0m";
        string lines[] = {
            "+=============================+",
            "+                             +",
            "+           G - Meow          +",
            "+                             +",
            "+=============================+"
        };
    
        int colorIndex = 0;
        for (int i = 0; i < 5; ++i) {
            for (size_t j = 0; j < lines[i].length(); ++j) {
                cout << rainbow[colorIndex % 6] << lines[i][j];
                colorIndex++;
            }
            cout << reset << "\n";
        }
    }
// Hàm băm chuỗi thành SHA256
// Input: chuỗi cần mã hóa
// Output: chuỗi mã hóa dạng hex
string sha256(const string& str) {
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256((unsigned char*)str.c_str(), str.size(), hash);
    stringstream ss;
    for (unsigned char c : hash) {
        ss << hex << setw(2) << setfill('0') << (int)c;
    }
    return ss.str();
}
// Hàm kiểm tra định dạng email (phải là gmail.com)
bool isValidGmail(const string& email) {
    regex pattern(R"(^[\w.+-]+@gmail\.com$)");
    return regex_match(email, pattern);
}
// Hàm kiểm tra tài khoản đã tồn tại trong file user.txt chưa
bool usernameExists(const string& username) {
    ifstream inFile(CREDENTIAL_FILE);
    string line;
    while (getline(inFile, line)) {
        if (line.substr(0, line.find(',')) == username) {
            return true;
        }
    }
    return false;
}
// Hàm đăng ký tài khoản mới
// Input: username (gmail), password
// Output: true nếu thành công, false nếu thất bại
bool registerUser(const string& username, const string& password) {
    if (!isValidGmail(username)) {
        cout << "Email must be in format 'example@gmail.com'.\n";
        return false;
    }

    if (usernameExists(username)) {
        cout << "Username already exists.\n";
        return false;
    }

    ofstream credFile(CREDENTIAL_FILE, ios::app);
    credFile << username << "," << sha256(password) << "\n";

    fs::create_directory(MAILBOX_FOLDER);
    ofstream mailbox(MAILBOX_FOLDER + username + ".txt");
    mailbox << "Inbox created for " << username << "\n";

    cout << "Registration successful.\n";
    return true;
}
// Hàm xác thực đăng nhập
// Input: username, password
// Output: true nếu đúng, false nếu sai
bool authenticateUser(const string& username, const string& password) {
    ifstream inFile(CREDENTIAL_FILE);
    string line, storedUser, storedHash;
    while (getline(inFile, line)) {
        stringstream ss(line);
        getline(ss, storedUser, ',');
        getline(ss, storedHash);
        if (storedUser == username && storedHash == sha256(password)) {
            return true;
        }
    }
    return false;
}
// Hàm lấy thời gian hiện tại dưới dạng chuỗi
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
// Hàm hiển thị hộp thư đến của người dùng
void viewInbox(const string& username) {
    string path = MAILBOX_FOLDER + username + ".txt";
    ifstream file(path);
    if (!file.is_open()) {
        cout << "Cannot open inbox.\n";
        return;
    }
    string line;
    cout << "\n=========== Inbox ============\n";
    while (getline(file, line)) {
        cout << line << "\n";
    }
}
// Hàm gửi mail cho người dùng khác
// Người dùng nhập recipient (người nhận) và nội dung tin nhắn
void sendMail(const string& sender) {
    string recipient, message;
    cout << "Send to: ";
    cin >> recipient;

    if (!isValidGmail(recipient)) {
        cout << "Recipient must be a valid Gmail address.\n";
        return;
    }

    if (!usernameExists(recipient)) {
        cout << "Recipient does not exist.\n";
        return;
    }

    cin.ignore();
    cout << "Message: ";
    getline(cin, message);

    ofstream inbox(MAILBOX_FOLDER + recipient + ".txt", ios::app);
    inbox << "From: " << sender << "\n";
    inbox << "Time: " << getCurrentTime() << "\n";
    inbox << "Message: " << message << "\n\n";
    cout << "Message sent.\n";
}
// Hàm quản lý phiên làm việc của người dùng trong hộp thư
void mailboxSession(const string& username) {
    int choice;
    do {
        cout << "\n========= Mail Menu ==========\n";
        cout << "1. View Inbox\n";
        cout << "2. Send Mail\n";
        cout << "3. Logout\n";
        cout << "Choice: ";
        cin >> choice;
        switch (choice) {
            case 1:
                viewInbox(username);
                break;
            case 2:
                sendMail(username);
                break;
            case 3:
                cout << "Logging out...\n";
                break;
            default:
                cout << "Invalid option.\n";
        }
    } while (choice != 3);
}

int main() {
    showBanner();
    int choice;
    string username, password;

    do {
        cout << "\n========== Mail App ==========\n";
        cout << "1. Register\n";
        cout << "2. Login\n";
        cout << "0. Exit\n";
        cout << "Choice: ";
        cin >> choice;

        switch (choice) {
            case 1:
                cout << "Username (Gmail): ";
                cin >> username;
                cout << "Password: ";
                cin >> password;
                registerUser(username, password);
                break;
            case 2:
                cout << "Username (Gmail): ";
                cin >> username;
                cout << "Password: ";
                cin >> password;
                if (authenticateUser(username, password)) {
                    mailboxSession(username);
                } else {
                    cout << "Login failed. Wrong credentials.\n";
                }
                break;
            case 0:
                cout << "Exiting...\n";
                break;
            default:
                cout << "Invalid choice.\n";
        }
    } while (choice != 0);

    return 0;
}
