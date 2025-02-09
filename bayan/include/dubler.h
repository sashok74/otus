#pragma once
#include <boost/algorithm/string.hpp>
#include <boost/crc.hpp>
#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>
#include <boost/uuid/detail/md5.hpp>
#include <fstream>
#include <iostream>
#include <set>
#include <unordered_map>
#include <vector>

int version();

namespace fs = boost::filesystem;

class Dubler {
   public:
    Dubler(const std::string& root, const std::set<std::string>& excludeDirs, int maxDepth,
           const std::vector<std::string>& masks, std::size_t minFileSize, std::size_t blockSize,
           const std::string& hashAlgorithm)
        : rootPath(root),
          excludeDirs(excludeDirs),
          maxDepth(maxDepth),
          masks(masks),
          minFileSize(minFileSize),
          blockSize(blockSize),
          hashAlgorithm(hashAlgorithm) {}

    std::vector<std::string> scan();

   private:
    std::string rootPath;
    std::set<std::string> excludeDirs;
    int maxDepth;
    std::vector<std::string> masks;
    std::size_t minFileSize;
    std::size_t blockSize;
    std::string hashAlgorithm;
    std::unordered_map<std::string, std::string> fileHashes;

    void processDirectory(const fs::path& dir, int depth);

    bool matchesMask(const std::string& filename);

    void processFile(const fs::path& file);

    std::string computeHash(const char* data, std::size_t length);
};

std::vector<std::string> scan_directory(const std::string& root,
                                        const std::set<std::string>& excludeDirs, int maxDepth,
                                        const std::vector<std::string>& masks,
                                        std::size_t minFileSize, std::size_t blockSize,
                                        const std::string& hashAlgorithm);

/*
extern "C" {
    __attribute__((visibility("default"))) std::vector<std::string> scan_directory(const
std::string& root, const std::set<std::string>& excludeDirs, int maxDepth, const
std::vector<std::string>& masks, std::size_t minFileSize, std::size_t blockSize, const std::string&
hashAlgorithm) { Dubler dubler(root, excludeDirs, maxDepth, masks, minFileSize, blockSize,
hashAlgorithm); return dubler.scan();
    }
}
*/