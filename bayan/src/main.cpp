
#include "dubler.h"
#include "finddub/finddub.h"
#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>
#include <chrono>
#include <iostream>
#include <set>
#include <vector>

namespace po = boost::program_options;

int main(int argc, char *argv[])
{
    boost::filesystem::path currentPath = boost::filesystem::current_path();

    try
    {
        // Определяем доступные параметры командной строки
        po::options_description desc("Allowed options");
        // clang-format off
        desc.add_options()
            ("help,h", "Show help")
            ("dirs,d", po::value<std::vector<std::string>>()->multitoken(), "Directories to scan")
            ("exclude,e", po::value<std::vector<std::string>>()->multitoken(), "Directories to exclude")
            ("depth,l", po::value<int>()->default_value(0), "Scanning depth (0 means no subdirectories)")
            ("min-size,m", po::value<size_t>()->default_value(1), "Minimum file size")
            ("mask,k", po::value<std::vector<std::string>>()->multitoken(), "File name masks")
            ("block-size,s", po::value<size_t>()->default_value(8192), "Block size for reading")
            ("hash,a", po::value<std::string>()->default_value("md5"), "Hash algorithm (crc32, md5)")
            ("version,v", "Library version")
            ("echo,r", "Print provided parameters")
            ("time,t", "Output runtime of the utility");;
        // clang-format on
        po::variables_map vm;
        po::store(po::parse_command_line(argc, argv, desc), vm);
        po::notify(vm);

        if (vm.count("help"))
        {
            std::cout << desc << std::endl;
            return 0;
        }

        if (vm.count("version"))
        {
            std::cout << "finddub lib, version " << version() << std::endl;
            return 0;
        }

        if (vm.count("echo"))
        {
            std::cout << "Directories to scan: ";
            if (vm.count("dirs"))
            {
                for (const auto &dir : vm["dirs"].as<std::vector<std::string>>())
                    std::cout << dir << " ";
            }
            std::cout << "\nDirectories to exclude: ";
            if (vm.count("exclude"))
            {
                for (const auto &dir : vm["exclude"].as<std::vector<std::string>>())
                    std::cout << dir << " ";
            }
            std::cout << "\nDepth: " << vm["depth"].as<int>()
                      << "\nMinimum size: " << vm["min-size"].as<size_t>()
                      << "\nBlock size: " << vm["block-size"].as<size_t>()
                      << "\nHash algorithm: " << vm["hash"].as<std::string>()
                      << "\nCurrent directory: " << currentPath.string() << std::endl;
            return 0;
        }

        // Извлекаем параметры
        std::vector<std::string> dirs;
        if (vm.count("dirs"))
            dirs = vm["dirs"].as<std::vector<std::string>>();

        std::vector<std::string> exclude;
        if (vm.count("exclude"))
            exclude = vm["exclude"].as<std::vector<std::string>>();

        int depth = vm["depth"].as<int>();
        size_t minSize = vm["min-size"].as<size_t>();
        size_t blockSize = vm["block-size"].as<size_t>();

        std::vector<std::string> masks;
        if (vm.count("mask"))
            masks = vm["mask"].as<std::vector<std::string>>();

        std::string hashAlgo = vm["hash"].as<std::string>();

        std::set<std::string> excludeDirs(exclude.begin(), exclude.end());
        // Начинаем замер времени до запуска сканирования
        auto start = std::chrono::steady_clock::now();

        // Запуск сканирования
        std::vector<std::string> duplicateFiles =
            scanDirectory(dirs, excludeDirs, depth, masks, minSize, blockSize, hashAlgo);

        // Останавливаем замер времени после завершения сканирования
        auto end = std::chrono::steady_clock::now();

        for (const auto &file : duplicateFiles)
            std::cout << file << "\n";

        // Если параметр --time указан, выводим время работы утилиты
        if (vm.count("time"))
        {
            auto duration =
                std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
            std::cout << "Elapsed time: " << duration << " ms" << std::endl;
        }

        // Если установлен параметр --time, выводим общее количество найденных файлов (исключая разделители)
        if (vm.count("time"))
        {
            size_t fileCount = 0;
            for (const auto &file : duplicateFiles)
            {
                if (!file.empty())
                    ++fileCount;
            }
            std::cout << "\nTotal duplicate files: " << fileCount << std::endl;
        }


    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}

//  conan install . --output-folder=bayan/build --build=missing -s
//  build_type=Debug cd bayan/build/ cmake ..
//  -DCMAKE_TOOLCHAIN_FILE=conan_toolchain.cmake -DCMAKE_BUILD_TYPE=Debug cmake
//  --build .
//  ./src/bayan