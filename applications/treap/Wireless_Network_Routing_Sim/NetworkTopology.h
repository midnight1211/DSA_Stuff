#include "Router.h"

class NetworkTopology {
    private:
        Node* root = nullptr;

        void clear(Node* node) {
            if (!node) return;
            clear(node->left);
            clear(node->right);
            delete node;
        }

        // Print routing tables ordered by IP address (Inorder BST traversal)
        void inorder(Node* node) const {
            if (!node) return;
            inorder(node->left);
            std::cout << "  IP: " << node->key.ip_address 
                      << " | Location: " << node->key.location 
                      << " | Signal: " << node->priority << " dBm\n";
            inorder(node->right);
        }

    public:
        NetworkTopology() = default;
        ~NetworkTopology() { clear(root); }

        Node* getRoot() const { return root; }
        void printRoutingTable() const { inorder(root); }

        // Core Rotations
        void rotateRight(Node*& rootRef) {
            Node* child = rootRef->left;
            rootRef->left = child->right;
            child->right = rootRef;
            rootRef = child;
        }

        void rotateLeft(Node*& rootRef) {
            Node* child = rootRef->right;
            rootRef->right = child->left;
            child->left = rootRef;
            rootRef = child;
        }

        // Standard BST Insertion + Max-Heap prioritization based on signal strength
        void insertRouter(Node*& node, const Router& router, double signal_strength) {
            if (!node) {
                node = new Node(router, signal_strength);
                return;
            }

            if (router < node->key) {
                insertRouter(node->left, router, signal_strength);
                // If child has a STRONGER signal (higher value closer to 0), rotate it up
                if (node->left->priority > node->priority) {
                    rotateRight(node);
                }
            } else if (router > node->key) {
                insertRouter(node->right, router, signal_strength);
                if (node->right->priority > node->priority) {
                    rotateLeft(node);
                }
            } else {
                // Router already exists! Update its signal strength in place
                node->priority = signal_strength;
            }
        }

        // Public wrapper for insertion
        void registerRouter(const Router& r, double signal) {
            insertRouter(root, r, signal);
        }

        // O(log N) lookup to see if an IP address exists in our grid
        Node* locateRouter(const std::string& ip) const {
            Node* curr = root;
            while (curr) {
                if (ip == curr->key.ip_address) return curr;
                curr = (ip < curr->key.ip_address) ? curr->left : curr->right;
            }
            return nullptr;
        }

        // O(1) Access to the absolute best gateway router currently online
        void printPrimaryGateway() const {
            if (!root) {
                std::cout << "No active network gateway found.\n";
                return;
            }
            std::cout << ">>> PRIMARY GATEWAY: " << root->key.location 
                      << " (" << root->key.ip_address << ") @ " 
                      << root->priority << " dBm <<<\n";
        }
};