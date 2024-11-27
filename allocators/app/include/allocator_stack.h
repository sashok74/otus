#pragma once
#include <stdexcept>
#include <cstddef>
#include <memory>

template <typename T, std::size_t Capacity>
class StackAllocator {
public:
    using value_type = T;

    static constexpr std::size_t BufferSize = Capacity * sizeof(T);
    alignas(T) char* buffer; // Внешний буфер
    std::size_t used = 0;

    explicit StackAllocator(char* extBuffer) : buffer(extBuffer) {}

    template <typename U>
    StackAllocator(const StackAllocator<U, Capacity>& other) noexcept
        : buffer(other.buffer), used(other.used) {}

    StackAllocator(const StackAllocator&) = default;
    StackAllocator& operator=(const StackAllocator&) = default;

    template <typename U>
    struct rebind {
        using other = StackAllocator<U, Capacity>;
    };

    T* allocate(std::size_t n) {
        if (used + n * sizeof(T) > BufferSize) {
            throw std::bad_alloc();  // Недостаточно памяти
        }
        T* result = reinterpret_cast<T*>(buffer + used);
        used += n * sizeof(T);
        return result;
    }

    void deallocate(T* p, std::size_t n) {
        // Никаких действий не требуется
    }
};