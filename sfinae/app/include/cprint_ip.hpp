#pragma once
#include <iostream>
#include <type_traits>
#include <string>
#include <list>
#include <vector>
#include <tuple>
#include <utility>
#include <sstream>
#include <cctype> 
#include <algorithm>
/**
 * @class IpPrinter
 * @brief Класс для вывода IP-адресов, представленных в различных форматах.
 *
 * IpPrinter предоставляет шаблонный метод для форматирования и вывода IP-адресов,
 * представленных в различных типах данных, таких как целочисленные типы, строки,
 * контейнеры (например, std::vector<int>, std::list<short>) и кортежи (std::tuple) с однотипными элементами.
 */
class IpPrinter
{
public:
    /**
     * @brief Выводит IP-адрес в формате, соответствующем типу входного значения.
     *
     * Шаблонный метод поддерживает следующие типы:
     * - **Целочисленные типы (int, long, uint32_t и т.д.)**: выводятся в формате IPv4, где каждые 8 бит
     *   преобразуются в байт и разделяются точкой.
     * - **std::string**: выводится как строка с пояснением.
     * - **Контейнеры (std::vector<int> или std::list<short>)**: значения выводятся через точку.
     * - **std::tuple с однотипными элементами**: значения кортежа выводятся через точку.
     *
     * @tparam T Тип входного значения.
     * @param ip IP-адрес, представленный в одном из поддерживаемых форматов.
     *
     * @note Для неподдерживаемых типов срабатывает static_assert.
     */
    template <typename T>
    void print(const T &ip) const
    {
        if constexpr (std::is_integral_v<T>)
        {
            // Если T - целочисленный тип
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
        else if constexpr (std::is_same_v<T, std::string>)
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
                // Убираем начальные и завершающие пробелы для красоты
                part.erase(part.begin(), std::find_if(part.begin(), part.end(), [](unsigned char ch) { return !std::isspace(ch); }));
                std::cout << part;
                first = false;
            }
            std::cout << std::endl;
        }
        else if constexpr (is_vector_int_or_list_short<T>::value)
        {
            // Если T - std::vector<int> или std::list<short>
            auto it = ip.begin();
            if (it != ip.end())
            {
                std::cout << *it;
                ++it;
            }

            for (; it != ip.end(); ++it)
            {
                std::cout << "." << *it;
            }

            std::cout << std::endl;
        }
        else if constexpr (is_tuple<T>::value && all_same_type<T>::value)
        {
            // Если T - std::tuple с одинаковыми типами
            std::apply([](const auto &...args)
                       {
                size_t n = 0;
                ((std::cout << (n++ ? "." : "") << args), ...);
                std::cout << std::endl; }, ip);
        }
        else
        {
            static_assert(always_false<T>::value, "Unsupported type");
        }
    }

private:
    /**
     * @brief Проверка, является ли тип контейнером std::vector<int> или std::list<short>.
     *
     * Используется для определения, может ли объект быть выведен как IP-адрес.
     * @tparam T Тип контейнера.
     */
    template <typename T>
    struct is_vector_int_or_list_short
        : std::bool_constant<
              std::is_same_v<T, std::vector<int>> || std::is_same_v<T, std::list<short>>>
    {
    };

    /**
     * @brief Проверка, является ли тип кортежем (std::tuple).
     *
     * @tparam T Проверяемый тип.
     */
    template <typename T>
    struct is_tuple : std::false_type
    {
    };

    /**
     * @brief Специализация шаблона для std::tuple.
     */
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

    /**
     * @brief Специализация для проверки однотипных элементов в std::tuple.
     */
    template <typename... T>
    struct all_same_type<std::tuple<T...>> : all_same_type<T...>
    {
    };

    /**
     * @brief Заглушка для static_assert, чтобы компиляция не проходила для неподдерживаемых типов.
     *
     * @tparam T Проверяемый тип.
     */
    template <typename>
    struct always_false : std::false_type
    {
    };
};
