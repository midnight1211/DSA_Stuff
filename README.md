# Data Structures and Algorithms Implementations

[![Ask DeepWiki](https://devin.ai/assets/askdeepwiki.png)](https://deepwiki.com/midnight1211/DSA_Stuff)

This repository contains C++ and Python implementations of various data structures and algorithms, complete with usage examples and practical applications. The projects cover topics like approximation algorithms for Max-Cut, probabilistic data structures like Bloom Filters, self-balancing trees (Splay Trees, Treaps), hashing techniques, and primality testing.

## Table of Contents

- [1. Max-Cut Algorithms](#1-max-cut-algorithms)
- [2. Trees: Splay & Treap](#2-trees-splay--treap)
- [3. Hashing Data Structures](#3-hashing-data-structures)
- [4. Bloom Filters](#4-bloom-filters)
- [5. Primality Tests](#5-primality-tests)
- [6. Applications](#6-applications)
  - [Splay Tree Applications](#splay-tree-applications)
  - [Treap Applications](#treap-applications)
- [How to Build and Run](#how-to-build-and-run)
- [License](#license)

## 1. Max-Cut Algorithms

The `MaxCut/` directory provides a C++ suite for solving the Max-Cut problem on weighted, undirected graphs. The goal of Max-Cut is to partition the vertices of a graph into two disjoint sets, S and T, such that the sum of the weights of the edges crossing the partition is maximized.

This implementation includes several approximation algorithms:

*   **Greedy 1/2-Approximation**: A deterministic algorithm that iterates through nodes, placing each in the partition that greedily maximizes the current cut weight. It guarantees a cut of at least OPT/2.
*   **Monte Carlo Randomized**: Assigns each node to a partition uniformly at random. The expected cut weight is W/2, where W is the total weight of all edges. The best result from many trials is kept.
*   **Las Vegas Randomized**: A variation of the Monte Carlo method that retries random assignments until a cut exceeding a predefined threshold is found, or a maximum number of trials is reached.
*   **Derandomized (Method of Conditional Expectations)**: A deterministic algorithm that sequentially assigns nodes to partitions by choosing the side that maximizes the *expected* final cut value, given previous assignments. This provides the same ≥ W/2 guarantee as the randomized methods.
*   **Goemans-Williamson 0.878-Approximation**: An advanced algorithm based on Semidefinite Programming (SDP) relaxation.
    *   **Vector Relaxation**: Instead of assigning nodes to {+1, -1}, each node is represented by a unit vector *v_i* in ℝⁿ.
    *   **SDP Solver**: An iterative gradient ascent method is used to approximate the solution to the SDP.
    *   **Random Hyperplane Rounding**: A random unit vector *r* is generated, and each node *i* is partitioned based on the sign of the dot product *v_i ⋅ r*.
    *   **Variants Implemented**:
        *   `Classic`: A single SDP solve followed by a single rounding.
        *   `Multi-Restart`: Performs multiple independent SDP solves and takes the best result.
        *   `Bipartite-Aware`: Biases the rounding threshold based on how antipodal a node's embedding is to its neighbors, exploiting near-bipartite structures.
        *   `Semi-Random`: Perturbs the vector embeddings with Gaussian noise before rounding to explore the solution space.

The `main.cpp` file generates a random graph, runs all implemented algorithms, and prints a comparative summary of their performance, cut quality, and runtime.

## 2. Trees: Splay & Treap

The `trees/` directory contains generic C++ implementations of Splay Trees and Treaps.

### Splay Tree
A self-balancing binary search tree that provides amortized O(log n) time complexity for standard operations. Its key feature is the **splay operation**: whenever a node is accessed (for search, insertion, or deletion), it is moved to the root position through a series of rotations. This keeps frequently accessed elements near the top of thetree for faster access.

*   `splay()`: The core operation that moves a node to the root.
*   `insert()`, `search()`, `remove()`: Standard BST operations enhanced with splaying.
*   `split()`, `join()`: Efficiently split a tree into two or merge two trees.

### Treap
A randomized binary search tree that maintains the BST property with respect to keys and the heap property with respect to randomly assigned priorities. This structure ensures the tree remains balanced on average, providing O(log n) expected time complexity for operations.

*   **BST Property**: For any node, all keys in its left subtree are smaller, and all keys in its right subtree are larger.
*   **Heap Property**: For any node, its priority is greater than or equal to the priorities of its children (max-heap).
*   Operations include `insert()`, `remove()`, `split()`, and `join()`.

## 3. Hashing Data Structures

The `hashing_dsa/` directory contains C++ implementations of two fundamental hash table strategies for collision resolution, based on the "Open Data Structures" textbook.

*   **Chained Hash Table**: Resolves collisions by storing all elements that hash to the same bucket in a linked list (`AList`). The table dynamically resizes (doubles or halves its capacity) to maintain an efficient load factor.
*   **Linear Probing Hash Table**: An open-addressing method where collisions are resolved by searching sequentially for the next empty slot in the array. This implementation uses "tombstone" markers (`del_val`) to handle deletions correctly without breaking search chains and resizes when the table becomes too full or too empty.

## 4. Bloom Filters

The `bloom_filters/` directory contains C++ implementations of two variants of the Bloom filter, a space-efficient probabilistic data structure used to test whether an element is a member of a set. It is known for allowing false positives but no false negatives.

*   **Standard Bloom Filter**: The classic implementation using a bit array and *k* independent hash functions. An element is "added" by setting *k* bits to 1, and it "may be in the set" if all *k* bits are 1 upon checking.
*   **Counting Bloom Filter**: An extension that supports element deletion. Instead of a bit array, it uses an array of counters. Adding an element increments *k* counters, and removing an element decrements them. This prevents the "false negatives" that would arise if bits were simply flipped back to 0 in a standard filter shared by multiple elements.

## 5. Primality Tests

The `primality_tests/primality_tests.py` script provides Python implementations and performance comparisons for several primality testing algorithms.

*   **Trial Division**: A deterministic O(sqrt(n)) algorithm that checks for divisibility by all integers up to the square root of n. It is precise but slow for large numbers.
*   **Fermat's Primality Test**: A probabilistic test based on Fermat's Little Theorem. It is fast but can be fooled by composite numbers known as *Carmichael numbers*.
*   **Miller-Rabin Primality Test**: A more sophisticated and widely used probabilistic test that is not vulnerable to Carmichael numbers. It is the industry standard for generating large primes in cryptography.

The script includes experiments that benchmark the runtime of each algorithm and demonstrate the failure of the Fermat test on Carmichael numbers, highlighting the robustness of the Miller-Rabin test.

## 6. Applications

This repository demonstrates how these data structures can be applied to solve real-world problems.

### Splay Tree Applications

Located in `applications/splay_trees/`, these simulations showcase the benefits of the splay tree's self-optimizing nature.

*   **Kernel VMM Simulation (`kernel/`)**: Simulates a kernel's virtual memory manager. A splay tree maps memory address ranges (e.g., text segment, stack, heap) to their descriptions. When a pointer is resolved, the corresponding memory region is splayed to the root, optimizing lookups for code that exhibits locality of reference.
*   **Network Route Caching (`network/`)**: Models an OSPF route cache in a network router. IP subnets are stored in a splay tree. When a packet is routed, the corresponding route is splayed, ensuring that high-traffic destinations are found quickly.
*   **Adaptive Huffman Coding (`huffman/`)**: Implements an adaptive Huffman compression engine. The frequency tree is a splay tree that dynamically adjusts as characters are processed. Frequently occurring characters are splayed closer to the root, shortening their codes over time and adapting to the input stream's statistics.

### Treap Applications

Located in `applications/treap/`, these simulations use treaps for their combination of BST ordering and randomized balancing.

*   **Text Editor Rope Engine (`Text_Editor_Rope_Engine/`)**: Implements a **Rope** data structure using an implicit-key treap. This allows for extremely fast insertion, deletion, and slicing of large text documents (e.g., in a code editor) without the need for large, contiguous memory allocations or costly array-copying operations. It can render a small "viewport" of a gigabyte-sized file instantly.
*   **Wireless Mesh Network Simulator (`Wireless_Network_Routing_Sim/`)**: Simulates an ad-hoc wireless network. The treap uses IP addresses for its BST key and signal strength (RSSI) for its heap priority. This unique combination allows for both efficient O(log n) lookups of any router by IP and instant O(1) access to the router with the strongest signal (the root of the treap), which acts as the primary gateway.

## How to Build and Run

### C++ Projects
The C++ projects are self-contained. You can compile and run each `main.cpp` file using a modern C++ compiler like g++.

For example, to run the Max-Cut simulation:
```bash
cd MaxCut/
g++ -std=c++17 -O2 main.cpp -o maxcut_runner
./maxcut_runner
```
You can pass command-line arguments to customize the graph generation and algorithm parameters:
```bash
# Generate a graph with 200 nodes and run all algorithms
./maxcut_runner --nodes=200 --prob=0.1

# Run with more Monte Carlo trials
./maxcut_runner --mc-trials=10000
```
The same approach can be used for the other C++ projects in `hashing_dsa/`, `trees/`, and `applications/`.

### Python Scripts
To run the primality testing script, simply use the Python interpreter:
```bash
cd primality_tests/
python primality_tests.py
```

## License

This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for details.
