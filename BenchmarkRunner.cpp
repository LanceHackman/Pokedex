#include "BenchmarkRunner.h"
#include "CandidateHeap.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <chrono>
#include <numeric>
#include <algorithm>
#include <functional>

#include <sys/resource.h>


// Constructor

BenchmarkRunner::BenchmarkRunner(const Pokedex& dex, const TypeGraph& graph)
    : dex(dex), graph(graph) {}


// Public: run all suites

void BenchmarkRunner::runAll(const std::string& reportPath) {
    outFile.open(reportPath);
    if (!outFile.is_open()) {
        std::cerr << "  Warning: could not open " << reportPath
                  << " — printing to terminal only.\n";
    }

    // Timestamp header
    auto now = std::chrono::system_clock::now();
    auto t   = std::chrono::system_clock::to_time_t(now);
    std::string ts = std::ctime(&t);
    ts.pop_back(); // remove trailing newline

    write("\n");
    write("╔══════════════════════════════════════════════════════════╗\n");
    write("║        POKÉDEX — FULL PERFORMANCE BENCHMARK REPORT       ║\n");
    write("╠══════════════════════════════════════════════════════════╣\n");
    write("║  Generated: " + ts + "  ║\n");
    write("║  Dataset:   " + std::to_string(dex.size()) +
          " Pokémon loaded from cache" +
          std::string(15, ' ') + "║\n");
    write("╚══════════════════════════════════════════════════════════╝\n\n");

    benchmarkPokedexLookup();
    benchmarkTypeGraph();
    benchmarkCandidateHeap();
    benchmarkSorting();
    benchmarkTeamBuilder();
    benchmarkTeamOptimizer();

    printSummaryTable();

    write("\n  Report saved to: " + reportPath + "\n\n");

    if (outFile.is_open()) outFile.close();
}


// SUITE 1 — Pokédex Lookup

void BenchmarkRunner::benchmarkPokedexLookup() {
    printHeader("1. POKÉDEX LOOKUP  (unordered_map vs linear scan)");

    // Warm up
    dex.find("charizard");

    //  A: Hash map lookup by name
    auto [timeA, tpsA] = measureThroughput([&]() {
        dex.find("charizard");
        dex.find("mewtwo");
        dex.find("rayquaza");
    }, 10000);

    addResult({
        "Pokédex Lookup", "unordered_map (hash)",
        "Name lookup × 3 per call",
        timeA / 3, tpsA * 3,
        measureMemoryDelta([&]() { dex.find("charizard"); }),
        "O(1) avg", "Hash map with separate chaining"
    });

    //  B: Linear scan simulation (what a vector would cost)
    auto pool = dex.all();
    auto [timeB, tpsB] = measureThroughput([&]() {
        for (const Pokemon* p : pool) {
            if (p->name == "charizard") break;
        }
    }, 1000);

    addResult({
        "Pokédex Lookup", "Linear scan (vector)",
        "Scan until name match",
        timeB, tpsB,
        0,
        "O(n)", "Simulates worst case of unsorted vector"
    });

    write("  to Hash map is ~" +
          std::to_string((int)(timeB / std::max(timeA / 3, 1LL))) +
          "× faster than linear scan for name lookup.\n\n");
}


// SUITE 2 — TypeGraph

