// Remove #include "splay.h" from this line!

template <typename T>
void SplayTree<T>::rotateRight(SNode<T>*& root) {
    SNode<T>* child = root->left;
    root->left = child->right;
    child->right = root;
    root = child;
}

template <typename T>
void SplayTree<T>::rotateLeft(SNode<T>*& root) {
    SNode<T>* child = root->right;
    root->right = child->left;
    child->left = root;
    root = child;
}

template <typename T>
void SplayTree<T>::split(SNode<T>* t, const T& k, SNode<T>*& left, SNode<T>*& right) {
    if (!t) {
        left = nullptr;
        right = nullptr;
        return;
    }

    t = splay(t, k);

    if (t->key <= k) {
        right = t->right;
        t->right = nullptr;
        left = t;
    } else {
        left = t->left;
        t->left = nullptr;
        right = t;
    }
}

template <typename T>
SNode<T>* SplayTree<T>::join(SNode<T>* left, SNode<T>* right) {
    if (!left) return right;
    if (!right) return left;

    SNode<T>* curr = left;
    while (curr->right) curr = curr->right;

    left = splay(left, curr->key);
    left->right = right;
    return left;
}

template <typename T>
void SplayTree<T>::insertViaSplit(const T& key) {
    SNode<T>* L = nullptr;
    SNode<T>* R = nullptr;
    split(root, key, L, R);

    SNode<T>* newNode = new SNode<T>(key);
    newNode->left = L;
    newNode->right = R;
    root = newNode;
}

template <typename T>
void SplayTree<T>::deleteViaJoin(const T& key) {
    if (!root) return;

    SNode<T>* L = nullptr;
    SNode<T>* R = nullptr;
    split(root, key - 1, L, R);

    SNode<T>* exactNode = nullptr;
    SNode<T>* farRight = nullptr;
    split(R, key, exactNode, farRight);

    delete exactNode;

    root = join(L, farRight);
}

template <typename T>
SNode<T>* SplayTree<T>::splay(SNode<T>* head, const T& key) {
    if (!head || head->key == key) return head;

    if (key < head->key) {
        if (!head->left) return head;

        if (key < head->left->key) {
            head->left->left = splay(head->left->left, key);
            rotateRight(head);
        } else if (key > head->left->key) {
            head->left->right = splay(head->left->right, key);
            if (head->left->right) rotateLeft(head->left);
        }

        if (!head->left) return head;
        rotateRight(head);
        return head;
    } else {
        if (!head->right) return head;

        if (key < head->right->key) {
            head->right->left = splay(head->right->left, key);
            if (head->right->left) rotateRight(head->right);
        } else if (key > head->right->key) {
            head->right->right = splay(head->right->right, key);
            rotateLeft(head);
        }

        if (!head->right) return head;
        rotateLeft(head);
        return head;
    }
}

template <typename T>
void SplayTree<T>::inorder(SNode<T>* node) const {
    if (!node) return;
    inorder(node->left);
    std::cout << node->key << " ";
    inorder(node->right);
}

template <typename T>
void SplayTree<T>::clear(SNode<T>* node) {
    if (!node) return;
    clear(node->left);
    clear(node->right);
    delete node;
}

template <typename T>
bool SplayTree<T>::search(const T& key) {
    if (!root) return false;
    root = splay(root, key);
    return (root->key == key);
}

template <typename T>
void SplayTree<T>::insert(const T& key) {
    if (!root) {
        root = new SNode<T>(key);
        return;
    }
    root = splay(root, key);

    if (root->key == key) return;

    SNode<T>* newNode = new SNode<T>(key);
    if (key < root->key) {
        newNode->right = root;
        newNode->left = root->left;
        root->left = nullptr;
    } else {
        newNode->left = root;
        newNode->right = root->right;
        root->right = nullptr;
    }
    root = newNode;
}

template <typename T>
void SplayTree<T>::remove(const T& key) {
    if (!root) return;

    root = splay(root, key);
    if (root->key != key) return;
    
    SNode<T>* temp = root;

    if (!root->left) {
        root = root->right;
    } else {
        SNode<T>* leftSubtree = root->left;
        leftSubtree = splay(leftSubtree, key);
        leftSubtree->right = root->right;
        root = leftSubtree;
    }
    delete temp;
}

template <typename T>
void SplayTree<T>::printInorder() const {
    inorder(root);
    std::cout << "\n";
}

template <typename T>
T SplayTree<T>::getRootKey() const {
    return root ? root->key : T{};
}