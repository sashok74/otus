// COO - Coordinate List
#pragma once
#include <map>

template <typename T>
class TypeOfMatrixMap
{
    T zero;
    using ColRow = std::pair<std::size_t, std::size_t>;
    std::map<ColRow, T> data;

public:
    TypeOfMatrixMap(T zero = T()) : zero(zero) {}

    void addElement(std::size_t row, std::size_t col, T value)
    {
        if (value == zero)
        {
            if (auto it = data.find({row, col}); it != data.end())
            {
                data.erase(it);
            }
            return;
        }
        data[{row, col}] = value;
    }

    T &getElementRef(std::size_t row, std::size_t col)
    {
        return data[{row, col}];
    }

    const T getElement(std::size_t row, std::size_t col) const
    {
        auto it = data.find({row, col});
        if (it != data.end())
        {
            return it->second;
        }
        return zero;
    }

    // Вложенный класс итератора
    class Iterator
    {
        typename std::map<ColRow, T>::const_iterator it;

    public:
        Iterator(typename std::map<ColRow, T>::const_iterator mapIt) : it(mapIt) {}

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
            return {it->first.first, it->first.second, it->second};
        }

        // доступ к row, column, and value
        std::size_t row() const { return it->first.first; }
        std::size_t col() const { return it->first.second; }
        const T &value() const { return it->second; }
    };

    // Методы для получения итераторов
    Iterator begin() const
    {
        return Iterator(data.cbegin());
    }

    Iterator end() const
    {
        return Iterator(data.cend());
    }

    std::size_t size() const
    {
        return data.size();
    }
};
