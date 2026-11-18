// Transaction.h - record type for the Mobile Money Agent Transaction Log + file load/save.
#ifndef TRANSACTION_H
#define TRANSACTION_H

#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include "LinkedList.h"

struct Transaction {
    int id = 0;               // unique transaction id
    std::string type;         // "deposit", "withdraw" or "send"
    std::string phone;        // customer phone number (kept as text to keep leading 0)
    int amount = 0;           // in UGX, > 0
    int time = 0;             // minutes after the agent opened (e.g. 485 = 08:05)
};

inline bool validType(const std::string& t) { return t == "deposit" || t == "withdraw" || t == "send"; }

// File format, one transaction per line:  id,type,phone,amount,time   ('#' = comment)
// Returns the number loaded, or -1 if the file cannot be opened.
// Malformed lines (wrong field count, non-numeric, bad type, amount <= 0, time < 0) are skipped.
inline int loadTransactions(const std::string& path, LinkedList<Transaction>& list, int* skipped = nullptr) {
    std::ifstream in(path);
    if (!in) return -1;
    int loaded = 0, bad = 0;
    std::string line;
    while (std::getline(in, line)) {
        if (line.empty() || line[0] == '#') continue;
        std::stringstream ss(line);
        std::string f[5];
        bool ok = true;
        for (int i = 0; i < 5 && ok; ++i) ok = static_cast<bool>(std::getline(ss, f[i], ','));
        Transaction t;
        if (ok) {
            try {
                t.id = std::stoi(f[0]);
                t.type = f[1];
                t.phone = f[2];
                t.amount = std::stoi(f[3]);
                t.time = std::stoi(f[4]);
                if (!validType(t.type) || t.amount <= 0 || t.time < 0) ok = false;
            } catch (...) { ok = false; }
        }
        if (ok) { list.push_back(t); ++loaded; } else ++bad;
    }
    if (skipped) *skipped = bad;
    return loaded;
}

// Returns false if the file cannot be written.
inline bool saveTransactions(const std::string& path, const LinkedList<Transaction>& list) {
    std::ofstream out(path);
    if (!out) return false;
    out << "# id,type,phone,amount,time\n";
    std::vector<Transaction> v(list.size());
    list.copyTo(v.data());                       // one O(n) walk (at(i) in a loop would be O(n^2))
    for (const Transaction& t : v)
        out << t.id << ',' << t.type << ',' << t.phone << ',' << t.amount << ',' << t.time << '\n';
    return static_cast<bool>(out);
}

#endif
