#include "derandomized.hpp"
#include "greedy.hpp"
#include "LasVegas.hpp"
#include "MonteCarlo.hpp"
#include "GoemansWilliamson.hpp"
#include "output.hpp"

int main(int argc, char* argv[]) {
    // ── Parse simple command-line overrides ──
     Config cfg;
 
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        auto eq = arg.find('=');
        if (eq == std::string::npos) continue;
        std::string key = arg.substr(0, eq);
        std::string val = arg.substr(eq + 1);
        if (key == "--nodes")         cfg.numNodes        = std::stoi(val);
        if (key == "--prob")          cfg.edgeProbability = std::stod(val);
        if (key == "--weighted")      cfg.weighted        = (val == "1" || val == "true");
        if (key == "--mc-trials")     cfg.monteCarloTrials= std::stoi(val);
        if (key == "--lv-trials")     cfg.lasVegasMaxTrials= std::stoi(val);
        if (key == "--lv-target")     cfg.lasVegasTargetRatio = std::stod(val);
        if (key == "--gw-restarts")   cfg.gwRestarts      = std::stoi(val);
        if (key == "--gw-iters")      cfg.gwSdpIterations = std::stoi(val);
        if (key == "--print-parts")   cfg.printPartitions = (val == "1" || val == "true");
        if (key == "--print-graph")   cfg.printGraph      = (val == "1" || val == "true");
    }
 
    // ── Seed RNG ──
    std::mt19937 rng(
        (uint32_t)std::chrono::steady_clock::now().time_since_epoch().count());
 
    // ── Generate graph ──
    printHeader("MAX CUT ALGORITHM SUITE");
    std::cout << "  Generating graph: n=" << cfg.numNodes
              << ", p=" << cfg.edgeProbability
              << ", weighted=" << (cfg.weighted ? "yes" : "no") << "\n\n";
 
    const Graph g = generateGraph(cfg, rng);
    printGraph(g, cfg);
 
    // ── Run algorithms ──
    printHeader("ALGORITHM RESULTS");
 
    std::vector<Result> results;
 
    std::cout << "\n  Running Greedy 1/2...\n";
    results.emplace_back(greedyHalf(g));
    printResult(results.back(), g.totalWeight);

    std::cout << "\n  Running Monte Carlo...\n";
    results.emplace_back(monteCarlo(g, cfg, rng));
    printResult(results.back(), g.totalWeight);

    std::cout << "\n  Running Las Vegas...\n";
    results.emplace_back(lasVegas(g, cfg, rng));
    printResult(results.back(), g.totalWeight);

    std::cout << "\n  Running Derandomized...\n";
    results.emplace_back(derandomized(g));
    printResult(results.back(), g.totalWeight);

    std::cout << "\n  Running GW Classic...\n";
    results.emplace_back(gwClassic(g, cfg, rng));
    printResult(results.back(), g.totalWeight);

    std::cout << "\n  Running GW Multi-Restart...\n";
    results.emplace_back(gwMultiRestart(g, cfg, rng));
    printResult(results.back(), g.totalWeight);

    std::cout << "\n  Running GW Bipartite-Aware...\n";
    results.emplace_back(gwBipartiteAware(g, cfg, rng));
    printResult(results.back(), g.totalWeight);

    std::cout << "\n  Running GW Semi-Random...\n";
    results.emplace_back(gwSemiRandom(g, cfg, rng));
 
    // ── Summary ──
    printComparativeTable(results, g.totalWeight);
    printExperimentalStats(results, g.totalWeight);
    printPartitions(results, g.n, cfg.printPartitions);
 
    printHeader("DONE");
    return 0;
}