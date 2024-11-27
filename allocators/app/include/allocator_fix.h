#pragma once
#include <iostream>
#include <memory>
#include <stdexcept>

template <typename T, size_t Capacity>
class FixedAllocator {
public:
    using value_type = T;

    FixedAllocator() : memory(nullptr), count(0) {
        // Выделяем выровненную память для Capacity элементов типа T
        memory = static_cast<T*>(std::aligned_alloc(alignof(T), sizeof(T) * Capacity));
        if (!memory) {
            throw std::bad_alloc();
        }
    }

    ~FixedAllocator() {
        ::operator delete(memory);
    }

    template <typename U>
    struct rebind {
        using other = FixedAllocator<U, Capacity>;
    };

    T *allocate(std::size_t n) {
        //std::cout << "allocate \n";
        if (n + count > Capacity) {
            throw std::bad_alloc();  // Превышина вместимость аллокатора
        }
        T *result = memory + count;
        count += n;
        return result;
    }

    template <typename U, typename... Args>
    void construct(U* p, Args&&... args) {
        //std::cout << "construct \n";
        new (p) U(std::forward<Args>(args)...); 
    }

    template <typename U>
    void destroy(U* p) {
       // std::cout << "destroy \n";
        p->~U(); 
    }

    void deallocate(T* p, std::size_t n) {
        // удад
    }

    FixedAllocator(const FixedAllocator&) = delete;
    FixedAllocator& operator=(const FixedAllocator&) = delete;

    FixedAllocator(FixedAllocator&& other) noexcept : memory(other.memory), count(other.count) {
        std::cout << "constr && \n";
        other.memory = nullptr;
        other.count = 0;
    }

    FixedAllocator& operator=(FixedAllocator&& other) noexcept {
        if (this != &other) {
            if (memory) {
                std::free(memory);
            }
            memory = other.memory;
            count = other.count;
            other.memory = nullptr;
            other.count = 0;
        }
        return *this;
    }
    
private:
    T* memory;
    size_t count;
};


template <typename T, size_t Capacity>
bool operator==(const FixedAllocator<T, Capacity> &, const FixedAllocator<T, Capacity> &) {
  return true;
}

template <typename T, size_t Capacity>
bool operator!=(const FixedAllocator<T, Capacity> &, const FixedAllocator<T, Capacity> &) {
  return false;
}