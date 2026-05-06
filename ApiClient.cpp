#define CPPHTTPLIB_OPENSSL_SUPPORT
#include "include/httplib.h"
#include "include/json.hpp"
#include "ApiClient.h"
#include <iostream>
#include <stdexcept>

using json = nlohmann::json;



Pokemon ApiClient::fetchById(int id) {
    std::string endpoint = "/api/v2/pokemon/" + std::to_string(id);
    std::string body = get(endpoint);
    return parse(body);
}



Pokemon ApiClient::fetchByName(const std::string& name) {
    std::string endpoint = "/api/v2/pokemon/" + name;
    std::string body = get(endpoint);
    return parse(body);
}



std::string ApiClient::get(const std::string& endpoint) {
    httplib::Client client(BASE_URL);

    // 10 second timeout
    client.set_connection_timeout(10);
    client.set_read_timeout(10);

    auto res = client.Get(endpoint);

    // Check for network-level failure (no response at all)
    if (!res) {
        throw std::runtime_error(
            "Network error on " + endpoint + ": " +
            httplib::to_string(res.error())
        );
    }

    // Check for HTTP error (404 not found, 429 rate limited, etc.)
    if (res->status != 200) {
        throw std::runtime_error(
            "HTTP " + std::to_string(res->status) +
            " on " + endpoint
        );
    }

    return res->body;
}



Pokemon ApiClient::parse(const std::string& jsonString) {
    json data = json::parse(jsonString);
    Pokemon p;

    p.id   = data["id"];
    p.name = data["name"];

    // PokéAPI returns stats as an array, always in this fixed order:
    p.hp        = data["stats"][0]["base_stat"];
    p.attack    = data["stats"][1]["base_stat"];
    p.defense   = data["stats"][2]["base_stat"];
    p.spAttack  = data["stats"][3]["base_stat"];
    p.spDefense = data["stats"][4]["base_stat"];
    p.speed     = data["stats"][5]["base_stat"];

    //  Types (1 or 2)
    for (const auto& slot : data["types"]) {
        p.types.push_back(slot["type"]["name"]);
    }

    if (!data["sprites"]["front_default"].is_null()) {
        p.spriteUrl = data["sprites"]["front_default"];
    } else {
        p.spriteUrl = "";
    }

    // Generation (derived from ID ranges)
    p.generation = generationFromId(p.id);

    //  Growth rate
    p.growthRate = "";

    return p;
}


int ApiClient::generationFromId(int id) {
    if (id <=  151) return 1;  // Red/Blue
    if (id <=  251) return 2;  // Gold/Silver
    if (id <=  386) return 3;  // Ruby/Sapphire
    if (id <=  493) return 4;  // Diamond/Pearl
    if (id <=  649) return 5;  // Black/White
    if (id <=  721) return 6;  // X/Y
    if (id <=  809) return 7;  // Sun/Moon
    if (id <=  905) return 8;  // Sword/Shield
    return 9;                  // Scarlet/Violet
}
