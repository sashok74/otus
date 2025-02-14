#include "dubler.h"
#include <cstdio>
#include <cstring>
#include <iomanip>
#include <sstream>
#include <stdexcept>

std::ostream &operator<<(std::ostream &os, const HashValue &hv)
{
    os << std::hex << std::setfill('0');
    for (const auto &byte : hv.data)
    {
        os << std::setw(2) << static_cast<int>(byte);
    }
    os << std::dec; // Возвращаем формат в десятичный
    return os;
}

//==========================================================================
// CRC32HashAlgorithm
//==========================================================================

HashValue CRC32HashAlgorithm::compute(const char *data, std::size_t length) const
{
    HashValue hv;
    hv.data.fill(0);
    boost::crc_32_type crc;
    crc.process_bytes(data, length);
    uint32_t crcValue = crc.checksum();
    std::memcpy(hv.data.data(), &crcValue, sizeof(crcValue));
    return hv;
}

//==========================================================================
// MD5HashAlgorithm
//==========================================================================

HashValue MD5HashAlgorithm::compute(const char *data, std::size_t length) const
{
    HashValue hv;
    hv.data.fill(0);
    boost::uuids::detail::md5 md5;
    md5.process_bytes(data, length);
    boost::uuids::detail::md5::digest_type digest;
    md5.get_digest(digest);
    for (int i = 0; i < 4; ++i)
    {
        uint32_t part = digest[i];
        std::memcpy(hv.data.data() + i * sizeof(uint32_t), &part, sizeof(uint32_t));
    }
    return hv;
}

//==========================================================================
// DuplicateFinder
//==========================================================================

DuplicateFinder::DuplicateFinder(const std::vector<std::string> &directories,
                                 const std::set<std::string> &excludeDirs,
                                 int maxDepth,
                                 const std::vector<std::string> &fileMasks,
                                 std::size_t minFileSize,
                                 std::size_t blockSize,
                                 const std::string &hashAlgorithmType)
  : directories_(directories), excludeDirs_(excludeDirs), maxDepth_(maxDepth),
    fileMasks_(fileMasks), minFileSize_(minFileSize), blockSize_(blockSize)
{
    /// выбираем алгоритм хэширования в зависимости от переданного типа.
    if (hashAlgorithmType == "crc32")
    {
        hashAlgorithmStrategy_ = std::make_unique<CRC32HashAlgorithm>();
    }
    else if (hashAlgorithmType == "md5")
    {
        hashAlgorithmStrategy_ = std::make_unique<MD5HashAlgorithm>();
    }
    else
    {
        throw std::invalid_argument("Unsupported hash algorithm type: " + hashAlgorithmType);
    }

    // Компиляция масок в регулярные выражения
    for (const auto &mask : fileMasks_)
    {
        std::string regexPattern = boost::replace_all_copy(mask, ".", "\\.");
        boost::replace_all(regexPattern, "*", ".*");
        boost::replace_all(regexPattern, "?", ".");
        try
        {
            compiledMasks_.emplace_back(regexPattern, boost::regex::icase);
        }
        catch (const boost::regex_error &ex)
        {
            std::cerr << "Ошибка компиляции маски: " << mask << " (" << ex.what() << ")"
                      << std::endl;
        }
    }
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
    if (compiledMasks_.empty())
        return true;

    for (const auto &pattern : compiledMasks_)
    {
        if (boost::regex_match(filename, pattern))
            return true;
    }
    return false;
}

HashValue DuplicateFinder::computeHash(const char *data, std::size_t length) const
{
    return hashAlgorithmStrategy_->compute(data, length);
}

