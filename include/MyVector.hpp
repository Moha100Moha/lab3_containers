#pragma once

#include <cstddef>    // std::size_t
#include <stdexcept>  // std::out_of_range
#include <utility>    // std::move, std::exchange

// Sequential container: all elements are stored in one continuous block of memory.
// Note: T must have a default constructor (we use new T[n]).
template <typename T>
class MyVector {
public:
    using size_type = std::size_t;

    // ---------- Iterator ----------
    // Encapsulates the logic of walking over the container.
    // For a continuous block of memory, it is just a wrapper around a pointer.
    struct iterator {
        T* ptr;

        explicit iterator(T* p = nullptr) : ptr(p) {}

        T& operator*() const { return *ptr; }   // dereference: *it
        T& get() const { return *ptr; }         // alternative: it.get()

        iterator& operator++() { ++ptr; return *this; }                         // ++it
        iterator operator++(int) { iterator tmp = *this; ++ptr; return tmp; }  // it++

        bool operator==(const iterator& other) const { return ptr == other.ptr; }
        bool operator!=(const iterator& other) const { return ptr != other.ptr; }
    };

    iterator begin() { return iterator(m_data); }            // first element
    iterator end() { return iterator(m_data + m_size); }     // one past the last element

    // ---------- Constructors / destructor ----------
    MyVector() = default;

    ~MyVector() { delete[] m_data; }

    // Copy constructor: creates an independent copy of all elements.
    MyVector(const MyVector& other)
        : m_data(other.m_capacity ? new T[other.m_capacity] : nullptr),
          m_size(other.m_size),
          m_capacity(other.m_capacity) {
        for (size_type i = 0; i < m_size; ++i) {
            m_data[i] = other.m_data[i];
        }
    }

    // Copy assignment (copy-and-swap idiom).
    MyVector& operator=(const MyVector& other) {
        if (this != &other) {
            MyVector tmp(other);
            swap(tmp);
        }
        return *this;
    }

    // Move constructor: "steals" the memory of 'other' instead of copying it.
    MyVector(MyVector&& other) noexcept
        : m_data(std::exchange(other.m_data, nullptr)),
          m_size(std::exchange(other.m_size, 0)),
          m_capacity(std::exchange(other.m_capacity, 0)) {}

    // Move assignment: free our memory, then steal the memory of 'other'.
    MyVector& operator=(MyVector&& other) noexcept {
        if (this != &other) {
            delete[] m_data;
            m_data = std::exchange(other.m_data, nullptr);
            m_size = std::exchange(other.m_size, 0);
            m_capacity = std::exchange(other.m_capacity, 0);
        }
        return *this;
    }

    // ---------- Minimal interface ----------
    // Add to the end (lvalue version: the value is copied).
    void push_back(const T& value) { insert(m_size, value); }

    // Add to the end (rvalue version: the value is moved).
    void push_back(T&& value) { insert(m_size, std::move(value)); }

    // Insert before position 'pos' (0-based). pos == size() means "append".
    void insert(size_type pos, const T& value) {
        T copy(value);                    // make a copy, then reuse the rvalue version
        insert(pos, std::move(copy));
    }

    void insert(size_type pos, T&& value) {
        if (pos > m_size) {
            throw std::out_of_range("MyVector::insert: position out of range");
        }
        grow_if_needed();                 // make sure there is a free slot
        for (size_type i = m_size; i > pos; --i) {
            m_data[i] = std::move(m_data[i - 1]);   // shift elements to the right
        }
        m_data[pos] = std::move(value);
        ++m_size;
    }

    // Remove the element at position 'pos' (0-based).
    void erase(size_type pos) {
        if (pos >= m_size) {
            throw std::out_of_range("MyVector::erase: position out of range");
        }
        for (size_type i = pos; i + 1 < m_size; ++i) {
            m_data[i] = std::move(m_data[i + 1]);   // shift elements to the left
        }
        --m_size;
    }

    size_type size() const { return m_size; }

    // Access by index (no bounds checking, like std::vector).
    T& operator[](size_type index) { return m_data[index]; }
    const T& operator[](size_type index) const { return m_data[index]; }

    // ---------- Helper methods ----------
    size_type capacity() const { return m_capacity; }   // how many elements fit without reallocation
    bool empty() const { return m_size == 0; }

    void swap(MyVector& other) noexcept {
        std::swap(m_data, other.m_data);
        std::swap(m_size, other.m_size);
        std::swap(m_capacity, other.m_capacity);
    }

private:
    // Allocate a new block, move the elements there, free the old block.
    void reallocate(size_type new_capacity) {
        T* new_data = new T[new_capacity];
        for (size_type i = 0; i < m_size; ++i) {
            new_data[i] = std::move(m_data[i]);
        }
        delete[] m_data;
        m_data = new_data;
        m_capacity = new_capacity;
    }

    // Reserve memory in advance: grow by 50% when the block is full,
    // so most push_back calls do NOT need a new allocation.
    void grow_if_needed() {
        if (m_size == m_capacity) {
            reallocate(m_capacity + m_capacity / 2 + 1);   // factor 1.5 (+1 so it grows from 0)
        }
    }

    T* m_data = nullptr;        // pointer to the block of memory
    size_type m_size = 0;       // how many elements are stored
    size_type m_capacity = 0;   // how many elements the block can hold
};