#pragma once
#include <string>
#include "Pokemon.h"

#ifndef POKEMONAPP_APICLIENT_H
#define POKEMONAPP_APICLIENT_H

class ApiClient {
public:
    // Fetch a single Pokémon by numeric ID (1–1025)
    Pokemon fetchById(int id);

    // Fetch a single Pokémon by name (e.g. "charizard")
    Pokemon fetchByName(const std::string& name);

private:
    // Base URL — no trailing slash
    const std::string BASE_URL = "https://pokeapi.co";

    // Makes the raw GET request, returns JSON string
    // Throws std::runtime_error if request fails
    std::string get(const std::string& endpoint);

    // Parses the raw JSON string into a Pokemon struct
    Pokemon parse(const std::string& jsonString);

    // Figures out which generation a Pokémon belongs to by ID
    int generationFromId(int id);
};

#endif