void BenchmarkRunner::benchmarkTypeGraph() {
    printHeader("2. TYPEGRAPH  (adjacency matrix vs map-of-maps)");

    //  A: 2D array lookup
    auto [timeA, tpsA] = measureThroughput([&]() {
        graph.effectiveness("fire",  "grass");
        graph.effectiveness("water", "fire");
        graph.effectiveness("ghost", "normal");
    }, 100000);

    addResult({
        "TypeGraph", "2D array [18][18]",
        "Single type effectiveness call",
        timeA / 3, tpsA * 3,
        sizeof(float) * 18 * 18,
        "O(1)", "Stack-allocated, cache-friendly"
    });

    //  B: Simulate map-of-maps overhead
    std::unordered_map<std::string,
        std::unordered_map<std::string, float>> mapChart;
    // Build the same data in a map
    const std::vector<std::string> types = {
        "normal","fire","water","electric","grass","ice",
        "fighting","poison","ground","flying","psychic","bug",
        "rock","ghost","dragon","dark","steel","fairy"
    };
    for (const auto& a : types)
        for (const auto& b : types)
            mapChart[a][b] = graph.effectiveness(a, b);

    auto [timeB, tpsB] = measureThroughput([&]() {
        mapChart.at("fire").at("grass");
        mapChart.at("water").at("fire");
        mapChart.at("ghost").at("normal");
    }, 100000);

    addResult({
        "TypeGraph", "unordered_map<string, map>",
        "Single type effectiveness call",
        timeB / 3, tpsB * 3,
        measureMemoryDelta([&](){
            std::unordered_map<std::string,
                std::unordered_map<std::string,float>> tmp;
            for (const auto& a : types)
                for (const auto& b : types)
                    tmp[a][b] = 1.0f;
        }),
        "O(1) avg", "Two hash lookups + heap allocation per call"
    });

    write("  to Array is ~" +
          std::to_string((int)((timeB / 3) / std::max(timeA / 3, 1LL))) +
          "× faster than map-of-maps.\n\n");
}


// SUITE 3 — CandidateHeap

void BenchmarkRunner::benchmarkCandidateHeap() {
    printHeader("3. CANDIDATE HEAP  (priority_queue vs sorted vector)");

    auto pool        = dex.all();
    auto enemyTypes  = std::vector<std::string>{"fire", "water", "grass"};

    //  A: Heap build + pop
    auto [timeA, tpsA] = measureThroughput([&]() {
        CandidateHeap heap;
        heap.build(pool, enemyTypes, graph);
        heap.popBest();
    }, 10);

    addResult({
        "CandidateHeap", "priority_queue (max-heap)",
        "Build heap from 1025 Pokémon + 1 pop",
        timeA, tpsA,
        measureMemoryDelta([&](){
            CandidateHeap h;
            h.build(pool, enemyTypes, graph);
        }),
        "O(n log n) build, O(log n) pop",
        "Each pop surfaces best-scoring Pokémon"
    });

    //  B: Full sort to find best
    auto [timeB, tpsB] = measureThroughput([&]() {
        std::vector<std::pair<float, const Pokemon*>> scored;
        scored.reserve(pool.size());
        for (const Pokemon* p : pool) {
            float s = graph.offensiveScore(p->types, enemyTypes);
            scored.push_back({s, p});
        }
        std::sort(scored.begin(), scored.end(),
                  [](const auto& a, const auto& b){ return a.first > b.first; });
    }, 10);

    addResult({
        "CandidateHeap", "Full sort (vector + std::sort)",
        "Score all + sort to find best",
        timeB, tpsB,
        0,
        "O(n log n)", "Sorts entire pool even if only top-k needed"
    });

    write("  to Heap and sort have similar complexity; heap wins when\n"
          "    only top-k results are needed (no wasted comparisons).\n\n");
}


// SUITE 4 — Sorting

void BenchmarkRunner::benchmarkSorting() {
    printHeader("4. SORTING  (STL introsort vs merge sort vs insertion sort)");

    Sorter sorter;
    auto pool = dex.all();

    struct SortCase {
        std::string label;
        std::function<bool(const Pokemon*, const Pokemon*)> cmp;
    };
    std::vector<SortCase> cases = {
        {"Base Stat Total", Sorter::byTotalStats},
        {"Speed",           Sorter::bySpeed},
        {"Name (A-Z)",      Sorter::byName},
    };

    for (const auto& sc : cases) {
        write("  Criterion: " + sc.label + "\n");

        // STL
        auto [tSTL, throughSTL] = measureThroughput(
            [&](){ sorter.stlSort(pool, sc.cmp); }, 20);
        addResult({
            "Sorting", "STL introsort",
            "Sort " + std::to_string(pool.size()) + " Pokémon by " + sc.label,
            tSTL, throughSTL,
            0, "O(n log n)", "Hybrid quicksort/heapsort/insertion"
        });

        // Merge sort
        auto [tMerge, throughMerge] = measureThroughput(
            [&](){ sorter.mergeSort(pool, sc.cmp); }, 20);
        addResult({
            "Sorting", "Merge sort",
            "Sort " + std::to_string(pool.size()) + " Pokémon by " + sc.label,
            tMerge, throughMerge,
            0, "O(n log n)", "Stable, recursive divide-and-conquer"
        });

        // Insertion sort — small sample only (n²)
        auto smallPool = std::vector<const Pokemon*>(pool.begin(),
                          pool.begin() + std::min((int)pool.size(), 200));
        auto [tIns, throughIns] = measureThroughput(
            [&](){ sorter.insertionSort(smallPool, sc.cmp); }, 20);
        addResult({
            "Sorting", "Insertion sort (n=200)",
            "Sort 200 Pokémon by " + sc.label,
            tIns, throughIns,
            0, "O(n²)", "Included to demonstrate quadratic scaling"
        });

        write("\n");
    }
}


