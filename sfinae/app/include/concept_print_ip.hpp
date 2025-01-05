#pragma once

#include <iostream>
#include <type_traits>
#include <string>
#include <vector>
#include <list>
#include <tuple>

/**
 * @brief Концепт для проверки, что тип является целым числом.
 * 
 * Использует std::is_integral для проверки на целочисленный тип.
 * @tparam T Тип для проверки.
 */
template <typename T>
concept Integral = std::is_integral_v<T>;

/**
 * @brief Концепт для проверки, что тип является std::string.
 * 
 * Использует std::is_same для проверки на соответствие std::string.
 * @tparam T Тип для проверки.
 */
template <typename T>
concept String = std::is_same_v<T, std::string>;

/**
 * @brief Концепт для проверки, что тип является одним из заданных контейнеров.
 * 
 * Допустимые типы: std::vector<int>, std::list<short>.
 * @tparam T Тип для проверки.
 */
template <typename T>
concept Container =
    std::same_as<T, std::vector<int>> ||
    std::same_as<T, std::list<short>>;

/**
 * @brief Концепт для проверки, что тип является кортежем (std::tuple).
 * 
 * Проверяет наличие типа std::tuple_size и исключает void.
 * @tparam T Тип для проверки.
 */
template <typename T>
concept Tuple = requires {
    typename std::tuple_size<T>::type; ///< Проверяет наличие размера кортежа.
} && !std::is_same_v<T, void>; ///< Исключает тип void.

/**
 * @brief Вспомогательная функция для проверки, что все элементы кортежа одного типа.
 * 
 * @tparam T Тип кортежа.
 * @tparam FirstType Тип первого элемента кортежа.
 * @tparam I Индексы элементов в кортеже.
 * @return true, если все элементы имеют одинаковый тип.
 */
template <typename T, typename FirstType, std::size_t... I>
constexpr bool all_same_impl(std::index_sequence<I...>) {
    return (std::is_same_v<FirstType, std::tuple_element_t<I, T>> && ...);
}

/**
 * @brief Концепт для проверки, что все элементы кортежа одного типа.
 * 
 * Проверяет, что тип соответствует Tuple и все его элементы однотипны.
 * @tparam T Тип для проверки.
 */
template <typename T>
concept HomogeneousTuple = Tuple<T> &&
                           all_same_impl<T, std::tuple_element_t<0, T>>(
                               std::make_index_sequence<std::tuple_size_v<T>>{});

/**
 * @brief Класс для вывода IP-адресов в разных форматах.
 * 
 * Использует концепты для определения допустимых типов данных.
 */
class IpPrinterCpt
{
public:
    /**
     * @brief Выводит IP-адрес в формате целого числа.
     * 
     * Разбивает число на байты и выводит в формате "x.x.x.x".
     * @tparam T Тип целого числа.
     * @param ip IP-адрес в виде целого числа.
     */
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

    /**
     * @brief Выводит IP-адрес в формате строки.
     * 
     * @tparam T Тип строки (std::string).
     * @param ip IP-адрес в виде строки.
     */
    template <String T>
    static void print(const T &ip)
    {
        std::cout << ip << std::endl;
    }

    /**
     * @brief Выводит IP-адрес, представленный контейнером.
     * 
     * Разделяет элементы контейнера точками.
     * @tparam T Тип контейнера (std::vector<int>, std::list<short>).
     * @param container Контейнер с элементами IP-адреса.
     */
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

    /**
     * @brief Выводит IP-адрес, представленный кортежем.
     * 
     * Разделяет элементы кортежа точками.
     * @tparam T Тип кортежа с однотипными элементами.
     * @param tuple Кортеж с элементами IP-адреса.
     */
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

