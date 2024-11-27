#include <iostream>
#include <vector>
#include <memory>
#include <string>
#include <stdexcept>

// Аллокатор с использованием пула памяти
template <typename T>
class PoolAllocator {
    struct Block {
        Block* next;
    };

    Block* freeList;                // Свободные блоки
    std::size_t blockSize;          // Размер одного блока
    std::size_t blocksPerChunk;     // Количество блоков в одном чанке
    std::vector<void*> chunks;      // Все выделенные чанки памяти

public:
    using value_type = T;

    explicit PoolAllocator(std::size_t blocksPerChunk = 1024)
        : freeList(nullptr), blockSize(sizeof(T)), blocksPerChunk(blocksPerChunk) {}

    // освобождаем всю выделенную память
    ~PoolAllocator() {
        for (void* chunk : chunks) {
            free(chunk);
        }
    }

    // Выделение памяти
    T* allocate(std::size_t n) {
        if (n != 1) throw std::bad_alloc(); // только по одному объекту
        if (!freeList) {
            allocateChunk(); 
        }
        Block* block = freeList;
        freeList = freeList->next;
        return reinterpret_cast<T*>(block);
    }

    // Освобождение памяти
    void deallocate(T* p, std::size_t) {
        Block* block = reinterpret_cast<Block*>(p);
        block->next = freeList;
        freeList = block;
    }

    template <typename U, typename... Args>
    void construct(U* p, Args&&... args) {
        //std::cout << "construct \n";
        new (p) U(std::forward<Args>(args)...); // placement new 
    }

    template <typename U>
    void destroy(U* p) {
        p->~U(); 
    }

    template <typename U>
    struct rebind {
        using other = PoolAllocator<U>;
    };

private:
    // Выделить новый чанк памяти и добавить блоки в список свободных
    void allocateChunk() {
        void* chunk = malloc(blockSize * blocksPerChunk);
        if (!chunk) throw std::bad_alloc(); // Проверяем на успешное выделение памяти
        chunks.push_back(chunk);
        char* start = static_cast<char*>(chunk);
        for (std::size_t i = 0; i < blocksPerChunk; ++i) {
            Block* block = reinterpret_cast<Block*>(start + i * blockSize);
            block->next = freeList;
            freeList = block;
        }
    }


};

template <typename T, typename U>
bool operator==(const PoolAllocator<T>&, const PoolAllocator<U>&) { return true; }

template <typename T, typename U>
bool operator!=(const PoolAllocator<T>&, const PoolAllocator<U>&) { return false; }

