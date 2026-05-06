//
// Created by Lance Hackman on 4/17/26.
//

#include "Pokedex.h"

#include "Pokedex.h"
#include <iostream>
#include <iomanip>
#include <algorithm>

Pokedex::Pokedex(std::unordered_map<std::string, Pokemon> data)
    : byName(std::move(data))   // move avoids copying 1025 Pokémon
{
    // Build the id to name secondary index
    for (const auto& [name, pokemon] : byName) {
        idToName[pokemon.id] = name;
    }
}


const Pokemon* Pokedex::find(const std::string& name) const {
    auto it = byName.find(name);
    return (it != byName.end()) ? &it->second : nullptr;
}

const Pokemon* Pokedex::findById(int id) const {
    auto it = idToName.find(id);
    if (it == idToName.end()) return nullptr;
    return find(it->second);
}

bool Pokedex::contains(const std::string& name) const {
    return byName.count(name) > 0;
}

int Pokedex::size() const {
    return static_cast<int>(byName.size());
}


std::vector<const Pokemon*> Pokedex::byType(const std::string& type) const {
    std::vector<const Pokemon*> result;
    for (const auto& [name, p] : byName) {
        for (const auto& t : p.types) {
            if (t == type) {
                result.push_back(&p);
                break;
            }
        }
    }
    return result;
}

std::vector<const Pokemon*> Pokedex::byGeneration(int gen) const {
    std::vector<const Pokemon*> result;
    for (const auto& [name, p] : byName) {
        if (p.generation == gen) result.push_back(&p);
    }
    return result;
}

std::vector<const Pokemon*> Pokedex::byGrowthRate(const std::string& rate) const {
    std::vector<const Pokemon*> result;
    for (const auto& [name, p] : byName) {
        if (p.growthRate == rate) result.push_back(&p);
    }
    return result;
}

std::vector<const Pokemon*> Pokedex::all() const {
    std::vector<const Pokemon*> result;
    result.reserve(byName.size());
    for (const auto& [name, p] : byName) {
        result.push_back(&p);
    }
    return result;
}


// Display

void Pokedex::display(const std::string& name) const {
    const Pokemon* p = find(name);
    if (!p) {
        std::cout << "Pokémon not found: " << name << "\n";
        return;
    }

    // Header
    std::cout << "\n";
    std::cout << "  #" << std::setw(4) << std::setfill('0') << p->id
              << "  " << p->name << "\n";
    std::cout << "  ";
    for (const auto& t : p->types) std::cout << "[" << t << "] ";
    std::cout << "\n";
    std::cout << "  Gen " << p->generation << "\n\n";

    // Stat bar (scales to 60 chars wide, max stat is ~255)
    auto printStat = [](const std::string& label, int value) {
        int bars = value / 5;  // 255 max to 51 bars max
        std::cout << "  " << std::setw(12) << std::setfill(' ')
                  << std::left << label
                  << std::setw(4) << std::right << value << "  ";
        for (int i = 0; i < bars; i++) std::cout << "█";
        std::cout << "\n";
    };

    printStat("HP",          p->hp);
    printStat("Attack",      p->attack);
    printStat("Defense",     p->defense);
    printStat("Sp. Attack",  p->spAttack);
    printStat("Sp. Defense", p->spDefense);
    printStat("Speed",       p->speed);

    int total = p->hp + p->attack + p->defense +
                p->spAttack + p->spDefense + p->speed;
    std::cout << "  " << std::string(30, '-') << "\n";
    std::cout << "  " << std::setw(12) << std::left << "Total"
              << std::setw(4) << std::right << total << "\n\n";
}

void Pokedex::displayCompact(const Pokemon& p) const {
    std::cout << "  #" << std::setw(4) << std::setfill('0') << p.id
              << "  " << std::setw(12) << std::setfill(' ') << std::left
              << p.name << "  ";
    for (const auto& t : p.types) std::cout << std::setw(9) << std::left << t;
    std::cout << "  Gen " << p.generation << "\n";
}