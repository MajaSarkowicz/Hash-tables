#pragma once
#include <iostream>
#include <cstdint>

template<typename T, typename C>
struct AVLNode {
    T key;
    C value;
    AVLNode<T,C>* left = nullptr;
    AVLNode<T,C>* right = nullptr;
    size_t height = 1;

    AVLNode(const T& k, C v) : key(k), value(v) {}
};

template<typename T, typename C>
class AVLTree {
private:
    AVLNode<T,C>* root = nullptr;

    void destroy(AVLNode<T,C> *node);
    size_t get_height(AVLNode<T,C> *node) const;
    AVLNode<T,C>* rotate_right(AVLNode<T,C> *node);
    AVLNode<T,C>* rotate_left(AVLNode<T,C> *node);
    int8_t balance_factor(AVLNode<T,C> *node) const;
    AVLNode<T,C>* balance_tree(AVLNode<T,C> *node);
    AVLNode<T,C>* insert_node(AVLNode<T,C>* node, const T& key, C value);
    AVLNode<T,C>* remove_node(AVLNode<T,C>* node, const T& key, C& removed_value);
    AVLNode<T,C>* find_node(AVLNode<T,C>* node, const T& key) const;
    void print_in_order(AVLNode<T,C>* node) const;

public:
    AVLTree();
    ~AVLTree();

    void insert(const T& key, C value);
    C remove(const T& key);
    AVLNode<T,C>* find(const T& key) const;
    void print(void) const;
};

// manual max implementation
template <typename T>
T max(T a, T b) {
    return (a > b) ? a : b;
}

template<typename T, typename C>
AVLTree<T,C>::AVLTree() {
    root = nullptr;
}

template<typename T, typename C>
void AVLTree<T,C>::destroy(AVLNode<T,C> *node) {
    if (node) {
        destroy(node->left);
        destroy(node->right);
        delete node;
    }
}

template<typename T, typename C>
AVLTree<T,C>::~AVLTree() {
    destroy(root);
}

template<typename T, typename C>
size_t AVLTree<T,C>::get_height(AVLNode<T,C>* node) const {
    return node ? node->height : 0;
}

template<typename T, typename C>
AVLNode<T,C>* AVLTree<T,C>::rotate_right(AVLNode<T,C> *node) {
    AVLNode<T,C>* temp = node->left;
    node->left = temp->right;
    temp->right = node;
    node->height = max(get_height(node->left), get_height(node->right)) + 1;
    temp->height = max(get_height(temp->left), get_height(temp->right)) + 1;
    return temp;
}

template<typename T, typename C>
AVLNode<T,C>* AVLTree<T,C>::rotate_left(AVLNode<T,C> *node) {
    AVLNode<T,C>* temp = node->right;
    node->right = temp->left;
    temp->left = node;
    node->height = max(get_height(node->left), get_height(node->right)) + 1;
    temp->height = max(get_height(temp->left), get_height(temp->right)) + 1;
    return temp;
}

template<typename T, typename C>
int8_t AVLTree<T,C>::balance_factor(AVLNode<T,C>* node) const {
    return node ? static_cast<int8_t>(get_height(node->left) - get_height(node->right)) : 0;
}

template<typename T, typename C>
AVLNode<T,C>* AVLTree<T,C>::balance_tree(AVLNode<T,C> *node) {
    node->height = max(get_height(node->left), get_height(node->right)) + 1;
    int8_t balance = balance_factor(node);

    if (balance > 1 && balance_factor(node->left) >= 0) {
        return rotate_right(node);
    }
    if (balance > 1 && balance_factor(node->left) < 0) {
        node->left = rotate_left(node->left);
        return rotate_right(node);
    }
    if (balance < -1 && balance_factor(node->right) <= 0) {
        return rotate_left(node);
    }
    if (balance < -1 && balance_factor(node->right) > 0) {
        node->right = rotate_right(node->right);
        return rotate_left(node);
    }

    return node;
}

template<typename T, typename C>
AVLNode<T,C>* AVLTree<T,C>::insert_node(AVLNode<T,C>* node, const T& key, C value) {
    if (!node) {
        return new AVLNode<T,C>(key, value);
    }
    if (key < node->key) {
        node->left = insert_node(node->left, key, value);
    }else if (key > node->key) {
        node->right = insert_node(node->right, key, value);
    }else {
        node->value = value;
    }

    return balance_tree(node);
}

template<typename T, typename C>
void AVLTree<T,C>::insert(const T& key, C value) {
    root = insert_node(root, key, value);
}

template<typename T, typename C>
AVLNode<T,C>* AVLTree<T,C>::remove_node(AVLNode<T,C>* node, const T& key, C& removed_value) {
    if (!node) {
        return nullptr;
    }
    if (key < node->key) {
        node->left = remove_node(node->left, key, removed_value);
    } else if (key > node->key) {
        node->right = remove_node(node->right, key, removed_value);
    } else {
        removed_value = node->value; // wartość oryginalnie usuwanego węzła - zapamiętana lokalnie
        if (!node->left || !node->right) {
            AVLNode<T,C>* temp = node->left ? node->left : node->right;
            delete node;
            return temp;
        } else {
            AVLNode<T,C>* successor = node->right;
            while (successor->left) {
                successor = successor->left;
            }
            node->key = successor->key;
            node->value = successor->value;

            C dummy{}; // rekurencja na następniku nie może nadpisać removed_value
            node->right = remove_node(node->right, successor->key, dummy);
        }
    }

    return balance_tree(node);
}

template<typename T, typename C>
C AVLTree<T,C>::remove(const T& key) {
    C removed_value{};
    root = remove_node(root, key, removed_value);
    return removed_value;
}

template<typename T, typename C>
AVLNode<T,C>* AVLTree<T,C>::find_node(AVLNode<T,C>* node, const T& key) const {
    if (!node) {
        return nullptr;
    }
    if (key < node->key) {
        return find_node(node->left, key);
    }else if (key > node->key) {
        return find_node(node->right, key);
    }else {
        return node;
    }
}

template<typename T, typename C>
AVLNode<T,C>* AVLTree<T,C>::find(const T& key) const {
    return find_node(root, key);
}

template<typename T, typename C>
void AVLTree<T,C>::print_in_order(AVLNode<T,C>* node) const {
    if (!node) {
        return;
    }
    print_in_order(node->left);
    std::cout << node->key << ": " << node->value << std::endl;
    print_in_order(node->right);
}

template<typename T, typename C>
void AVLTree<T,C>::print(void) const {
    print_in_order(root);
}
