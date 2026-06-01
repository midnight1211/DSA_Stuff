#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <algorithm>

struct HuffmanNode {
    char ch;
    int frequency;
    HuffmanNode *left = nullptr, *right = nullptr, *parent = nullptr;

    HuffmanNode(char c, int f) : ch(c), frequency(f) {}
};

class AdaptiveHuffmanTree {
private:
    HuffmanNode* root = nullptr;
    HuffmanNode* leaves[256] = {nullptr}; // Direct O(1) leaf pointers for characters

    void rotateRight(HuffmanNode* x) {
        HuffmanNode* y = x->left;
        x->left = y->right;
        if (y->right) y->right->parent = x;
        y->parent = x->parent;
        if (!x->parent) root = y;
        else if (x == x->parent->right) x->parent->right = y;
        else x->parent->left = y;
        y->right = x;
        x->parent = y;
    }

    void rotateLeft(HuffmanNode* x) {
        HuffmanNode* y = x->right;
        x->right = y->left;
        if (y->left) y->left->parent = x;
        y->parent = x->parent;
        if (!x->parent) root = y;
        else if (x == x->parent->left) x->parent->left = y;
        else x->parent->right = y;
        y->left = x;
        x->parent = y;
    }

    void splay(HuffmanNode* x) {
        while (x->parent) {
            if (!x->parent->parent) {
                if (x == x->parent->left) rotateRight(x->parent);
                else rotateLeft(x->parent);
            } else if (x == x->parent->left && x->parent == x->parent->parent->left) {
                rotateRight(x->parent->parent);
                rotateRight(x->parent);
            } else if (x == x->parent->right && x->parent == x->parent->parent->right) {
                rotateLeft(x->parent->parent);
                rotateLeft(x->parent);
            } else if (x == x->parent->left && x->parent == x->parent->parent->right) {
                rotateRight(x->parent);
                rotateLeft(x->parent);
            } else {
                rotateLeft(x->parent);
                rotateRight(x->parent);
            }
        }
    }

    void clear(HuffmanNode* node) {
        if (!node) return;
        clear(node->left);
        clear(node->right);
        delete node;
    }

public:
    AdaptiveHuffmanTree() = default;
    ~AdaptiveHuffmanTree() { clear(root); }

    void processCharacter(char c, std::vector<bool>& output_bits) {
        HuffmanNode* node = leaves[static_cast<unsigned char>(c)];
        
        if (node) {
            // Step 1: Generate the code path before updating tree geometry
            std::vector<bool> path;
            HuffmanNode* curr = node;
            while (curr->parent) {
                HuffmanNode* p = curr->parent;
                path.push_back(curr == p->right); // false = 0 (left), true = 1 (right)
                curr = p;
            }
            std::reverse(path.begin(), path.end());
            output_bits.insert(output_bits.end(), path.begin(), path.end());

            // Step 2: Increment frequency and adjust caching bias via Splay
            node->frequency++;
            splay(node);
        } else {
            // New character discovered: Insert it into the tree
            HuffmanNode* new_node = new HuffmanNode(c, 1);
            leaves[static_cast<unsigned char>(c)] = new_node;

            if (!root) {
                root = new_node;
            } else {
                // Attach to current tree as a new leaf (simplified BST insert by char value)
                HuffmanNode* curr = root;
                HuffmanNode* parent = nullptr;
                while (curr) {
                    parent = curr;
                    curr = (c < curr->ch) ? curr->left : curr->right;
                }
                new_node->parent = parent;
                if (c < parent->ch) parent->left = new_node;
                else parent->right = new_node;

                splay(new_node);
            }
        }
    }
};