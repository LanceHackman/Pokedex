#pragma once
#include <string>
#include <vector>
#include "Pokemon.h"
#include "Pokedex.h"
#include "TypeGraph.h"
#include "CandidateHeap.h"
#include "TeamBuilder.h"

class TeamOptimizer {
public:
    explicit TeamOptimizer(const Pokedex& dex, const TypeGraph& graph);

    // Build the strongest general team with no specific enemy in mind
    // Optimizes for: type coverage, stat spread, and defensive balance
    TeamResult buildStrongTeam(int teamSize = 6) const;

    // Build a team filtered to a specific generation
    TeamResult buildStrongTeam(int teamSize, int generation) const;

    // Display the team with stat breakdown
    void displayTeam(const TeamResult& result) const;

private:
    const Pokedex&   dex;
    const TypeGraph& graph;

    // Scores a Pokémon for general use (not vs a specific enemy)
    // Considers: type coverage, base stat total, defensive weaknesses
    float scoreGeneral(const Pokemon& p,
                       const std::vector<const Pokemon*>& currentTeam) const;

    // Penalizes type overlap with already-chosen teammates
    float typeDiversityBonus(const Pokemon& candidate,
                              const std::vector<const Pokemon*>& currentTeam) const;

    // Returns the base stat total of a Pokémon
    static int bst(const Pokemon& p);
};
