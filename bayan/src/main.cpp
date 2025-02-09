#include <iostream>

#include "finddub/finddub.h"
#include "dubler.h"
#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>
namespace po = boost::program_options;

int main(int argc, char* argv[]){
    boost::filesystem::path current_path = boost::filesystem::current_path();    
    try {
        po::options_description desc("Allowed options");
        desc.add_options()
            ("help,h", "Показать справку")
            ("dirs,d", po::value<std::vector<std::string>>()->multitoken(), "Директории для сканирования")
            ("exclude,e", po::value<std::vector<std::string>>()->multitoken(), "Директории для исключения")
            ("depth,l", po::value<int>()->default_value(0), "Уровень сканирования (0 - без вложенных)")
            ("min-size,m", po::value<size_t>()->default_value(1), "Минимальный размер файла")
            ("mask,k", po::value<std::vector<std::string>>()->multitoken(), "Маски имен файлов")
            ("block-size,s", po::value<size_t>()->default_value(8), "Размер блока чтения")
            ("hash,h", po::value<std::string>()->default_value("md5"), "Алгоритм хэширования (crc32, md5)")
            ("version,v", "Версия библиотеки")
            ("echo,eh", "Вывести переданные параметры");

        po::variables_map vm;
        po::store(po::parse_command_line(argc, argv, desc), vm);
        po::notify(vm);

        if (vm.count("help")) {
            std::cout << desc << std::endl;
            return 0;
        }

        std::vector<std::string> dirs;
        if (vm.count("dirs")) {
            dirs = vm["dirs"].as<std::vector<std::string>>();
        }

        std::vector<std::string> exclude;
        if (vm.count("exclude")) {
            exclude = vm["exclude"].as<std::vector<std::string>>();
        }

        int depth = vm["depth"].as<int>();
        size_t min_size = vm["min-size"].as<size_t>();
        size_t block_size = vm["block-size"].as<size_t>();
        std::vector<std::string> masks;
        if (vm.count("mask")) {
            masks = vm["mask"].as<std::vector<std::string>>();
        }
       
        std::string hash_algo =  vm["hash"].as<std::string>();

        if (vm.count("version")) {
            std::cout << "finddub lib, версия " << version() << std::endl;
            return 0;
        }

        if (vm.count("echo")) {
            size_t block_size = vm["block-size"].as<size_t>();
            std::string hash_algo = vm["hash"].as<std::string>();

            std::cout << "Директории для сканирования: ";
            for (const auto& dir : dirs) std::cout << dir << " ";
            std::cout << "\nДиректории для исключения: ";
            for (const auto& dir : exclude) std::cout << dir << " ";
            std::cout << "\nГлубина: " << depth << "\nМинимальный размер: " << min_size
                      << "\nРазмер блока: " << block_size << "\nАлгоритм хэширования: " << hash_algo
                      << "\nТекущая директория: " << current_path.string()
                      << std::endl;
            return 0;          
        }

        for (const auto& dir : dirs) {
          //  std::vector<std::string> files = scan_directory(dir, exclude, depth, masks, min_size, block_size, hash_algo);
        //    std::cout << "Файлы в " << dir << ":\n";
       //     for (const auto& file : files) {
       //         std::cout << file << "\n";
        //    }
        }


    } catch (const std::exception& e) {
        std::cerr << "Ошибка: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}

//  conan install . --output-folder=bayan/build --build=missing -s build_type=Debug
//  cd bayan/build/
//  cmake .. -DCMAKE_TOOLCHAIN_FILE=conan_toolchain.cmake -DCMAKE_BUILD_TYPE=Debug
//  cmake --build .
//  ./src/bayan