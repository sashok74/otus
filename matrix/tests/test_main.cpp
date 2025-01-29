
#include <gtest/gtest.h>
#include "matrix.h"

// Тест для создания матрицы по умолчанию
TEST(MatrixTest, DefaultConstructor)
{
    matrix<int> m;
    EXPECT_EQ(m.size(), 0); // Матрица пустая
}

// Тест для создания матрицы с размером 3x3 и значением по умолчанию
TEST(MatrixTest, ConstructorWithSize)
{
    matrix<int> m(0);
    m.set(0, 0, 0);
    EXPECT_EQ(m.size(), 0); // Матрица по-прежнему пуста, так как это разреженная матрица
}

// Тест для установки и получения значения элемента матрицы
TEST(MatrixTest, SetGetElement)
{
    matrix<int> m(0);
    m.set(0, 0, 5);
    EXPECT_EQ(m.get(0, 0), 5);
    m.set(1, 1, 10);
    EXPECT_EQ(m.get(1, 1), 10);
}

// Тест для оператора доступа []
TEST(MatrixTest, AccessOperator)
{
    matrix<int> m(0);
    m[0][0] = 1;
    m[1][1] = 2;
    m[2][2] = 3;
    EXPECT_EQ(m[0][0], 1);
    EXPECT_EQ(m[1][1], 2);
    EXPECT_EQ(m[2][2], 3);
}

// Тест для итераторов
TEST(MatrixTest, IteratorTest)
{
    matrix<int> m(0);
    m.set(0, 0, 1);
    m.set(1, 1, 2);
    m.set(2, 2, 3);

    std::vector<int> values;
    for (auto it = m.begin(); it != m.end(); ++it) {
        values.push_back(std::get<2>(*it));
    }

    EXPECT_EQ(values.size(), 3); // Должно быть 3 элемента
    EXPECT_EQ(values[0], 1);
    EXPECT_EQ(values[1], 2);
    EXPECT_EQ(values[2], 3);
}

// Тест для копирования значений через прокси
TEST(MatrixTest, ProxyAssignment)
{
    matrix<int> m(0);
    m[0][0] = 5;
    matrix<int> m2(0);
    m2[0][0] = m[0][0];
    EXPECT_EQ(m2.get(0, 0), 5);
}

// Тест на корректную работу с разными типами
TEST(MatrixTest, DifferentTypes)
{
    matrix<double> m(0.0);
    m.set(0, 0, 5.5);
    EXPECT_EQ(m.get(0, 0), 5.5);
    m.set(1, 1, 10.5);
    EXPECT_EQ(m.get(1, 1), 10.5);
}

// Тест на количество заполненных элементов
TEST(MatrixTest, SizeTest)
{
    matrix<int> m(0);
    EXPECT_EQ(m.size(), 0); // Поначалу пустая

    m.set(0, 0, 1);
    EXPECT_EQ(m.size(), 1);

    m.set(1, 1, 2);
    EXPECT_EQ(m.size(), 2);

    m.set(2, 2, 3);
    EXPECT_EQ(m.size(), 3);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}