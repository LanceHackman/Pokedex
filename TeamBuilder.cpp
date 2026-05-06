#include "TeamBuilder.h"
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <chrono>
#include <stdexcept>


// Constructor

TeamBuilder::TeamBuilder(const Pokedex& dex, const TypeGraph& graph)
    : dex(dex), graph(graph) {}


// Core: greedy counter-team builder
// O(n log n) — heap pop is O(log n), done teamSize times

TeamResult TeamBuilder::buildCounterTeam(
    const std::vector<std::string>& enemyNames, int teamSize) const
{
    auto start = std::chrono::high_resolution_clock::now();

    TeamResult result;
    std::vector<std::string> remainingTypes = extractTypes(enemyNames);
    std::vector<const Pokemon*> pool        = dex.all();

    for (int pick = 0; pick < teamSize && !remainingTypes.empty(); pick++) {
        // Build a fresh heap scored against remaining uncovered types
        CandidateHeap heap;
        heap.build(pool, remainingTypes, graph);

        // Skip Pokémon already on the team
        Candidate best = heap.popBest();
        while (!heap.empty()) {
            bool alreadyPicked = false;
            for (const auto* p : result.team) {
                if (p->name == best.pokemon->name) {
                    alreadyPicked = true;
                    break;
                }
            }
            if (!alreadyPicked) break;
            best = heap.popBest();
        }

        result.team.push_back(best.pokemon);
        result.scores.push_back(best.score);
        result.totalScore += best.score;

        // Remove types that this pick covers (hit super effectively)
        remainingTypes.erase(
            std::remove_if(remainingTypes.begin(), remainingTypes.end(),
                [&](const std::string& t) {
                    return graph.offensiveScore(best.pokemon->types, {t}) >= 2.0f;
                }),
            remainingTypes.end()
        );
    }

    // Fill remaining slots if we ran out of uncovered types early
    if ((int)result.team.size() < teamSize) {
        CandidateHeap heap;
        heap.build(pool, extractTypes(enemyNames), graph);
        while ((int)result.team.size() < teamSize && !heap.empty()) {
            Candidate c = heap.popBest();
            bool alreadyPicked = false;
            for (const auto* p : result.team)
                if (p->name == c.pokemon->name) { alreadyPicked = true; break; }
            if (!alreadyPicked) {
                result.team.push_back(c.pokemon);
                result.scores.push_back(c.score);
                result.totalScore += c.score;
            }
        }
    }

    auto end = std::chrono::high_resolution_clock::now();
    result.computeTimeMs =
        std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();

    return result;
}


// Benchmark: sorted vector approach
// O(n log n) same complexity but worse cache performance than heap

TeamResult TeamBuilder::buildCounterTeamSortedVec(
    const std::vector<std::string>& enemyNames, int teamSize) const
{
    auto start = std::chrono::high_resolution_clock::now();

    TeamResult result;
    std::vector<std::string> remainingTypes = extractTypes(enemyNames);
    std::vector<const Pokemon*> pool        = dex.all();

    for (int pick = 0; pick < teamSize && !remainingTypes.empty(); pick++) {
        // Score all candidates into a vector then sort — no heap
        std::vector<std::pair<float, const Pokemon*>> scored;
        scored.reserve(pool.size());

        for (const Pokemon* p : pool) {
            float score = graph.offensiveScore(p->types, remainingTypes)
                        - 0.3f * graph.defensiveScore(p->types);
            scored.push_back({score, p});
        }

        // Sort descending by score
        std::sort(scored.begin(), scored.end(),
                  [](const auto& a, const auto& b) { return a.first > b.first; });

        // Pick best not already on team
        for (const auto& [score, pokemon] : scored) {
            bool alreadyPicked = false;
            for (const auto* p : result.team)
                if (p->name == pokemon->name) { alreadyPicked = true; break; }
            if (!alreadyPicked) {
                result.team.push_back(pokemon);
                result.scores.push_back(score);
                result.totalScore += score;

                remainingTypes.erase(
                    std::remove_if(remainingTypes.begin(), remainingTypes.end(),
                        [&](const std::string& t) {
                            return graph.offensiveScore(pokemon->types, {t}) >= 2.0f;
                        }),
                    remainingTypes.end()
                );
                break;
            }
        }
    }

    auto end = std::chrono::high_resolution_clock::now();
    result.computeTimeMs =
        std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();

    return result;
}


// Benchmark: brute force (only feasible with small pool)
// Samples a subset to keep it runnable

