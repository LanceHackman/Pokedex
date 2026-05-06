#pragma once
#include <string>
#include <vector>
#include <functional>
#include "Pokemon.h"

// Benchmark result for one sorting run
struct SortResult {
    std::string              algorithm;
    long long                timeUs;      // microseconds
    std::vector<const Pokemon*> sorted;
};

class Sorter {
public:
    //  Three sorting algorithms

    // STL sort (introsort — O(n log n) guaranteed)
    SortResult stlSort(std::vector<const Pokemon*> pool,
                       std::function<bool(const Pokemon*, const Pokemon*)> cmp) const;

    // Merge sort — O(n log n), stable, good for benchmarking comparison
    SortResult mergeSort(std::vector<const Pokemon*> pool,
                         std::function<bool(const Pokemon*, const Pokemon*)> cmp) const;

    // Insertion sort — O(n²), included to show why it's worse at scale
    SortResult insertionSort(std::vector<const Pokemon*> pool,
                             std::function<bool(const Pokemon*, const Pokemon*)> cmp) const;

    //  Pre-built comparators ─────────
    static bool byId(const Pokemon* a, const Pokemon* b);
    static bool byName(const Pokemon* a, const Pokemon* b);
    static bool byGeneration(const Pokemon* a, const Pokemon* b);
    static bool byTotalStats(const Pokemon* a, const Pokemon* b);
    static bool bySpeed(const Pokemon* a, const Pokemon* b);
    static bool byHp(const Pokemon* a, const Pokemon* b);

    // ── Benchmarking ────────────────────────────────────────

    // Runs all three algorithms on the same pool with the same comparator
    // and prints a comparison table
    void benchmark(const std::vector<const Pokemon*>& pool,
                   std::function<bool(const Pokemon*, const Pokemon*)> cmp,
                   const std::string& sortLabel) const;

private:
    // Merge sort internals
    void mergeSortHelper(std::vector<const Pokemon*>& arr, int left, int right,
                         std::function<bool(const Pokemon*, const Pokemon*)> cmp) const;
    void merge(std::vector<const Pokemon*>& arr, int left, int mid, int right,
               std::function<bool(const Pokemon*, const Pokemon*)> cmp) const;
};
