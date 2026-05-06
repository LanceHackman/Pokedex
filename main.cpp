#include <iostream>
#include <string>
#include <vector>
#include <limits>
#include <algorithm>
#include <thread>
#include <chrono>

#include "Pokemon.h"
#include "ApiClient.h"
#include "CacheManager.h"
#include "Pokedex.h"
#include "TypeGraph.h"
#include "CandidateHeap.h"
#include "TeamBuilder.h"
#include "Sorter.h"
#include "TeamOptimizer.h"
#include "BenchmarkRunner.h"


// Helper: clear screen

void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}


// Helper: print a divider

void divider(char c = '-', int width = 55) {
    std::cout << std::string(width, c) << "\n";
}


// Helper: read a non-empty trimmed string

std::string readString(const std::string& prompt) {
    std::string input;
    while (true) {
        std::cout << prompt;
        std::getline(std::cin, input);
        // trim leading/trailing whitespace
        size_t start = input.find_first_not_of(" \t");
        size_t end   = input.find_last_not_of(" \t");
        if (start != std::string::npos) {
            input = input.substr(start, end - start + 1);
            // lowercase
            std::transform(input.begin(), input.end(), input.begin(), ::tolower);
            return input;
        }
        std::cout << "  Input cannot be empty. Try again.\n";
    }
}


// Helper: read an integer in [lo, hi]

int readInt(const std::string& prompt, int lo, int hi) {
    int val;
    while (true) {
        std::cout << prompt;
        if (std::cin >> val && val >= lo && val <= hi) {
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            return val;
        }
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "  Invalid input. Enter a number between "
                  << lo << " and " << hi << ".\n";
    }
}


// Helper: read a list of Pokémon names

std::vector<std::string> readPokemonList(const std::string& prompt,
                                          int count,
                                          const Pokedex& dex) {
    std::vector<std::string> names;
    std::cout << prompt << "\n";
    while ((int)names.size() < count) {
        std::string name = readString(
            "  Pokémon " + std::to_string(names.size() + 1) + ": ");
        if (!dex.contains(name)) {
            std::cout << "  '" << name << "' not found in Pokédex. Check spelling.\n";
        } else {
            names.push_back(name);
        }
    }
    return names;
}


// Helper: press enter to continue

void pressEnter() {
    std::cout << "\n  Press Enter to continue...";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}


// FEATURE 1 — Pokédex Browser

void runPokedex(const Pokedex& dex) {
    while (true) {
        clearScreen();
        divider('=');
        std::cout << "  POKÉDEX BROWSER\n";
        divider('=');
        std::cout << "  1. Search by name\n";
        std::cout << "  2. Search by ID\n";
        std::cout << "  3. Browse by type\n";
        std::cout << "  4. Browse by generation\n";
        std::cout << "  0. Back to main menu\n";
        divider();

        int choice = readInt("  Choice: ", 0, 4);

        if (choice == 0) return;

        if (choice == 1) {
            std::string name = readString("  Enter Pokémon name: ");
            dex.display(name);
            pressEnter();
        }
        else if (choice == 2) {
            int id = readInt("  Enter Pokémon ID (1-1025): ", 1, 1025);
            const Pokemon* p = dex.findById(id);
            if (!p) std::cout << "  No Pokémon found with ID " << id << "\n";
            else    dex.display(p->name);
            pressEnter();
        }
        else if (choice == 3) {
            const std::vector<std::string> types = {
                "normal","fire","water","electric","grass","ice",
                "fighting","poison","ground","flying","psychic","bug",
                "rock","ghost","dragon","dark","steel","fairy"
            };
            std::cout << "\n  Types:\n";
            for (int i = 0; i < (int)types.size(); i++)
                std::cout << "  " << (i+1) << ". " << types[i] << "\n";
            divider();
            int t = readInt("  Select type (1-18): ", 1, 18);
            auto results = dex.byType(types[t-1]);

            // Sort results by ID for clean display
            std::sort(results.begin(), results.end(),
                      [](const Pokemon* a, const Pokemon* b){ return a->id < b->id; });

            std::cout << "\n  " << results.size() << " Pokémon of type ["
                      << types[t-1] << "]:\n\n";
            for (const Pokemon* p : results) dex.displayCompact(*p);
            pressEnter();
        }
        else if (choice == 4) {
            int gen = readInt("  Enter generation (1-9): ", 1, 9);
            auto results = dex.byGeneration(gen);

            std::sort(results.begin(), results.end(),
                      [](const Pokemon* a, const Pokemon* b){ return a->id < b->id; });

            std::cout << "\n  " << results.size()
                      << " Pokémon in Generation " << gen << ":\n\n";
            for (const Pokemon* p : results) dex.displayCompact(*p);
            pressEnter();
        }
    }
}


