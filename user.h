#ifndef USER_H
#define USER_H
#include <string>
#include <unordered_map>
struct User {
    std::string username;
    std::string hashedPassword;
    std::string role; 
    double balance;
    std::string fullName;
	std::string dob;
	std::string email;
	bool requirePasswordChange = false; 
};
extern std::unordered_map<std::string, User> users;
std::string hashPassword(const std::string& password);
void saveUsersToFile();
void loadUsersFromFile();
bool registerUser();
User* loginUser();
void userMenu(User* user);
void viewAllUsers();
#endif

