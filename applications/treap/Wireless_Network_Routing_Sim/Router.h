#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

struct Router {
    std::string ip_address;
    std::string location;

    // Overloading operators so the Treap's BST property can sort by IP address
    bool operator<(const Router& other) const { return ip_address < other.ip_address; }
    bool operator>(const Router& other) const { return ip_address > other.ip_address; }
    bool operator==(const Router& other) const { return ip_address == other.ip_address; }
};

struct Node {
    Router key;
    double priority; // This will store the signal strength. Higher is better.
    Node *left = nullptr, *right = nullptr;

    Node(Router r, double p) : key(r), priority(p) {}
};