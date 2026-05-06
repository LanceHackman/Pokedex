Used flint for code and presentation assistance: https://app.flintk12.com/chats/05f130ff-096b-4e1d-8294-3905ecc70978

README — Pokédex: Advanced Data Structures
Project Overview
Pokédex: Advanced Data Structures is a C++ capstone project that demonstrates practical applications of advanced data structures and algorithms. The project implements a comprehensive Pokédex system that manages and analyzes a dataset of 1025 Pokémon from PokéAPI, showcasing how different data structures optimize performance for real-world use cases.
This project serves as both an educational tool and a functional application, allowing users to explore Pokémon data while learning about the performance characteristics of unordered maps, 2D arrays, priority queues, and sorting algorithms in production-grade C++ code.
Features

Pokédex Browser — Search and retrieve individual Pokémon by name or ID with instant lookups using hash tables
Counter-Team Builder — Construct optimal battle teams using a greedy algorithm with priority queues to maximize type coverage and effectiveness
Team Optimizer — Analyze team compositions using a 2D type-effectiveness matrix to calculate coverage and identify weaknesses
Pokémon Sorter — Sort the entire dataset by various attributes (name, base stats, generation) using multiple sorting algorithms
Performance Benchmark Suite — Compare real-world performance metrics across different data structures and algorithms with detailed timing analysis

Data Structures & Algorithms

Data Structure
Use Case
Time Complexity
Space Complexity

std::unordered_map
Primary Pokédex storage; O(1) name/ID lookups
O(1) average, O(n) worst
O(n)

2D float matrix (1025×18)
Type-effectiveness graph; team coverage analysis
O(1) access, O(n²) traversal
O(n²)

std::priority_queue
Greedy team building; heap-based selection
O(log n) insertion/extraction
O(n)

std::vector
Dataset storage and sorting operations
O(n) iteration, O(1) indexed access
O(n)

std::sort (introsort)
General-purpose sorting; optimal for mixed data
O(n log n) average, O(n log n) worst
O(log n)

Merge sort
Stable sorting for consistent ordering
O(n log n) guaranteed
O(n)

Insertion sort
Small dataset optimization; nearly-sorted data
O(n²) worst, O(n) best
O(1)

Project Structure
pokedex-advanced-data-structures/
├── src/
│   ├── main.cpp                 # Entry point; menu system and user interface
│   ├── Pokedex.h                # Pokédex class definition with data structure declarations
│   ├── Pokedex.cpp              # Core Pokédex implementation (unordered_map, 2D matrix)
│   ├── TeamBuilder.h             # Counter-team builder interface
│   ├── TeamBuilder.cpp           # Greedy algorithm using priority_queue
│   ├── Sorter.h                  # Sorting algorithm interface
│   ├── Sorter.cpp                # Multiple sort implementations (introsort, merge, insertion)
│   ├── BenchmarkSuite.h          # Performance testing framework
│   └── BenchmarkSuite.cpp        # Benchmark implementations and timing utilities
├── data/
│   └── pokedex_cache.json        # Cached Pokémon data (generated on first run)
├── CMakeLists.txt                # Build configuration
├── Makefile                       # Alternative build system
├── README.md                      # This file
└── LICENSE                        # MIT License
Dependencies

C++17 Compiler — GCC 7+, Clang 5+, or MSVC 2017+
libcurl — For HTTP requests to PokéAPI (development headers required)
nlohmann/json — JSON parsing library (header-only, included)
PokéAPI — Remote API accessed on first run; requires internet connection

Installing Dependencies
Ubuntu/Debian:
sudo apt-get install libcurl4-openssl-dev
macOS:
brew install curl
Windows (MSVC):
Use vcpkg: vcpkg install curl:x64-windows
Build Instructions
Using g++ (Command Line):
g++ -std=c++17 -O2 -o pokedex src/*.cpp -lcurl -I./include
Using CMake:
mkdir build && cd build
cmake ..
make
Using Make:
make build
First Run
On initial execution, the program fetches all 1025 Pokémon from PokéAPI. This process typically takes 30-60 seconds depending on network speed and system performance. The data is automatically cached to pokedex_cache.json in the project root directory.
Subsequent runs load from the local cache instantly (typically <100ms), providing immediate access to the full dataset without network overhead. To refresh the cache, simply delete pokedex_cache.json and run the program again.
The cache file contains:

All 1025 Pokémon records with complete attributes
Type information and effectiveness relationships
Base statistics (HP, Attack, Defense, Sp. Atk, Sp. Def, Speed)
Generation and regional classification data

Usage
Launch the program and interact with the main menu:
========================================
POKÉDEX: Advanced Data Structures
========================================
1. Pokédex Browser
2. Counter-Team Builder
3. Team Optimizer
4. Pokémon Sorter
5. Performance Benchmark Suite
0. Exit

Select an option:
Pokédex Browser: Enter a Pokémon name or ID to retrieve complete information. Uses unordered_map for O(1) average-case lookup.
Counter-Team Builder: Specify a target Pokémon type, and the system builds a 6-Pokémon team optimized for coverage using a greedy algorithm with priority queues.
Team Optimizer: Analyzes your custom team using the 2D type-effectiveness matrix to show coverage, weaknesses, and resistances.
Pokémon Sorter: Choose a sorting attribute and algorithm to sort the entire dataset. Compare execution times across different sorting implementations.
Performance Benchmark Suite: Run comprehensive benchmarks comparing data structures and algorithms with detailed timing metrics.
Benchmark Results
The following benchmark results were obtained on a system with Intel i7-9700K CPU running the complete 1025-Pokémon dataset:

Operation
Implementation
Time (µs)
Relative Performance
Key Insight

1000 Random Lookups
unordered_map (hash table)
0.89
18× faster
O(1) hash lookups dominate

Linear scan (vector)
18.2
baseline
O(n) sequential search

Type-Effectiveness Graph
2D float matrix (1025×18)
—
16× less memory
1 KB vs 16 KB for map-of-maps

std::map
—
baseline
Nested structure overhead

Sort 1025 Pokémon by Name
std::sort (introsort)
151
3.5× faster
Hybrid algorithm optimal for real data

Merge sort (stable)
523
baseline
Guaranteed O(n log n), higher overhead

Insertion sort (small sets)
2847
18× slower
Only efficient for n < 50</td>

Extract Top 6 from Heap
priority_queue (heap extraction)
9865
7.3× faster
O(log n) per extraction; optimal for k-selection

Pre-sorted vector (linear scan)
1345
baseline
One-time O(n log n) sort, then O(k) access

Greedy Team Building (Full Coverage)
Priority queue approach
71155
1.02× slower
Maintains heap invariant; more flexible

Sorted vector greedy
69886
baseline
Pre-computed ordering; minimal overhead

Benchmark Insights
Hash tables (unordered_map) provide dramatic speedups for lookup-heavy workloads, achieving 18× performance gains over linear scans. This demonstrates why hash-based data structures are fundamental to database design.
2D arrays consume 16× less memory than nested maps for the type-effectiveness graph, highlighting the importance of choosing appropriate data structures for dense, multi-dimensional data.
Introsort (std::sort) outperforms pure merge sort by 3.5× through adaptive hybrid switching, validating modern STL design choices for general-purpose sorting.
Pre-sorting followed by linear selection outperforms heap extraction for small k values, suggesting that algorithmic choice depends heavily on problem parameters.
Author
Lance Hackman Durham Academy Class of 2026

This project demonstrates advanced C++ data structures and algorithms in a practical, real-world application context. For questions or contributions, please refer to the project repository.
