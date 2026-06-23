#ifndef DERANDOMIZED_HPP
#define DERANDOMIZED_HPP

#include "config.h"
#include "graph.h"

// ============================================
// ALGORITHM 4 - DERANDOMIZED MAX CUT
// Method of Conditional Expectations (Erdős-Selfridge):
// For each node u, fix side[u] to whichever of {0, 1}
// does not decrease E[cut | prior assignments].
// E[cut | fixed prefix] = (sum of undecided cross-edges)/2
//                       + (sum of already-cut decided edges)
// Choosing the side that minimizes this greedy expected
// value produces the same >= W/2 guarantee as Monte Carlo
// but deterministically.
// ============================================
Result derandomized(const Graph& g) {
    Timer t;
    // For efficiency: for each unfixed node, track the
    // contribution of each choice.
    std::vector<int> side(g.n, -1);

    // delta[u][s] = increase in cut if we set node u = s,
    // considering only already-decided neighbors.
    // We recompute on the fly per node.

    for (int u = 0; u < g.n; ++u) {
        long long gain0 = 0, gain1 = 0;
        for (auto& [v, w] : g.adj[u]) {
            if (side[v] == -1) {
                // undecided: both choices contribute w/2 in expectation -
                // equivalent contribution, so it cancels; ignore.
            } else if (side[v] == 0) {
                gain1 += w;   // placing u=1 cuts this edge
            } else {
                gain0 += w;   // placing u=0 cuts this edge
            }
        }
        side[u] = (gain1 >= gain0) ? 1 : 0;
    }

    Result r;
    r.name      = "Derandomized (Conditional Expectations)";
    r.partition = side;
    r.cutValue  = g.cutWeight(side);
    r.ratio     = (double)r.cutValue / g.totalWeight;
    r.runtimeMs = t.ms();
    r.notes     = "Deterministic; same ≥W/2 guarantee as MC, "
                  "identical logic to greedy but derived from probability theory.";
    return r;
}

#endif