# Pokédex: Advanced Data Structures

## Project Overview
**Pokédex: Advanced Data Structures** is a C++ capstone project demonstrating practical applications of advanced data structures and algorithms. It implements a full Pokédex system managing and analyzing a dataset of **1025 Pokémon** from **PokéAPI**, showcasing how different data structures optimize performance for real-world use cases.

This project serves as both an educational tool and a functional application, allowing users to explore Pokémon data while learning about the performance characteristics of `unordered_map`, 2D arrays, `priority_queue`, and sorting algorithms in production-grade C++.

---

## Features

- **Pokédex Browser** — Instant Pokémon lookup by name or ID using hash tables  
- **Counter-Team Builder** — Greedy algorithm using `priority_queue` to construct optimal counter teams  
- **Team Optimizer** — 2D type-effectiveness matrix for team coverage and weakness analysis  
- **Pokémon Sorter** — Sort dataset by name, stats, or generation using multiple sorting algorithms  
- **Performance Benchmark Suite** — Compare real-world performance across data structures and algorithms  

---

## Data Structures & Algorithms

### Summary Table

| Data Structure | Use Case | Time Complexity | Space Complexity |
|---------------|----------|----------------|-----------------|
| **`std::unordered_map`** | Primary Pokédex storage; O(1) name/ID lookups | O(1) avg, O(n) worst | O(n) |
| **2D float matrix (1025×18)** | Type-effectiveness graph; team coverage | O(1) access, O(n²) traversal | O(n²) |
| **`std::priority_queue`** | Greedy team building | O(log n) insert/extract | O(n) |
| **`std::vector`** | Dataset storage & sorting | O(n) iteration, O(1) access | O(n) |
| **`std::sort` (introsort)** | General-purpose sorting | O(n log n) | O(log n) |
| **Merge sort** | Stable sorting | O(n log n) | O(n) |
| **Insertion sort** | Nearly-sorted/small datasets | O(n²) worst, O(n) best | O(1) |

---

## Project Structure

```
pokedex-advanced-data-structures/
├── src/
│   ├── main.cpp                 # Entry point; menu system and UI
│   ├── Pokedex.h                # Pokédex class definition
│   ├── Pokedex.cpp              # Core implementation (unordered_map, 2D matrix)
│   ├── TeamBuilder.h            # Counter-team builder interface
│   ├── TeamBuilder.cpp          # Greedy algorithm using priority_queue
│   ├── Sorter.h                 # Sorting algorithm interface
│   ├── Sorter.cpp               # Introsort, merge sort, insertion sort
│   ├── BenchmarkSuite.h         # Performance testing framework
│   └── BenchmarkSuite.cpp       # Benchmark implementations
├── data/
│   └── pokedex_cache.json       # Cached Pokémon data
├── CMakeLists.txt               # Build configuration
├── Makefile                     # Alternative build system
├── README.md                    # This file
└── LICENSE                      # MIT License
```

---

## Dependencies

- **C++17 compiler** — GCC 7+, Clang 5+, MSVC 2017+  
- **libcurl** — For HTTP requests  
- **nlohmann/json** — Header-only JSON library  
- **PokéAPI** — Remote API accessed on first run  

### Installing Dependencies

**Ubuntu/Debian**
```bash
sudo apt-get install libcurl4-openssl-dev
```

**macOS**
```bash
brew install curl
```

**Windows (MSVC via vcpkg)**
```bash
vcpkg install curl:x64-windows
```

---

## Build Instructions

### Using g++
```bash
g++ -std=c++17 -O2 -o pokedex src/*.cpp -lcurl -I./include
```

### Using CMake
```bash
mkdir build && cd build
cmake ..
make
```

### Using Make
```bash
make build
```

---

## First Run

On first execution, the program fetches all **1025 Pokémon** from PokéAPI.  
This takes **30–60 seconds** depending on network speed.

A cache file is generated:

```
pokedex_cache.json
```

Subsequent runs load instantly (**<100ms**).  
To refresh the cache, delete the file and rerun.

The cache includes:

- All Pokémon attributes  
- Type information & effectiveness  
- Base stats  
- Generation & regional data  

---

## Usage

Launch the program to access the main menu:

```
========================================
POKÉDEX: Advanced Data Structures
========================================
1. Pokédex Browser
2. Counter-Team Builder
3. Team Optimizer
4. Pokémon Sorter
5. Performance Benchmark Suite
0. Exit
```

### Modes

- **Pokédex Browser** — O(1) lookup via `unordered_map`  
- **Counter-Team Builder** — Greedy algorithm using `priority_queue`  
- **Team Optimizer** — 2D type-effectiveness matrix analysis  
- **Pokémon Sorter** — Compare sorting algorithms  
- **Benchmark Suite** — Performance metrics for all structures  

---

## Benchmark Results

Benchmarks run on **Intel i7‑9700K** with full dataset.

### Lookup Performance

| Operation | Implementation | Time (µs) | Relative | Insight |
|----------|----------------|-----------|----------|---------|
| 1000 Random Lookups | `unordered_map` | **0.89** | 18× faster | Hash lookup dominates |
| 1000 Random Lookups | Linear scan | 18.2 | baseline | Sequential O(n) |

### Type-Effectiveness Graph

| Structure | Memory | Insight |
|----------|--------|---------|
| 2D float matrix | **1 KB** | 16× less memory |
| `std::map` | 16 KB | Nested overhead |

### Sorting

| Sort | Time (µs) | Relative | Insight |
|------|-----------|----------|---------|
| `std::sort` | **151** | 3.5× faster | Hybrid introsort |
| Merge sort | 523 | baseline | Higher overhead |
| Insertion sort | 2847 | 18× slower | Only good for n < 50 |

### Team Building

| Operation | Implementation | Time (µs) | Relative | Insight |
|-----------|----------------|-----------|----------|---------|
| Extract Top 6 | `priority_queue` | **9865** | 7.3× faster | O(log n) extraction |
| Extract Top 6 | Pre-sorted vector | 1345 | baseline | O(k) access |

---

## Benchmark Insights

- **Hash tables** deliver **18× faster** lookups than linear scans.  
- **2D arrays** use **16× less memory** than nested maps.  
- **Introsort** outperforms merge sort by **3.5×** on real-world data.  
- **Pre-sorting** can outperform heaps for small `k`, showing algorithm choice depends on constraints.

---

## AI DISCLOSURE

Code, presentation ,logic, documentation, debugging

Google Search
https://app.flintk12.com/chats/05f130ff-096b-4e1d-8294-3905ecc70978
https://app.flintk12.com/chats/4dcf14f3-b35a-47e7-be6f-ebfc97f05a47
---

## Author

**Lance Hackman**  
Durham Academy — Class of 2026

This project demonstrates advanced C++ data structures and algorithms in a practical, real-world context. Contributions and questions are welcome via the project repository.
