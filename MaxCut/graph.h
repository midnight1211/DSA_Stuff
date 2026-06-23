#pragma once

#include "config.h"

// ---------------------------------------------
// GRAPH
// ---------------------------------------------
struct Edge {
    int u, v, w;   // u < v, weight w (1 for unweighted)
};

struct Graph {
    int n;
    std::vector<Edge> edges;
    std::vector<std::vector<std::pair<int, int>>> adj; // adj[u] = {u, v}
    long long totalWeight = 0;

    Graph(int n) : n(n), adj(n) {}

    void addEdge(int u, int v, int w = 1) {
        if (u > v) std::swap(u, v);
        edges.push_back({u, v, w});
        adj[u].push_back({v, w});
        adj[v].push_back({u, w});
        totalWeight += w;
    }

    // weight of edges crossing the partition (S=0, T=1)
    long long cutWeight(const std::vector<int>& side) const {
        long long cut = 0;
        for (auto& e : edges)
            if (side[e.u] != side[e.v])
                cut += e.w;
        return cut;
    }
};

// ----------------------------------------------
// GRAPH GENERATOR
// ----------------------------------------------
Graph generateGraph(const Config& cfg, std::mt19937& rng) {
    int n = std::max(2, std::min(cfg.numNodes, 10000));
    Graph g(n);

    std::uniform_real_distribution<double> prob(0.0, 1.0);
    std::uniform_int_distribution<int>     weight(1, 100);

    for (int u = 0; u < n; ++u) 
        for (int v = u + 1; v < n; ++v)
            if (prob(rng) < cfg.edgeProbability)
                g.addEdge(u, v, cfg.weighted ? weight(rng) : 1);
    
    // ensure connectivity: add a spanning path
    std::vector<int> perm(n);
    std::iota(perm.begin(), perm.end(), 0);
    std::shuffle(perm.begin(), perm.end(), rng);
    for (int i = 0; i + 1 < n; ++i) {
        int u = perm[i], v = perm[i + 1];
        // check if edge already exists
        bool found = false;
        for (auto& [nb, _] : g.adj[u]) if (nb == v) { found = true; break; }
        if (!found)
            g.addEdge(u, v, cfg.weighted ? weight(rng) : 1);
    }

    return g;
}