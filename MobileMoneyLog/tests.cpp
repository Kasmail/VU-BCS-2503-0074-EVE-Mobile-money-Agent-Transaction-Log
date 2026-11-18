// tests.cpp - test table for A1 and A2. One tab-separated line per test:
// ID <TAB> operation <TAB> expected <TAB> actual <TAB> PASS/FAIL
#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>
#include "Algorithms.h"
#include "Transaction.h"

static int passed = 0, failed = 0;
static void check(const std::string& id, const std::string& op, const std::string& exp, const std::string& act) {
    bool ok = exp == act;
    (ok ? passed : failed)++;
    std::cout << id << '\t' << op << '\t' << exp << '\t' << act << '\t' << (ok ? "PASS" : "FAIL") << '\n';
}
static std::string b(bool v) { return v ? "true" : "false"; }
static std::string n(long v) { return std::to_string(v); }

static Transaction mk(int id, int amount, int time = 0) {
    Transaction t; t.id = id; t.type = "deposit"; t.phone = "0770000000"; t.amount = amount; t.time = time;
    return t;
}
static std::string ids(const LinkedList<Transaction>& l) {
    std::vector<Transaction> v(l.size());
    l.copyTo(v.data());
    std::string s;
    for (std::size_t i = 0; i < v.size(); ++i) s += (i ? "," : "") + n(v[i].id);
    return s.empty() ? "(empty)" : s;
}
static std::string ids(const std::vector<Transaction>& v) {
    std::string s;
    for (std::size_t i = 0; i < v.size(); ++i) s += (i ? "," : "") + n(v[i].id);
    return s.empty() ? "(empty)" : s;
}
static auto byAmount = [](const Transaction& a, const Transaction& c) { return a.amount < c.amount; };
static auto byAmountHigh = [](const Transaction& a, const Transaction& c) { return a.amount > c.amount; };
static auto byTime = [](const Transaction& a, const Transaction& c) { return a.time < c.time; };
static auto byId = [](const Transaction& a, const Transaction& c) { return a.id < c.id; };
static auto byPhone = [](const Transaction& a, const Transaction& c) { return a.phone < c.phone; };
static auto idOf = [](int id) { return [id](const Transaction& t) { return t.id == id; }; };

