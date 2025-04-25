#ifndef WALLET_H
#define WALLET_H
#include <string>
#include <vector>
#include "user.h"
struct Transaction {
    std::string sender;
    std::string receiver;
    double amount;
    std::string timestamp;
    std::string note;
};
extern std::vector<Transaction> transactions;
void transferPoints(User* sender);
void viewTransactionHistory(User* user);
void saveTransactionsToFile();
void loadTransactionsFromFile();

#endif

