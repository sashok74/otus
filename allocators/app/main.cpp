#include "allocator_stack.h"
#include "allocator_fix.h"
#include "allocator_pool.h"
#include "CustomContainer.h"
#include <iostream>
#include <memory>
#include <map>
#include <stdexcept>
#include <chrono>
#include <random>

template <typename Func>
void benchmark(const std::string &name, Func func)
{
    auto start = std::chrono::high_resolution_clock::now();
    func();
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;
    std::cout << name << ": " << elapsed.count() << " seconds\n";
}

// Тест производительности
void testPerformance()
{
    const int numElements = 1'000'000;

    // Генератор случайных чисел
    std::mt19937 gen(42);
    std::uniform_int_distribution<int> dist(1, numElements);

    // Стандартный аллокатор
    benchmark("Standard Allocator", [&]()
              {
        std::map<int, std::string> myMap;
        for (int i = 0; i < numElements; ++i) {
            myMap.emplace(i, "Value");
        } });

    // Кастомный аллокатор
    benchmark("Custom Pool Allocator", [&]()
              {
        using CustomAllocator = PoolAllocator<std::pair<const int, std::string>>;
        std::map<int, std::string, std::less<>, CustomAllocator> myMap;
        for (int i = 0; i < numElements; ++i) {
            myMap.emplace(i, "Value");
        } });
}

// Тест производительности чтений из карты
void testReadPerformance()
{
    const int numReads = 1'000'000; // Количество операций чтения
    const int numElements = 100;    // Количество записей в карте

    // Генератор случайных ключей
    std::mt19937 gen(42);
    std::uniform_int_distribution<int> dist(0, numElements - 1);

    // Размер буфера
    constexpr std::size_t BufferSize = 10 * 1024; // 10 КБ
    alignas(std::max_align_t) char buffer[BufferSize];

    // Стандартный аллокатор
    benchmark("Standard Allocator - Reads", [&]()
              {
        std::map<int, std::string> myMap;
        for (int i = 0; i < numElements; ++i) {
            myMap.emplace(i, "Value");
        }

        for (int i = 0; i < numReads; ++i) {
            for (int j = 0; j < 100; j++)
                volatile auto res = myMap[j]; 
        } });

    // Кастомный аллокатор (StackAllocator)
    benchmark("Stack Allocator - Reads", [&]()
              {
        using CustomAllocator = StackAllocator<std::pair<const int, std::string>, BufferSize / sizeof(std::pair<const int, std::string>)>;
        std::map<int, std::string, std::less<>, CustomAllocator> myMap{CustomAllocator(buffer)};
        for (int i = 0; i < numElements/100; ++i) {
            myMap.emplace(i, "Value");
        }

        for (int i = 0; i < numReads; ++i) {
            for (int j = 0; j < 100; j++)
                volatile auto res = myMap[j]; 
        } });
}

int factorial(int n)
{
    int result = 1;
    for (int i = 2; i <= n; ++i)
    {
        result *= i;
    }
    return result;
}

class my_class
{
public:
    int i;
    std::string s;
    my_class(int I, std::string S) : i(I), s(S)
    {
       // std::cout << "my_class  \n";
    }
};

int main()
{
    std::cout << "std::map \n";
    std::map<int, int> map_std;
    for (int i = 0; i < 10; ++i)
    {
        map_std[i] = factorial(i);
    }
    for (auto &pair : map_std)
    {
        std::cout << pair.first << " " << pair.second << std::endl;
    }

    std::cout << "std::map пул фиксированное количество  \n";
    using AllocatorStr = FixedAllocator<std::pair<const int, std::string>, 10>; // Аллоцируем до 10 элементов
    std::map<int, std::string, std::less<>, AllocatorStr> map_fix_str;
    for (int i = 0; i < 10; ++i)
    {
        map_fix_str.emplace(i, "строка " + std::to_string(factorial(i)));
    }
    for (auto &pair : map_fix_str)
    {
        std::cout << pair.first << " " << pair.second << std::endl;
    }
/*
    std::map<int, std::string, std::less<>, AllocatorStr> map_fix_str2;
    map_fix_str2 = map_fix_str;

    for (auto &pair : map_fix_str2)
    {
        std::cout << pair.first << " " << pair.second << std::endl;
    }*/

    std::map<int, std::string, std::less<>, AllocatorStr> map_fix_str2(std::move(map_fix_str));

    for (auto &pair : map_fix_str2)
    {
        std::cout << pair.first << " " << pair.second << std::endl;
    }

    std::cout << "std::map пул фиксированное количество  \n";
    using Allocator = FixedAllocator<std::pair<const int, int>, 10>; // Аллоцируем до 10 элементов
    std::map<int, int, std::less<>, Allocator> map_fix;
    for (int i = 0; i < 10; ++i)
    {
        map_fix[i] = factorial(i);
    }
    for (auto &pair : map_fix)
    {
        std::cout << pair.first << " " << pair.second << std::endl;
    }

    std::cout << "std::map пул фиксированное количество на стеке \n";
    constexpr size_t capacity = 100;
    alignas(std::pair<const int, int>) char buffer[capacity * sizeof(std::pair<const int, int>)];
    using Alloc = StackAllocator<std::pair<const int, int>, capacity>;
    Alloc allocator(buffer);
    std::map<int, int, std::less<>, Alloc> map_stack(std::less<>{}, allocator);
    for (int i = 0; i < 10; ++i)
    {
        map_stack[i] = factorial(i);
    }
    for (auto &pair : map_stack)
    {
        std::cout << pair.first << " " << pair.second << std::endl;
    }

    std::cout << "std::map пул расширяемый  \n";
    using PoolAlloc = PoolAllocator<std::pair<const int, int>>; 
    std::map<int, int, std::less<>, PoolAlloc> map_pool;
    for (int i = 0; i < 10; ++i)
    {
        map_pool[i] = factorial(i);
    }
    for (auto &pair : map_pool)
    {
        std::cout << pair.first << " " << pair.second << std::endl;
    }

    std::cout << "свой контейнер аллокатор стандартный  \n";
    CustomContainer<int> container;
    for (int i = 0; i < 10; ++i)
    {
        container.push_back(factorial(i));
    }
    int index = 0;
    for (auto it = container.begin(); it != container.end(); ++it)
    {
        std::cout << index << " " << *it << std::endl;
        index++;
    }

    std::cout << "свой контейнер  и pool аллокатор  \n";
    using CustomAPoolAlloc = PoolAllocator<int>;
    CustomContainer<int, CustomAPoolAlloc> container2;
    for (int i = 0; i < 10; ++i)
    {
        container2.push_back(factorial(i));
    }
    index = 0;
    for (auto it = container2.begin(); it != container2.end(); ++it)
    {
        std::cout << index << " " << *it << std::endl;
        index++;
    }

    std::cout << "свой контейнер  pool аллокатор emplace_back \n";
    using CustomAPoolAlloc2 = PoolAllocator<my_class>;
    CustomContainer<my_class, CustomAPoolAlloc2> container3;
    for (int i = 0; i < 10; ++i)
    {
        container3.emplace_back(i, "строка " + std::to_string(factorial(i)));
    }

    for (auto &it : container3)
    {
        std::cout << " i:" << it.i << " fac:" << it.s << std::endl;
    }

    testPerformance();
    testReadPerformance();
    return 0;
}