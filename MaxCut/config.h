#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <numeric>
#include <random>
#include <chrono>
#include <cmath>
#include <cassert>
#include <climits>
#include <functional>
#include <map>

// ---------------------------------------------
// CONFIGURATION
// ---------------------------------------------

struct Config {
    int    numNodes            = 100;     // nodes in generated graph (≤10000)
    double edgeProbability     = 0.15;    // Erdős-Rényi p
    bool   weighted            = true;    // use random integer weights 1-100
    int    monteCarloTrials    = 500;     // MC random assignments
    int    lasVegasMaxTrials   = 2000;    // LV restarts cap
    double lasVegasTargetRatio = 0.80;    // LV stops when cut/totalWeight > this
    int     gwRestarts         = 20;      // GW random-restart count
    int     gwSdpIterations    = 300;     // Cholesky power-iteration count
    bool    printGraph         = true;    // print adjacency list
    bool    printPartitions    = false;   // print node assignments (verbose)
};

// ----------------------------------------------
// RESULT STRUCT
// ----------------------------------------------

struct Result {
    std::string name;
    long long   cutValue = 0;
    double      ratio     = 0.0;             // cut / totalWeight
    double      runtimeMs = 0.0;
    int         iterations = 0;              // where applicable
    std::vector<int> partition;              // 0/1 for each node
    std::string notes;
};

// ----------------------------------------------
// TIMER HELPER
// ----------------------------------------------

using Clock = std::chrono::high_resolution_clock;

struct Timer {
    std::chrono::time_point<Clock> start;
    Timer() : start(Clock::now()) {}
    double ms() const {
        return std::chrono::duration<double, std::milli>(Clock::now() - start).count();
    }
};