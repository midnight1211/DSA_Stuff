#ifndef TREAP_H
#define TREAP_H

#include <iostream>
#include <random>
#include <vector>

template <typename T>
struct Node {
    T key;
    double priority;
    Node *left = nullptr, *right = nullptr;

    Node(T k, double p = 0.0) : key(k), priority(p) {}
};

template <typename T>
class Treap {
    private:
        Node<T>* root = nullptr;
        std::mt19937 rng;
        std::uniform_real_distribution<double> dist;

        void insertWithPriority(Node<T>*& node, const T& key, double priority);
        void insert(Node<T>*& node, const T& key);
        void remove(Node<T>*& node, const T& key);
        void inorder(Node<T>* node) const;
        void clear(Node<T>* node);

    public:
        Treap() : rng(std::random_device{}()), dist(0.0, 1.0) {}
        ~Treap() { clear(root); }

        void insert(const T& key);
        void remove(const T& key);

        bool search(const T& key) const;
        void printInorder() const;

        void split(Node<T>* t, const T& k, Node<T>*& left, Node<T>*& right);
        Node<T>* join(Node<T>* left, Node<T>* right);

        void rotateRight(Node<T>*& root);
        void rotateLeft(Node<T>*& root);
};

#include "treap.hpp"

#endif