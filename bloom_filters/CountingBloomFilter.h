#pragma once
#ifndef COUNTINGBLOOMFILTER_H
#define COUNTINGBLOOMFILTER_H

#include <cstdlib>
#include <iostream>
#include "utils/array.h"
#include "utils/OdsHash.h"

class CountingBloomFilter {
private:
    Array<int> counter_array;
    Array<unsigned int> z; 
    int k;                 
    int d;                 
    const int w = 32;

    int hash(int x, int hash_id) const {
        return static_cast<int>((z[hash_id] * ods::hashCode(x)) >> (w - d));
    }

public:
    CountingBloomFilter(int dimension = 5, int num_hashes = 3) : k(num_hashes), d(dimension) {
        int capacity = 1 << d;
        counter_array = Array<int>(capacity);
        for (int i = 0; i < capacity; ++i) {
            counter_array[i] = 0;
        }

        z = Array<unsigned int>(k);
        std::srand(84); 
        for (int i = 0; i < k; ++i) {
            z[i] = std::rand() | 1;
        }
    }

    // Add an element by incrementing all k hashed positions
    void add(int x) {
        for (int i = 0; i < k; ++i) {
            counter_array[hash(x, i)]++;
        }
    }

    // Remove an element by decrementing all k hashed positions
    void remove(int x) {
        // Validation step ensures we do not decrement below zero due to a false positive remove call
        if (!find(x)) return; 
        
        for (int i = 0; i < k; ++i) {
            if (counter_array[hash(x, i)] > 0) {
                counter_array[hash(x, i)]--;
            }
        }
    }

    // Find queries if all k mapped positions have a counter greater than zero
    bool find(int x) const {
        for (int i = 0; i < k; ++i) {
            if (counter_array[hash(x, i)] <= 0) {
                return false; 
            }
        }
        return true; 
    }

    // Visualizes the internal numerical saturation across storage indexes
    void printFilter() const {
        std::cout << "Counting Bloom Filter Counters: [";
        for (int i = 0; i < counter_array.getLength(); ++i) {
            std::cout << counter_array[i];
            if (i < counter_array.getLength() - 1) std::cout << " ";
        }
        std::cout << "]" << std::endl;
    }
};

#endif