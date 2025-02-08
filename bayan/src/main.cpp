#include "finddub/finddub.h"
#include <iostream>

int main (int, char **) 
{
    std::cout << "Version: " << version() << std::endl;
    return 0;
}

//  conan install . --output-folder=bayan/build --build=missing -s build_type=Debug
//  cd bayan/build/
//  cmake .. -DCMAKE_TOOLCHAIN_FILE=conan_toolchain.cmake -DCMAKE_BUILD_TYPE=Debug
//  cmake --build .
//  ./src/bayan