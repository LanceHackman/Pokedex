cmake_minimum_required(VERSION 3.15)
project(Pokedex)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(OPENSSL_ROOT_DIR /opt/homebrew/opt/openssl)

include_directories(include)

set(OPENSSL_ROOT_DIR /opt/homebrew/opt/openssl)
set(SFML_DIR /opt/homebrew/opt/sfml/lib/cmake/SFML)

find_package(nlohmann_json REQUIRED)
find_package(SFML 3 COMPONENTS Graphics Window System Audio REQUIRED)
find_package(OpenSSL REQUIRED)

add_executable(Pokedex
        ApiClient.cpp
        ApiClient.h
        CacheManager.cpp
        CacheManager.h
        CandidateHeap.cpp
        CandidateHeap.h
        main.cpp
        Pokedex.cpp
        Pokedex.h
        Pokemon.h
        Sorter.cpp
        Sorter.h
        TeamBuilder.cpp
        TeamOptimizer.cpp
        TypeGraph.cpp
        TypeGraph.h
        BenchmarkRunner.cpp
        BenchmarkRunner.h)

# All target_link_libraries must use the SAME target name — PokemonApp
target_link_libraries(Pokedex PRIVATE
        nlohmann_json::nlohmann_json
         SFML::Graphics SFML::Window SFML::System
        OpenSSL::SSL
        OpenSSL::Crypto
        "-framework CoreFoundation"
        "-framework Security")