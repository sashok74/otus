#pragma once

#include <boost/algorithm/string/replace.hpp>
#include <boost/crc.hpp>
#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>
#include <boost/regex.hpp>
#include <boost/uuid/detail/md5.hpp>

#include <array>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <memory>
#include <set>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>
namespace fs = boost::filesystem;

/**
 * @brief Структура для представления значения хэша.
 */
struct HashValue
{
    std::array<unsigned char, 16> data;

    bool operator==(const HashValue &other) const { return data == other.data; }
};

/**
 * @brief Перегружает оператор << для вывода объекта HashValue в поток.
 *
 * Выводит содержимое хэш-значения в виде шестнадцатеричной строки.
 *
 * @param os Поток вывода.
 * @param hv Объект HashValue.
 * @return std::ostream& Ссылка на поток вывода.
 */
std::ostream &operator<<(std::ostream &os, const HashValue &hv);

/**
 * @brief Специализация std::hash для структуры HashValue.
 */
namespace std
{
template <> struct hash<HashValue>
{
    std::size_t operator()(const HashValue &hv) const
    {
        std::size_t result;
        std::memcpy(&result, hv.data.data(), sizeof(result));
        return result;
    }
};
} // namespace std

/**
 * @brief Абстрактный базовый класс для алгоритмов хэширования.
 *
 * Реализует паттерн «Стратегия» для вычисления хэшей.
 */
class IHashAlgorithm
{
  public:
    /**
     * @brief Вычисляет хэш для заданного блока данных.
     *
     * @param data Указатель на данные.
     * @param length Размер данных.
     * @return HashValue Вычисленный хэш.
     */
    virtual HashValue compute(const char *data, std::size_t length) const = 0;

    virtual ~IHashAlgorithm() = default;
};

/**
 * @brief Класс для вычисления хэша с использованием алгоритма CRC32.
 */
class CRC32HashAlgorithm : public IHashAlgorithm
{
  public:
    virtual HashValue compute(const char *data, std::size_t length) const override;
};

/**
 * @brief Класс для вычисления хэша с использованием алгоритма MD5.
 */
class MD5HashAlgorithm : public IHashAlgorithm
{
  public:
    virtual HashValue compute(const char *data, std::size_t length) const override;
};

/// @brief  Структура для хранения открытого файла.
struct FileCacheEntry {
  std::ifstream stream;
  std::size_t currentBlock = 0; // номер следующего ожидаемого блока
};

/**
 * @brief Класс для поиска дублирующихся файлов.
 *
 * Класс сканирует заданные директории, фильтрует файлы по маскам и минимальному
 * размеру, затем группирует файлы по хэшам считанных блоков.
 */
class DuplicateFinder
{
  public:
    /**
     * @brief Конструктор.
     *
     * @param directories Список директорий для сканирования.
     * @param excludeDirs Директории, которые нужно исключить.
     * @param maxDepth Максимальная глубина рекурсии.
     * @param fileMasks Маски файлов для фильтрации.
     * @param minFileSize Минимальный размер файла.
     * @param blockSize Размер блока для чтения файла.
     * @param hashAlgorithmType Тип алгоритма хэширования ("crc32" или "md5").
     */
    DuplicateFinder(const std::vector<std::string> &directories,
                    const std::set<std::string> &excludeDirs,
                    int maxDepth,
                    const std::vector<std::string> &fileMasks,
                    std::size_t minFileSize,
                    std::size_t blockSize,
                    const std::string &hashAlgorithmType);

    /**
     * @brief Запускает процесс сканирования директорий.
     *
     * @return std::vector<std::string> Список найденных дубликатов.
     */
    std::vector<std::string> scan();

    /**
     * @brief Регистрирует группу дублирующихся файлов.
     *
     * @param duplicateList Список дублирующихся файлов.
     */
    void registerDuplicateList(std::vector<std::string> &duplicateList);

    /**
     * @brief Читает блок данных из файла.
     *
     * @param filePath Путь к файлу.
     * @param blockNumber Номер блока.
     * @return std::vector<char> Считанный блок данных.
     */
    std::vector<char> readFileBlock(const std::string &filePath, std::size_t blockNumber) const;

