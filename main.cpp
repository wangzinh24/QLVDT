#include <iostream>
#include "wallet.h"
#include "user.h"
#include "utils.h"
#include "banner.h"
using namespace std;
int main() {
    showBanner();
    loadUsersFromFile();
    setupConsole();
    loadTransactionsFromFile();
    int choice;
    do {
        cout << "\n============ MENU ============\n";
        cout << "1. Register\n";
        cout << "2. Login\n";
        cout << "0. Exit\n";
        cout << "Select: ";
        cin >> choice;

        switch (choice) {
            case 1:
                registerUser();
                break;
            case 2: {
                User* user = loginUser();
                if (user) userMenu(user);
                break;
            }
            case 0:
                cout << "Goodbye!\n";
                break;
            default:
                cout << "Invalid option.\n";
        }
    } while (choice != 0);

    return 0;
}
