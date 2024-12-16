#pragma once

#include <iostream>
#include <type_traits>
#include <string>
#include <list>
#include <vector>
#include <tuple>
#include <utility>
#include <sstream>

/// @brief Шаблонная функция для вывода IP-адреса, если тип является целочисленным.
/// @tparam T Тип данных (ограничен только целочисленными типами, такими как int, short, char и т.д.).
/// @param ip Целочисленное значение IP-адреса, которое нужно вывести.
/// Функция выводит IP-адрес, преобразуя каждую группу байтов в число от 0 до 255.
template <typename T>
std::enable_if_t<std::is_integral<T>::value, void>
print_ip(T ip)
{
    constexpr size_t numBytes = sizeof(T); // Количество байтов в типе T
    for (size_t i = numBytes; i-- > 0;)
    {
        // Извлечение байта на позиции i
        uint8_t byte = (ip >> (i * 8)) & 0xFF;
        std::cout << static_cast<int>(byte);
        if (i > 0)
        {
            std::cout << '.'; // Разделитель между байтами
        }
    }
    std::cout << std::endl;
}

/// @brief Шаблонная функция для вывода строки, если тип является std::string.
/// @tparam T Тип данных (ограничен std::string).
/// @param ip Строковое значение IP-адреса.
/// Функция выводит строку с дополнительным пояснением.
template <typename T>
std::enable_if_t<std::is_same<T, std::string>::value, void>
print_ip(T ip)
{
    std::istringstream stream(ip);
    std::string part;
    bool first = true;

    while (std::getline(stream, part, ','))
    { // Разделяем строку по запятой
        if (!first)
        {
            std::cout << ".";
        }
        std::cout << part;
        first = false;
    }
    std::cout << std::endl;
}

/// @brief Шаблонная функция для вывода контейнеров std::vector<int> и std::list<short>.
/// @tparam T Тип контейнера (ограничен std::vector<int> и std::list<short>).
/// @param container Контейнер с числами, представляющими части IP-адреса.
/// Функция выводит содержимое контейнера, разделяя элементы точками.
template <typename T>
std::enable_if_t<
    (std::is_same_v<T, std::vector<int>> ||
     std::is_same_v<T, std::list<short>>),
    void>
print_ip(const T &container)
{
    auto it = container.begin();
    if (it != container.end())
    {
        std::cout << *it; // Вывод первого элемента
        ++it;
    }

    for (; it != container.end(); ++it)
    {
        std::cout << "." << *it; // Вывод оставшихся элементов через точку
    }

    std::cout << std::endl;
}

/// @brief Структура для проверки, является ли тип std::tuple.
/// @tparam T Тип для проверки.
template <typename T>
struct is_tuple : std::false_type
{
};

/// @brief Специализация структуры для std::tuple.
/// @tparam Args Параметры типа tuple.
template <typename... Args>
struct is_tuple<std::tuple<Args...>> : std::true_type
{
};

/// @brief Шаблонная структура для проверки, являются ли все элементы однотипными.
/// @tparam First Первый тип.
/// @tparam Rest Оставшиеся типы.
template <typename First, typename... Rest>
struct all_same_type
{
    static constexpr bool value = (std::is_same_v<First, Rest> && ...);
};

/// @brief Специализация структуры для проверки всех элементов std::tuple.
/// @tparam T Параметры tuple.
template <typename... T>
struct all_same_type<std::tuple<T...>> : all_same_type<T...>
{
};

/// @brief Шаблонная функция для вывода содержимого std::tuple, если все его элементы однотипны.
/// @tparam T Тип данных (ограничен std::tuple, где все элементы имеют одинаковый тип).
/// @param t Кортеж значений, представляющий части IP-адреса.
/// Функция выводит элементы кортежа через точки.
template <typename T>
std::enable_if_t<is_tuple<T>::value && all_same_type<T>::value, void>
print_ip(const T &t)
{
    std::apply([](const auto &...args)
               {
        size_t n = 0;
        ((std::cout << (n++ ? "." : "") << args), ...); // Вывод элементов с разделением точками
        std::cout << std::endl; }, t);
}
