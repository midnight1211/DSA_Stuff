#include <iostream>
#include <random>
#include <string>
#include <limits>
#include <algorithm>

// ============================================================================
// 1. THE IMPLICIT TREAP NODE
// ============================================================================
struct Node {
    std::string fragment;
    double priority;
    int size; // Total number of characters in this node's entire subtree
    Node *left = nullptr, *right = nullptr;

    Node(std::string text, double p = 0.0) 
        : fragment(text), priority(p), size(text.length()) {}
};

// ============================================================================
// 2. THE SCALABLE ROPE ENGINE (IMPLICIT TREAP)
// ============================================================================
class ScalableRope {
    private:
        Node* root = nullptr;
        std::mt19937 rng;
        std::uniform_real_distribution<double> dist;

        int getSize(Node* node) const { return node ? node->size : 0; }

        void updateSize(Node* node) {
            if (node) {
                node->size = getSize(node->left) + getSize(node->right) + node->fragment.length();
            }
        }

        void clear(Node* node) {
            if (!node) return;
            clear(node->left);
            clear(node->right);
            delete node;
        }

        // Internal localized printing function that prints only what it sees
        void inorderPrint(Node* node) const {
            if (!node) return;
            inorderPrint(node->left);
            std::cout << node->fragment;
            inorderPrint(node->right);
        }

    public:
        ScalableRope() : rng(std::random_device{}()), dist(0.0, 1.0) {}
        ~ScalableRope() { clear(root); }

        Node* getRoot() const { return root; }
        void setRoot(Node* new_root) { root = new_root; }
        int totalCharacters() const { return getSize(root); }

        void rotateRight(Node*& rootRef) {
            Node* child = rootRef->left;
            rootRef->left = child->right;
            child->right = rootRef;
            updateSize(rootRef);
            updateSize(child);
            rootRef = child;
        }

        void rotateLeft(Node*& rootRef) {
            Node* child = rootRef->right;
            rootRef->right = child->left;
            child->left = rootRef;
            updateSize(rootRef);
            updateSize(child);
            rootRef = child;
        }

        // Split the tree by character index
        void split(Node* t, int idx, Node*& left, Node*& right) {
            if (!t) {
                left = nullptr;
                right = nullptr;
                return;
            }

            int left_subtree_chars = getSize(t->left);

            if (idx <= left_subtree_chars) {
                split(t->left, idx, left, t->left);
                right = t;
                updateSize(right);
            } 
            else if (idx > left_subtree_chars && idx < left_subtree_chars + (int)t->fragment.length()) {
                int cut_point = idx - left_subtree_chars;
                std::string left_chunk = t->fragment.substr(0, cut_point);
                std::string right_chunk = t->fragment.substr(cut_point);

                Node* right_node = new Node(right_chunk, dist(rng));
                right_node->right = t->right;
                
                t->fragment = left_chunk;
                t->right = nullptr;

                left = t;
                right = right_node;
                
                updateSize(left);
                updateSize(right);
            } 
            else {
                int remaining_idx = idx - left_subtree_chars - t->fragment.length();
                split(t->right, remaining_idx, t->right, right);
                left = t;
                updateSize(left);
            }
        }

        // Merge two trees together
        Node* join(Node* left, Node* right) {
            if (!left) return right;
            if (!right) return left;

            if (left->priority > right->priority) {
                left->right = join(left->right, right);
                updateSize(left);
                return left;
            } else {
                right->left = join(left, right->left);
                updateSize(right);
                return right;
            }
        }

        void insert(int char_idx, const std::string& text) {
            if (char_idx < 0 || char_idx > totalCharacters()) char_idx = totalCharacters();
            Node* left_tree = nullptr;
            Node* right_tree = nullptr;

            split(root, char_idx, left_tree, right_tree);
            Node* new_node = new Node(text, dist(rng));
            root = join(join(left_tree, new_node), right_tree);
        }

