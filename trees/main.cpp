#include "treap.h"
#include "splay.h"

int main() {
    std::vector<int> data = {50, 30, 70, 20, 40, 60, 80};

    std::cout << "--- Executing Treap (BST Rules + Heap Priorities) ---\n";
    Treap<int> treap;
    for (int x : data) treap.insert(x);
    std::cout << "Inorder Treap Output: ";
    treap.printInorder();

    std::cout << "\nRemoving 30 from Treap...\n";
    treap.remove(30);
    std::cout << "Inorder Treap Output: ";
    treap.printInorder();

    std::cout << "\n--- Executing Splay Tree (Amortized Balancing) ---\n";
    SplayTree<int> splay;
    for (int x : data) splay.insert(x);
    std::cout << "Root after sequential additions: " << splay.getRootKey() << " (Expected: 80)\n";

    std::cout << "Searching for element 40...\n";
    splay.search(40);
    std::cout << "New Root after calling search(40): " << splay.getRootKey() << " (Splayed to Root!)\n";

    std::cout << "Inorder Splay Output: ";
    splay.printInorder();

    return 0;
}