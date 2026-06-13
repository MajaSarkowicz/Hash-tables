#pragma once
#include "HashTable.hpp"
#include "AVLTree.hpp"

template<typename T, typename C>
class ChainAVL : public HashTable<T,C> {
private:
    size_t table_size;
    AVLTree<T,C> **table;
    
    size_t hash(const T& key) const;

public:
    ChainAVL(size_t size);
    ~ChainAVL();

    void insert(const T& key, C value) override;
    C remove(const T& key) override;
    C get(const T& key) const override;
    void print(void) const override;
};

template<typename T, typename C>
ChainAVL<T,C>::ChainAVL(size_t size) {
    table_size = size ? size : 1;
    table = new AVLTree<T,C>*[table_size];
    for (size_t i = 0; i < table_size; i++) {
        table[i] = new AVLTree<T,C>();
    }
}

template<typename T, typename C>
ChainAVL<T,C>::~ChainAVL() {
    for (size_t i = 0; i < table_size; i++) {
        delete table[i];
    }
    delete[] table;
}

template<typename T, typename C>
size_t ChainAVL<T,C>::hash(const T& key) const {
    // Knuth multiplicative hash
    return (static_cast<size_t>(key) * 2654435761u) % table_size;
}

template<typename T, typename C>
void ChainAVL<T,C>::insert(const T& key, C value) {
    table[hash(key)]->insert(key, value);
}

template<typename T, typename C>
C ChainAVL<T,C>::remove(const T& key) {
    return table[hash(key)]->remove(key);
}

template<typename T, typename C>
void ChainAVL<T,C>::print(void) const {
    for(size_t i = 0; i < table_size; i++) {
        std::cout << "\nTree #" << i << std::endl;
        table[i]->print();
    }
}

template<typename T, typename C>
C ChainAVL<T,C>::get(const T& key) const {
    AVLNode<T,C>* temp = table[hash(key)]->find(key);
    return (temp) ? temp->value : C{};
}
