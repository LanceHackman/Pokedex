//
// Created by Lance Hackman on 4/17/26.
//

#include "CandidateHeap.h"


void CandidateHeap::build(const std::vector<const Pokemon*>& pool,
                           const std::vector<std::string>& enemyTypes,
                           const TypeGraph& graph) {
    clear();

    for (const Pokemon* p : pool) {
        // Offensive score: how hard does this Pokémon hit the enemy types?
        float offScore = graph.offensiveScore(p->types, enemyTypes);

        float defScore = graph.defensiveScore(p->types);


        float score = (0.7f * offScore) - (0.3f * defScore);

        heap.push({p, score});
    }
}

Candidate CandidateHeap::popBest() {
    if (heap.empty())
        throw std::runtime_error("CandidateHeap is empty");

    Candidate best = heap.top();
    heap.pop();
    return best;
}

const Candidate& CandidateHeap::peekBest() const {
    if (heap.empty())
        throw std::runtime_error("CandidateHeap is empty");
    return heap.top();
}

bool CandidateHeap::empty() const { return heap.empty(); }
int  CandidateHeap::size()  const { return static_cast<int>(heap.size()); }
void CandidateHeap::clear()       { heap = std::priority_queue<Candidate>(); }