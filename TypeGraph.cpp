//
// Created by Lance Hackman on 4/17/26.
//

#include "TypeGraph.h"

#include "TypeGraph.h"
#include <stdexcept>


// Constructor — build the graph on startup

TypeGraph::TypeGraph() {
    // All 18 types in a fixed order
    types = {
        "normal", "fire", "water", "electric", "grass", "ice",
        "fighting", "poison", "ground", "flying", "psychic", "bug",
        "rock", "ghost", "dragon", "dark", "steel", "fairy"
    };

    // Map each type name to its index for O(1) lookup
    for (int i = 0; i < (int)types.size(); i++) {
        typeIndex[types[i]] = i;
    }

    // Initialize all matchups to 1x (neutral)
    for (int i = 0; i < 18; i++)
        for (int j = 0; j < 18; j++)
            matrix[i][j] = 1.0f;

    buildMatrix();
}


// Public: get effectiveness multiplier

float TypeGraph::effectiveness(const std::string& attacking,
                                const std::string& defending) const {
    auto a = typeIndex.find(attacking);
    auto d = typeIndex.find(defending);

    if (a == typeIndex.end())
        throw std::invalid_argument("Unknown attacking type: " + attacking);
    if (d == typeIndex.end())
        throw std::invalid_argument("Unknown defending type: " + defending);

    return matrix[a->second][d->second];
}


// Public: offensive score of attackingTypes vs defendingTypes
// For a dual-type attacker, takes the best multiplier of the two types
// For a dual-type defender, multiplies the two multipliers together

float TypeGraph::offensiveScore(const std::vector<std::string>& attackingTypes,
                                 const std::vector<std::string>& defendingTypes) const {
    float best = 0.0f;

    for (const auto& atk : attackingTypes) {
        float multiplier = 1.0f;
        for (const auto& def : defendingTypes) {
            multiplier *= effectiveness(atk, def);
        }
        if (multiplier > best) best = multiplier;
    }

    return best;
}


// Public: defensive score (sum of weaknesses)
// Lower score = fewer weaknesses = better defender

float TypeGraph::defensiveScore(const std::vector<std::string>& defendingTypes) const {
    float weaknesses = 0.0f;

    for (const auto& atk : types) {
        float multiplier = 1.0f;
        for (const auto& def : defendingTypes) {
            multiplier *= effectiveness(atk, def);
        }
        if (multiplier > 1.0f) weaknesses += multiplier;
    }

    return weaknesses;
}


// Public: all 18 type names

const std::vector<std::string>& TypeGraph::allTypes() const {
    return types;
}


// Private: helper to set one attacker's matchups

void TypeGraph::setMatchups(const std::string& attacker,
                             const std::vector<std::string>& superAgainst,
                             const std::vector<std::string>& halfAgainst,
                             const std::vector<std::string>& immuneAgainst) {
    int i = typeIndex.at(attacker);

    for (const auto& t : superAgainst)
        matrix[i][typeIndex.at(t)] = 2.0f;

    for (const auto& t : halfAgainst)
        matrix[i][typeIndex.at(t)] = 0.5f;

    for (const auto& t : immuneAgainst)
        matrix[i][typeIndex.at(t)] = 0.0f;
}


// Private: full Gen 6+ type chart
// Source: Bulbapedia — type chart (Generation VI+)

void TypeGraph::buildMatrix() {
    //                attacker      super effective        not very effective       immune
    setMatchups("normal",   {},                            {"rock","steel"},         {"ghost"});
    setMatchups("fire",     {"grass","ice","bug","steel"}, {"fire","water","rock","dragon"}, {});
    setMatchups("water",    {"fire","ground","rock"},      {"water","grass","dragon"},{});
    setMatchups("electric", {"water","flying"},            {"electric","grass","dragon"},{"ground"});
    setMatchups("grass",    {"water","ground","rock"},     {"fire","grass","poison","flying","bug","dragon","steel"},{});
    setMatchups("ice",      {"grass","ground","flying","dragon"},{"water","ice"},    {"steel"});
    setMatchups("fighting", {"normal","ice","rock","dark","steel"},
                            {"poison","flying","psychic","bug","fairy"},             {"ghost"});
    setMatchups("poison",   {"grass","fairy"},             {"poison","ground","rock","ghost"},{"steel"});
    setMatchups("ground",   {"fire","electric","poison","rock","steel"},
                            {"grass","bug"},                                         {"flying"});
    setMatchups("flying",   {"grass","fighting","bug"},    {"electric","rock","steel"},{});
    setMatchups("psychic",  {"fighting","poison"},         {"psychic","steel"},      {"dark"});
    setMatchups("bug",      {"grass","psychic","dark"},    {"fire","fighting","flying","ghost","steel","fairy"},{});
    setMatchups("rock",     {"fire","ice","flying","bug"}, {"fighting","ground","steel"},{});
    setMatchups("ghost",    {"psychic","ghost"},           {"dark"},                 {"normal"});
    setMatchups("dragon",   {"dragon"},                    {"steel"},                {"fairy"});
    setMatchups("dark",     {"psychic","ghost"},           {"fighting","dark","fairy"},{});
    setMatchups("steel",    {"ice","rock","fairy"},        {"fire","water","electric","steel"},{});
    setMatchups("fairy",    {"fighting","dragon","dark"},  {"fire","poison","steel"}, {});
}