#include "dubler.h"
#include <boost/filesystem.hpp>
#include <fstream>
#include <gtest/gtest.h>
#include <set>
#include <string>
#include <vector>

namespace fs = boost::filesystem;

/**
 * @brief Фикстура для тестирования DuplicateFinder.
 *
 * Создаёт временную директорию для генерации тестовых файлов и удаляет её по завершению.
 */
class DuplicateFinderTest : public ::testing::Test
{
  protected:
    fs::path tempDir;

    void SetUp() override
    {
        // Создаём уникальную временную директорию.
        tempDir = fs::temp_directory_path() / fs::unique_path("dubler_test_%%%%-%%%%-%%%%");
        fs::create_directory(tempDir);
    }

    void TearDown() override
    {
        // Удаляем временную директорию и все её содержимое.
        fs::remove_all(tempDir);
    }

    /**
     * @brief Вспомогательная функция для создания файла с заданным содержимым.
     *
     * @param filePath Путь к создаваемому файлу.
     * @param content Содержимое файла.
     */
    void createFile(const fs::path &filePath, const std::string &content)
    {
        std::ofstream ofs(filePath.string());
        ofs << content;
    }
};

TEST_F(DuplicateFinderTest, NoDuplicatesFound)
{
    // Создаём два файла с разным содержимым.
    auto file1 = tempDir / "file1.txt";
    auto file2 = tempDir / "file2.txt";
    createFile(file1, "Hello World");
    createFile(file2, "Goodbye World");

    std::vector<std::string> directories = {tempDir.string()};
    std::set<std::string> excludeDirs;
    int maxDepth = 1;
    std::vector<std::string> fileMasks = {"*.txt"};
    std::size_t minFileSize = 1;
    std::size_t blockSize = 1024;
    std::string hashAlgorithm = "md5";

    auto duplicates = scanDirectory(directories, excludeDirs, maxDepth, fileMasks, minFileSize,
                                    blockSize, hashAlgorithm);

    // Ожидаем, что дубликатов не будет.
    EXPECT_TRUE(duplicates.empty());
}

TEST_F(DuplicateFinderTest, DuplicatesFound)
{
    // Создаём два файла с одинаковым содержимым и один с уникальным.
    auto file1 = tempDir / "file1.txt";
    auto file2 = tempDir / "file2.txt";
    auto file3 = tempDir / "file3.txt";
    createFile(file1, "Duplicate Content");
    createFile(file2, "Duplicate Content");
    createFile(file3, "Unique Content");

    std::vector<std::string> directories = {tempDir.string()};
    std::set<std::string> excludeDirs;
    int maxDepth = 10;
    std::vector<std::string> fileMasks = {"*.txt"};
    std::size_t minFileSize = 1;
    std::size_t blockSize = 1024;
    std::string hashAlgorithm = "md5";

    auto duplicates = scanDirectory(directories, excludeDirs, maxDepth, fileMasks, minFileSize,
                                    blockSize, hashAlgorithm);

    // Функция scanDirectory возвращает группы дубликатов,
    // где каждая группа представлена в виде вектора строк, разделённых пустой строкой.
    // В данном тесте ожидается, что будет одна группа из двух файлов.
    std::vector<std::string> duplicateGroup;
    for (const auto &s : duplicates)
    {
        if (!s.empty())
        {
            duplicateGroup.push_back(s);
        }
    }
    EXPECT_EQ(duplicateGroup.size(), 2);
    std::set<std::string> foundFiles(duplicateGroup.begin(), duplicateGroup.end());
    std::set<std::string> expectedFiles = {file1.string(), file2.string()};
    EXPECT_EQ(foundFiles, expectedFiles);
}

TEST_F(DuplicateFinderTest, FileMaskFiltering)
{
    // файлы с разными расширениями.
    auto file1 = tempDir / "file1.txt";
    auto file2 = tempDir / "file2.log";
    createFile(file1, "Some content");
    createFile(file2, "Some content");

    std::vector<std::string> directories = {tempDir.string()};
    std::set<std::string> excludeDirs;
    int maxDepth = 1;
    // Фильтруем только файлы с расширением .txt.
    std::vector<std::string> fileMasks = {"*.txt"};
    std::size_t minFileSize = 1;
    std::size_t blockSize = 1024;
    std::string hashAlgorithm = "md5";

    auto duplicates = scanDirectory(directories, excludeDirs, maxDepth, fileMasks, minFileSize,
                                    blockSize, hashAlgorithm);

    // Поскольку только один файл удовлетворяет маске, дубликатов обнаружено не будет.
    EXPECT_TRUE(duplicates.empty());
}

TEST_F(DuplicateFinderTest, MinFileSizeFiltering)
{
    // Создаём два файла с содержимым, длина которого меньше требуемого минимального размера.
    auto file1 = tempDir / "small1.txt";
    auto file2 = tempDir / "small2.txt";
    createFile(file1, "123");
    createFile(file2, "123");

    std::vector<std::string> directories = {tempDir.string()};
    std::set<std::string> excludeDirs;
    int maxDepth = 1;
    std::vector<std::string> fileMasks = {"*.txt"};
    std::size_t minFileSize = 10; // Минимальный размер больше, чем размер содержимого файлов.
    std::size_t blockSize = 1024;
    std::string hashAlgorithm = "md5";

    auto duplicates = scanDirectory(directories, excludeDirs, maxDepth, fileMasks, minFileSize,
                                    blockSize, hashAlgorithm);

    EXPECT_TRUE(duplicates.empty());
}

TEST_F(DuplicateFinderTest, DuplicatesFoundCRC32)
{
    // Создаём два файла с одинаковым содержимым для проверки алгоритма CRC32.
    auto file1 = tempDir / "file1.txt";
    auto file2 = tempDir / "file2.txt";
    createFile(file1, "Duplicate Content");
    createFile(file2, "Duplicate Content");

    std::vector<std::string> directories = {tempDir.string()};
    std::set<std::string> excludeDirs;
    int maxDepth = 1;
    std::vector<std::string> fileMasks = {"*.txt"};
    std::size_t minFileSize = 1;
    std::size_t blockSize = 1024;
    std::string hashAlgorithm = "crc32";

    auto duplicates = scanDirectory(directories, excludeDirs, maxDepth, fileMasks, minFileSize,
                                    blockSize, hashAlgorithm);

    std::vector<std::string> duplicateGroup;
    for (const auto &s : duplicates)
    {
        if (!s.empty())
        {
            duplicateGroup.push_back(s);
        }
    }
    EXPECT_EQ(duplicateGroup.size(), 2);
    std::set<std::string> foundFiles(duplicateGroup.begin(), duplicateGroup.end());
    std::set<std::string> expectedFiles = {file1.string(), file2.string()};
    EXPECT_EQ(foundFiles, expectedFiles);
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
