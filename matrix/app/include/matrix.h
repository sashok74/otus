#pragma once
#include <iostream>
#include <map>
#include <utility>
#include <tuple>
#include <vector>
#include <algorithm>
#include <cstddef>
#include "coo.h"

/**
 * @file matrix.h
 * @brief Шаблонный класс разреженной матрицы.
 * 
 * Этот файл содержит определение шаблонного класса `matrix`, который представляет собой разреженную матрицу
 * с доступом к элементам через прокси-классы и возможностью итерации по заполненным элементам.
 * 
 * Более подробное описание принципов работы можно найти в @ref matrix_details.
 */

/// @brief Шаблонный класс разреженной матрицы.
/// @tparam T Тип элементов матрицы.
/// @tparam TypeOfMatrix Тип структуры хранения разреженной матрицы (по умолчанию `TypeOfMatrixMap<T>`).
/// @details Подробнее о принципах работы можно прочитать в @ref matrix_details.
template <typename T, typename TypeOfMatrix = TypeOfMatrixMap<T>>
class matrix
{
    TypeOfMatrix data;

public:
    /**
     * @brief Прокси-класс для доступа к элементам матрицы.
     * 
     * Используется для разделения операций чтения и записи элементов.
     * Подробнее о работе прокси-класса см. @ref proxy_element_details.
     */
    class ProxyElement
    {
        matrix &m;
        std::size_t row;
        std::size_t col;

    public:
        ProxyElement(matrix &m, std::size_t row, std::size_t col) : m(m), row(row), col(col) {}

        /// @brief Оператор приведения к типу `T` для чтения элемента.
        operator T() const
        {
            return m.data.getElement(row, col);
        }

        /// @brief Оператор присваивания для записи значения в элемент.
        ProxyElement &operator=(const T &value)
        {
            m.data.addElement(row, col, value);
            return *this;
        }
        
        /// @brief Оператор присваивания для копирования значения другого элемента.
        ProxyElement &operator=(const ProxyElement &other)
        {
            T value = other.m.data.getElement(other.row, other.col); 
            m.data.addElement(row, col, value);                      
            return *this;
        }
    };

    /**
     * @brief Итератор по заполненным элементам матрицы.
     * 
     * Позволяет перебирать только те элементы, которые явно заданы в матрице.
     * Подробнее о работе итератора см. @ref iterator_details.
     */
    class Iterator
    {
        typename TypeOfMatrix::Iterator it;

    public:
        Iterator(typename TypeOfMatrix::Iterator it) : it(it) {}

        Iterator &operator++()
        {
            ++it;
            return *this;
        }

        bool operator!=(const Iterator &other) const
        {
            return it != other.it;
        }

        std::tuple<std::size_t, std::size_t, T> operator*() const
        {
            return std::make_tuple(it.row(), it.col(), it.value());
        }
    };

    /// @brief Возвращает итератор на первый заполненный элемент.
    Iterator begin() const { return Iterator(data.begin()); }

    /// @brief Возвращает итератор на последний заполненный элемент.
    Iterator end() const { return Iterator(data.end()); }

    /// @brief Класс для неконстантного доступа к строкам матрицы.
    class Row
    {
        matrix &m;
        std::size_t row;

    public:
        Row(matrix &m, std::size_t row) : m(m), row(row) {}

        ProxyElement operator[](std::size_t col)
        {
            return ProxyElement(m, row, col);
        }
    };

    /// @brief Класс для константного доступа к строкам матрицы.
    class RowConst
    {
        const matrix &m;
        std::size_t row;

    public:
        RowConst(const matrix &m, std::size_t row) : m(m), row(row) {}

        const T operator[](std::size_t col) const
        {
            return m.data.getElement(row, col);
        }
    };

    /// @brief Конструктор по умолчанию.
    matrix(T zero = T()) : data(zero) {}

    /// @brief Конструктор с указанием размеров матрицы.
    matrix(std::size_t rows, std::size_t cols, T zero = T()) : data(rows, cols, zero) {}

    /// @brief Устанавливает значение элемента матрицы.
    void set(std::size_t row, std::size_t col, T value)
    {
        data.addElement(row, col, value);
    }

    /// @brief Получает значение элемента матрицы.
    T get(std::size_t row, std::size_t col) const
    {
        return data.getElement(row, col);
    }

    /// @brief Оператор `[]` для доступа к строкам.
    Row operator[](std::size_t row)
    {
        return Row(*this, row);
    }

    /// @brief Оператор `[]` для константного доступа к строкам.
    RowConst operator[](std::size_t row) const
    {
        return RowConst(*this, row);
    }

    /// @brief Возвращает количество заполненных элементов в матрице.
    std::size_t size() const
    {
        return data.size();
    }
};
