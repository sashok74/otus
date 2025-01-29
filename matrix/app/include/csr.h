#pragma once
#include <iostream>
#include <map>
#include <utility>
#include <tuple>
#include <vector>
#include <algorithm>
#include <cstddef>

template <std::size_t... I>
constexpr std::array<int, sizeof...(I)> makeArray(std::index_sequence<I...>)
{
    return {static_cast<int>(I)...};
}

template <typename T>
class SparseMatrixCSR
{
    std::vector<T> values;                // Ненулевые значения
    std::vector<std::size_t> colIndices;  // Соответствующие индексы столбцов
    std::vector<std::size_t> rowPointers; // Указатели на начало каждой строки в values и colIndices
    std::size_t numRows;
    std::size_t numCols;
    T zero; // Значение для пустых элементов (обычно T())

public:
    SparseMatrixCSR(std::size_t row, std::size_t col, T zero = T())
        : numRows(row), numCols(col), zero(zero)
    {
        rowPointers.resize(numRows + 1, 0);
    }

    // Добавление элемента в матрицу
    void addElement(std::size_t row, std::size_t col, T value)
    {
        if (row >= numRows || col >= numCols)
            throw std::out_of_range("Индексы строки или столбца вне допустимого диапазона.");

        // Ищем позицию для вставки
        std::size_t rowStart = rowPointers[row];
        std::size_t rowEnd = rowPointers[row + 1];

        auto it = std::lower_bound(colIndices.begin() + rowStart, colIndices.begin() + rowEnd, col);
        std::size_t index = it - colIndices.begin();

        if (it != colIndices.begin() + rowEnd && *it == col)
        {
            // Элемент уже существует, обновляем значение
            values[index] = value;
        }
        else
        {
            // Вставляем новый элемент
            colIndices.insert(it, col);
            values.insert(values.begin() + index, value);

            // Обновляем rowPointers
            for (std::size_t i = row + 1; i < rowPointers.size(); ++i)
            {
                rowPointers[i]++;
            }
        }
    }

    // Получение константного значения элемента
    T getElement(std::size_t row, std::size_t col) const
    {
        if (row >= numRows || col >= numCols)
            throw std::out_of_range("Индексы строки или столбца вне допустимого диапазона.");

        std::size_t rowStart = rowPointers[row];
        std::size_t rowEnd = rowPointers[row + 1];

        auto it = std::lower_bound(colIndices.begin() + rowStart, colIndices.begin() + rowEnd, col);
        if (it != colIndices.begin() + rowEnd && *it == col)
        {
            std::size_t index = it - colIndices.begin();
            return values[index];
        }
        else
        {
            return zero;
        }
    }

    // Вложенный класс итератора
    class Iterator
    {
        const SparseMatrixCSR<T> &matrix;
        std::size_t iterRow;
        std::size_t index;

    public:
        Iterator(const SparseMatrixCSR<T> &matrix, std::size_t iterRow, std::size_t index)
            : matrix(matrix), iterRow(iterRow), index(index) {}

        Iterator &operator++()
        {
            ++index;
            while (iterRow < matrix.numRows && index >= matrix.rowPointers[iterRow + 1])
            {
                ++iterRow;
            }
            return *this;
        }

        bool operator!=(const Iterator &other) const
        {
            return index != other.index || iterRow != other.iterRow;
        }

        std::tuple<std::size_t, std::size_t, T> operator*() const
        {
            return {iterRow, matrix.colIndices[index], matrix.values[index]};
        }

        std::size_t row() const { return iterRow; }
        std::size_t col() const { return matrix.colIndices[index]; }
        const T &value() const { return matrix.values[index]; }
    };

    // Методы для получения итераторов
    Iterator begin() const
    {
        std::size_t row = 0;
        while (row < numRows && rowPointers[row] == rowPointers[row + 1])
        {
            ++row;
        }
        return Iterator(*this, row, rowPointers[row]);
    }

    Iterator end() const
    {
        return Iterator(*this, numRows, values.size());
    }

    // Дополнительные методы для получения размеров
    std::size_t getNumRows() const { return numRows; }
    std::size_t getNumCols() const { return numCols; }
};
