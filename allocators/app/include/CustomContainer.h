#pragma once
#include <memory>
#include <iostream>

template <typename T, typename Allocator = std::allocator<T>>
class CustomContainer
{

    struct Node
    {
        T data;
        Node *next;

        //  Node(std::allocator_arg_t, const Allocator &alloc, Args &&...args)  : data(std::allocator_arg, alloc, std::forward<Args>(args)...), next(nullptr) {}
        template <typename... Args>
        Node(Args &&...args) : data(std::forward<Args>(args)...), next(nullptr)
        {
         //   std::cout << "Node argg \n";
        }

        Node(T &t) : data(t), next(nullptr)
        {
          //  std::cout << "Node  \n";
        }
    };

    using NodeAllocator = typename Allocator::template rebind<Node>::other; // вот где ребинд

    Node *head;
    Node *tail;
    std::size_t count;
    NodeAllocator alloc;

public:
    // Итератор
    class iterator
    {
        Node *current;

    public:
        explicit iterator(Node *node) : current(node) {}

        T &operator*() { return current->data; }
        iterator &operator++()
        {
            current = current->next;
            return *this;
        }
        // для обхода контейнера
        bool operator!=(const iterator &other) const { return current != other.current; }
    };

    CustomContainer() : head(nullptr), tail(nullptr), count(0) {}

    ~CustomContainer()
    {
        clear();
    }

    // Добавление элемента
    void push_back(const T &value)
    {
        Node *newNode = alloc.allocate(1);
        // alloc.construct(newNode, Node{value, nullptr});
        //alloc.construct(newNode, Node(value));
        alloc.construct(newNode, value);   
        if (!head)
        {
            head = tail = newNode;
        }
        else
        {
            tail->next = newNode;
            tail = newNode;
        }
        ++count;
    }

    template <typename... Args>
    void emplace_back(Args &&...args)
    {
        Node *newNode = alloc.allocate(1);
        alloc.construct(newNode, std::forward<Args>(args)...);

        if (!head)
        {
            head = tail = newNode;
        }
        else
        {
            tail->next = newNode;
            tail = newNode;
        }
        ++count;
    }

    // Очистка контейнера
    void clear()
    {
        Node *current = head;
        while (current)
        {
            Node *next = current->next;
            alloc.destroy(current);
            alloc.deallocate(current, 1);
            current = next;
        }
        head = tail = nullptr;
        count = 0;
    }

    // Размер контейнера
    std::size_t size() const { return count; }

    // Проверка на пустоту
    bool empty() const { return count == 0; }

    // Итераторы
    iterator begin() { return iterator(head); }
    iterator end() { return iterator(nullptr); }

    // Обход контейнера (примерный вывод)
    void print() const
    {
        Node *current = head;
        while (current)
        {
            std::cout << current->data << " ";
            current = current->next;
        }
        std::cout << std::endl;
    }
};