// main.cpp - Mobile Money Agent Transaction Log: menu-driven application.
#include <iostream>
#include <limits>
#include <stdexcept>
#include <string>
#include <vector>
#include "Algorithms.h"
#include "Transaction.h"

static const char* FILE_NAME = "transactions.txt";

static bool byTime(const Transaction& a, const Transaction& b) { return a.time < b.time; }
static bool byAmountHigh(const Transaction& a, const Transaction& b) { return a.amount > b.amount; }
static bool byId(const Transaction& a, const Transaction& b) { return a.id < b.id; }

static void print(const Transaction& t) {
    std::cout << "  #" << t.id << "  " << t.type << "  " << t.phone << "  UGX " << t.amount
              << "  time " << t.time << "\n";
}
static void printAll(const std::vector<Transaction>& v) {
    if (v.empty()) { std::cout << "  (no transactions)\n"; return; }
    for (const Transaction& t : v) print(t);
}
static std::vector<Transaction> toVector(const LinkedList<Transaction>& l) {
    std::vector<Transaction> v(l.size());
    l.copyTo(v.data());
    return v;
}
static void showMenu() {
    std::cout << "\n====================================\n";
    std::cout << "   MOBILE MONEY TRANSACTION LOG\n";
    std::cout << "====================================\n";
    std::cout << "1. Record Transaction\n";
    std::cout << "2. Show All Transactions\n";
    std::cout << "3. Update Amount\n";
    std::cout << "4. Remove Transaction\n";
    std::cout << "5. Sort by Time\n";
    std::cout << "6. Ranking by Amount (Merge Sort)\n";
    std::cout << "7. Search by Id (Binary Search)\n";
    std::cout << "8. Save\n";
    std::cout << "0. Save and Exit\n";
    std::cout << "------------------------------------\n";
}

static int readInt(const char* prompt) {
    int v;
    std::cout << prompt;
    while (!(std::cin >> v)) {
        if (std::cin.eof()) throw std::runtime_error("input closed");
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "Enter a number: ";
    }
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    return v;
}
static std::string readLine(const char* prompt) {
    std::string s;
    std::cout << prompt;
    if (!std::getline(std::cin, s)) throw std::runtime_error("input closed");
    return s;
}

int main() {
    LinkedList<Transaction> log;
    int skipped = 0;
    int n = loadTransactions(FILE_NAME, log, &skipped);
    if (n < 0) std::cout << "Could not open " << FILE_NAME << " - starting empty.\n";
    else std::cout << "Loaded " << n << " transactions (" << skipped << " bad lines skipped).\n";

    try {
        for (;;) {
            showMenu();
            int c = readInt("Choice: ");
            if (c == 1) {
                Transaction t;
                t.id = readInt("Id: ");
                t.type = readLine("Type (deposit/withdraw/send): ");
                t.phone = readLine("Customer phone: ");
                t.amount = readInt("Amount (UGX): ");
                t.time = readInt("Time (minutes after opening): ");
                if (!validType(t.type)) std::cout << "Rejected: type must be deposit, withdraw or send.\n";
                else if (t.amount <= 0 || t.time < 0) std::cout << "Rejected: amount must be above 0 and time 0 or more.\n";
                else if (log.find([&](const Transaction& x) { return x.id == t.id; })) std::cout << "Rejected: id already exists.\n";
                else { log.push_back(t); std::cout << "Recorded.\n"; }
            } else if (c == 2) {
                printAll(toVector(log));
                std::cout << "  (" << log.size() << " transactions)\n";
            } else if (c == 3) {
                int id = readInt("Id: ");
                int amt = readInt("New amount (UGX): ");
                Transaction* p = log.find([&](const Transaction& x) { return x.id == id; });
                if (!p) std::cout << "No such transaction.\n";
                else if (amt <= 0) std::cout << "Rejected: amount must be above 0.\n";
                else { p->amount = amt; std::cout << "Updated.\n"; }
            } else if (c == 4) {
                int id = readInt("Id to remove: ");
                Transaction gone;
                if (log.removeFirstWhere([&](const Transaction& x) { return x.id == id; }, &gone)) { std::cout << "Removed:\n"; print(gone); }
                else std::cout << "No such transaction.\n";
            } else if (c == 5) {
                std::vector<Transaction> v = toVector(log);
                mergeSort(v.data(), v.size(), byTime);
                log.assign(v.data(), v.size());
                std::cout << "Log sorted by time (earliest first).\n";
            } else if (c == 6) {
                std::vector<Transaction> v = toVector(log);
                mergeSort(v.data(), v.size(), byTime);          // 1st: by time
                mergeSort(v.data(), v.size(), byAmountHigh);    // 2nd (stable): by amount, biggest first
                std::cout << "Ranking by amount (equal amounts stay in time order):\n";
                printAll(v);
            } else if (c == 7) {
                std::vector<Transaction> v = toVector(log);     // the list cannot jump to the middle,
                mergeSort(v.data(), v.size(), byId);            // so search a sorted array copy
                int key = readInt("Id to find: ");
                long i = binarySearch(v.data(), v.size(), key, [](const Transaction& t) { return t.id; });
                if (i < 0) std::cout << "Not found.\n";
                else print(v[static_cast<std::size_t>(i)]);
            } else if (c == 8 || c == 0) {
                if (saveTransactions(FILE_NAME, log)) std::cout << "Saved " << log.size() << " transactions.\n";
                else std::cout << "Save failed.\n";
                if (c == 0) break;
            } else std::cout << "Unknown choice.\n";
        }
    } catch (const std::runtime_error& e) {
        std::cout << "\nInput ended (" << e.what() << "); exiting without saving.\n";
    }
    return 0;
}