TeamResult TeamBuilder::buildCounterTeamBrute(
    const std::vector<std::string>& enemyNames, int teamSize) const
{
    auto start = std::chrono::high_resolution_clock::now();

    // Limit to first 150 Pokémon — full 1025 would take too long
    std::vector<const Pokemon*> pool;
    for (const Pokemon* p : dex.all()) {
        if (p->id <= 150) pool.push_back(p);
        if ((int)pool.size() >= 150) break;
    }

    std::vector<std::string> enemyTypes = extractTypes(enemyNames);
    TeamResult best;
    best.totalScore = -999.0f;

    // Try every combination of teamSize Pokémon from the pool
    int n = static_cast<int>(pool.size());
    std::vector<int> indices(teamSize);
    for (int i = 0; i < teamSize; i++) indices[i] = i;

    auto scoreTeam = [&](const std::vector<int>& idx) {
        float total = 0.0f;
        for (int i : idx)
            total += graph.offensiveScore(pool[i]->types, enemyTypes);
        return total;
    };

    // Iterate all combinations using the classic increment pattern
    while (true) {
        float s = scoreTeam(indices);
        if (s > best.totalScore) {
            best.totalScore = s;
            best.team.clear();
            best.scores.clear();
            for (int i : indices) {
                best.team.push_back(pool[i]);
                best.scores.push_back(
                    graph.offensiveScore(pool[i]->types, enemyTypes));
            }
        }

        // Advance to next combination
        int i = teamSize - 1;
        while (i >= 0 && indices[i] == n - teamSize + i) i--;
        if (i < 0) break;
        indices[i]++;
        for (int j = i + 1; j < teamSize; j++) indices[j] = indices[j-1] + 1;
    }

    auto end = std::chrono::high_resolution_clock::now();
    best.computeTimeMs =
        std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();

    return best;
}


// Benchmark: run all three and print comparison

void TeamBuilder::benchmark(const std::vector<std::string>& enemyNames) const {
    std::cout << "\n=== BENCHMARK: Counter Team Algorithms ===\n\n";
    std::cout << std::left
              << std::setw(24) << "Algorithm"
              << std::setw(14) << "Time (µs)"
              << std::setw(12) << "Score"
              << "Top Pick\n";
    std::cout << std::string(60, '-') << "\n";

    auto print = [](const std::string& name, const TeamResult& r) {
        std::cout << std::left
                  << std::setw(24) << name
                  << std::setw(14) << r.computeTimeMs
                  << std::setw(12) << std::fixed << std::setprecision(2)
                  << r.totalScore
                  << (r.team.empty() ? "none" : r.team[0]->name) << "\n";
    };

    print("Heap (greedy)",      buildCounterTeam(enemyNames));
    print("Sorted Vec (greedy)", buildCounterTeamSortedVec(enemyNames));
    print("Brute Force (n=150)", buildCounterTeamBrute(enemyNames));

    std::cout << "\nNote: Brute force limited to first 150 Pokémon.\n\n";
}


// Display

void TeamBuilder::displayTeam(const TeamResult& result) const {
    std::cout << "\n=== Counter Team ===\n\n";
    for (int i = 0; i < (int)result.team.size(); i++) {
        const Pokemon* p = result.team[i];
        std::cout << "  " << (i+1) << ". "
                  << std::setw(14) << std::left << p->name;
        for (const auto& t : p->types)
            std::cout << std::setw(10) << std::left << t;
        std::cout << "  score: " << std::fixed << std::setprecision(2)
                  << result.scores[i] << "\n";
    }
    std::cout << "\n  Total score: " << result.totalScore
              << "  |  Computed in: " << result.computeTimeMs << " µs\n\n";
}


// Private helpers

std::vector<std::string> TeamBuilder::extractTypes(
    const std::vector<std::string>& names) const
{
    std::vector<std::string> types;
    for (const auto& name : names) {
        const Pokemon* p = dex.find(name);
        if (!p) {
            std::cerr << "Warning: Pokémon not found: " << name << "\n";
            continue;
        }
        for (const auto& t : p->types) {
            // Only add if not already in the list
            if (std::find(types.begin(), types.end(), t) == types.end())
                types.push_back(t);
        }
    }
    return types;
}

float TeamBuilder::scoreCandidateAgainst(
    const Pokemon& candidate,
    const std::vector<std::string>& remainingTypes) const
{
    return graph.offensiveScore(candidate.types, remainingTypes)
         - 0.3f * graph.defensiveScore(candidate.types);
}
