#include "Sorter.h"
#include <algorithm>
#include <chrono>
#include <iostream>
#include <iomanip>
#include <numeric>


// STL sort — uses introsort under the hood

SortResult Sorter::stlSort(
    std::vector<const Pokemon*> pool,
    std::function<bool(const Pokemon*, const Pokemon*)> cmp) const
{
    auto start = std::chrono::high_resolution_clock::now();
    std::sort(pool.begin(), pool.end(), cmp);
    auto end = std::chrono::high_resolution_clock::now();

    return {
        "STL sort (introsort)",
        std::chrono::duration_cast<std::chrono::microseconds>(end - start).count(),
        pool
    };
}


// Merge sort — recursive, stable

SortResult Sorter::mergeSort(
    std::vector<const Pokemon*> pool,
    std::function<bool(const Pokemon*, const Pokemon*)> cmp) const
{
    auto start = std::chrono::high_resolution_clock::now();
    mergeSortHelper(pool, 0, static_cast<int>(pool.size()) - 1, cmp);
    auto end = std::chrono::high_resolution_clock::now();

    return {
        "Merge sort",
        std::chrono::duration_cast<std::chrono::microseconds>(end - start).count(),
        pool
    };
}

void Sorter::mergeSortHelper(
    std::vector<const Pokemon*>& arr, int left, int right,
    std::function<bool(const Pokemon*, const Pokemon*)> cmp) const
{
    if (left >= right) return;
    int mid = left + (right - left) / 2;
    mergeSortHelper(arr, left,    mid,   cmp);
    mergeSortHelper(arr, mid + 1, right, cmp);
    merge(arr, left, mid, right, cmp);
}

void Sorter::merge(
    std::vector<const Pokemon*>& arr, int left, int mid, int right,
    std::function<bool(const Pokemon*, const Pokemon*)> cmp) const
{
    std::vector<const Pokemon*> temp(right - left + 1);
    int i = left, j = mid + 1, k = 0;

    while (i <= mid && j <= right) {
        if (cmp(arr[i], arr[j])) temp[k++] = arr[i++];
        else                      temp[k++] = arr[j++];
    }
    while (i <= mid)   temp[k++] = arr[i++];
    while (j <= right) temp[k++] = arr[j++];

    for (int x = 0; x < (int)temp.size(); x++)
        arr[left + x] = temp[x];
}


// Insertion sort — O(n²), slow on large input
// Included to demonstrate why it loses at scale

SortResult Sorter::insertionSort(
    std::vector<const Pokemon*> pool,
    std::function<bool(const Pokemon*, const Pokemon*)> cmp) const
{
    auto start = std::chrono::high_resolution_clock::now();

    for (int i = 1; i < (int)pool.size(); i++) {
        const Pokemon* key = pool[i];
        int j = i - 1;
        while (j >= 0 && cmp(key, pool[j])) {
            pool[j + 1] = pool[j];
            j--;
        }
        pool[j + 1] = key;
    }

    auto end = std::chrono::high_resolution_clock::now();

    return {
        "Insertion sort",
        std::chrono::duration_cast<std::chrono::microseconds>(end - start).count(),
        pool
    };
}


// Comparators

bool Sorter::byId(const Pokemon* a, const Pokemon* b) {
    return a->id < b->id;
}
bool Sorter::byName(const Pokemon* a, const Pokemon* b) {
    return a->name < b->name;
}
bool Sorter::byGeneration(const Pokemon* a, const Pokemon* b) {
    return a->generation < b->generation;
}
bool Sorter::byTotalStats(const Pokemon* a, const Pokemon* b) {
    int totalA = a->hp + a->attack + a->defense + a->spAttack + a->spDefense + a->speed;
    int totalB = b->hp + b->attack + b->defense + b->spAttack + b->spDefense + b->speed;
    return totalA > totalB;  // descending — strongest first
}
bool Sorter::bySpeed(const Pokemon* a, const Pokemon* b) {
    return a->speed > b->speed;  // descending
}
bool Sorter::byHp(const Pokemon* a, const Pokemon* b) {
    return a->hp > b->hp;  // descending
}


// Benchmark: run all three, print table

void Sorter::benchmark(
    const std::vector<const Pokemon*>& pool,
    std::function<bool(const Pokemon*, const Pokemon*)> cmp,
    const std::string& sortLabel) const
{
    std::cout << "\n=== BENCHMARK: Sorting by " << sortLabel
              << " (" << pool.size() << " Pokémon) ===\n\n";
    std::cout << std::left
              << std::setw(28) << "Algorithm"
              << std::setw(14) << "Time (µs)"
              << "First result\n";
    std::cout << std::string(55, '-') << "\n";

    auto print = [](const SortResult& r) {
        std::cout << std::left
                  << std::setw(28) << r.algorithm
                  << std::setw(14) << r.timeUs
                  << (r.sorted.empty() ? "none" : r.sorted[0]->name) << "\n";
    };

    print(stlSort(pool, cmp));
    print(mergeSort(pool, cmp));
    print(insertionSort(pool, cmp));
    std::cout << "\n";
}
