#pragma once
#include <algorithm>
#include <iostream>


template <typename T>
class DynamicArray
{
public:
    DynamicArray();
    DynamicArray(size_t cap);
    DynamicArray(const DynamicArray& other);
    ~DynamicArray();

    T *getCollection() { return collection; }
    size_t &getSize() { return size; }
    size_t &getCapacity() { return capacity; }
    
    void upsize_();
    bool needsToUpsize() { return this->size + 1 >= this->capacity; }

    const T get(int index) const;

    void insert(T val, size_t position);
    void add_back(T val);
    void add_front(T val);

    void remove(size_t position);
    void remove_back();
    void remove_front();

    int search(T e, bool compatibility_flag);

    void clear();
    void resize(size_t new_size);
    void replace(size_t position, T val);

    void print();

private:
    T *collection;   // array of elements
    size_t capacity; // current capacity of the array
    size_t size;     // number of elements in the array
};

template <typename T>
DynamicArray<T>::DynamicArray() : capacity(4), size(0)
{
    collection = new T[capacity];
}

template <typename T>
DynamicArray<T>::DynamicArray(size_t cap) : capacity(cap), size(0)
{
    collection = new T[capacity];
}

template <typename T>
DynamicArray<T>::DynamicArray(const DynamicArray& other) : capacity(other.capacity), size(other.size)
{
    collection = new T[capacity];
    for (size_t i = 0; i < size; i++) {
        collection[i] = other.collection[i];
    }
}

template <typename T>
DynamicArray<T>::~DynamicArray()
{
    delete[] collection;
}

template <typename T>
void DynamicArray<T>::upsize_()
{
    size_t new_capacity = capacity * 2; // double the capacity
    T *new_collection = new T[new_capacity];
    std::move(collection, collection + size, new_collection); // move elements to new array
    delete[] collection;
    collection = new_collection;
    capacity = new_capacity;
}

template <typename T>
const T DynamicArray<T>::get(int index) const
{
    return collection[index];
}

template <typename T>
void DynamicArray<T>::insert(T val, size_t position)
{
    if (needsToUpsize())
    {
        upsize_(); // check and resize if necessary
    }
    std::move(collection + position, collection + size, collection + position + 1); // shift elements right from position
    collection[position] = val;
    size++;
}

template <typename T>
void DynamicArray<T>::add_front(T val)
{
    if (needsToUpsize())
    {
        upsize_(); // check and resize
    }
    std::move(collection, collection + size, collection + 1); // shift all elements right
    collection[0] = val;
    size++;
}

template <typename T>
void DynamicArray<T>::add_back(T val)
{
    if (needsToUpsize())
    {
        upsize_();
    }
    collection[size] = val;
    size++;
}

template <typename T>
void DynamicArray<T>::remove(size_t position)
{
    std::move(collection + position + 1, collection + size, collection + position);
    size--;
}

template <typename T>
void DynamicArray<T>::remove_back()
{
    size--;
}

template <typename T>
void DynamicArray<T>::remove_front()
{
    std::move(collection + 1, collection + size, collection);
    size--;
}

template <typename T>
int DynamicArray<T>::search(T e, bool compatibility_flag)
{
    for (size_t i = 0; i < size; i++)
    {
        if (collection[i] == e)
            return i;
    }
    return -1;
}

template <typename T>
void DynamicArray<T>::clear()
{
    size = 0;
}

template <typename T>
void DynamicArray<T>::resize(size_t new_size)
{
    if (new_size == capacity)
        return;
    
    T *new_collection = new T[new_size];
    size_t copy_size = std::min(size, new_size);
    std::move(collection, collection + copy_size, new_collection);
    delete[] collection;
    collection = new_collection;
    capacity = new_size;
    if (size > capacity)
        size = capacity;
}

template <typename T>
void DynamicArray<T>::replace(size_t position, T val)
{
    if (position < size) {
        collection[position] = val;
    }
}

template <typename T>
void DynamicArray<T>::print(){
    std::cout << "[";
    for (size_t i = 0; i < size; i++)
    {
        cout << collection[i];
        if(i != size-1){
            std::cout << ", ";
        }
    }
    std::cout << "]\n";
}