#pragma once

#include <cstddef>
#include <stdexcept>
#include <utility>

// List container: every element lives in its own node.
// Nodes are connected with pointers to the next AND previous node.
template <typename T>
class DoublyLinkedList {
private:
    struct Node {
        T data;        // user data
        Node* next;    // pointer to the next node (nullptr for the last one)
        Node* prev;    // pointer to the previous node (nullptr for the first one)

        explicit Node(const T& value) : data(value), next(nullptr), prev(nullptr) {}
        explicit Node(T&& value) : data(std::move(value)), next(nullptr), prev(nullptr) {}
    };

public:
    using size_type = std::size_t;

    // ---------- Iterator ----------
    class iterator {
    public:
        explicit iterator(Node* node = nullptr) : m_node(node) {}

        T& operator*() const { return m_node->data; }   // *it
        T& get() const { return m_node->data; }         // it.get()

        iterator& operator++() { m_node = m_node->next; return *this; }
        iterator operator++(int) { iterator tmp = *this; m_node = m_node->next; return tmp; }

        bool operator==(const iterator& other) const { return m_node == other.m_node; }
        bool operator!=(const iterator& other) const { return m_node != other.m_node; }

    private:
        Node* m_node;   // current node (nullptr means "end")
    };

    iterator begin() { return iterator(m_head); }
    iterator end() { return iterator(nullptr); }

    // ---------- Constructors / destructor ----------
    DoublyLinkedList() = default;

    ~DoublyLinkedList() { clear(); }

    // Copy constructor: copy every element one by one.
    DoublyLinkedList(const DoublyLinkedList& other) {
        for (Node* n = other.m_head; n != nullptr; n = n->next) {
            push_back(n->data);
        }
    }

    // Copy assignment (copy-and-swap).
    DoublyLinkedList& operator=(const DoublyLinkedList& other) {
        if (this != &other) {
            DoublyLinkedList tmp(other);
            swap(tmp);
        }
        return *this;
    }

    // Move constructor: take over the nodes of 'other', leave it empty.
    DoublyLinkedList(DoublyLinkedList&& other) noexcept
        : m_head(std::exchange(other.m_head, nullptr)),
          m_tail(std::exchange(other.m_tail, nullptr)),
          m_size(std::exchange(other.m_size, 0)) {}

    // Move assignment: delete our nodes, take over the nodes of 'other'.
    DoublyLinkedList& operator=(DoublyLinkedList&& other) noexcept {
        if (this != &other) {
            clear();
            m_head = std::exchange(other.m_head, nullptr);
            m_tail = std::exchange(other.m_tail, nullptr);
            m_size = std::exchange(other.m_size, 0);
        }
        return *this;
    }

    // ---------- Minimal interface ----------
    void push_back(const T& value) { insert(m_size, value); }
    void push_back(T&& value) { insert(m_size, std::move(value)); }

    // Insert before position 'pos' (0-based). pos == size() means "append".
    void insert(size_type pos, const T& value) {
        T copy(value);
        insert(pos, std::move(copy));
    }

    void insert(size_type pos, T&& value) {
        if (pos > m_size) {
            throw std::out_of_range("DoublyLinkedList::insert: position out of range");
        }
        Node* node = new Node(std::move(value));

        if (pos == m_size) {
            // Append to the end.
            node->prev = m_tail;
            if (m_tail) m_tail->next = node;
            else m_head = node;              // list was empty
            m_tail = node;
        } else {
            // Insert before the node that currently has index 'pos'.
            Node* target = node_at(pos);
            node->next = target;
            node->prev = target->prev;
            if (target->prev) target->prev->next = node;
            else m_head = node;              // inserting at the very beginning
            target->prev = node;
        }
        ++m_size;
    }

    // Remove the element at position 'pos' (0-based).
    void erase(size_type pos) {
        Node* node = node_at(pos);           // throws if pos is out of range

        // Connect the neighbours to each other, skipping 'node'.
        if (node->prev) node->prev->next = node->next;
        else m_head = node->next;            // removing the first node

        if (node->next) node->next->prev = node->prev;
        else m_tail = node->prev;            // removing the last node

        delete node;
        --m_size;
    }

    size_type size() const { return m_size; }

    // Access by index: walks through the list, so it is O(n).
    T& operator[](size_type index) { return node_at(index)->data; }
    const T& operator[](size_type index) const { return node_at(index)->data; }

    // ---------- Helper methods ----------
    bool empty() const { return m_size == 0; }

    // Delete all nodes.
    void clear() {
        Node* current = m_head;
        while (current != nullptr) {
            Node* next = current->next;   // remember next BEFORE deleting
            delete current;
            current = next;
        }
        m_head = nullptr;
        m_tail = nullptr;
        m_size = 0;
    }

    void swap(DoublyLinkedList& other) noexcept {
        std::swap(m_head, other.m_head);
        std::swap(m_tail, other.m_tail);
        std::swap(m_size, other.m_size);
    }

private:
    // Find the node by index. Starts from the closer end (head or tail).
    Node* node_at(size_type index) const {
        if (index >= m_size) {
            throw std::out_of_range("DoublyLinkedList: index out of range");
        }
        Node* current;
        if (index < m_size / 2) {
            current = m_head;
            for (size_type i = 0; i < index; ++i) current = current->next;
        } else {
            current = m_tail;
            for (size_type i = m_size - 1; i > index; --i) current = current->prev;
        }
        return current;
    }

    Node* m_head = nullptr;    // first node
    Node* m_tail = nullptr;    // last node
    size_type m_size = 0;
};