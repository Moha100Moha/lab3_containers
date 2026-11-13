#pragma once

#include <cstddef>
#include <stdexcept>
#include <utility>

// List container: nodes are connected only with a pointer to the NEXT node.
// We can walk through the list in one direction only (from head to tail).
template <typename T>
class SinglyLinkedList {
private:
    struct Node {
        T data;       // user data
        Node* next;   // pointer to the next node (nullptr for the last one)

        explicit Node(const T& value) : data(value), next(nullptr) {}
        explicit Node(T&& value) : data(std::move(value)), next(nullptr) {}
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
        Node* m_node;
    };

    iterator begin() { return iterator(m_head); }
    iterator end() { return iterator(nullptr); }

    // ---------- Constructors / destructor ----------
    SinglyLinkedList() = default;

    ~SinglyLinkedList() { clear(); }

    SinglyLinkedList(const SinglyLinkedList& other) {
        for (Node* n = other.m_head; n != nullptr; n = n->next) {
            push_back(n->data);
        }
    }

    SinglyLinkedList& operator=(const SinglyLinkedList& other) {
        if (this != &other) {
            SinglyLinkedList tmp(other);
            swap(tmp);
        }
        return *this;
    }

    // Move constructor: take over the nodes of 'other', leave it empty.
    SinglyLinkedList(SinglyLinkedList&& other) noexcept
        : m_head(std::exchange(other.m_head, nullptr)),
          m_tail(std::exchange(other.m_tail, nullptr)),
          m_size(std::exchange(other.m_size, 0)) {}

    // Move assignment: delete our nodes, take over the nodes of 'other'.
    SinglyLinkedList& operator=(SinglyLinkedList&& other) noexcept {
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
            throw std::out_of_range("SinglyLinkedList::insert: position out of range");
        }
        Node* node = new Node(std::move(value));

        if (pos == 0) {
            // Insert at the beginning: new node becomes the head.
            node->next = m_head;
            m_head = node;
            if (m_tail == nullptr) m_tail = node;   // list was empty
        } else if (pos == m_size) {
            // Insert at the end: the old tail points to the new node.
            m_tail->next = node;
            m_tail = node;
        } else {
            // Insert in the middle: we need the node BEFORE the position,
            // because we cannot go backwards in a singly linked list.
            Node* before = node_at(pos - 1);
            node->next = before->next;
            before->next = node;
        }
        ++m_size;
    }

    // Remove the element at position 'pos' (0-based).
    void erase(size_type pos) {
        if (pos >= m_size) {
            throw std::out_of_range("SinglyLinkedList::erase: position out of range");
        }
        if (pos == 0) {
            Node* old = m_head;
            m_head = m_head->next;
            if (m_head == nullptr) m_tail = nullptr;   // list became empty
            delete old;
        } else {
            Node* before = node_at(pos - 1);   // node before the one we delete
            Node* old = before->next;
            before->next = old->next;          // skip the removed node
            if (old == m_tail) m_tail = before;
            delete old;
        }
        --m_size;
    }

    size_type size() const { return m_size; }

    // Access by index: walks through the list, so it is O(n).
    T& operator[](size_type index) { return node_at(index)->data; }
    const T& operator[](size_type index) const { return node_at(index)->data; }

    // ---------- Helper methods ----------
    bool empty() const { return m_size == 0; }

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

    void swap(SinglyLinkedList& other) noexcept {
        std::swap(m_head, other.m_head);
        std::swap(m_tail, other.m_tail);
        std::swap(m_size, other.m_size);
    }

private:
    // Find the node by index, always starting from the head.
    Node* node_at(size_type index) const {
        if (index >= m_size) {
            throw std::out_of_range("SinglyLinkedList: index out of range");
        }
        Node* current = m_head;
        for (size_type i = 0; i < index; ++i) current = current->next;
        return current;
    }

    Node* m_head = nullptr;   // first node
    Node* m_tail = nullptr;   // last node (makes push_back fast)
    size_type m_size = 0;
};