#ifndef SPLAY_H
#define SPLAY_H

#include <iostream>
#include <random>
#include <vector>

template <typename T>
struct SNode {
    T key;
    SNode *left = nullptr, *right = nullptr;
    SNode(T k) : key(k) {}
};

template <typename T>
class SplayTree {
    private:
        SNode<T>* root = nullptr;

        SNode<T>* splay(SNode<T>* head, const T& key);
        void inorder(SNode<T>* node) const;
        void clear(SNode<T>* node);

    public:
        SplayTree() = default;
        ~SplayTree() { clear(root); }
        
        bool search(const T& key);

        void insert(const T& key);
        void insertViaSplit(const T& key);
        void deleteViaJoin(const T& key);
        void remove(const T& key);
        void printInorder() const;
        T getRootKey() const;
        void rotateRight(SNode<T>*& root);
        void rotateLeft(SNode<T>*& root);
        void split(SNode<T>* t, const T& k, SNode<T>*& left, SNode<T>*& right);
        SNode<T>* join(SNode<T>* left, SNode<T>* right);
};

#include "splay.hpp"

#endif