// SUITE 5 — TeamBuilder

void BenchmarkRunner::benchmarkTeamBuilder() {
    printHeader("5. TEAM BUILDER  (heap-greedy vs sorted-vec vs brute force)");

    TeamBuilder builder(dex, graph);
    std::vector<std::string> enemy = {"charizard","gyarados","alakazam",
                                       "garchomp","gengar","ferrothorn"};

    // Heap greedy
    auto [tHeap, tpsHeap] = measureThroughput(
        [&](){ builder.buildCounterTeam(enemy); }, 5);
    addResult({
        "TeamBuilder", "Heap greedy",
        "Counter team for 6-Pokémon enemy",
        tHeap, tpsHeap,
        measureMemoryDelta([&](){ builder.buildCounterTeam(enemy); }),
        "O(kn log n)", "k=team size, rebuilds heap each round"
    });

    // Sorted vector greedy
    auto [tVec, tpsVec] = measureThroughput(
        [&](){ builder.buildCounterTeamSortedVec(enemy); }, 5);
    addResult({
        "TeamBuilder", "Sorted vector greedy",
        "Counter team for 6-Pokémon enemy",
        tVec, tpsVec,
        0,
        "O(kn log n)", "Full sort each round, worse cache behaviour"
    });

    // Brute force
    auto [tBrute, tpsBrute] = measureThroughput(
        [&](){ builder.buildCounterTeamBrute(enemy); }, 3);
    addResult({
        "TeamBuilder", "Brute force (n=150)",
        "Counter team from first 150 Pokémon",
        tBrute, tpsBrute,
        0,
        "O(nᵏ)", "Exhaustive — limited to 150 Pokémon"
    });

    write("  Enemy team: charizard / gyarados / alakazam / "
          "garchomp / gengar / ferrothorn\n\n");
}


// SUITE 6 — TeamOptimizer

void BenchmarkRunner::benchmarkTeamOptimizer() {
    printHeader("6. TEAM OPTIMIZER  (full pool vs generation-filtered)");

    TeamOptimizer optimizer(dex, graph);

    // Full pool
    auto [tAll, tpsAll] = measureThroughput(
        [&](){ optimizer.buildStrongTeam(6); }, 3);
    addResult({
        "TeamOptimizer", "All Pokémon (n=1025)",
        "Build strongest general team",
        tAll, tpsAll,
        measureMemoryDelta([&](){ optimizer.buildStrongTeam(6); }),
        "O(k·n)", "k rounds, each scores full pool"
    });

    // Gen 1 only
    auto [tGen1, tpsGen1] = measureThroughput(
        [&](){ optimizer.buildStrongTeam(6, 1); }, 10);
    addResult({
        "TeamOptimizer", "Gen 1 only (n=151)",
        "Build strongest Gen 1 team",
        tGen1, tpsGen1,
        0,
        "O(k·n)", "Same algorithm, smaller pool"
    });

    write("  to Filtering by generation reduces compute time "
          "proportionally to pool size.\n\n");
}


// Summary table

