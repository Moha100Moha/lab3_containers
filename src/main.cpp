#include <iostream>
#include <string>
#include <utility>

#include "MyVector.hpp"
#include "DoublyLinkedList.hpp"
#include "SinglyLinkedList.hpp"

// Print the content of any container using its iterator.
template <typename Container>
void print(const std::string& label, Container& c) {
    std::cout << label;
    bool first = true;
    for (auto it = c.begin(); it != c.end(); ++it) {
        if (!first) std::cout << ", ";
        std::cout << *it;
        first = false;
    }
    std::cout << '\n';
}

// The 12 steps from the lab assignment.
template <typename Container>
void demo(const std::string& name) {
    std::cout << "===== " << name << " =====\n";

    Container c;                                   // 1. create the container

    for (int i = 0; i < 10; ++i) {                 // 2. add 0..9
        c.push_back(i);
    }
    print("Content: ", c);                         // 3. expected: 0, 1, ..., 9
    std::cout << "Size: " << c.size() << '\n';     // 4. expected: 10

    // 5. Remove the 3rd, 5th and 7th elements (indices 2, 4, 6).
    //    We go from the END so the indices of the remaining targets do not shift.
    c.erase(6);
    c.erase(4);
    c.erase(2);
    print("Content: ", c);                         // 6. expected: 0, 1, 3, 5, 7, 8, 9

    c.insert(0, 10);                               // 7. add 10 to the beginning
    print("Content: ", c);                         // 8. expected: 10, 0, 1, 3, 5, 7, 8, 9

    c.insert(c.size() / 2, 20);                    // 9. add 20 to the middle
    print("Content: ", c);                         // 10. expected: 10, 0, 1, 3, 20, 5, 7, 8, 9

    c.push_back(30);                               // 11. add 30 to the end
    print("Content: ", c);                         // 12. expected: 10, 0, 1, 3, 20, 5, 7, 8, 9, 30

    std::cout << "c[4] = " << c[4] << " (operator[] demo)\n\n";
}

// Demonstration of move semantics.
template <typename Container>
void demo_move(const std::string& name) {
    std::cout << "----- Move semantics: " << name << " -----\n";

    Container a;
    a.push_back(1);
    a.push_back(2);
    a.push_back(3);

    Container b(std::move(a));                     // move constructor
    print("b after move constructor: ", b);
    std::cout << "a.size() after move: " << a.size() << '\n';   // 0: 'a' was emptied

    Container c;
    c = std::move(b);                              // move assignment
    print("c after move assignment: ", c);
    std::cout << "b.size() after move: " << b.size() << "\n\n";
}

int main() {
    demo<MyVector<int>>("MyVector (sequential)");
    demo<DoublyLinkedList<int>>("DoublyLinkedList");
    demo<SinglyLinkedList<int>>("SinglyLinkedList");

    demo_move<MyVector<int>>("MyVector");
    demo_move<DoublyLinkedList<int>>("DoublyLinkedList");
    demo_move<SinglyLinkedList<int>>("SinglyLinkedList");

    // Capacity demo: spare memory is reserved in advance (growth factor 1.5).
    MyVector<int> v;
    std::cout << "----- Capacity growth of MyVector -----\n";
    for (int i = 0; i < 12; ++i) {
        v.push_back(i);
        std::cout << "size = " << v.size() << ", capacity = " << v.capacity() << '\n';
    }

    return 0;
}