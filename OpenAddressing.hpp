#pragma once
#include "HashTable.hpp"

template<typename T, typename C>
struct OpenAddressingNode {
    bool isDeleted = false;
    T key;
    C value;
    OpenAddressingNode(const T& k, C v) : key(k), value(v) {}
};

template<typename T, typename C>
class OpenAddressing : public HashTable<T,C> {
private:
    OpenAddressingNode<T,C>** table;
    size_t capacity;
    size_t size;

    void destroy();
    OpenAddressingNode<T,C>* find(const T& key) const;
    size_t hash(const T& key) const;
    OpenAddressingNode<T,C>* insert_node(const T& key, C value);
    void resize();

public:
    OpenAddressing();
    OpenAddressing(size_t initial_capacity);
    OpenAddressing(const OpenAddressing<T,C>& other);
    OpenAddressing<T,C>& operator=(const OpenAddressing<T,C>& other);
    ~OpenAddressing();

    void insert(const T& key, C value) override;
    C remove(const T& key) override;
    C get(const T& key) const override;
    void print(void) const override;
};

template<typename T, typename C>
OpenAddressing<T,C>::OpenAddressing() : capacity(16), size(0) {
    table = new OpenAddressingNode<T,C>*[capacity];
    for (size_t i = 0; i < capacity; i++) {
        table[i] = nullptr;
    }
}

template<typename T, typename C>
OpenAddressing<T,C>::OpenAddressing(size_t initial_capacity)
    : capacity(initial_capacity ? initial_capacity : 1), size(0) {
    table = new OpenAddressingNode<T,C>*[capacity];
    for (size_t i = 0; i < capacity; i++) {
        table[i] = nullptr;
    }
}

template<typename T, typename C>
OpenAddressing<T,C>::OpenAddressing(const OpenAddressing<T,C>& other)
    : capacity(other.capacity), size(other.size) {
    table = new OpenAddressingNode<T,C>*[capacity];
    for (size_t i = 0; i < capacity; i++) {
        if (other.table[i]) {
            table[i] = new OpenAddressingNode<T,C>(*other.table[i]);
        } else {
            table[i] = nullptr;
        }
    }
}

template<typename T, typename C>
OpenAddressing<T,C>& OpenAddressing<T,C>::operator=(const OpenAddressing<T,C>& other) {
    if (this != &other) {
        destroy();
        capacity = other.capacity;
        size = other.size;
        table = new OpenAddressingNode<T,C>*[capacity];
        for (size_t i = 0; i < capacity; i++) {
            if (other.table[i]) {
                table[i] = new OpenAddressingNode<T,C>(*other.table[i]);
            } else {
                table[i] = nullptr;
            }
        }
    }
    return *this;
}

template<typename T, typename C>
void OpenAddressing<T,C>::destroy() {
    if (!table) {
        return;
    }
    for (size_t i = 0; i < capacity; i++) {
        if (table[i] != nullptr) {
            delete table[i];
        }
    }
    delete[] table;
    table = nullptr;
}

template<typename T, typename C>
OpenAddressing<T,C>::~OpenAddressing() {
    destroy();
}

template<typename T, typename C>
size_t OpenAddressing<T,C>::hash(const T& key) const {
    // Knuth multiplicative hash
    return (static_cast<size_t>(key) * 2654435761u) % capacity;
}


template<typename T, typename C>
void OpenAddressing<T,C>::resize() {
    size_t old_capacity = capacity;
    OpenAddressingNode<T,C>** old_table = table;

    capacity *= 2;
    table = new OpenAddressingNode<T,C>*[capacity];
    for (size_t i = 0; i < capacity; i++) {
        table[i] = nullptr;
    }

    size = 0;

    for (size_t i = 0; i < old_capacity; i++) {
        if (old_table[i] != nullptr) {

            if (!old_table[i]->isDeleted) {
                // kopiujemy tylko aktywne
                insert(old_table[i]->key, old_table[i]->value);
            }

            // usuwamy WSZYSTKIE stare węzły
            delete old_table[i];
        }
    }

    delete[] old_table;
}


template<typename T, typename C>
OpenAddressingNode<T,C>* OpenAddressing<T,C>::insert_node(const T& key, C value) {
    size_t index = hash(key);
    size_t original = index;

    do {
        if (table[index] == nullptr || table[index]->isDeleted) {
            if (table[index] != nullptr) {
                delete table[index];
            }
            table[index] = new OpenAddressingNode<T,C>(key, value);
            size++;
            return table[index];
        }
        if (table[index]->key == key) {
            table[index]->value = value;
            return table[index];
        }
        index = (index + 1) % capacity;
    } while (index != original);

    return nullptr;
}

template<typename T, typename C>
void OpenAddressing<T,C>::insert(const T& key, C value) {
    if (static_cast<double>(size) / capacity >= 0.7) {
        resize();
    }

    OpenAddressingNode<T,C>* node = insert_node(key, value);
    if (!node) {
        resize();
        insert_node(key, value);
    }
}

template<typename T, typename C>
C OpenAddressing<T,C>::remove(const T& key) {
    size_t index = hash(key);
    size_t original = index;

    do {
        if (table[index] == nullptr) {
            return C{};
        }
        if (!table[index]->isDeleted && table[index]->key == key) {
            C value = table[index]->value;
            table[index]->isDeleted = true;
            size--;
            return value;
        }
        index = (index + 1) % capacity;
    } while (index != original);

    return C{};
}

template<typename T, typename C>
OpenAddressingNode<T,C>* OpenAddressing<T,C>::find(const T& key) const {
    size_t index = hash(key);
    size_t original = index;

    do {
        if (table[index] == nullptr) {
            return nullptr;
        }
        if (!table[index]->isDeleted && table[index]->key == key) {
            return table[index];
        }
        index = (index + 1) % capacity;
    } while (index != original);

    return nullptr;
}

template<typename T, typename C>
C OpenAddressing<T,C>::get(const T& key) const {
    OpenAddressingNode<T,C>* temp = find(key);
    return (temp) ? temp->value : C{};
}

template<typename T, typename C>
void OpenAddressing<T,C>::print(void) const {
    std::cout << std::endl;
    for (size_t i = 0; i < capacity; i++) {
        if (table[i] != nullptr && !table[i]->isDeleted) {
            std::cout << table[i]->key << ": " << table[i]->value << std::endl;
        }
    }
}
