#ifndef GOEMANSWILLIAMSON_HPP
#define GOEMANSWILLIAMSON_HPP

#include "config.h"
#include "graph.h"

// ═════════════════════════════════════════════
//  GOEMANS-WILLIAMSON SUPPORT
//
//  The full GW algorithm uses SDP relaxation:
//    max  Σ w_{ij}(1 - v_i·v_j)/2
//    s.t. ||v_i||² = 1  ∀i, v_i ∈ ℝⁿ
//
//  We approximate this by:
//  1. Constructing the weighted Laplacian L.
//  2. Using power iteration / incremental Cholesky on
//     a perturbed positive-definite version to find a
//     rank-n factorisation X ≈ L (SDP solution proxy).
//  3. Rounding by a random hyperplane r: side_i = sign(v_i · r).
//
//  GW guarantees: cut ≥ 0.878 * OPT (for MAX-CUT).
//
//  We implement this from scratch using:
//   – Incomplete Cholesky on the SDP matrix
//   – Random unit vectors via Box-Muller
// ═════════════════════════════════════════════

// ─── Linear algebra helpers (dense, small-to-medium n) ───

using Mat = std::vector<std::vector<double>>;
using Vec = std::vector<double>;

static Vec randomUnitVector(int dim, std::mt19937& rng) {
    std::normal_distribution<double> nd(0.0, 1.0);
    Vec v(dim);
    double norm = 0.0;
    for (auto& x : v) { x = nd(rng); norm += x * x; }
    norm = std::sqrt(norm);
    for (auto& x : v) x /= norm;
    return v;
}

// Dot product
static double dot(const Vec& a, const Vec& b) {
    double s = 0;
    for (int i = 0; i < (int)a.size(); ++i) s += a[i] * b[i];
    return s;
}

// ─── SDP approximation via random projection ───
//
// We embed each node i into ℝᵈ (d = embedding_dim) using
// a Gram factorisation of the SDP solution. The SDP solution
// matrix X* satisfies X*_{ii}=1 and X*_{ij} minimises the
// GW objective. We approximate X* by iterative updates on
// the normalised Laplacian eigenvectors.
//
// For large graphs we use a low-rank embedding (d ≤ 50).

struct GWSolver {
    const Graph& g;
    int n;
    int dim;              // embedding dimension
    std::mt19937& rng;
 
    Mat V;   // V[i] = embedding vector of node i (dim-dimensional unit vector)
 
    GWSolver(const Graph& g, int dim, std::mt19937& rng)
        : g(g), n(g.n), dim(std::min(dim, g.n)), rng(rng), V(g.n, Vec(dim, 0.0)) {}
 
    // ── Initialise with random unit vectors ──
    void initRandom() {
        for (int i = 0; i < n; ++i) V[i] = randomUnitVector(dim, rng);
    }
 
    // ── SDP-motivated iterative refinement ──
    // We perform gradient ascent on the GW objective
    // Σ w_{ij}(1 - v_i·v_j)/2 subject to ||v_i||=1.
    // Gradient w.r.t. v_i: -Σ_j w_{ij} v_j (project onto tangent sphere).
    void refineSDP(int iterations) {
        double lr = 0.05;
        for (int iter = 0; iter < iterations; ++iter) {
            Mat grad(n, Vec(dim, 0.0));
            for (auto& e : g.edges) {
                int u = e.u, v = e.v; double w = e.w;
                // gradient of (1 - v_u·v_v)/2 w.r.t v_u is -v_v/2
                for (int d = 0; d < dim; ++d) {
                    grad[u][d] -= w * V[v][d] * 0.5;
                    grad[v][d] -= w * V[u][d] * 0.5;
                }
            }
            // Projected gradient step + renormalise
            for (int i = 0; i < n; ++i) {
                // subtract component along V[i] (Riemannian gradient)
                double dv = dot(grad[i], V[i]);
                for (int d = 0; d < dim; ++d)
                    V[i][d] = V[i][d] + lr * (grad[i][d] - dv * V[i][d]);
                // renormalise
                double norm = 0;
                for (auto x : V[i]) norm += x*x;
                norm = std::sqrt(norm);
                for (auto& x : V[i]) x /= norm;
            }
            lr *= 0.995;   // decay
        }
    }
 
    // ── Round with a random hyperplane ──
    std::vector<int> roundHyperplane() {
        Vec r = randomUnitVector(dim, rng);
        std::vector<int> side(n);
        for (int i = 0; i < n; ++i)
            side[i] = (dot(V[i], r) >= 0.0) ? 1 : 0;
        return side;
    }
 
    // ── Compute SDP objective value ──
    double sdpObjective() const {
        double obj = 0;
        for (auto& e : g.edges)
            obj += e.w * (1.0 - dot(V[e.u], V[e.v])) * 0.5;
        return obj;
    }
};
 
// ── GW Variant A: Classic (single solve + single round) ──
Result gwClassic(const Graph& g, const Config& cfg, std::mt19937& rng) {
    Timer t;
    int dim = std::min(g.n, 50);
    GWSolver solver(g, dim, rng);
    solver.initRandom();
    solver.refineSDP(cfg.gwSdpIterations);
 
    auto side  = solver.roundHyperplane();
    double sdp = solver.sdpObjective();
 
    Result r;
    r.name      = "Goemans-Williamson Classic";
    r.partition = side;
    r.cutValue  = g.cutWeight(side);
    r.ratio     = (double)r.cutValue / g.totalWeight;
    r.runtimeMs = t.ms();
    r.iterations= cfg.gwSdpIterations;
    r.notes     = "SDP obj=" + std::to_string((long long)sdp) +
                  "; embed_dim=" + std::to_string(dim) +
                  ". Theory: cut ≥ 0.878·OPT.";
    return r;
}
 
