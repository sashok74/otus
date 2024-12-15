#pragma once

#include <iostream>
#include <type_traits>
#include <string>
#include <vector>
#include <list>
#include <tuple>

// Концепт для проверки на то, что тип является целым числом
template <typename T>
concept Integral = std::is_integral_v<T>;

// Концепт для проверки на то, что тип является std::string
template <typename T>
concept String = std::is_same_v<T, std::string>;

// Концепт для проверки, что тип является конкретным контейнером
template <typename T>
concept Container =
    std::same_as<T, std::vector<int>> ||
    std::same_as<T, std::list<short>>;

// Концепт для проверки, является ли тип std::tuple.
template <typename T>
concept Tuple = requires {
    typename std::tuple_size<T>::type; // Проверяет, что тип имеет размер, как у std::tuple.
} && !std::is_same_v<T, void>; // Исключает void.

// Концепт для проверки, что все элементы кортежа однотипны.
/* красиво но не прокатило.
template <typename T>
concept HomogeneousTuple = Tuple<T> &&
    requires {
        []<std::size_t... I>(std::index_sequence<I...>) {
            using FirstType = std::tuple_element_t<0, T>;
            return ((std::is_same_v<FirstType, std::tuple_element_t<I, T>>) && ...);
        }(std::make_index_sequence<std::tuple_size_v<T>>{});
    };*/

template <typename T, typename FirstType, std::size_t... I>
constexpr bool all_same_impl(std::index_sequence<I...>) {
    return (std::is_same_v<FirstType, std::tuple_element_t<I, T>> && ...);
}

template <typename T>
concept HomogeneousTuple = Tuple<T> &&
                           all_same_impl<T, std::tuple_element_t<0, T>>(
                               std::make_index_sequence<std::tuple_size_v<T>>{});

class IpPrinterCpt
{
public:
    template <Integral T>
    static void print(const T &ip)
    {
        constexpr size_t numBytes = sizeof(T);
        for (size_t i = numBytes; i-- > 0;)
        {
            uint8_t byte = (ip >> (i * 8)) & 0xFF;
            std::cout << static_cast<int>(byte);
            if (i > 0)
            {
                std::cout << '.';
            }
        }
        std::cout << std::endl;
    }

    template <String T>
    static void print(const T &ip)
    {
        std::cout << ip << std::endl;
    }

    template <Container T>
    static void print(const T &container)
    {
        auto it = container.begin();
        if (it != container.end())
        {
            std::cout << *it;
            ++it;
        }
        for (; it != container.end(); ++it)
        {
            std::cout << "." << *it;
        }
        std::cout << std::endl;
    }

template <HomogeneousTuple T>
    static void print(const T &tuple)
    {
        std::apply([](const auto &...args)
                   {
            size_t n = 0;
            ((std::cout << (n++ ? "." : "") << args), ...);
            std::cout << std::endl; }, tuple);
    }
};
