#pragma once
#ifndef LINEARHASHTABLE_H
#define LINEARHASHTABLE_H

#include <cstdlib>
#include <utility>
#include "utils/array.h"
#include "utils/OdsHash.h"

template <typename T>
class LinearHashTable {
private:
    Array<T> t;
    int n;          // Count of actual active elements
    int q;          // Count of non-empty slots (active elements + 'del' tombstones)
    int d;          // Dimension exponent
    const int w = 32;
    unsigned int z;
    
    T null_val;     // Constant denoting a completely empty, unvisited slot
    T del_val;      // Constant acting as a "Tombstone" indicator for removed items

    int hash(const T& x) const {
        return static_cast<int>((z * ods::hashCode(x)) >> (w - d));
    }

    void resize() {
        d = 1;
        while ((1 << d) < 3 * n) d++; // Pick a d value where load factor remains small
        
        int newCapacity = 1 << d;
        Array<T> newTable(newCapacity);
        for (int i = 0; i < newCapacity; ++i) {
            newTable[i] = null_val;
        }

        for (int i = 0; i < t.getLength(); ++i) {
            if (t[i] != null_val && t[i] != del_val) {
                int j = static_cast<int>((z * ods::hashCode(t[i])) >> (w - d));
                while (newTable[j] != null_val) {
                    j = (j == newTable.getLength() - 1) ? 0 : j + 1;
                }
                newTable[j] = t[i];
            }
        }
        t = std::move(newTable);
        q = n; // Tombstones are discarded completely during rehash cycles
    }

public:
    LinearHashTable(T emptySentinel, T deletedSentinel) 
        : n(0), q(0), d(2), null_val(emptySentinel), del_val(deletedSentinel) {
        t = Array<T>(1 << d);
        for (int i = 0; i < t.getLength(); ++i) {
            t[i] = null_val;
        }
        std::srand(23);
        z = std::rand() | 1;
    }

    bool add(T x) {
        if (find(x) != null_val) return false;
        // If array is 50% saturated with items/tombstones, proactively clear space
        if (2 * (q + 1) > t.getLength()) resize();

        int i = hash(x);
        while (t[i] != null_val && t[i] != del_val) {
            i = (i == t.getLength() - 1) ? 0 : i + 1; // Loop smoothly around array boundaries
        }
        if (t[i] == null_val) q++;
        t[i] = x;
        n++;
        return true;
    }

    T remove(T x) {
        int i = hash(x);
        while (t[i] != null_val) {
            if (t[i] != del_val && t[i] == x) {
                T savedItem = t[i];
                t[i] = del_val; // Insert a tombstone tracking flag
                n--;
                if (8 * n < t.getLength()) resize(); // Shrink array if severely underpopulated
                return savedItem;
            }
            i = (i == t.getLength() - 1) ? 0 : i + 1;
        }
        return null_val;
    }

    T find(T x) {
        int i = hash(x);
        while (t[i] != null_val) {
            if (t[i] != del_val && t[i] == x) {
                return t[i];
            }
            i = (i == t.getLength() - 1) ? 0 : i + 1;
        }
        return null_val;
    }

    void printTable() {
    std::cout << "--- Linear Probing Hash Table Map ---" << std::endl;
    for (int i = 0; i < t.getLength(); i++) {
        std::cout << "Slot [" << i << "]: ";
        if (t[i] == null_val) {
            std::cout << "[Empty / Unvisited]";
        } else if (t[i] == del_val) {
            std::cout << "[DEL / Tombstone]";
        } else {
            std::cout << t[i];
        }
        std::cout << std::endl;
    }
    std::cout << "-------------------------------------" << std::endl;
}

    int size() const { return n; }
};

#endif