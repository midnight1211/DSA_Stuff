#include <iostream>
#include <string>
#include <vector>
#include <iomanip>

struct VMRegion {
    unsigned long long start_addr;
    unsigned long long end_addr;
    std::string mapping_name;
};

struct SplayNode {
    VMRegion region;
    SplayNode *left = nullptr, *right = nullptr;
    SplayNode(VMRegion r) : region(r) {}
};

class KernelVMM {
private:
    SplayNode* root = nullptr;

    SplayNode* rotateRight(SplayNode* x) {
        SplayNode* y = x->left;
        x->left = y->right;
        y->right = x;
        return y;
    }

    SplayNode* rotateLeft(SplayNode* x) {
        SplayNode* y = x->right;
        x->right = y->left;
        y->left = x;
        return y;
    }

    // Splays based on whether an address drops INSIDE a target node's range boundaries
    SplayNode* splayAddress(SplayNode* h, unsigned long long target_addr) {
        if (!h) return nullptr;

        // Found exact range intersection!
        if (target_addr >= h->region.start_addr && target_addr <= h->region.end_addr) {
            return h;
        }

        if (target_addr < h->region.start_addr) {
            if (!h->left) return h;
            
            // Left Left
            if (target_addr < h->left->region.start_addr) {
                h->left->left = splayAddress(h->left->left, target_addr);
                h = rotateRight(h);
            }
            // Left Right
            else if (h->left->right && target_addr > h->left->region.end_addr) {
                h->left->right = splayAddress(h->left->right, target_addr);
                h->left = rotateLeft(h->left);
            }
            if (!h->left) return h;
            return rotateRight(h);
        } else {
            if (!h->right) return h;

            // Right Left
            if (h->right->left && target_addr < h->right->region.start_addr) {
                h->right->left = splayAddress(h->right->left, target_addr);
                h->right = rotateRight(h->right);
            }
            // Right Right
            else if (target_addr > h->right->region.end_addr) {
                h->right->right = splayAddress(h->right->right, target_addr);
                h = rotateLeft(h);
            }
            if (!h->right) return h;
            return rotateLeft(h);
        }
    }

public:
    KernelVMM() = default;

    void mmap_allocate(unsigned long long start, unsigned long long end, std::string name) {
        VMRegion r{start, end, name};
        if (!root) {
            root = new SplayNode(r);
            return;
        }
        root = splayAddress(root, start);
        
        SplayNode* new_node = new SplayNode(r);
        if (start < root->region.start_addr) {
            new_node->right = root;
            new_node->left = root->left;
            root->left = nullptr;
        } else {
            new_node->left = root;
            new_node->right = root->right;
            root->right = nullptr;
        }
        root = new_node;
    }

    // Resolves memory boundaries via O(Amortized log N) interval search
    VMRegion* resolve_pointer(unsigned long long addr) {
        if (!root) return nullptr;
        root = splayAddress(root, addr);
        
        // Post-Splay intersection check
        if (addr >= root->region.start_addr && addr <= root->region.end_addr) {
            return &(root->region);
        }
        return nullptr;
    }
};