#pragma once
#ifndef BLOOMFILTER_H
#define BLOOMFILTER_H

#include <cstdlib>
#include <iostream>
#include "utils/array.h"
#include "utils/OdsHash.h"

class BloomFilter {
private:
    Array<bool> bit_array;
    Array<unsigned int> z; // Array of k random odd multipliers
    int k;                 // Number of hash functions
    int d;                 // Dimension exponent (Filter size = 2^d)
    const int w = 32;

    // Multiplicative hash function mapping item x through hash function index 'hash_id'
    int hash(int x, int hash_id) const {
        return static_cast<int>((z[hash_id] * ods::hashCode(x)) >> (w - d));
    }

public:
    // Initializes a filter of size 2^d using k hash functions
    BloomFilter(int dimension = 5, int num_hashes = 3) : k(num_hashes), d(dimension) {
        int capacity = 1 << d;
        bit_array = Array<bool>(capacity);
        for (int i = 0; i < capacity; ++i) {
            bit_array[i] = false;
        }

        // Generate k independent random odd multipliers
        z = Array<unsigned int>(k);
        std::srand(42); // Seeded for reproducibility 
        for (int i = 0; i < k; ++i) {
            z[i] = std::rand() | 1;
        }
    }

    // Add an element by flipping all k hash locations to true
    void add(int x) {
        for (int i = 0; i < k; ++i) {
            bit_array[hash(x, i)] = true;
        }
    }

    // Returns false if definitely not in the set, true if probably in the set
    bool find(int x) const {
        for (int i = 0; i < k; ++i) {
            if (!bit_array[hash(x, i)]) {
                return false; // Guaranteed absence
            }
        }
        return true; // Probable presence
    }

    // Visualizes the bit representation of the filter array
    void printFilter() const {
        std::cout << "Bloom Filter Bits: [";
        for (int i = 0; i < bit_array.getLength(); ++i) {
            std::cout << (bit_array[i] ? "1" : "0");
            if (i < bit_array.getLength() - 1) std::cout << " ";
        }
        std::cout << "]" << std::endl;
    }
};

#endif