int main() {
    Transaction out;

    // ---------------- A1: linked list ----------------
    LinkedList<Transaction> a;
    check("A1-01", "new list: empty(), size()", "true, 0", b(a.empty()) + ", " + n(a.size()));
    bool r2 = a.removeAt(0);
    check("A1-02", "removeAt(0) on empty list", "false", b(r2));
    check("A1-03", "front()/back()/at(0) on empty list", "all nullptr",
          (a.front() == nullptr && a.back() == nullptr && a.at(0) == nullptr) ? "all nullptr" : "not null");
    bool r4 = a.removeFirstWhere(idOf(1));
    bool r4b = a.pop_front(out);
    check("A1-04", "removeFirstWhere / pop_front on empty list", "false, false", b(r4) + ", " + b(r4b));

    a.push_back(mk(1, 50));
    check("A1-05", "one record: size, front id, back id", "1, 1, 1", n(a.size()) + ", " + n(a.front()->id) + ", " + n(a.back()->id));
    bool r6 = a.removeAt(0, &out);
    std::string st6 = b(r6) + ", removed " + n(out.id) + ", " + (a.empty() ? "empty" : "not empty");
    a.push_back(mk(7, 50));
    check("A1-06", "one record: removeAt(0), then push_back(7) works", "true, removed 1, empty / front 7 back 7",
          st6 + " / front " + n(a.front()->id) + " back " + n(a.back()->id));

    LinkedList<Transaction> l;
    for (int i = 1; i <= 3; ++i) l.push_back(mk(i, 10));
    check("A1-07", "push_back 1,2,3: order, back id", "1,2,3, back 3", ids(l) + ", back " + n(l.back()->id));
    l.push_front(mk(0, 10));
    check("A1-08", "push_front(0): order, front id", "0,1,2,3, front 0", ids(l) + ", front " + n(l.front()->id));

    LinkedList<Transaction> d;
    d.push_back(mk(1, 70)); d.push_back(mk(2, 70));
    check("A1-09", "duplicate key (two transactions of amount 70) both stored", "2, 1,2", n(d.size()) + ", " + ids(d));

    LinkedList<Transaction> r;
    for (int i = 1; i <= 5; ++i) r.push_back(mk(i, 60));
    bool x10 = r.removeAt(0, &out);
    check("A1-10", "removeAt(0) (first record)", "true, removed 1, left 2,3,4,5, front 2", b(x10) + ", removed " + n(out.id) + ", left " + ids(r) + ", front " + n(r.front()->id));
    bool x11 = r.removeAt(r.size() - 1);
    check("A1-11", "removeAt(size-1) (last record): tail moves back", "true, left 2,3,4, back 4", b(x11) + ", left " + ids(r) + ", back " + n(r.back()->id));
    r.push_back(mk(9, 60));
    check("A1-12", "push_back after removing the last record", "2,3,4,9, back 9", ids(r) + ", back " + n(r.back()->id));
    bool x13 = r.removeAt(1);
    check("A1-13", "removeAt(1) (middle)", "true, left 2,4,9", b(x13) + ", left " + ids(r));
    bool x14 = r.removeAt(99);
    check("A1-14", "removeAt(99) invalid index", "false, unchanged 2,4,9", b(x14) + ", unchanged " + ids(r));
    bool x15 = r.removeFirstWhere(idOf(555));
    bool x15b = r.removeFirstWhere(idOf(4), &out);
    check("A1-15", "removeFirstWhere: absent id 555 / present id 4", "false / true, removed 4, left 2,9", b(x15) + " / " + b(x15b) + ", removed " + n(out.id) + ", left " + ids(r));
    bool x16 = r.pop_front(out);
    check("A1-16", "pop_front", "true, popped 2, left 9", b(x16) + ", popped " + n(out.id) + ", left " + ids(r));

    LinkedList<Transaction> u;
    u.push_back(mk(1, 100)); u.push_back(mk(2, 200));
    Transaction* p = u.find(idOf(2));
    if (p) p->amount = 999;
    check("A1-17", "find id 2 and update its amount through the pointer; find absent id", "999, nullptr",
          n(u.at(1)->amount) + ", " + (u.find(idOf(77)) == nullptr ? "nullptr" : "found"));

    LinkedList<Transaction> c1; for (int i = 1; i <= 3; ++i) c1.push_back(mk(i, 10));
    LinkedList<Transaction> c2(c1);
    c2.removeAt(0); c2.push_back(mk(9, 10));
    check("A1-18", "copy constructor is deep: change copy, original intact", "orig 1,2,3 / copy 2,3,9", "orig " + ids(c1) + " / copy " + ids(c2));
    LinkedList<Transaction> c3; c3 = c1; c3.removeAt(0);
    LinkedList<Transaction>& self = c1; c1 = self;
    check("A1-19", "copy assignment deep, self-assignment safe", "orig 1,2,3 / copy 2,3", "orig " + ids(c1) + " / copy " + ids(c3));

    LinkedList<Transaction> big;
    for (int i = 1; i <= 10000; ++i) big.push_back(mk(i, 10));
    check("A1-20", "10,000 push_back calls: size, front id, back id", "10000, 1, 10000", n(big.size()) + ", " + n(big.front()->id) + ", " + n(big.back()->id));

    LinkedList<Transaction> f; int skipped = 0;
    int loaded = loadTransactions("transactions.txt", f, &skipped);
    check("A1-21", "load transactions.txt (12 valid records)", "12 loaded, 0 skipped", n(loaded) + " loaded, " + n(skipped) + " skipped");
    LinkedList<Transaction> none;
    check("A1-22", "load a file that does not exist", "-1", n(loadTransactions("nope.txt", none)));
    f.pop_front(out); f.push_back(mk(6000, 1234, 600));
    bool saved = saveTransactions("tmp_saved.txt", f);
    LinkedList<Transaction> g2; int gl = loadTransactions("tmp_saved.txt", g2);
    check("A1-23", "update (1 removed, 1 added), save, reload", "true, 12, same order", b(saved) + ", " + n(gl) + ", " + (ids(g2) == ids(f) ? "same order" : "DIFFERENT"));
    { FILE* t = fopen("tmp_bad.txt", "w");
      fputs("1,deposit,0770,500,10\nbad line\n2,loan,0770,500,10\n3,send,0770,-5,10\n4,send,0770,300\n5,send,0770,300,20\n", t); fclose(t); }
    LinkedList<Transaction> bad; int bs = 0; int bl = loadTransactions("tmp_bad.txt", bad, &bs);
    check("A1-24", "file with 4 malformed lines of 6 (bad line, type loan, amount -5, missing field)", "2 loaded, 4 skipped", n(bl) + " loaded, " + n(bs) + " skipped");

    // ---------------- A2: sorting & searching ----------------
    std::vector<Transaction> v;
    mergeSort(v.data(), v.size(), byAmount); insertionSort(v.data(), v.size(), byAmount);
    check("A2-01", "both sorts on empty array", "no crash, (empty)", "no crash, " + ids(v));
    v = {mk(1, 50)};
    mergeSort(v.data(), v.size(), byAmount); insertionSort(v.data(), v.size(), byAmount);
    check("A2-02", "both sorts on one record", "1", ids(v));

    std::vector<Transaction> sorted, rev;
    for (int i = 1; i <= 6; ++i) sorted.push_back(mk(i, 1000 * ((i + 1) / 2)));      // amounts 1000,1000,2000,2000,3000,3000
    rev = sorted; std::reverse(rev.begin(), rev.end());
    auto s1 = sorted, s2 = sorted;
    insertionSort(s1.data(), s1.size(), byAmount); mergeSort(s2.data(), s2.size(), byAmount);
    check("A2-03", "already sorted input (insertion / merge)", "1,2,3,4,5,6 / 1,2,3,4,5,6", ids(s1) + " / " + ids(s2));
    s1 = rev; s2 = rev;
    insertionSort(s1.data(), s1.size(), byAmount); mergeSort(s2.data(), s2.size(), byAmount);
    check("A2-04", "reversed input 6..1 by amount (equal amounts keep input order)", "2,1,4,3,6,5 / 2,1,4,3,6,5", ids(s1) + " / " + ids(s2));

    std::srand(7);
    std::vector<Transaction> rnd; for (int i = 0; i < 500; ++i) rnd.push_back(mk(i, 1 + std::rand() % 100));
    auto ref = rnd; std::stable_sort(ref.begin(), ref.end(), byAmount);
    s1 = rnd; s2 = rnd;
    insertionSort(s1.data(), s1.size(), byAmount); mergeSort(s2.data(), s2.size(), byAmount);
    check("A2-05", "500 random records vs std::stable_sort", "identical, identical",
          std::string(ids(s1) == ids(ref) ? "identical" : "DIFFERENT") + ", " + (ids(s2) == ids(ref) ? "identical" : "DIFFERENT"));

    // stability on the real sample data: list is in time order; sort by amount, biggest first
    LinkedList<Transaction> st; loadTransactions("transactions.txt", st);
    std::vector<Transaction> sv(st.size()); st.copyTo(sv.data());
    mergeSort(sv.data(), sv.size(), byAmountHigh);
    st.assign(sv.data(), sv.size());
    check("A2-06", "stability: sample log (time order) ranked by amount, biggest first; ties stay in time order",
          "5011,5006,5003,5002,5005,5012,5001,5007,5010,5009,5004,5008", ids(st));

    std::vector<Transaction> nm = {mk(3, 1), mk(1, 1), mk(4, 1), mk(2, 1)};
    nm[0].phone = "0773"; nm[1].phone = "0771"; nm[2].phone = "0774"; nm[3].phone = "0772";
    s1 = nm; s2 = nm;
    insertionSort(s1.data(), s1.size(), byPhone); mergeSort(s2.data(), s2.size(), byPhone);
    check("A2-07", "sort by a different field (phone), input ids 3,1,4,2", "1,2,3,4 / 1,2,3,4", ids(s1) + " / " + ids(s2));

    auto key = [](const Transaction& t) { return t.amount; };
    std::vector<Transaction> e;
    check("A2-08", "binary search in empty array", "-1", n(binarySearch(e.data(), e.size(), 50, key)));
    std::vector<Transaction> one = {mk(1, 50)};
    check("A2-09", "binary search, one record: present(50) / absent(40)", "0 / -1",
          n(binarySearch(one.data(), 1, 50, key)) + " / " + n(binarySearch(one.data(), 1, 40, key)));
    check("A2-10", "search duplicate amount 2000 returns FIRST occurrence", "2", n(binarySearch(sorted.data(), sorted.size(), 2000, key)));
    check("A2-11", "search first key (1000) and last key (3000)", "0 / 4",
          n(binarySearch(sorted.data(), sorted.size(), 1000, key)) + " / " + n(binarySearch(sorted.data(), sorted.size(), 3000, key)));
    check("A2-12", "search absent amounts 500, 2500 and 9999", "-1 / -1 / -1",
          n(binarySearch(sorted.data(), sorted.size(), 500, key)) + " / " + n(binarySearch(sorted.data(), sorted.size(), 2500, key)) + " / " + n(binarySearch(sorted.data(), sorted.size(), 9999, key)));

    LinkedList<Transaction> logl; loadTransactions("transactions.txt", logl);
    std::vector<Transaction> lv(logl.size()); logl.copyTo(lv.data());
    mergeSort(lv.data(), lv.size(), byId);
    long pf = binarySearch(lv.data(), lv.size(), 5007, [](const Transaction& t) { return t.id; });
    long qf = binarySearch(lv.data(), lv.size(), 5099, [](const Transaction& t) { return t.id; });
    check("A2-13", "log copied to array, sorted by id, binary search 5007 / 5099", "phone 0775000005 / -1",
          (pf >= 0 ? "phone " + lv[static_cast<std::size_t>(pf)].phone : "-1") + " / " + n(qf));
    mergeSort(lv.data(), lv.size(), byTime);
    check("A2-14", "sort log by time restores chronological order", "5003,5001,5009,5002,5007,5011,5004,5010,5006,5012,5005,5008", ids(lv));

    std::cout << "SUMMARY\t" << passed << " passed, " << failed << " failed\n";
    std::remove("tmp_saved.txt"); std::remove("tmp_bad.txt");
    return failed ? 1 : 0;
}