// FEATURE 2 — Counter-Team Builder

void runCounterTeam(const Pokedex& dex, const TypeGraph& graph) {
    while (true) {
        clearScreen();
        divider('=');
        std::cout << "  COUNTER-TEAM BUILDER\n";
        divider('=');
        std::cout << "  1. Build counter team for an enemy team\n";
        std::cout << "  2. Run benchmark (heap vs sorted-vec vs brute force)\n";
        std::cout << "  0. Back to main menu\n";
        divider();

        int choice = readInt("  Choice: ", 0, 2);
        if (choice == 0) return;

        TeamBuilder builder(dex, graph);

        if (choice == 1) {
            int size = readInt("  How many Pokémon on the enemy team? (1-6): ", 1, 6);
            auto enemyNames = readPokemonList(
                "  Enter enemy Pokémon names:", size, dex);

            std::cout << "\n  Building counter team...\n";
            TeamResult result = builder.buildCounterTeam(enemyNames);
            builder.displayTeam(result);
            pressEnter();
        }
        else if (choice == 2) {
            int size = readInt("  Enemy team size for benchmark (1-6): ", 1, 6);
            auto enemyNames = readPokemonList(
                "  Enter enemy Pokémon names:", size, dex);

            std::cout << "\n  Running benchmark...\n\n";
            builder.benchmark(enemyNames);
            pressEnter();
        }
    }
}


// FEATURE 3 — Team Optimizer

void runTeamOptimizer(const Pokedex& dex, const TypeGraph& graph) {
    while (true) {
        clearScreen();
        divider('=');
        std::cout << "  TEAM OPTIMIZER\n";
        divider('=');
        std::cout << "  1. Build best general team (all Pokémon)\n";
        std::cout << "  2. Build best team from a specific generation\n";
        std::cout << "  0. Back to main menu\n";
        divider();

        int choice = readInt("  Choice: ", 0, 2);
        if (choice == 0) return;

        TeamOptimizer optimizer(dex, graph);

        if (choice == 1) {
            std::cout << "\n  Optimizing team across all 1025 Pokémon...\n";
            TeamResult result = optimizer.buildStrongTeam(6);
            optimizer.displayTeam(result);
            pressEnter();
        }
        else if (choice == 2) {
            int gen = readInt("  Enter generation (1-9): ", 1, 9);
            std::cout << "\n  Optimizing Gen " << gen << " team...\n";
            TeamResult result = optimizer.buildStrongTeam(6, gen);
            if (result.team.empty()) {
                std::cout << "  No Pokémon found for generation " << gen << ".\n";
            } else {
                optimizer.displayTeam(result);
            }
            pressEnter();
        }
    }
}


// FEATURE 4 — Pokémon Sorter

void runSorter(const Pokedex& dex) {
    while (true) {
        clearScreen();
        divider('=');
        std::cout << "  POKÉMON SORTER\n";
        divider('=');
        std::cout << "  Sort by:\n";
        std::cout << "  1. Base stat total (strongest first)\n";
        std::cout << "  2. Speed (fastest first)\n";
        std::cout << "  3. HP (tankiest first)\n";
        std::cout << "  4. Name (A-Z)\n";
        std::cout << "  5. ID (Pokédex order)\n";
        std::cout << "  6. Generation\n";
        std::cout << "  7. Run full benchmark (all criteria, all algorithms)\n";
        std::cout << "  0. Back to main menu\n";
        divider();

        int choice = readInt("  Choice: ", 0, 7);
        if (choice == 0) return;

        Sorter sorter;
        auto pool = dex.all();

        // Map choice to comparator and label
        using Cmp = std::function<bool(const Pokemon*, const Pokemon*)>;
        Cmp cmp;
        std::string label;

        if      (choice == 1) { cmp = Sorter::byTotalStats;  label = "Base Stat Total"; }
        else if (choice == 2) { cmp = Sorter::bySpeed;       label = "Speed"; }
        else if (choice == 3) { cmp = Sorter::byHp;          label = "HP"; }
        else if (choice == 4) { cmp = Sorter::byName;        label = "Name (A-Z)"; }
        else if (choice == 5) { cmp = Sorter::byId;          label = "ID"; }
        else if (choice == 6) { cmp = Sorter::byGeneration;  label = "Generation"; }

        if (choice >= 1 && choice <= 6) {
            // Show top 20 results
            SortResult result = sorter.stlSort(pool, cmp);

            std::cout << "\n  Top 20 Pokémon sorted by " << label << ":\n\n";
            for (int i = 0; i < std::min(20, (int)result.sorted.size()); i++) {
                std::cout << "  " << (i+1) << ". ";
                dex.displayCompact(*result.sorted[i]);
            }
            std::cout << "\n  Sorted in " << result.timeUs << " µs\n";
            pressEnter();
        }
        else if (choice == 7) {
            std::cout << "\n  Running full benchmark...\n\n";
            sorter.benchmark(pool, Sorter::byTotalStats,  "Base Stat Total");
            sorter.benchmark(pool, Sorter::bySpeed,       "Speed");
            sorter.benchmark(pool, Sorter::byName,        "Name");
            pressEnter();
        }
    }
}


