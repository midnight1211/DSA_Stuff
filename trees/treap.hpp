template <typename T>
void Treap<T>::insert(Node<T>*& node, const T& key) {
    if (!node) {
        node = new Node<T>(key, dist(rng));
        return;
    }
    if (key < node->key) {
        insert(node->left, key);
        if (node->left->priority > node->priority) {
            rotateRight(node);
        }
    } else if (key > node->key) {
        insert(node->right, key);
        if (node->right->priority > node->priority) {
            rotateLeft(node);
        }
    }
}

template <typename T>
void Treap<T>::remove(Node<T>*& node, const T& key) {
    if (!node) return;

    if (key < node->key) {
        remove(node->left, key);
    } else if (key > node->key) {
        remove(node->right, key);
    } else {
        // Node found: Rotate down until it becomes a leaf or single child
        if (!node->left) {
            Node<T>* temp = node->right;
            delete node;
            node = temp;
        } else if (!node->right) {
            Node<T>* temp = node->left;
            delete node;
            node = temp;
        } else {
            // Pick the child with the higher priority to maintain heap rule
            if (node->left->priority > node->right->priority) {
                rotateRight(node);
                remove(node->right, key);
            } else {
                rotateLeft(node);
                remove(node->left, key);
            }
        }
    }
}

template <typename T>
void Treap<T>::inorder(Node<T>* node) const {
    if (!node) return;
    inorder(node->left);
    std::cout << "(" << node->key << ", " << node->priority << ") ";
    inorder(node->right);
}

template <typename T>
void Treap<T>::clear(Node<T>* node) {
    if (!node) return;
    clear(node->left);
    clear(node->right);
    delete node;
}

template <typename T>
void Treap<T>::insertWithPriority(Node<T>*& node, const T& key, double priority) {
    if (!node) {
        node = new Node<T>(key, priority);
        return;
    }
    if (key < node->key) {
        insertWithPriority(node->left, key, priority);
        if (node->left->priority > node->priority) rotateRight(node);
    } else {
        insertWithPriority(node->right, key, priority);
        if (node->right->priority > node->priority) rotateLeft(node);
    }
}

template <typename T>
void Treap<T>::insert(const T& key) { insert(root, key); }

template <typename T>
void Treap<T>::remove(const T& key) { remove(root, key); }

template <typename T>
bool Treap<T>::search(const T& key) const {
    Node<T>* curr = root;
    while (curr) {
        if (key == curr->key) return true;
        curr = (key < curr->key) ? curr->left : curr->right;
    }
    return false;
}

template <typename T>
void Treap<T>::printInorder() const {
    inorder(root);
    std::cout << "\n";
}

template <typename T>
void Treap<T>::split(Node<T>* t, const T& k, Node<T>*& left, Node<T>*& right) {
    if (!t) {
        left = nullptr;
        right = nullptr;
        return;
    }
    // Force the split-key to bubble up to the root using max priority
    insertWithPriority(t, k, std::numeric_limits<double>::infinity());

    left = t->left;
    right = t->right;

    delete t; // Remove the temporary dummy root node
}

template <typename T>
Node<T>* Treap<T>::join(Node<T>* left, Node<T>* right) {
    if (!left) return right;
    if (!right) return left;

    // The root with the higher priority must stay on top to satisfy the heap property
    if (left->priority > right->priority) {
        left->right = join(left->right, right);
        return left;
    } else {
        right->left = join(left, right->left);
        return right;
    }
}

template <typename T>
void Treap<T>::rotateRight(Node<T>*& root) {
    Node<T>* child = root->left;
    root->left = child->right;
    child->right = root;
    root = child;
}

template <typename T>
void Treap<T>::rotateLeft(Node<T>*& root) {
    Node<T>* child = root->right;
    root->right = child->left;
    child->left = root;
    root = child;
}
