#pragma once
#include <string>
#include <vector>
#include "Pokemon.h"
#include "Pokedex.h"
#include "TypeGraph.h"
#include "CandidateHeap.h"

// Result of a team build — contains the selected Pokémon and their scores
struct TeamResult {
    std::vector<const Pokemon*> team;
    std::vector<float>          scores;
    float                       totalScore = 0.0f;
    long long                   computeTimeMs = 0LL;  // for benchmarking
};

class TeamBuilder {
public:
    explicit TeamBuilder(const Pokedex& dex, const TypeGraph& graph);

    //  Core feature: counter-team builder

    // Given a list of enemy Pokémon names, find the best 6-Pokémon counter team
    // Uses greedy set cover algorithm
    TeamResult buildCounterTeam(const std::vector<std::string>& enemyNames,
                                int teamSize = 6) const;

    //  Benchmarking ────────────────────

    // Brute-force approach: score every possible combination
    // O(n^teamSize) — only feasible for small pools or small teamSize
    TeamResult buildCounterTeamBrute(const std::vector<std::string>& enemyNames,
                                     int teamSize = 6) const;

    // Greedy without heap: uses sorted vector instead of priority_queue
    // For benchmarking comparison against heap-based approach
    TeamResult buildCounterTeamSortedVec(const std::vector<std::string>& enemyNames,
                                         int teamSize = 6) const;

    // Prints a side-by-side benchmark of all three approaches
    void benchmark(const std::vector<std::string>& enemyNames) const;

    // ── Display ──
    void displayTeam(const TeamResult& result) const;

private:
    const Pokedex&    dex;
    const TypeGraph&  graph;

    // Extracts all unique defending types from a list of Pokémon names
    std::vector<std::string> extractTypes(
        const std::vector<std::string>& names) const;

    // Scores a single Pokémon against remaining uncovered enemy types
    float scoreCandidateAgainst(const Pokemon& candidate,
                                const std::vector<std::string>& remainingTypes) const;
};
