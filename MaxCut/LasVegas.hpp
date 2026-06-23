#ifndef LASVEGAS_HPP
#define LASVEGAS_HPP

#include "graph.h"
#include "config.h"

// ============================================
// ALGORITHM 3 - LAS VEGAS RANDOMIZED MAX CUT
// Randomly assign nodes; if the cut >= threshold,
// accept. Otherwise retry. Terminates with probability 1.
// Threshold: expected value = W/2 (expected number
// of trials before success <= 2 for target = W/2).
// ============================================
Result lasVegas(const Graph& g, const Config& cfg, std::mt19937& rng) {
    Timer t;
    std::uniform_int_distribution<int> coin(0, 1);

    double threshold = g.totalWeight * cfg.lasVegasTargetRatio;
    long long bestCut = -1;
    std::vector<int> bestSide;
    int actualTrials = 0;

    for (int trial = 0; trial < cfg.lasVegasMaxTrials; ++trial) {
        ++actualTrials;
        std::vector<int> side(g.n);
        for (int u = 0; u < g.n; ++u) side[u] = coin(rng);
        long long c = g.cutWeight(side);
        if (c > bestCut) { bestCut = c; bestSide = side; }
        if (c >= threshold) break;   // success condition
    }

    bool succeeded = bestCut >= (long long)threshold;

    Result r;
    r.name       = "Las Vegas Randomized";
    r.partition  = bestSide;
    r.cutValue   = bestCut;
    r.ratio      = (double)bestCut / g.totalWeight;
    r.runtimeMs  = t.ms();
    r.iterations = actualTrials;
    r.notes      = "Target ratio=" + std::to_string(cfg.lasVegasTargetRatio) +
                   ". " + (succeeded ? "Succeeded" : "Max trials reached") +
                   " after " + std::to_string(actualTrials) + " trial(s).";
    return r;
}

#endif