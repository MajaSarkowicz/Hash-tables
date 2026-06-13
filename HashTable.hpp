#pragma once
#include <iostream>

template<typename T, typename C>
class HashTable {
public:
    // Add key-value pair to the dictionary (if element with specified key already exists, then update the value)
    virtual void insert(const T& key, C value) = 0;

    // Remove element with specified key and return its value
    virtual C remove(const T& key) = 0;

    // Return value of element with specified key
    virtual C get(const T& key) const = 0;

    // Display the key-value pairs inside the dictionary
    virtual void print(void) const = 0;

    virtual ~HashTable() {}
};