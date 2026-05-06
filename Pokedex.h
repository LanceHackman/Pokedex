//
// Created by Lance Hackman on 4/17/26.
//

#ifndef POKEDEX_POKEDEX_H
#define POKEDEX_POKEDEX_H

#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include "Pokemon.h"

class Pokedex {
public:
    // Load from the cache map built by CacheManager
    explicit Pokedex(std::unordered_map<std::string, Pokemon> data);

    // Returns pointer to Pokémon by name, nullptr if not found
    const Pokemon* find(const std::string& name) const;

    // Returns pointer to Pokémon by ID, nullptr if not found
    const Pokemon* findById(int id) const;

    // Returns true if name exists in the dex
    bool contains(const std::string& name) const;

    // Total number of Pokémon loaded
    int size() const;

    //  Filtering

    // All Pokémon of a given type (e.g. "fire")
    std::vector<const Pokemon*> byType(const std::string& type) const;

    // All Pokémon from a given generation (1–9)
    std::vector<const Pokemon*> byGeneration(int gen) const;

    // All Pokémon with a given growth rate
    std::vector<const Pokemon*> byGrowthRate(const std::string& rate) const;

    // All Pokémon — useful for sorting algorithms
    std::vector<const Pokemon*> all() const;

    //  Display

    // Prints a single Pokémon's full stat block to stdout
    void display(const std::string& name) const;

    // Prints a compact one-line summary (for list views)
    void displayCompact(const Pokemon& p) const;

private:
    // Primary store: name to Pokemon
    std::unordered_map<std::string, Pokemon> byName;

    // Secondary index: id to name (avoids duplicating data)
    std::unordered_map<int, std::string> idToName;
};

#endif //POKEDEX_POKEDEX_H