#ifndef LRUCACHE_H
#define LRUCACHE_H

#include <list>
#include <stdexcept>
#include <unordered_map>
#include <utility>

/**
 * @brief Контейнер LRU-кэша (Least Recently Used).
 *
 * Этот шаблонный класс реализует простой LRU-кэш, который поддерживает
 * отображение ключей в значения и автоматически удаляет дольше всех
 * не используемые элементы, когда достигается заданная ёмкость.
 *
 * @tparam Key Тип ключа.
 * @tparam Value Тип значения.
 */
template <typename Key, typename Value> class LRUCache
{
  public:
    /**
     * @brief Конструктор LRUCache.
     *
     * Инициализирует кэш с заданной ёмкостью.
     *
     * @param capacity Максимальное количество элементов, которое можно хранить.
     * @throws std::invalid_argument Если capacity равно 0.
     */
    explicit LRUCache(std::size_t capacity) : capacity_(capacity)
    {
        if (capacity_ == 0)
            throw std::invalid_argument("кеш не может быть размером 0");
    }

    /**
     * @brief Получает значение по ключу.
     *
     * Если элемент найден, возвращает указатель на значение и обновляет его
     * положение в списке LRU (перемещая его в начало списка).
     *
     * @param key Ключ, по которому производится поиск.
     * @return Указатель на значение, если элемент найден, иначе nullptr.
     */
    Value *get(const Key &key)
    {
        auto it = cacheMap_.find(key);
        if (it == cacheMap_.end())
            return nullptr;
        // Перемещаем элемент в начало списка, отмечая его как недавно использованный.
        lruList_.splice(lruList_.begin(), lruList_, it->second.second);
        return &(it->second.first);
    }

    /**
     * @brief Добавляет или обновляет значение по ключу.
     *
     * Если элемент с данным ключом уже существует, его значение обновляется и
     * перемещается в начало списка. Если элемента нет и кэш переполнен, то
     * удаляется наименее недавно использованный элемент перед добавлением нового.
     *
     * Данная версия использует универсальную ссылку, что позволяет корректно работать
     * с типами, которые не поддерживают копирование, используя перемещение.
     *
     * @tparam T Тип значения (обычно Value).
     * @param key Ключ элемента.
     * @param value Значение, связанное с ключом.
     */
    template <typename T> void put(const Key &key, T &&value)
    {
        auto it = cacheMap_.find(key);
        if (it != cacheMap_.end())
        {
            // Обновляем значение с использованием перемещения и отмечаем элемент как недавно
            // использованный.
            it->second.first = std::forward<T>(value);
            lruList_.splice(lruList_.begin(), lruList_, it->second.second);
        }
        else
        {
            // Если кэш переполнен, удаляем наименее недавно использованный элемент.
            if (cacheMap_.size() >= capacity_)
            {
                const Key &lruKey = lruList_.back();
                cacheMap_.erase(lruKey);
                lruList_.pop_back();
            }
            // Вставляем новый элемент.
            lruList_.push_front(key);
            cacheMap_.emplace(key, std::make_pair(std::forward<T>(value), lruList_.begin()));
        }
    }

    /**
     * @brief Удаляет элемент из кэша по ключу.
     *
     * @param key Ключ удаляемого элемента.
     */
    void remove(const Key &key)
    {
        auto it = cacheMap_.find(key);
        if (it != cacheMap_.end())
        {
            lruList_.erase(it->second.second);
            cacheMap_.erase(it);
        }
    }

    /**
     * @brief Очищает кэш, удаляя все элементы.
     */
    void clear()
    {
        cacheMap_.clear();
        lruList_.clear();
    }

  private:
    std::size_t capacity_; ///< Максимальное количество элементов в кэше.
    std::list<Key> lruList_; ///< Список ключей в порядке использования:
                             ///< front — наиболее недавно использованный,
                             ///< back — наименее недавно использованный.
    /**
     * @brief Карта, отображающая ключи в пары значений и итераторов в списке LRU.
     */
    std::unordered_map<Key, std::pair<Value, typename std::list<Key>::iterator>> cacheMap_;
};

#endif // LRUCACHE_H
