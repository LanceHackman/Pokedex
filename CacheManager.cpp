#include "CacheManager.h"
#include "include/json.hpp"       // nlohmann/json — single header
#include <fstream>
#include <iostream>

using json = nlohmann::json;


bool CacheManager::cacheExists(const std::string& filepath) {
    std::ifstream file(filepath);
    return file.good();
}


bool CacheManager::saveToFile(
    const std::unordered_map<std::string, Pokemon>& pokedex,
    const std::string& filepath)
{
    try {
        json root;

        // Loop through every entry in the map and convert to JSON
        for (const auto& [name, pokemon] : pokedex) {
            root[name] = pokemonToJson(pokemon);
        }

        std::ofstream file(filepath);
        if (!file.is_open()) {
            std::cerr << "CacheManager: could not open file for writing: "
                      << filepath << "\n";
            return false;
        }

        file << root.dump(2);
        std::cout << "CacheManager: saved " << pokedex.size()
                  << " Pokémon to " << filepath << "\n";
        return true;

    } catch (const std::exception& e) {
        std::cerr << "CacheManager: save failed — " << e.what() << "\n";
        return false;
    }
}



std::unordered_map<std::string, Pokemon> CacheManager::loadFromFile(
    const std::string& filepath)
{
    std::unordered_map<std::string, Pokemon> pokedex;

    try {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            std::cerr << "CacheManager: cache file not found: "
                      << filepath << "\n";
            return pokedex;  // return empty map
        }

        json root = json::parse(file);

        // Loop through every key (Pokémon name) in the JSON object
        for (const auto& [name, data] : root.items()) {
            pokedex[name] = jsonToPokemon(data);
        }

        std::cout << "CacheManager: loaded " << pokedex.size()
                  << " Pokémon from cache\n";

    } catch (const json::parse_error& e) {
        std::cerr << "CacheManager: JSON parse error — " << e.what() << "\n";
        // Return whatever was loaded before the error
    } catch (const std::exception& e) {
        std::cerr << "CacheManager: load failed — " << e.what() << "\n";
    }

    return pokedex;
}



json CacheManager::pokemonToJson(const Pokemon& p) {
    return {
        {"id",         p.id},
        {"name",       p.name},
        {"hp",         p.hp},
        {"attack",     p.attack},
        {"defense",    p.defense},
        {"spAttack",   p.spAttack},
        {"spDefense",  p.spDefense},
        {"speed",      p.speed},
        {"types",      p.types},        // vector<string> serializes automatically
        {"spriteUrl",  p.spriteUrl},
        {"generation", p.generation},
        {"growthRate", p.growthRate}
    };
}



Pokemon CacheManager::jsonToPokemon(const json& j) {
    Pokemon p;

    p.id         = j.value("id",         0);
    p.name       = j.value("name",       "");
    p.hp         = j.value("hp",         0);
    p.attack     = j.value("attack",     0);
    p.defense    = j.value("defense",    0);
    p.spAttack   = j.value("spAttack",   0);
    p.spDefense  = j.value("spDefense",  0);
    p.speed      = j.value("speed",      0);
    p.spriteUrl  = j.value("spriteUrl",  "");
    p.generation = j.value("generation", 0);
    p.growthRate = j.value("growthRate", "");

    // types is a JSON array — convert to vector<string>
    if (j.contains("types") && j["types"].is_array()) {
        p.types = j["types"].get<std::vector<std::string>>();
    }

    return p;
}