    /**
     * @brief Вычисляет хэш для заданного блока данных.
     *
     * @param data Указатель на данные.
     * @param length Размер данных.
     * @return HashValue Вычисленный хэш.
     */
    HashValue computeHash(const char *data, std::size_t length) const;

  private:
    std::vector<std::vector<std::string> *> duplicateLists_; ///< Списки групп одинаковых файлов.
   // std::unique_ptr<class Node> rootNode_; ///< Корневой узел дерева.
    std::unordered_map<std::size_t, std::unique_ptr<class Node>> roots_;
    std::vector<std::string> directories_;
    std::set<std::string> excludeDirs_;
    int maxDepth_;
    std::vector<std::string> fileMasks_;
    std::size_t minFileSize_;
    std::size_t blockSize_;
    std::unique_ptr<IHashAlgorithm> hashAlgorithmStrategy_; ///< Стратегия вычисления хэша.
    mutable std::unordered_map<std::string, FileCacheEntry> fileCache_;   ///< Кэш открытых потоков.

    /**
     * @brief Рекурсивно обрабатывает директорию.
     *
     * @param directory Путь к директории.
     * @param depth Текущая глубина рекурсии.
     */
    void processDirectory(const fs::path &directory, int depth);

    /**
     * @brief Проверяет, соответствует ли имя файла заданным маскам.
     *
     * @param filename Имя файла.
     * @return true, если имя соответствует хотя бы одной маске.
     * @return false в противном случае.
     */
    bool matchesMask(const std::string &filename) const;

    /**
     * @brief Обрабатывает отдельный файл.
     *
     * @param filePath Путь к файлу.
     */
    void processFile(const fs::path &filePath);

    /**
     * @brief Добавляет файл для последующей обработки.
     *
     * @param fileName Имя файла.
     * @param fileSize Размер файла.
     */
    void addFile(std::string fileName, std::size_t fileSize);

    friend class Node;
};

/**
 * @brief Узел дерева для сравнения файлов.
 *
 * Каждый узел сравнивает блок файла на определённом уровне.
 */
class Node
{
  public:
    /**
     * @brief Конструктор узла.
     *
     * @param finder Указатель на экземпляр DuplicateFinder.
     * @param initialFile Первый файл, использованный для создания узла.
     * @param fileSize Размер файла.
     * @param level Текущий уровень (номер блока).
     */
    Node(DuplicateFinder *finder, std::string initialFile, std::size_t fileSize, std::size_t level);

    // Запрещаем копирование и перемещение
    Node(const Node &) = delete;
    Node &operator=(const Node &) = delete;
    Node(Node &&) = delete;
    Node &operator=(Node &&) = delete;

    /**
     * @brief Обрабатывает файл – либо добавляет его к найденным дубликатам, либо
     * передаёт на следующий уровень сравнения.
     *
     * @param fileName Имя файла.
     * @param fileSize Размер файла.
     */
    void processFile(std::string fileName, std::size_t fileSize);

  private:
    DuplicateFinder *finder_;
    std::size_t level_; ///< Текущий уровень сравнения (номер блока).
    std::string initialFile_; ///< Первый файл, использованный для создания узла.
    std::size_t fileSize_; ///< Первый файл, размер.
    std::vector<std::string> duplicateFiles_; ///< Группа файлов, совпадающих на данном уровне.
    std::unordered_map<HashValue, std::unique_ptr<Node>> children_; ///< Узлы для следующего уровня.
};

/**
 * @brief Утилитарная функция для быстрого запуска сканирования директорий.
 *
 * @param directories Список директорий для сканирования.
 * @param excludeDirs Директории, которые нужно исключить.
 * @param maxDepth Максимальная глубина рекурсии.
 * @param fileMasks Маски файлов.
 * @param minFileSize Минимальный размер файла.
 * @param blockSize Размер блока для чтения файла.
 * @param hashAlgorithmType Тип алгоритма хэширования ("crc32" или "md5").
 * @return std::vector<std::string> Список найденных дубликатов.
 */
std::vector<std::string> scanDirectory(const std::vector<std::string> &directories,
                                       const std::set<std::string> &excludeDirs,
                                       int maxDepth,
                                       const std::vector<std::string> &fileMasks,
                                       std::size_t minFileSize,
                                       std::size_t blockSize,
                                       const std::string &hashAlgorithmType);
