//
// Created by Lance Hackman on 4/17/26.
//

#ifndef POKEDEX_CANDIDATEHEAP_H
#define POKEDEX_CANDIDATEHEAP_H

#pragma once
#include <string>
#include <vector>
#include <queue>
#include "Pokemon.h"
#include "TypeGraph.h"

// A scored candidate — wraps a Pokemon pointer with a computed score
struct Candidate {
    const Pokemon* pokemon;
    float score;

    // Max-heap: higher score = higher priority
    bool operator<(const Candidate& other) const {
        return score < other.score;
    }
};

class CandidateHeap {
public:
    // Scores all Pokémon in the pool against the given enemy types
    // and pushes them into the heap
    void build(const std::vector<const Pokemon*>& pool,
               const std::vector<std::string>& enemyTypes,
               const TypeGraph& graph);

    // Returns and removes the highest-scoring candidate
    Candidate popBest();

    // Peeks at the highest-scoring candidate without removing it
    const Candidate& peekBest() const;

    // Returns true if the heap is empty
    bool empty() const;

    // Number of candidates in the heap
    int size() const;

    // Clears the heap
    void clear();

private:
    std::priority_queue<Candidate> heap;
};

#endif //POKEDEX_CANDIDATEHEAP_H