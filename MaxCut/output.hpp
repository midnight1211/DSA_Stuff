#include "config.h"
#include "graph.h"


// ─────────────────────────────────────────────
//  OUTPUT HELPERS
// ─────────────────────────────────────────────
 
static std::string bar(double ratio, int width = 40) {
    int filled = (int)std::round(ratio * width);
    filled = std::max(0, std::min(width, filled));
    std::string result;
    for (int i = 0; i < filled; ++i)      result += "#";
    for (int i = filled; i < width; ++i)  result += ".";
    return result;
}
 
static void printSeparator(char c = '-', int w = 72) {
    std::cout << std::string(w, c) << "\n";
}
 
static void printHeader(const std::string& s) {
    printSeparator('=');
    std::cout << "  " << s << "\n";
    printSeparator('=');
}
 
static void printResult(const Result& r, long long totalW) {
    printSeparator();
    std::cout << "  >>  " << r.name << "\n";
    printSeparator('-');
    std::cout << std::fixed << std::setprecision(4);
    std::cout << "  Cut Weight   : " << r.cutValue
              << " / " << totalW << "\n";
    std::cout << "  Ratio W/ΣW   : " << r.ratio * 100.0 << "%\n";
    std::cout << "  [" << bar(r.ratio) << "]\n";
    std::cout << "  Runtime      : " << r.runtimeMs << " ms\n";
    if (r.iterations > 0)
        std::cout << "  Iterations   : " << r.iterations << "\n";
    std::cout << "  Notes        : " << r.notes << "\n";
}
 
static void printGraph(const Graph& g, const Config& cfg) {
    printHeader("GRAPH STRUCTURE");
    std::cout << "  Nodes         : " << g.n << "\n";
    std::cout << "  Edges         : " << g.edges.size() << "\n";
    std::cout << "  Total Weight  : " << g.totalWeight << "\n";
    std::cout << "  Weighted      : " << (cfg.weighted ? "Yes" : "No") << "\n";
 
    double density = (g.n > 1)
        ? 2.0 * g.edges.size() / ((double)g.n * (g.n - 1))
        : 0.0;
    std::cout << "  Density       : "
              << std::fixed << std::setprecision(4) << density << "\n";
 
    // Degree statistics
    std::vector<int> deg(g.n, 0);
    for (auto& e : g.edges) { deg[e.u]++; deg[e.v]++; }
    int minDeg = *std::min_element(deg.begin(), deg.end());
    int maxDeg = *std::max_element(deg.begin(), deg.end());
    double avgDeg = 2.0 * g.edges.size() / g.n;
    std::cout << "  Min/Avg/Max Degree: "
              << minDeg << " / "
              << std::fixed << std::setprecision(2) << avgDeg
              << " / " << maxDeg << "\n";
 
    if (cfg.printGraph && g.n <= 30) {
        std::cout << "\n  Adjacency List:\n";
        for (int u = 0; u < g.n; ++u) {
            std::cout << "    " << std::setw(3) << u << " → ";
            for (auto& [v, w] : g.adj[u])
                std::cout << v << (cfg.weighted ? "[" + std::to_string(w) + "]" : "") << " ";
            std::cout << "\n";
        }
    } else if (g.n > 30) {
        std::cout << "  (Adjacency list suppressed for n > 30)\n";
    }
}
 
static void printComparativeTable(const std::vector<Result>& results, long long totalW) {
    printHeader("COMPARATIVE SUMMARY TABLE");
 
    // Find best
    long long best = 0;
    for (auto& r : results) best = std::max(best, r.cutValue);
 
    // Header
    std::cout << std::left
              << std::setw(42) << "Algorithm"
              << std::setw(12) << "Cut"
              << std::setw(10) << "Ratio%"
              << std::setw(12) << "Time(ms)"
              << std::setw(6)  << "Best?" << "\n";
    printSeparator('-');
    for (auto& r : results) {
        std::cout << std::left
                  << std::setw(42) << r.name
                  << std::setw(12) << r.cutValue
                  << std::setw(10) << std::fixed << std::setprecision(2) << r.ratio * 100.0
                  << std::setw(12) << std::fixed << std::setprecision(3) << r.runtimeMs
                  << (r.cutValue == best ? "  ★" : "") << "\n";
    }
    printSeparator();
 
    // Theoretical lower bound
    double halfW = totalW / 2.0;
    std::cout << "\n  Theoretical W/2 lower bound : "
              << std::fixed << std::setprecision(2) << halfW << "\n";
    std::cout << "  GW theory bound (0.878·OPT): ≥ "
              << std::fixed << std::setprecision(2) << 0.878 * best
              << " (approximation; OPT estimate = best found = "
              << best << ")\n";
}
 
static void printPartitions(const std::vector<Result>& results, int n, bool enabled) {
    if (!enabled) return;
    printHeader("NODE PARTITIONS (S=0, T=1)");
    for (auto& r : results) {
        std::cout << "\n  [" << r.name << "]\n  S: ";
        for (int i = 0; i < n; ++i) if (r.partition[i] == 0) std::cout << i << " ";
        std::cout << "\n  T: ";
        for (int i = 0; i < n; ++i) if (r.partition[i] == 1) std::cout << i << " ";
        std::cout << "\n";
    }
}
 
static void printExperimentalStats(const std::vector<Result>& results, long long totalW) {
    printHeader("EXPERIMENTAL STATISTICS");
 
    long long best = 0, worst = LLONG_MAX;
    double totalTime = 0;
    for (auto& r : results) {
        best  = std::max(best, r.cutValue);
        worst = std::min(worst, r.cutValue);
        totalTime += r.runtimeMs;
    }
 
    std::cout << "  Total algorithms run  : " << results.size() << "\n";
    std::cout << "  Best cut found        : " << best
              << " (" << std::fixed << std::setprecision(2)
              << 100.0 * best / totalW << "% of total weight)\n";
    std::cout << "  Worst cut found       : " << worst
              << " (" << 100.0 * worst / totalW << "%)\n";
    std::cout << "  Cut spread (best-worst): " << best - worst << "\n";
    std::cout << "  Total runtime         : "
              << std::fixed << std::setprecision(3) << totalTime << " ms\n";
 
    // Per-algorithm ratio vs best
    std::cout << "\n  Approximation quality (ratio vs best found):\n";
    for (auto& r : results) {
        double q = (best > 0) ? (double)r.cutValue / best * 100.0 : 0.0;
        std::cout << "    " << std::left << std::setw(42) << r.name
                  << std::fixed << std::setprecision(2) << q << "%\n";
    }
 
    // GW theoretical analysis
    std::cout << "\n  Goemans-Williamson Analysis:\n";
    std::cout << "    The GW rounding theorem guarantees:\n";
    std::cout << "    E[cut(r)] / OPT ≥ min_{0≤θ≤π} (2/π · θ / (1-cos θ)) ≈ 0.8785\n";
    std::cout << "    where θ = arccos(x_{ij}) for the SDP relaxation variable x_{ij}.\n";
    std::cout << "    This ratio beats the random 1/2 bound by ~75.7% relatively.\n";
    std::cout << "    (Under Unique Games Conjecture, 0.878 is optimal for poly-time.)\n";
}