// FEATURE 5 — Performance Benchmark Suite

void runBenchmarks(const Pokedex& dex, const TypeGraph& graph) {
    clearScreen();
    divider('=');
    std::cout << "  PERFORMANCE BENCHMARK SUITE\n";
    divider('=');
    std::cout << "  This runs all benchmark suites and produces a full\n";
    std::cout << "  performance report with throughput, latency, and\n";
    std::cout << "  memory metrics across all data structures.\n\n";
    std::cout << "  Output goes to both the terminal and a report file.\n";
    divider();
    std::cout << "  1. Run full benchmark suite\n";
    std::cout << "  2. Run full suite with custom report filename\n";
    std::cout << "  0. Back to main menu\n";
    divider();

    int choice = readInt("  Choice: ", 0, 2);
    if (choice == 0) return;

    std::string filename = "benchmark_report.txt";
    if (choice == 2) {
        filename = readString("  Enter filename (e.g. my_report.txt): ");
        if (filename.find(".txt") == std::string::npos)
            filename += ".txt";
    }

    std::cout << "\n  Starting benchmarks — this may take 30-60 seconds...\n\n";

    BenchmarkRunner runner(dex, graph);
    runner.runAll(filename);

    pressEnter();
}


// STARTUP: Load or fetch the Pokédex

std::unordered_map<std::string, Pokemon> loadPokedex() {
    if (CacheManager::cacheExists()) {
        std::cout << "  Loading Pokédex from cache...\n";
        return CacheManager::loadFromFile();
    }

    std::cout << "  No cache found. Downloading all 1025 Pokémon from PokéAPI.\n";
    std::cout << "  This takes about 60 seconds and only happens once.\n\n";

    ApiClient api;
    std::unordered_map<std::string, Pokemon> pokedex;

    for (int id = 1; id <= 1025; id++) {
        try {
            Pokemon p = api.fetchById(id);
            pokedex[p.name] = p;
            if (id % 100 == 0)
                std::cout << "  Fetched " << id << "/1025...\n";
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        } catch (const std::exception& e) {
            std::cerr << "  Skipping id " << id << ": " << e.what() << "\n";
        }
    }

    CacheManager::saveToFile(pokedex);
    std::cout << "  Pokédex cached! Future runs will load instantly.\n\n";
    return pokedex;
}


// MAIN

int main() {
    clearScreen();
    divider('=');
    std::cout << "  POKÉDEX — Advanced Data Structures Capstone\n";
    divider('=');
    std::cout << "\n";

    //  Load data
    auto rawData = loadPokedex();
    Pokedex  dex(std::move(rawData));
    TypeGraph graph;

    std::cout << "\n  Pokédex ready: " << dex.size() << " Pokémon loaded.\n";
    pressEnter();

    //  Main menu loop
    while (true) {
        clearScreen();
        divider('=');
        std::cout << "  MAIN MENU\n";
        divider('=');
        std::cout << "  1. Pokédex Browser\n";
        std::cout << "  2. Counter-Team Builder\n";
        std::cout << "  3. Team Optimizer\n";
        std::cout << "  4. Pokémon Sorter\n";
        std::cout << "  5. Performance Benchmark Suite\n";
        divider();
        std::cout << "  0. Quit\n";
        divider();

        int choice = readInt("  Choice: ", 0, 5);

        switch (choice) {
            case 0:
                clearScreen();
                std::cout << "  Goodbye!\n\n";
                return 0;
            case 1: runPokedex(dex);                  break;
            case 2: runCounterTeam(dex, graph);       break;
            case 3: runTeamOptimizer(dex, graph);     break;
            case 4: runSorter(dex);                   break;
            case 5: runBenchmarks(dex, graph);        break;
        }
    }
}