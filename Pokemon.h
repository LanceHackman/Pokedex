#pragma once
#include <string>
#include <vector>

#ifndef POKEMONAPP_POKEMON_H
#define POKEMONAPP_POKEMON_H

struct Pokemon {
    int         id;
    std::string name;

    // Base stats
    int hp;
    int attack;
    int defense;
    int spAttack;
    int spDefense;
    int speed;

    // Types (1 or 2)
    std::vector<std::string> types;

    // Sprite URL from PokéAPI
    std::string spriteUrl;

    // Generation (1–9)
    int generation;

    // Growth rate (e.g. "slow", "medium", "fast")
    std::string growthRate;
};

#endif