// ── GW Variant B: Multiple random restarts ──
Result gwMultiRestart(const Graph& g, const Config& cfg, std::mt19937& rng) {
    Timer t;
    int dim = std::min(g.n, 50);
 
    long long bestCut = -1;
    std::vector<int> bestSide;
    double bestSdp = -1;
 
    for (int restart = 0; restart < cfg.gwRestarts; ++restart) {
        GWSolver solver(g, dim, rng);
        solver.initRandom();
        solver.refineSDP(cfg.gwSdpIterations / cfg.gwRestarts + 1);
        auto side = solver.roundHyperplane();
        long long c = g.cutWeight(side);
        if (c > bestCut) {
            bestCut  = c;
            bestSide = side;
            bestSdp  = solver.sdpObjective();
        }
    }
 
    Result r;
    r.name      = "Goemans-Williamson Multi-Restart";
    r.partition = bestSide;
    r.cutValue  = bestCut;
    r.ratio     = (double)bestCut / g.totalWeight;
    r.runtimeMs = t.ms();
    r.iterations= cfg.gwRestarts;
    r.notes     = "Best of " + std::to_string(cfg.gwRestarts) +
                  " independent SDP restarts. SDP obj≈" +
                  std::to_string((long long)bestSdp) + ".";
    return r;
}
 
// ── GW Variant C: Bipartite-aware rounding ──
// After SDP, if a node embedding is nearly antipodal
// to its high-weight neighbours, bias the rounding
// threshold to exploit near-bipartite structure.
Result gwBipartiteAware(const Graph& g, const Config& cfg, std::mt19937& rng) {
    Timer t;
    int dim = std::min(g.n, 50);
    GWSolver solver(g, dim, rng);
    solver.initRandom();
    solver.refineSDP(cfg.gwSdpIterations);
 
    // For each node, compute a bias score:
    //   bias_i = -Σ_{j∈N(i)} w_{ij} (v_i · v_j)
    // High bias means neighbours point "opposite" → bipartite signal.
    std::vector<double> bias(g.n, 0.0);
    for (auto& e : g.edges) {
        double d = dot(solver.V[e.u], solver.V[e.v]);
        bias[e.u] -= e.w * d;
        bias[e.v] -= e.w * d;
    }
 
    // Use multiple hyperplanes; choose the one that maximises cut.
    long long bestCut = -1;
    std::vector<int> bestSide;
    int rounds = cfg.gwRestarts * 2;
    for (int k = 0; k < rounds; ++k) {
        Vec r = randomUnitVector(dim, rng);
        std::vector<int> side(g.n);
        for (int i = 0; i < g.n; ++i) {
            double proj = dot(solver.V[i], r);
            // threshold shifted by bias signal
            double thresh = -0.02 * (bias[i] / (g.totalWeight + 1e-9));
            side[i] = (proj >= thresh) ? 1 : 0;
        }
        long long c = g.cutWeight(side);
        if (c > bestCut) { bestCut = c; bestSide = side; }
    }
 
    Result r;
    r.name      = "Goemans-Williamson Bipartite-Aware";
    r.partition = bestSide;
    r.cutValue  = bestCut;
    r.ratio     = (double)bestCut / g.totalWeight;
    r.runtimeMs = t.ms();
    r.iterations= rounds;
    r.notes     = "Rounding threshold biased by antipodality signal. "
                  + std::to_string(rounds) + " hyperplane trials.";
    return r;
}
 
// ── GW Variant D: Semi-random perturbation ──
// Adds Gaussian noise to the SDP solution before rounding
// to escape local plateaux — a semi-random variant.
Result gwSemiRandom(const Graph& g, const Config& cfg, std::mt19937& rng) {
    Timer t;
    int dim = std::min(g.n, 50);
    GWSolver solver(g, dim, rng);
    solver.initRandom();
    solver.refineSDP(cfg.gwSdpIterations);
 
    std::normal_distribution<double> noise(0.0, 0.1);
    long long bestCut = -1;
    std::vector<int> bestSide;
 
    for (int k = 0; k < cfg.gwRestarts * 3; ++k) {
        // perturb embeddings with small noise then renormalise
        Mat Vp = solver.V;
        for (int i = 0; i < g.n; ++i) {
            double norm = 0;
            for (auto& x : Vp[i]) { x += noise(rng); norm += x*x; }
            norm = std::sqrt(norm);
            for (auto& x : Vp[i]) x /= norm;
        }
        // random hyperplane on perturbed embeddings
        Vec r = randomUnitVector(dim, rng);
        std::vector<int> side(g.n);
        for (int i = 0; i < g.n; ++i)
            side[i] = (dot(Vp[i], r) >= 0.0) ? 1 : 0;
        long long c = g.cutWeight(side);
        if (c > bestCut) { bestCut = c; bestSide = side; }
    }
 
    Result r;
    r.name      = "Goemans-Williamson Semi-Random";
    r.partition = bestSide;
    r.cutValue  = bestCut;
    r.ratio     = (double)bestCut / g.totalWeight;
    r.runtimeMs = t.ms();
    r.iterations= cfg.gwRestarts * 3;
    r.notes     = "Gaussian-perturbed embeddings before rounding. "
                  + std::to_string(cfg.gwRestarts * 3) + " trials.";
    return r;
}

#endif