        void deleteRange(int start_idx, int length) {
            if (start_idx < 0 || start_idx >= totalCharacters() || length <= 0) return;
            if (start_idx + length > totalCharacters()) length = totalCharacters() - start_idx;

            Node* left_tree = nullptr;
            Node* middle_tree = nullptr;
            Node* right_tree = nullptr;

            split(root, start_idx, left_tree, right_tree);
            split(right_tree, length, middle_tree, right_tree);

            clear(middle_tree);
            root = join(left_tree, right_tree);
        }

        // SCALABLE RENDERING: Slices out only the target range, prints it, and puts it back.
        // Zero memory copies of the entire document string are made.
        void renderPage(int start_idx, int num_chars) const {
            if (totalCharacters() == 0) {
                std::cout << "[Empty Document]\n";
                return;
            }

            // Const cast helper to allow splitting/joining on a logical const render operation
            ScalableRope* mutable_this = const_cast<ScalableRope*>(this);

            Node* left_invisible = nullptr;
            Node* visible_screen = nullptr;
            Node* right_invisible = nullptr;

            // 1. Isolate the viewport text chunk in O(log N) time
            mutable_this->split(mutable_this->root, start_idx, left_invisible, right_invisible);
            mutable_this->split(right_invisible, num_chars, visible_screen, right_invisible);

            // 2. Direct streaming output of only the visible sub-window
            mutable_this->inorderPrint(visible_screen);

            // 3. Re-stitch the document tree together instantly
            mutable_this->root = mutable_this->join(mutable_this->join(left_invisible, visible_screen), right_invisible);
        }
};

// ============================================================================
// 3. USER INTERFACE AND CONTROLLER
// ============================================================================
int main() {
    ScalableRope editor;
    
    // Seed the editor with a mock large text set
    editor.insert(0, "Lorem ipsum dolor sit amet, consectetur adipiscing elit. ");
    editor.insert(editor.totalCharacters(), "Sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. ");
    editor.insert(editor.totalCharacters(), "Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. ");
    editor.insert(editor.totalCharacters(), "Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. ");
    editor.insert(editor.totalCharacters(), "Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum.");

    int scroll_position = 0;
    const int VIEWPORT_SIZE = 60; // Max number of characters shown on screen at a time

    while (true) {
        std::cout << "\n=================== SCALABLE TREAP TEXT ENGINE ===================\n";
        std::cout << "Viewing Chars [" << scroll_position << " - " 
                  << std::min(scroll_position + VIEWPORT_SIZE, editor.totalCharacters()) 
                  << "] out of " << editor.totalCharacters() << "\n";
        std::cout << "------------------------------------------------------------------\n\"";
        
        // Scalable display: Uses the zero-allocation window slicer
        editor.renderPage(scroll_position, VIEWPORT_SIZE);
        
        std::cout << "\"\n------------------------------------------------------------------\n";
        std::cout << "[A] Scroll Left  |  [D] Scroll Right  |  [I] Insert  |  [X] Delete  |  [Q] Quit\n";
        std::cout << "Command: ";
        
        char command;
        std::cin >> command;
        command = std::tolower(command);

        if (command == 'a') {
            scroll_position = std::max(0, scroll_position - 10);
        } 
        else if (command == 'd') {
            if (scroll_position + VIEWPORT_SIZE < editor.totalCharacters()) {
                scroll_position += 10;
            }
        } 
        else if (command == 'i') {
            int idx;
            std::string text;
            std::cout << "Enter character index to insert text at: ";
            std::cin >> idx;
            std::cin.ignore();
            std::cout << "Enter text to insert: ";
            std::getline(std::cin, text);
            
            editor.insert(idx, text);
        } 
        else if (command == 'x') {
            int idx, len;
            std::cout << "Enter starting character index to delete from: ";
            std::cin >> idx;
            std::cout << "Enter number of characters to wipe out: ";
            std::cin >> len;
            
            editor.deleteRange(idx, len);
            // Adjust scroll bar location if text shrank past current viewport view
            if (scroll_position >= editor.totalCharacters()) {
                scroll_position = std::max(0, editor.totalCharacters() - VIEWPORT_SIZE);
            }
        } 
        else if (command == 'q') {
            std::cout << "Exiting. Systems clean.\n";
            break;
        } 
        else {
            std::cout << "Unknown navigation or modification instruction.\n";
        }
    }
    return 0;
}