void DuplicateFinder::processDirectory(const fs::path &directory, int depth)
{
    if (maxDepth_ > 0 && depth > maxDepth_)
        return;

    // Если полный путь содержит хотя бы один из исключаемых директорий, пропускаем обработку.
    std::string dirStr = directory.string();
    for (const auto &exclude : excludeDirs_)
    {
        if (dirStr.find(exclude) != std::string::npos)
            return;
    }

    try
    {
        for (const auto &entry : fs::directory_iterator(directory))
        {
            try
            {
                if (!fs::exists(entry) || !(fs::status(entry).permissions() & fs::owner_read))
                {
                    std::cerr << "Skipping (no access): " << entry.path() << std::endl;
                    continue;
                }
                if (fs::is_symlink(entry))
                    continue;

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
    auto it = roots_.find(fileSize);
    if (it == roots_.end())
    {
        // Если для данного размера ещё нет корневого узла, создаём его.
        roots_[fileSize] = std::make_unique<Node>(this, std::move(fileName), fileSize, 0);
    }
    else
    {
        try
        {
            // Если корневой узел для такого размера уже есть, сравниваем файл с существующими.
            it->second->processFile(std::move(fileName), fileSize);
        }
        catch (...)
        {
            // Здесь можно добавить обработку исключений и логирование.
        }
    }
}

std::vector<char> DuplicateFinder::readFileBlock(const std::string &filePath,
                                                 std::size_t blockNumber) const
{
    FileCacheEntry *entry = fileCache_.get(filePath); ///< особенно ничего не дал. больше возьни.
    if (!entry)
    {
        // Если запись отсутствует, создаём новую запись для файла.
        FileCacheEntry newEntry;
        newEntry.stream = std::make_unique<std::ifstream>(filePath, std::ios::binary);
        if (!newEntry.stream || !newEntry.stream->is_open())
            throw std::runtime_error("Unable to open file: " + filePath);
        newEntry.currentBlock = 0;
        // Метод put() автоматически выполнит LRU-эвакуацию, если кэш переполнен.
        fileCache_.put(filePath, std::move(newEntry));
        entry = fileCache_.get(filePath);
    }

    std::vector<char> buffer(blockSize_);
    entry->stream->read(buffer.data(), blockSize_);
    std::streamsize bytesRead = entry->stream->gcount();
    buffer.resize(bytesRead);
    entry->currentBlock++; // обновляем номер следующего блока для последовательного чтения
    return buffer;
}

//==========================================================================
// Node
//==========================================================================

Node::Node(DuplicateFinder *finder,
           std::string initialFile,
           std::size_t fileSize,
           std::size_t level)
  : finder_(finder), level_(level), initialFile_(std::move(initialFile)), fileSize_(fileSize)
{
    if (finder_->blockSize_ * level_ >= fileSize)
    {
        duplicateFiles_.push_back(std::move(initialFile_));
        if (duplicateFiles_.size() == 2)
            finder_->registerDuplicateList(duplicateFiles_);
        return;
    }
}

void Node::processFile(std::string fileName, std::size_t fileSize)
{
    // Если достигли конца файла на данном уровне, файл считается дубликатом.
    if (finder_->blockSize_ * level_ >= fileSize)
    {
        duplicateFiles_.push_back(std::move(fileName));
        if (duplicateFiles_.size() == 2)
            finder_->registerDuplicateList(duplicateFiles_);
        return;
    }

    // Для первого файла инициализируем дочерний узел.
    if (children_.empty())
    {
        auto buffer = finder_->readFileBlock(initialFile_, level_);
        auto hash = finder_->computeHash(buffer.data(), buffer.size());
        children_.emplace(std::move(hash),
                          std::make_unique<Node>(finder_, initialFile_, fileSize_, level_ + 1));
    }

    auto buffer = finder_->readFileBlock(fileName, level_);
    auto hash = finder_->computeHash(buffer.data(), buffer.size());

    auto it = children_.find(hash);
    if (it != children_.end())
        it->second->processFile(std::move(fileName), fileSize);
    else
        children_.emplace(std::move(hash), std::make_unique<Node>(finder_, std::move(fileName),
                                                                  fileSize, level_ + 1));
}

//==========================================================================
// Утилитарная функция
//==========================================================================

std::vector<std::string> scanDirectory(const std::vector<std::string> &directories,
                                       const std::set<std::string> &excludeDirs,
                                       int maxDepth,
                                       const std::vector<std::string> &fileMasks,
                                       std::size_t minFileSize,
                                       std::size_t blockSize,
                                       const std::string &hashAlgorithmType)
{
    DuplicateFinder finder(directories, excludeDirs, maxDepth, fileMasks, minFileSize, blockSize,
                           hashAlgorithmType);
    return finder.scan();
}
