// Algorithms.h - A2: insertion sort, merge sort (reg. number ends in an EVEN digit),
// and iterative binary search. All sorts take a comparator: comp(a, b) == true
// means "a must come strictly before b".
#ifndef ALGORITHMS_H
#define ALGORITHMS_H

#include <cstddef>
#include <utility>

// Stable. O(n^2) worst/average, O(n) best (already sorted). In place.
// Loop invariant (outer loop, before iteration i): a[0..i-1] holds the original
// first i elements in sorted order (stable w.r.t. comp).
template <typename T, typename Comp>
void insertionSort(T* a, std::size_t n, Comp comp) {
    for (std::size_t i = 1; i < n; ++i) {
        T key = a[i];
        std::size_t j = i;
        // Strict comparison: an equal element is never moved past 'key' => stable.
        while (j > 0 && comp(key, a[j - 1])) {
            a[j] = a[j - 1];
            --j;
        }
        a[j] = key;
    }
}

namespace detail {
template <typename T, typename Comp>
void mergeSortRec(T* a, T* tmp, std::size_t lo, std::size_t hi, Comp comp) {
    if (hi - lo < 2) return;                      // 0 or 1 element
    std::size_t mid = lo + (hi - lo) / 2;
    mergeSortRec(a, tmp, lo, mid, comp);
    mergeSortRec(a, tmp, mid, hi, comp);
    std::size_t i = lo, j = mid, k = lo;
    while (i < mid && j < hi) {
        // Take from the right only if it is strictly smaller => ties come from
        // the left run first => stable.
        if (comp(a[j], a[i])) tmp[k++] = a[j++];
        else                  tmp[k++] = a[i++];
    }
    while (i < mid) tmp[k++] = a[i++];
    while (j < hi)  tmp[k++] = a[j++];
    for (std::size_t m = lo; m < hi; ++m) a[m] = tmp[m];
}
} // namespace detail

// Stable. Theta(n log n) best/average/worst. Uses O(n) extra space.
template <typename T, typename Comp>
void mergeSort(T* a, std::size_t n, Comp comp) {
    if (n < 2) return;
    T* tmp = new T[n];
    detail::mergeSortRec(a, tmp, 0, n, comp);
    delete[] tmp;
}

// Iterative binary search on an array sorted ascending by keyOf(element).
// Returns the index of the FIRST element whose key equals 'key', or -1 if absent.
// O(log n). keyOf(const T&) -> K, K must support operator<.
template <typename T, typename K, typename KeyOf>
long binarySearch(const T* a, std::size_t n, const K& key, KeyOf keyOf) {
    std::size_t lo = 0, hi = n;                   // search window [lo, hi)
    while (lo < hi) {
        std::size_t mid = lo + (hi - lo) / 2;
        if (keyOf(a[mid]) < key) lo = mid + 1;    // first match must be to the right
        else                     hi = mid;        // a[mid] >= key: keep it as candidate
    }
    if (lo < n && !(key < keyOf(a[lo]))) return static_cast<long>(lo);
    return -1;
}

#endif
