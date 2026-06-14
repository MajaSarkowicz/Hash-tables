#pragma once

#include "HashTable.hpp"
#include "DynamicArray.hpp"
#include <functional>



template <typename T, typename C> class CuckooHashTable : public HashTable<T, C>{
    private:
        struct Element {
            T key;
            C value;
            bool has_data;
            Element() : has_data(false) {}
            Element(const T& k, const C& v) : key(k), value(v), has_data(true) {}
        };    
        
        DynamicArray<Element> table1;
        DynamicArray<Element> table2;
        
        const int MAX_KICKS = 32;

        size_t currentSize;
        size_t capacity;

        static uint64_t splitmix64(uint64_t x);

        size_t hash1(const T& key) const;
        size_t hash2(const T& key) const;
       
        void rehash();

    public:
        CuckooHashTable(size_t cap = 16);
        CuckooHashTable(const CuckooHashTable& other);
        ~CuckooHashTable();
        void insert(const T& key, C value) override;
        C remove(const T& key) override;
        C get(const T& key) const override;
        void print(void) const override;
    };

template <typename T, typename C> static uint64_t CuckooHashTable<T, C>::splitmix64(uint64_t x){
    x += 0x9e3779b97f4a7c15ULL;

    x = (x ^ (x >> 30)) * 0xbf58476d1ce4e5b9ULL;

    x = (x ^ (x >> 27)) * 0x94d049bb133111ebULL;

    x = x ^ (x >> 31);

    return x;
}

template <typename T, typename C> size_t CuckooHashTable<T, C>::hash1(const T& key) const{
    uint64_t h = std::hash<T>{}(key);
    return splitmix64(h + 0x243f6a8885a308d3ULL) % capacity;
}


template <typename T, typename C> size_t CuckooHashTable<T, C>::hash2(const T& key) const{
    uint64_t h = std::hash<T>{}(key);
    return splitmix64(h + 0x13198a2e03707344ULL) % capacity;
}


template <typename T, typename C> void CuckooHashTable<T, C>::rehash(){
    DynamicArray<Element> old_table1 = DynamicArray<Element>(table1);
    DynamicArray<Element> old_table2 = DynamicArray<Element>(table2);

    capacity *= 2;

    table1.clear();
    table2.clear();

    table1.resize(capacity);
    table2.resize(capacity);
    
    for(int i = 0; i < old_table1.getSize()){
        if(old_table1.get(i).has_data){
            insert(old_table1.get(i).key, old_table1.get(i).value);
        }
    }

    for(int i = 0; i < old_table2.getSize()){
        if(old_table2.get(i).has_data){
            insert(old_table2.get(i).key, old_table2.get(i).value);
        }
    }
}

template <typename T, typename C> CuckooHashTable<T, C>::CuckooHashTable(size_t cap) : capacity(cap), currentSize(0), table1(cap), table2(cap){}

template <typename T, typename C> CuckooHashTable<T, C>::CuckooHashTable(const CuckooHashTable& other) : capacity(other.capacity), currentSize(other.currentSize), table1(other.table1), table2(other.table2){}

template <typename T, typename C> void CuckooHashTable<T, C>::insert(const T& key, C value){
    Element new_elem = Element(key, value);

    size_t pos = hash1(key);
    if (table1.get(pos).has_data && table1.get(pos).key == key){
        table1.replace(pos, new_elem);
        return;
    }

    pos = hash2(key);
    if (table2.get(pos).has_data && table2.get(pos).key == key){
        table2.replace(pos, new_elem);
        return;
    }
    
    bool first_table = true;
}