void BenchmarkRunner::printSummaryTable() {
    printHeader("SUMMARY TABLE");

    // Column widths
    const int wCat  = 16;
    const int wAlg  = 30;
    const int wTime = 12;
    const int wTps  = 16;
    const int wMem  = 14;
    const int wCmx  = 14;

    std::ostringstream hdr;
    hdr << "  " << std::left
        << std::setw(wCat)  << "Category"
        << std::setw(wAlg)  << "Algorithm"
        << std::setw(wTime) << "Avg µs"
        << std::setw(wTps)  << "Throughput/s"
        << std::setw(wMem)  << "Mem delta"
        << std::setw(wCmx)  << "Complexity"
        << "\n";
    write(hdr.str());
    write("  " + std::string(wCat+wAlg+wTime+wTps+wMem+wCmx, '-') + "\n");

    std::string lastCat;
    for (const auto& e : results) {
        if (e.category != lastCat) {
            lastCat = e.category;
        }
        std::ostringstream row;
        std::string memStr = (e.memoryBytes > 0)
            ? std::to_string(e.memoryBytes / 1024) + " KB"
            : "—";
        std::string tpsStr;
        if (e.throughput > 1e6)
            tpsStr = std::to_string((int)(e.throughput / 1e6)) + "M";
        else if (e.throughput > 1e3)
            tpsStr = std::to_string((int)(e.throughput / 1e3)) + "K";
        else
            tpsStr = std::to_string((int)e.throughput);

        row << "  " << std::left
            << std::setw(wCat)  << e.category
            << std::setw(wAlg)  << e.algorithm.substr(0, wAlg-1)
            << std::setw(wTime) << e.timeUs
            << std::setw(wTps)  << tpsStr
            << std::setw(wMem)  << memStr
            << std::setw(wCmx)  << e.complexity
            << "\n";
        write(row.str());
    }
    write("\n");
}


// Output helpers

void BenchmarkRunner::write(const std::string& s) {
    std::cout << s;
    if (outFile.is_open()) outFile << s;
}

void BenchmarkRunner::printHeader(const std::string& title) {
    // Box interior is 53 chars wide ("  │  " prefix + title + padding + "│")
    // Cast to int before subtracting — size_t underflow causes length_error
    const int BOX_WIDTH = 53;
    int padding = BOX_WIDTH - static_cast<int>(title.size());
    if (padding < 0) padding = 0;  // truncate gracefully if title too long

    write("\n");
    write("  ┌───┐\n");
    write("  │  " + title + std::string(padding, ' ') + "│\n");
    write("  └───────┘\n\n");
}

void BenchmarkRunner::printEntry(const BenchmarkEntry& e) {
    std::ostringstream oss;
    oss << "  Algorithm : " << e.algorithm        << "\n"
        << "  Metric    : " << e.metric           << "\n"
        << "  Time      : " << e.timeUs           << " µs"
        << "  (" << std::fixed << std::setprecision(1)
        << e.throughput / 1000.0 << "K ops/sec)\n"
        << "  Memory    : " << (e.memoryBytes > 0
                                ? std::to_string(e.memoryBytes / 1024) + " KB delta"
                                : "not measured") << "\n"
        << "  Complexity: " << e.complexity       << "\n";
    if (!e.notes.empty())
        oss << "  Notes     : " << e.notes        << "\n";
    oss << "\n";
    write(oss.str());
}

void BenchmarkRunner::addResult(BenchmarkEntry e) {
    printEntry(e);
    results.push_back(std::move(e));
}


// Memory measurement (macOS / Linux)

long long BenchmarkRunner::currentMemoryBytes() {
    struct rusage usage;
    getrusage(RUSAGE_SELF, &usage);
#ifdef __APPLE__
    // macOS: ru_maxrss is in bytes
    return static_cast<long long>(usage.ru_maxrss);
#else
    // Linux: ru_maxrss is in kilobytes
    return static_cast<long long>(usage.ru_maxrss) * 1024LL;
#endif
}

long long BenchmarkRunner::measureMemoryDelta(std::function<void()> fn) {
    long long before = currentMemoryBytes();
    fn();
    long long after  = currentMemoryBytes();
    return std::max(0LL, after - before);
}


// Throughput measurement

std::pair<long long, double> BenchmarkRunner::measureThroughput(
    std::function<void()> fn, int iterations)
{
    // Warm up
    fn();

    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < iterations; i++) fn();
    auto end   = std::chrono::high_resolution_clock::now();

    long long totalUs = std::chrono::duration_cast<std::chrono::microseconds>(
                            end - start).count();
    long long avgUs   = totalUs / iterations;
    double    tps     = (avgUs > 0)
                        ? (1'000'000.0 / avgUs)
                        : 0.0;

    return { avgUs, tps };
}