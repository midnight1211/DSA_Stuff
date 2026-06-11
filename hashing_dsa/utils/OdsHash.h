#pragma once
#ifndef ODSHASH_H
#define ODSHASH_H

#include <cstdint>
#include <string>

namespace ods {
    // 5.3.1: Hashing Primitive Integers (Returns the value itself)
    inline unsigned int hashCode(int x) { return static_cast<unsigned int>(x); }
    inline unsigned int hashCode(unsigned int x) { return x; }
    inline unsigned int hashCode(long long x) { return static_cast<unsigned int>(x ^ (x >> 32)); }

    // 5.3.2: Hashing 3-Tuples (The exact random.org snippet you provided!)
    inline unsigned int hashCode(int x0, int x1, int x2) {
        long long z[] = {0x2058cc50L, 0xcb19137eL, 0x2cb6b6fdL};
        long long zz = 0xbea0107e5067d19dL;
        long long h0 = hashCode(x0);
        long long h1 = hashCode(x1);
        long long h2 = hashCode(x2);
        return static_cast<unsigned int>(((z[0]*h0 + z[1]*h1 + z[2]*h2)*zz) >> 32);
    }

    // 5.3.3: Hashing Strings (Using a base-p polynomial breakdown)
    inline unsigned int hashCode(const std::string& s) {
        unsigned int h = 0;
        // 0xdeadbeef acts as a pseudo-random seed multiplier
        unsigned int p = 0xdeadbeef | 1; 
        for (char c : s) {
            h = p * h + static_cast<unsigned int>(c);
        }
        return h;
    }
}

#endif