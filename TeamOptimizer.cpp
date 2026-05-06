#include "TeamOptimizer.h"
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <chrono>


// Constructor

TeamOptimizer::TeamOptimizer(const Pokedex& dex, const TypeGraph& graph)
    : dex(dex), graph(graph) {}


// Build strong team — no specific enemy

TeamResult TeamOptimizer::buildStrongTeam(int teamSize) const {
    auto start = std::chrono::high_resolution_clock::now();

    TeamResult result;
    std::vector<const Pokemon*> pool = dex.all();

    for (int pick = 0; pick < teamSize; pick++) {
        const Pokemon* bestPick  = nullptr;
        float          bestScore = -9999.0f;

        for (const Pokemon* candidate : pool) {
            // Skip if already on team
            bool alreadyPicked = false;
            for (const auto* p : result.team)
                if (p->name == candidate->name) { alreadyPicked = true; break; }
            if (alreadyPicked) continue;

            float score = scoreGeneral(*candidate, result.team);
            if (score > bestScore) {
                bestScore = score;
                bestPick  = candidate;
            }
        }

        if (bestPick) {
            result.team.push_back(bestPick);
            result.scores.push_back(bestScore);
            result.totalScore += bestScore;
        }
    }

    auto end = std::chrono::high_resolution_clock::now();
    result.computeTimeMs =
        std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();

    return result;
}


// Build strong team filtered by generation

TeamResult TeamOptimizer::buildStrongTeam(int teamSize, int generation) const {
    auto start = std::chrono::high_resolution_clock::now();

    TeamResult result;
    std::vector<const Pokemon*> pool = dex.byGeneration(generation);

    for (int pick = 0; pick < teamSize; pick++) {
        const Pokemon* bestPick  = nullptr;
        float          bestScore = -9999.0f;

        for (const Pokemon* candidate : pool) {
            bool alreadyPicked = false;
            for (const auto* p : result.team)
                if (p->name == candidate->name) { alreadyPicked = true; break; }
            if (alreadyPicked) continue;

            float score = scoreGeneral(*candidate, result.team);
            if (score > bestScore) {
                bestScore = score;
                bestPick  = candidate;
            }
        }

        if (bestPick) {
            result.team.push_back(bestPick);
            result.scores.push_back(bestScore);
            result.totalScore += bestScore;
        }
    }

    auto end = std::chrono::high_resolution_clock::now();
    result.computeTimeMs =
        std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();

    return result;
}


// Display

void TeamOptimizer::displayTeam(const TeamResult& result) const {
    std::cout << "\n=== Optimized Team ===\n\n";
    std::cout << std::left
              << std::setw(4)  << "#"
              << std::setw(14) << "Name"
              << std::setw(20) << "Types"
              << std::setw(8)  << "BST"
              << "Score\n";
    std::cout << std::string(60, '-') << "\n";

    for (int i = 0; i < (int)result.team.size(); i++) {
        const Pokemon* p = result.team[i];
        std::string types;
        for (const auto& t : p->types) types += t + " ";

        std::cout << std::left
                  << std::setw(4)  << (i + 1)
                  << std::setw(14) << p->name
                  << std::setw(20) << types
                  << std::setw(8)  << bst(*p)
                  << std::fixed << std::setprecision(2)
                  << result.scores[i] << "\n";
    }
    std::cout << "\n  Total score:  " << result.totalScore
              << "  |  Computed in: " << result.computeTimeMs << " µs\n\n";
}


// Private: general scoring function

float TeamOptimizer::scoreGeneral(
    const Pokemon& p,
    const std::vector<const Pokemon*>& currentTeam) const
{
    // 1. Base stat total — normalized to 0–1 range (max BST ~780)
    float statScore = static_cast<float>(bst(p)) / 780.0f;

    // 2. Offensive coverage — how many of the 18 types can this hit 2x?
    float coverageScore = 0.0f;
    for (const auto& type : graph.allTypes()) {
        if (graph.offensiveScore(p.types, {type}) >= 2.0f)
            coverageScore += 1.0f;
    }
    coverageScore /= 18.0f;  // normalize to 0–1

    // 3. Defensive score — fewer weaknesses is better
    // Normalized: 0 weaknesses = 1.0, many weaknesses = closer to 0
    float defScore = 1.0f / (1.0f + graph.defensiveScore(p.types));

    // 4. Type diversity bonus — reward types not already on the team
    float diversityScore = typeDiversityBonus(p, currentTeam);

    // Weighted combination — tune these for your presentation
    // Discuss trade-offs: a team of 6 Mewtwo would score high on stats
    // but zero on diversity — the diversity penalty prevents that
    return (0.3f * statScore)
         + (0.3f * coverageScore)
         + (0.2f * defScore)
         + (0.2f * diversityScore);
}


// Private: type diversity bonus
// Returns 1.0 if all types are new, 0.0 if all overlap

float TeamOptimizer::typeDiversityBonus(
    const Pokemon& candidate,
    const std::vector<const Pokemon*>& currentTeam) const
{
    if (currentTeam.empty()) return 1.0f;

    int newTypes = 0;
    for (const auto& t : candidate.types) {
        bool seen = false;
        for (const auto* teammate : currentTeam)
            for (const auto& tt : teammate->types)
                if (tt == t) { seen = true; break; }
        if (!seen) newTypes++;
    }

    return static_cast<float>(newTypes) /
           static_cast<float>(candidate.types.size());
}


// Private: base stat total

int TeamOptimizer::bst(const Pokemon& p) {
    return p.hp + p.attack + p.defense +
           p.spAttack + p.spDefense + p.speed;
}
