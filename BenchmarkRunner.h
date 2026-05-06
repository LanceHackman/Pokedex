//
// Created by Lance Hackman on 5/5/26.
//

#ifndef POKEDEX_BENCHMARKRUNNER_H
#define POKEDEX_BENCHMARKRUNNER_H


#pragma once
#include <string>
#include <vector>
#include <fstream>
#include "Pokedex.h"
#include "TypeGraph.h"
#include "Sorter.h"
#include "TeamBuilder.h"
#include "TeamOptimizer.h"

//  One benchmark result row
struct BenchmarkEntry {
    std::string category;       // e.g. "Sorting"
    std::string algorithm;      // e.g. "STL introsort"
    std::string metric;         // e.g. "Sort 1025 Pokémon by BST"
    long long   timeUs;         // microseconds
    double      throughput;     // operations per second
    long long   memoryBytes;    // peak memory delta in bytes
    std::string complexity;     // Big O string
    std::string notes;
};

class BenchmarkRunner {
public:
    explicit BenchmarkRunner(const Pokedex& dex, const TypeGraph& graph);

    // Run all benchmarks and write results to terminal + file
    void runAll(const std::string& reportPath = "benchmark_report.txt");

private:
    const Pokedex&   dex;
    const TypeGraph& graph;

    std::vector<BenchmarkEntry> results;
    std::ofstream               outFile;

    //  Individual benchmark suites ────────────
    void benchmarkSorting();
    void benchmarkPokedexLookup();
    void benchmarkTypeGraph();
    void benchmarkTeamBuilder();
    void benchmarkTeamOptimizer();
    void benchmarkCandidateHeap();

    // ── Output helpers
    void printHeader(const std::string& title);
    void printEntry(const BenchmarkEntry& e);
    void printSummaryTable();
    void write(const std::string& s);   // write to both terminal and file

    // ── Memory helpers
    long long currentMemoryBytes();     // current RSS in bytes (macOS/Linux)
    long long measureMemoryDelta(std::function<void()> fn); // peak delta

    // ── Throughput helper ────────────────────────────────────────
    // Runs fn N times, returns avg µs and operations/sec
    std::pair<long long, double> measureThroughput(
        std::function<void()> fn, int iterations = 100);

    void addResult(BenchmarkEntry e);
};


#endif //POKEDEX_BENCHMARKRUNNER_H