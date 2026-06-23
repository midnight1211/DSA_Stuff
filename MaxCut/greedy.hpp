#ifndef GREEDY_HPP
#define GREEDY_HPP

#include "config.h"
#include "graph.h"

// ===================================================
// ALGORITHM 1 - GREEDY 1/2-APPROXIMATION
// For each node in order, place it in whichever side
// maximizes current cut weight. Guarantees > OPT/2.
// ===================================================
Result greedyHalf(const Graph& g) {
    Timer t;
    std::vector<int> side(g.n, -1);

    for (int u = 0; u < g.n; ++u) {
        long long gain0 = 0, gain1 = 0;
        for (auto& [v, w] : g.adj[u]) {
            if (side[v] == 0) gain1 += w;
            if (side[v] == 1) gain0 += w;
        }
        side[u] = (gain1 >= gain0) ? 1 : 0;
    }

    Result r;
    r.name      = "Greedy 1/2-Approximation";
    r.partition = side;
    r.cutValue  = g.cutWeight(side);
    r.ratio     = (double)r.cutValue / g.totalWeight;
    r.runtimeMs = t.ms();
    r.notes     = "Deterministic; guaranteed >= OPT/2. O(n+m) time.";
    return r;
}

#endif