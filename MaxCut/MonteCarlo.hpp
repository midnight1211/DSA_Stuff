#ifndef MONTECARLO_HPP
#define MONTECARLO_HPP

#include "config.h"
#include "graph.h"

// ============================================
// ALGORITHM 2 - MONTE CARLO RANDOMIZED MAX CUT
// Assign each node uniformly at random to S or T.
// E[cut] = totalWeight / 2. Run many trials, keep best.
// ============================================
Result monteCarlo(const Graph& g, const Config& cfg, std::mt19937& rng) {
    Timer t;
    std::uniform_int_distribution<int> coin(0, 1);

    long long  bestCut = -1;
    std::vector<int> bestSide;

    for (int trial = 0; trial < cfg.monteCarloTrials; ++trial) {
        std::vector<int> side(g.n);
        for (int u = 0; u < g.n; ++u) side[u] = coin(rng);
        long long c = g.cutWeight(side);
        if (c > bestCut) { bestCut = c; bestSide = side; }
    }

    Result r;
    r.name       = "Monte Carlo Randomized";
    r.partition  = bestSide;
    r.cutValue   = bestCut;
    r.ratio      = (double)bestCut / g.totalWeight;
    r.runtimeMs  = t.ms();
    r.iterations = cfg.monteCarloTrials;
    r.notes      = "E[cut] = W/2 per trial. Best of " +
                   std::to_string(cfg.monteCarloTrials) + " trials.";
    return r;
}


#endif