
#include <gtest/gtest.h>
#include "allocator_stack.h"
#include "allocator_fix.h"
#include "allocator_pool.h"
#include "CustomContainer.h"
#include <vector>
#include <map>

class StackAllocatorTest : public ::testing::Test {
protected:
    static constexpr int capacity = 10;
    alignas(int) char buffer[capacity * sizeof(int)];  // Буфер под аллокатор
    StackAllocator<int, capacity> allocator{buffer};

    void SetUp() override {
        // Подготовка перед каждым тестом, если необходимо
    }

    void TearDown() override {
        // Очистка после каждого теста, если необходимо
    }
};

TEST_F(StackAllocatorTest, Allocate) {
    ASSERT_NO_THROW({
        int* p = allocator.allocate(1);
        ASSERT_NE(p, nullptr);
        *p = 10;  // Простое присвоение для проверки доступности памяти
    });
}

TEST_F(StackAllocatorTest, Overflow) {
    ASSERT_THROW(allocator.allocate(capacity + 1), std::bad_alloc);
}

TEST_F(StackAllocatorTest, AllocateMultipleTimes) {
    int* first = allocator.allocate(5);
    int* second = allocator.allocate(4);  // Должно умещаться
    ASSERT_EQ(second - first, 5);  // Расстояние между блоками должно быть 5 int
    ASSERT_NO_THROW(allocator.allocate(1));  // Должно еще умещаться
    ASSERT_THROW(allocator.allocate(1), std::bad_alloc);  // Переполнение
}

class CustomContainerTest : public ::testing::Test {
protected:
    CustomContainer<int> container;

    void SetUp() override {
        // Подготовка перед каждым тестом
    }

    void TearDown() override {
        // Очистка после каждого теста
    }
};

TEST_F(CustomContainerTest, EmptyOnCreation) {
    EXPECT_TRUE(container.empty());
    EXPECT_EQ(container.size(), 0);
}

TEST_F(CustomContainerTest, PushBackIncreasesSize) {
    container.push_back(10);
    EXPECT_FALSE(container.empty());
    EXPECT_EQ(container.size(), 1);
}

TEST_F(CustomContainerTest, ClearContainer) {
    container.push_back(10);
    container.push_back(20);
    container.clear();
    EXPECT_TRUE(container.empty());
}

TEST_F(CustomContainerTest, IterateThroughElements) {
    container.push_back(10);
    container.push_back(20);
    int sum = 0;
    for (auto it = container.begin(); it != container.end(); ++it) {
        sum += *it;
    }
    EXPECT_EQ(sum, 30);
}


// Тесты для std::map с использованием PoolAllocator
TEST(MapWithPoolAllocatorTest, AddAndRemoveElements) {

    using PoolAlloc = PoolAllocator<std::pair<const int, int>>; 
    std::map<int, int, std::less<>, PoolAlloc> myMap;

    // Добавление элементов
    const int numElements = 100000;
    for (int i = 0; i < numElements; ++i) {
        myMap[i] = i * 2;
    }

    // Проверка добавления
    for (int i = 0; i < numElements; ++i) {
        EXPECT_EQ(myMap[i], i * 2);
    }

    // Удаление элементов
    for (int i = 0; i < numElements; i += 2) {
        myMap.erase(i);
    }

    // Проверка удаления
    for (int i = 0; i < numElements; i += 2) {
        EXPECT_EQ(myMap.find(i), myMap.end());
    }

    // Проверка оставшихся элементов
    for (int i = 1; i < numElements; i += 2) {
        EXPECT_NE(myMap.find(i), myMap.end());
    }

}


int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}