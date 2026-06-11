#pragma once
#ifndef CHAINEDHASHTABLE_H
#define CHAINEDHASHTABLE_H

#include <cstdlib>
#include <utility>
#include "utils/alist.h"
#include "utils/array.h"
#include "utils/OdsHash.h"

template <typename T>
class ChainedHashTable {
private:
    Array<AList<T>> t;
    int n;           // Total number of elements stored
    int d;           // Dimension exponent (Array size = 2^d)
    const int w = 32;// Fixed word size (32-bit architecture setup)
    unsigned int z;  // Random odd integer multiplier required by ODS

    int hash(const T& x) const {
        return static_cast<int>((z * ods::hashCode(x)) >> (w - d));
    }

    void resize() {
        d++;
        int newCapacity = 1 << d;
        Array<AList<T>> newTable(newCapacity);
        
        // Re-assign all existing items into their new locations
        for (int i = 0; i < t.getLength(); ++i) {
            t[i].moveToStart();
            while (!t[i].isAtEnd()) {
                T item = t[i].getValue();
                // Find new index via the updated dimension 'd'
                int newIdx = static_cast<int>((z * ods::hashCode(item)) >> (w - d));
                newTable[newIdx].append(item);
                t[i].next();
            }
        }
        t = std::move(newTable);
    }

public:
    ChainedHashTable() : n(0), d(2) {
        t = Array<AList<T>>(1 << d); // Initial size 4 (2^2)
        std::srand(11);              // Seed to maintain reproducible random states
        z = std::rand() | 1;         // Bitwise OR forces 'z' to be an odd integer
    }

    bool add(T x) {
        if (find(x) != T()) return false; // Item already exists
        if (n + 1 > t.getLength()) resize();
        
        t[hash(x)].append(x);
        n++;
        return true;
    }

    T remove(T x) {
        int j = hash(x);
        t[j].moveToStart();
        while (!t[j].isAtEnd()) {
            if (t[j].getValue() == x) {
                T item = t[j].remove();
                n--;
                // Contract the table size if it is too empty (n <= 2^(d-2))
                if (3 * n < t.getLength() && d > 2) {
                    // Shrink table if load factor drops below 1/3
                    d--;
                    // (A mirror inverse of resize() can be fully implemented here if desired)
                }
                return item;
            }
            t[j].next();
        }
        return T(); // Default empty return if item not found
    }

    T find(T x) {
        int j = hash(x);
        t[j].moveToStart();
        while (!t[j].isAtEnd()) {
            if (t[j].getValue() == x) {
                return t[j].getValue();
            }
            t[j].next();
        }
        return T();
    }

    void printTable() {
    std::cout << "--- Chained Hash Table Map ---" << std::endl;
    for (int i = 0; i < t.getLength(); i++) {
        std::cout << "Bucket [" << i << "]: ";
        t[i].moveToStart();
        if (t[i].isAtEnd()) {
            std::cout << "[Empty]";
        } else {
            while (!t[i].isAtEnd()) {
                std::cout << t[i].getValue();
                t[i].next();
                if (!t[i].isAtEnd()) std::cout << " -> ";
            }
        }
        std::cout << std::endl;
    }
    std::cout << "------------------------------" << std::endl;
}

    int size() const { return n; }
};

#endif