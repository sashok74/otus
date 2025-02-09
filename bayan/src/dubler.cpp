#include "dubler.h"

std::vector<std::string> Dubler::scan() {
    processDirectory(rootPath, 0);
    std::vector<std::string> result;
    for (auto& [key, value] : fileHashes) {
        result.push_back(key);
    }
    return result;
}

void Dubler::processDirectory(const fs::path& dir, int depth) {
    if (maxDepth >= 0 && depth > maxDepth) return;
    if (excludeDirs.find(dir.filename().string()) != excludeDirs.end()) return;

    for (const auto& entry : fs::directory_iterator(dir)) {
        if (fs::is_directory(entry)) {
            processDirectory(entry, depth + 1);
        } else if (fs::is_regular_file(entry)) {
            processFile(entry);
        }
    }
}

bool Dubler::matchesMask(const std::string& filename) {
    if (masks.empty()) return true;
    for (const auto& mask : masks) {
        if (boost::algorithm::icontains(filename, mask)) {
            return true;
        }
    }
    return false;
}

void Dubler::processFile(const fs::path& file) {
    if (!matchesMask(file.filename().string())) return;
    if (fs::file_size(file) < minFileSize) return;

    std::ifstream inFile(file.string(), std::ios::binary);
    if (!inFile) return;

    std::vector<char> buffer(blockSize);
    inFile.read(buffer.data(), blockSize);
    std::size_t bytesRead = inFile.gcount();

    std::string hash = computeHash(buffer.data(), bytesRead);
    fileHashes[file.string()] = hash;
    std::cout << "File: " << file.string() << " Hash: " << hash << "\n";
}

std::string Dubler::computeHash(const char* data, std::size_t length) {
    if (hashAlgorithm == "crc32") {
        boost::crc_32_type crc;
        crc.process_bytes(data, length);
        return std::to_string(crc.checksum());
    } else if (hashAlgorithm == "md5") {
        boost::uuids::detail::md5 md5;
        md5.process_bytes(data, length);
        boost::uuids::detail::md5::digest_type digest;
        md5.get_digest(digest);
        char hashStr[33];
        for (int i = 0; i < 4; ++i) {
            std::sprintf(hashStr + i * 8, "%08x", digest[i]);
        }
        return std::string(hashStr, 32);
    }
    return "";
}

std::vector<std::string> scan_directory(const std::string& root,
                                        const std::set<std::string>& excludeDirs, int maxDepth,
                                        const std::vector<std::string>& masks,
                                        std::size_t minFileSize, std::size_t blockSize,
                                        const std::string& hashAlgorithm) {
    Dubler dubler(root, excludeDirs, maxDepth, masks, minFileSize, blockSize, hashAlgorithm);
    return dubler.scan();
}