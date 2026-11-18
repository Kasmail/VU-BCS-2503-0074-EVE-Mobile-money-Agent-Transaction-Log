// timing.cpp - A4: time insertion sort vs merge sort on random data, n = 1000..64000.
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <random>
#include <vector>
#include "Algorithms.h"

struct Rec { int key; int id; };   // lightweight stand-in for a transaction record (key = amount)

template <typename Sorter>
double timeOnce(Sorter sorter, const std::vector<Rec>& src) {
    std::vector<Rec> a = src;                       // fresh unsorted copy each run
    auto t0 = std::chrono::steady_clock::now();
    sorter(a.data(), a.size());
    auto t1 = std::chrono::steady_clock::now();
    for (std::size_t i = 1; i < a.size(); ++i)      // verify result so the work isn't optimised away
        if (a[i].key < a[i - 1].key) { std::fprintf(stderr, "NOT SORTED\n"); std::exit(1); }
    return std::chrono::duration<double, std::milli>(t1 - t0).count();
}

int main() {
    auto cmp = [](const Rec& a, const Rec& b) { return a.key < b.key; };
    std::mt19937 rng(12345);
    std::printf("n,insertion_ms,merge_ms\n");
    for (std::size_t n = 1000; n <= 64000; n *= 2) {
        double ti = 0, tm = 0;
        for (int rep = 0; rep < 3; ++rep) {
            std::vector<Rec> data(n);
            for (std::size_t i = 0; i < n; ++i) data[i] = {static_cast<int>(rng() % 1000000), static_cast<int>(i)};
            ti += timeOnce([&](Rec* p, std::size_t m) { insertionSort(p, m, cmp); }, data);
            tm += timeOnce([&](Rec* p, std::size_t m) { mergeSort(p, m, cmp); }, data);
        }
        std::printf("%zu,%.4f,%.4f\n", n, ti / 3, tm / 3);
    }
}
