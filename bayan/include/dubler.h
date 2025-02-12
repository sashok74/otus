#pragma once

#include <boost/algorithm/string/replace.hpp>
#include <boost/crc.hpp>
#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>
#include <boost/regex.hpp>
#include <boost/uuid/detail/md5.hpp>

#include <cstdio>
#include <fstream>
#include <iostream>
#include <memory>
#include <set>
#include <string>
#include <unordered_map>
#include <vector>

namespace fs = boost::filesystem;

/**
 * @brief Класс для поиска дублирующихся файлов.
 *
 * Класс сканирует заданные директории, фильтрует файлы по маскам и минимальному
 * размеру, затем группирует файлы по хэшам считанных блоков.
 */
class DuplicateFinder
{
  public:
    DuplicateFinder(const std::vector<std::string> &directories,
                    const std::set<std::string> &excludeDirs,
                    int maxDepth,
                    const std::vector<std::string> &fileMasks,
                    std::size_t minFileSize,
                    std::size_t blockSize,
                    const std::string &hashAlgorithm);

    /// Основной метод сканирования директорий
    std::vector<std::string> scan();

    /// Позволяет зарегистрировать список найденных дублей (используется внутри
    /// класса)
    void registerDuplicateList(std::vector<std::string> &duplicateList);

  private:
    std::vector<std::vector<std::string> *> duplicateLists_; //< Списки групп одинаковых файлов
    std::unique_ptr<class Node> rootNode_; //< Корневой узел дерева
    std::vector<std::string> directories_;
    std::set<std::string> excludeDirs_;
    int maxDepth_;
    std::vector<std::string> fileMasks_;
    std::size_t minFileSize_;
    std::size_t blockSize_;
    std::string hashAlgorithm_;

    // Вспомогательные методы
    void processDirectory(const fs::path &directory, int depth);
    bool matchesMask(const std::string &filename) const;
    void processFile(const fs::path &filePath);
    std::string computeHash(const char *data, std::size_t length) const;
    void addFile(std::string fileName, std::size_t fileSize);
    std::vector<char> readFileBlock(const std::string &filePath, std::size_t blockNumber) const;

    friend class Node;
};

/**
 * @brief Узел дерева, который группирует файлы по хэшам блоков.
 *
 * Каждый узел отвечает за сравнение блока файла на определённом уровне.
 */
class Node
{
  public:
    Node(DuplicateFinder *finder, std::string initialFile, std::size_t fileSize, std::size_t level);

    // Запрещаем копирование и перемещение
    Node(const Node &) = delete;
    Node &operator=(const Node &) = delete;
    Node(Node &&) = delete;
    Node &operator=(Node &&) = delete;

    /// Обрабатывает файл – либо добавляет его к найденным дубликатам, либо
    /// передаёт на следующий уровень
    void processFile(std::string fileName, std::size_t fileSize);

  private:
    DuplicateFinder *finder_;
    std::size_t level_; // Номер блока (уровень сравнения)
    std::string initialFile_; // Первый файл, использованный для создания узла
    std::size_t fileSize_;
    std::vector<std::string> duplicateFiles_; // Группа файлов, совпадающих на данном уровне
    std::unordered_map<std::string, std::unique_ptr<Node>> children_; // Узлы для следующих уровней
};

/// Утилитарная функция для быстрого запуска сканирования
std::vector<std::string> scanDirectory(const std::vector<std::string> &directories,
                                       const std::set<std::string> &excludeDirs,
                                       int maxDepth,
                                       const std::vector<std::string> &fileMasks,
                                       std::size_t minFileSize,
                                       std::size_t blockSize,
                                       const std::string &hashAlgorithm);
