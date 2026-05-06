//
// Created by Lance Hackman on 4/17/26.
//

#ifndef POKEDEX_TYPEGRAPH_H
#define POKEDEX_TYPEGRAPH_H


#pragma once
#include <string>
#include <unordered_map>
#include <vector>

class TypeGraph {
public:
    // Loads all 18 type relationships into the adjacency matrix
    TypeGraph();

    // Returns the damage multiplier when 'attacking' hits 'defending'
    // e.g. effectiveness("fire", "grass") to 2.0
    //      effectiveness("fire", "water") to 0.5
    //      effectiveness("normal", "ghost") to 0.0
    float effectiveness(const std::string& attacking,
                        const std::string& defending) const;

    // Returns the total offensive score of a Pokémon's types
    // against a list of defending types
    // Used by TeamBuilder to rank candidates
    float offensiveScore(const std::vector<std::string>& attackingTypes,
                         const std::vector<std::string>& defendingTypes) const;

    // Returns the total defensive score of a Pokémon's types
    // (how many weaknesses it has — lower is better)
    float defensiveScore(const std::vector<std::string>& defendingTypes) const;

    // Returns all 18 type names
    const std::vector<std::string>& allTypes() const;

private:
    // The 18 type names in a fixed order
    std::vector<std::string> types;

    // Maps type name to its index in the matrix
    std::unordered_map<std::string, int> typeIndex;

    // 18×18 adjacency matrix
    // matrix[i][j] = damage multiplier when type i attacks type j
    float matrix[18][18];

    // Populates the matrix with all type matchup data
    void buildMatrix();

    // Helper: set all matchups for one attacking type at once
    // halfAgainst:  types this deals 0.5x to
    // immuneAgainst: types this deals 0x to
    // superAgainst: types this deals 2x to
    // Everything not listed defaults to 1x
    void setMatchups(const std::string& attacker,
                     const std::vector<std::string>& superAgainst,
                     const std::vector<std::string>& halfAgainst,
                     const std::vector<std::string>& immuneAgainst);
};


#endif //POKEDEX_TYPEGRAPH_H