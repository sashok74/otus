#include "dubler.h"

#include <cstdio>
#include <iomanip>
#include <sstream>
#include <stdexcept>

//==========================================================================
// DuplicateFinder Implementation
//==========================================================================

DuplicateFinder::DuplicateFinder(const std::vector<std::string> &directories,
                                 const std::set<std::string> &excludeDirs,
                                 int maxDepth,
                                 const std::vector<std::string> &fileMasks,
                                 std::size_t minFileSize,
                                 std::size_t blockSize,
                                 const std::string &hashAlgorithm)
  : directories_(directories), excludeDirs_(excludeDirs), maxDepth_(maxDepth),
    fileMasks_(fileMasks), minFileSize_(minFileSize), blockSize_(blockSize),
    hashAlgorithm_(hashAlgorithm)
{
}

void DuplicateFinder::registerDuplicateList(std::vector<std::string> &duplicateList)
{
    duplicateLists_.push_back(&duplicateList);
}

std::vector<std::string> DuplicateFinder::scan()
{
    for (const auto &dir : directories_)
    {
        processDirectory(fs::path(dir), 0);
    }

    std::vector<std::string> result;
    for (std::size_t i = 0; i < duplicateLists_.size(); ++i)
    {
        if (duplicateLists_[i])
        {
            result.insert(result.end(), duplicateLists_[i]->begin(), duplicateLists_[i]->end());
            if (i < duplicateLists_.size() - 1)
                result.push_back(""); // Разделитель между группами
        }
    }
    return result;
}

bool DuplicateFinder::matchesMask(const std::string &filename) const
{
    if (fileMasks_.empty())
        return true;

    for (const auto &mask : fileMasks_)
    {
        std::string regexPattern = boost::replace_all_copy(mask, ".", "\\.");
        boost::replace_all(regexPattern, "*", ".*");
        boost::replace_all(regexPattern, "?", ".");
        boost::regex pattern(regexPattern, boost::regex::icase);

        if (boost::regex_match(filename, pattern))
            return true;
    }
    return false;
}

std::string DuplicateFinder::computeHash(const char *data, std::size_t length) const
{
    if (hashAlgorithm_ == "crc32")
    {
        boost::crc_32_type crc;
        crc.process_bytes(data, length);
        return std::to_string(crc.checksum());
    }
    else if (hashAlgorithm_ == "md5")
    {
        boost::uuids::detail::md5 md5;
        md5.process_bytes(data, length);
        boost::uuids::detail::md5::digest_type digest;
        md5.get_digest(digest);

        char hashStr[33] = {0}; // 32 символа + завершающий ноль
        for (int i = 0; i < 4; ++i)
            std::sprintf(hashStr + i * 8, "%08x", digest[i]);
        return std::string(hashStr, 32);
    }
    return "";
}

void DuplicateFinder::processDirectory(const fs::path &directory, int depth)
{
    if (maxDepth_ > 0 && depth > maxDepth_)
        return;

    if (excludeDirs_.find(directory.filename().string()) != excludeDirs_.end())
        return;

    try
    {
        for (const auto &entry : fs::directory_iterator(directory))
        {
            try
            {
                // Проверяем наличие файла и права доступа
                if (!fs::exists(entry) || !(fs::status(entry).permissions() & fs::owner_read))
                {
                    std::cerr << "Skipping (no access): " << entry.path() << std::endl;
                    continue;
                }

                // Пропускаем символьные ссылки
                if (fs::is_symlink(entry))
                    continue;

                // Рекурсивно обрабатываем директории
                if (fs::is_directory(entry))
                    processDirectory(entry, depth + 1);
                else if (fs::is_regular_file(entry))
                    processFile(entry);
            }
            catch (const fs::filesystem_error &ex)
            {
                std::cerr << "Warning: " << ex.what() << " Path: " << entry.path() << std::endl;
            }
        }
    }
    catch (const fs::filesystem_error &ex)
    {
        std::cerr << "Error: " << ex.what() << " Directory: " << directory << std::endl;
    }
}

void DuplicateFinder::processFile(const fs::path &filePath)
{
    if (!matchesMask(filePath.filename().string()))
        return;

    std::size_t fileSize = fs::file_size(filePath);
    if (fileSize < minFileSize_)
        return;

    addFile(filePath.string(), fileSize);
}

void DuplicateFinder::addFile(std::string fileName, std::size_t fileSize)
{
    if (!rootNode_)
    {
        rootNode_ = std::make_unique<Node>(this, std::move(fileName), fileSize, 0);
        return;
    }
    try
    {
        rootNode_->processFile(std::move(fileName), fileSize);
    }
    catch (...)
    {
        // Обработка исключений (при необходимости можно добавить логирование)
    }
}

std::vector<char> DuplicateFinder::readFileBlock(const std::string &filePath,
                                                 std::size_t blockNumber) const
{
    std::ifstream file(filePath, std::ios::binary);
    if (!file.is_open())
        throw std::runtime_error("Unable to open file: " + filePath);

    file.seekg(blockSize_ * blockNumber, std::ios::beg);
    if (file.fail())
        throw std::runtime_error("Failed to seek in file: " + filePath);

    std::vector<char> buffer(blockSize_);
    file.read(buffer.data(), blockSize_);
    buffer.resize(file.gcount());
    return buffer;
}

//==========================================================================
// Node Implementation
//==========================================================================

Node::Node(DuplicateFinder *finder,
           std::string initialFile,
           std::size_t fileSize,
           std::size_t level)
  : finder_(finder), level_(level), initialFile_(std::move(initialFile)), fileSize_(fileSize)
{
}

void Node::processFile(std::string fileName, std::size_t fileSize)
{
    // Если достигли конца файла (больше нет данных для чтения на данном
    // уровне), файл считается дубликатом
    if (finder_->blockSize_ * level_ >= fileSize)
    {
        duplicateFiles_.push_back(std::move(fileName));
        if (duplicateFiles_.size() == 2)
            finder_->registerDuplicateList(duplicateFiles_);
        return;
    }

    // Для первого файла инициализируем дочерний узел
    if (children_.empty())
    {
        auto buffer = finder_->readFileBlock(initialFile_, level_);
        std::string hash = finder_->computeHash(buffer.data(), finder_->blockSize_);
        children_.emplace(std::move(hash),
                          std::make_unique<Node>(finder_, initialFile_, fileSize_, level_ + 1));
    }

    auto buffer = finder_->readFileBlock(fileName, level_);
    std::string hash = finder_->computeHash(buffer.data(), buffer.size());

    auto it = children_.find(hash);
    if (it != children_.end())
        it->second->processFile(std::move(fileName), fileSize);
    else
        children_.emplace(std::move(hash), std::make_unique<Node>(finder_, std::move(fileName),
                                                                  fileSize, level_ + 1));
}

//==========================================================================
// Вспомогательная функция для сканирования директорий
//==========================================================================

std::vector<std::string> scanDirectory(const std::vector<std::string> &directories,
                                       const std::set<std::string> &excludeDirs,
                                       int maxDepth,
                                       const std::vector<std::string> &fileMasks,
                                       std::size_t minFileSize,
                                       std::size_t blockSize,
                                       const std::string &hashAlgorithm)
{
    DuplicateFinder finder(directories, excludeDirs, maxDepth, fileMasks, minFileSize, blockSize,
                           hashAlgorithm);
    return finder.scan();
}
