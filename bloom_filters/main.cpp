#include <iostream>
#include "BloomFilter.h"
#include "CountingBloomFilter.h"

int main() {
    std::cout << "==========================================" << std::endl;
    std::cout << "          STANDARD BLOOM FILTER           " << std::endl;
    std::cout << "==========================================" << std::endl;

    // Dimension 4 implies 2^4 = 16 bit slots, using 3 hash functions
    BloomFilter bf(4, 3);

    std::cout << "Initial state:" << std::endl;
    bf.printFilter();

    std::cout << "\nAdding items: 12, 55, 87..." << std::endl;
    bf.add(12);
    bf.add(55);
    bf.add(87);
    bf.printFilter();

    std::cout << "\nQuery Membership Tests:" << std::endl;
    std::cout << "Find 12 (Expected: 1/True): " << bf.find(12) << std::endl;
    std::cout << "Find 55 (Expected: 1/True): " << bf.find(55) << std::endl;
    std::cout << "Find 99 (Expected: 0/False - unless collision occurs): " << bf.find(99) << std::endl;


    std::cout << "\n==========================================" << std::endl;
    std::cout << "          COUNTING BLOOM FILTER           " << std::endl;
    std::cout << "==========================================" << std::endl;

    CountingBloomFilter cbf(4, 3);

    std::cout << "Initial state:" << std::endl;
    cbf.printFilter();

    std::cout << "\nAdding items: 12, 12 (duplicate), 44..." << std::endl;
    cbf.add(12);
    cbf.add(12); // Duplicate increments counters at its hashed spots
    cbf.add(44);
    cbf.printFilter();

    std::cout << "\nQuery Membership Tests Before Removal:" << std::endl;
    std::cout << "Find 12 (Expected: 1/True): " << cbf.find(12) << std::endl;
    std::cout << "Find 44 (Expected: 1/True): " << cbf.find(44) << std::endl;

    std::cout << "\nRemoving one instance of 12..." << std::endl;
    cbf.remove(12);
    cbf.printFilter();
    std::cout << "Find 12 after first removal (Expected: 1/True due to duplicate): " << cbf.find(12) << std::endl;

    std::cout << "\nRemoving the second instance of 12..." << std::endl;
    cbf.remove(12);
    cbf.printFilter();
    std::cout << "Find 12 after final removal (Expected: 0/False): " << cbf.find(12) << std::endl;
    std::cout << "Find 44 (Expected: 1/True): " << cbf.find(44) << std::endl;

    return 0;
}