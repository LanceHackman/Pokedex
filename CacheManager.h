#pragma once
#include <string>
#include <unordered_map>
#include "Pokemon.h"
#include "include/json.hpp"

#ifndef POKEMONAPP_CACHEMANAGER_H
#define POKEMONAPP_CACHEMANAGER_H

class CacheManager {
public:
    // Returns true if a cache file already exists on disk
    static bool cacheExists(const std::string& filepath = "pokedex_cache.json");

    // Save the full pokedex map to a JSON file on disk
    static bool saveToFile(
        const std::unordered_map<std::string, Pokemon>& pokedex,
        const std::string& filepath = "pokedex_cache.json"
    );

    // Load the JSON file from disk into a pokedex map
    // Returns empty map if file is missing or malformed
    static std::unordered_map<std::string, Pokemon> loadFromFile(
        const std::string& filepath = "pokedex_cache.json"
    );

private:
    // Helpers to convert between Pokemon struct and JSON
    static nlohmann::json pokemonToJson(const Pokemon& p);
    static Pokemon        jsonToPokemon(const nlohmann::json& j);
};

#endif