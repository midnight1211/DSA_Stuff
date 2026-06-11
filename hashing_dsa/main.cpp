#include <iostream>
#include <cassert>
#include <string>
#include "ChainedHashTable.h"
#include "LinearHashTable.h"
#include "utils/OdsHash.h"

// Simple helper to print test results
void printResult(const std::string& testName, bool success) {
    std::cout << "[" << (success ? "PASS" : "FAIL") << "] " << testName << std::endl;
}

void testChainedHashTable() {
    std::cout << "--- Testing ChainedHashTable ---" << std::endl;
    
    ChainedHashTable<int> ht;
    
    // Test 1: Insertion
    bool a1 = ht.add(15);
    bool a2 = ht.add(32);
    bool a3 = ht.add(99);
    bool a4 = ht.add(15); // Duplicate, should fail
    
    printResult("Insert distinct items", a1 && a2 && a3);
    printResult("Reject duplicate item", !a4);
    printResult("Size check after inserts", ht.size() == 3);
    ht.printTable();

    // Test 2: Retrieval / Finding
    printResult("Find existing item (32)", ht.find(32) == 32);
    printResult("Find missing item (55)", ht.find(55) == 0); // 0 is default int fallback

    // Test 3: Removal
    int removed = ht.remove(32);
    printResult("Remove existing item (32)", removed == 32);
    printResult("Size check after removal", ht.size() == 2);
    printResult("Verify item is gone", ht.find(32) == 0);
    ht.printTable();

    // Test 4: Triggering internal resize
    // Initial capacity is 4 (2^2). Adding more elements forces d to increment.
    for (int i = 200; i < 210; ++i) {
        ht.add(i);
    }
    printResult("Bulk insert and automatic resizing", ht.size() == 12);
    printResult("Find item after table resize", ht.find(205) == 205);
    ht.printTable();
    
    std::cout << std::endl;
}

void testLinearHashTable() {
    std::cout << "--- Testing LinearHashTable ---" << std::endl;

    // Standard ODS requirements: define sentinel tokens for empty vs deleted entries.
    int emptySentinel = -1;
    int deletedSentinel = -2;
    
    LinearHashTable<int> ht(emptySentinel, deletedSentinel);

    // Test 1: Insertion
    bool a1 = ht.add(10);
    bool a2 = ht.add(25);
    bool a3 = ht.add(40); // Might collide depending on random z seed, testing probing
    bool a4 = ht.add(10); // Duplicate check

    printResult("Insert distinct items", a1 && a2 && a3);
    printResult("Reject duplicate item", !a4);
    printResult("Size check after inserts", ht.size() == 3);
    ht.printTable();

    // Test 2: Retrieval
    printResult("Find existing item (25)", ht.find(25) == 25);
    printResult("Find missing item (100)", ht.find(100) == emptySentinel);

    // Test 3: Removal (Tombstone validation)
    int removed = ht.remove(25);
    printResult("Remove item (leaves tombstone)", removed == 25);
    printResult("Size check updates", ht.size() == 2);
    printResult("Verify item is unfindable", ht.find(25) == emptySentinel);
    ht.printTable();
    
    // Verify that linear probing still skips tombstones correctly to find later elements
    printResult("Search path intact past tombstone", ht.find(40) == 40);

    // Test 4: Heavy load capacity resizing
    for (int i = 50; i < 70; ++i) {
        ht.add(i);
    }
    printResult("Bulk insertion past safety thresholds", ht.size() == 22);
    printResult("Find item inside resized storage array", ht.find(62) == 62);
    ht.printTable();
    
    std::cout << std::endl;
}

int main() {
    // Run the full test suites
    testChainedHashTable();
    testLinearHashTable();

    std::cout << "All Hashing components executed." << std::endl;
    return 0;
}