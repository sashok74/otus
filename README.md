# otus с++ prof

Простой проект на CMake
Date: 2024-11-18 - Time: 11:55

Tags: #QUESTION #ANSWER

Question:
Простой проект на CMake
Answer:
шаблон проект с библиотекой.

Detail:
по шагам.

Шаг 1: Простой вариант
Структура папок:

otus
-si-sd (папка и название проекта)
--lib
---include
-----lib.h
-----version.h
---src
----lib.cpp
---CMakeLists.txt (для библиотеки) 
--app
---CMakeLists.txt  (для приложения)
---main.cpp
-CMakeLists.txt (общий)

содержание файлов:


main.cpp
#include "lib.h"
#include <iostream>
int main (int, char **) {
    std::cout << "Version: " << version() << std::endl;
    std::cout << "Hello, world!" << std::endl;
    return 0;}

version.h:
#pragma once
#define PROJECT_VERSION_PATCH 3

lib.cpp
#include "lib.h"
#include "version.h"
int version() {return PROJECT_VERSION_PATCH;}

lib.h
#pragma once
int version();
Описание файлов CMake
Главный CMakeLists.txt
В корневом CMakeLists.txt файле вы задаете минимальную версию CMake, название проекта, и добавляете поддиректории, содержащие приложение и библиотеку:

cmake_minimum_required(VERSION 3.10)
project(MyProject)

# Добавляем поддиректорию с библиотекой
add_subdirectory(lib)

# Добавляем поддиректорию с приложением
add_subdirectory(app)
CMakeLists.txt для библиотеки
В lib/CMakeLists.txt определите библиотеку, указав её исходные файлы и публичные заголовки:

add_library(mylib SHARED
    src/mylib.cpp
)

target_include_directories(mylib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

set_target_properties(Versionlib PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS true)
CMakeLists.txt для приложения
В app/CMakeLists.txt создайте исполняемый файл и линкуйте его с библиотекой:

add_executable(myapp
    main.cpp
)

target_link_libraries(myapp
    PRIVATE mylib
)

target_include_directories(myapp PRIVATE
    ../lib/include
)
собираем:

mkdir build
cd ./build
cmake ..
cmake --build .
Сделаем выбор статическая или динамическая линковка
В корневом CMakeLists.txt (отсутствует в вашем вопросе, так что считаем его наличие в корне вашего проекта) добавьте опцию BUILD_SHARED_LIBS, которая отвечает за тип сборки библиотеки:
cmake_minimum_required(VERSION 3.25)
project(MyProject)

# Опция для выбора типа сборки библиотеки: ON для динамической, OFF для статической
option(BUILD_SHARED_LIBS "Build using shared libraries" ON)

add_subdirectory(lib)
add_subdirectory(src)
Это позволит вам передавать следующий аргумент при вызове CMake:

-DBUILD_SHARED_LIBS=ON для сборки динамических библиотек
-DBUILD_SHARED_LIBS=OFF для сборки статических библиотек
Шаг 2: Использование двух вариантов линковки
В файле CMakeList.txt, который находится в папке lib, учитывайте опцию BUILD_SHARED_LIBS:

add_library(mylib src/lib.cpp)

target_include_directories(mylib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

Теперь при добавлении библиотеки add_library автоматически выберет тип библиотеки на основании значения BUILD_SHARED_LIBS.

Шаг 3: Конфигурация и сборка с разным типом линковки
Для сборки проекта с конкретным типом библиотеки используйте передачу опции через командную строку:

mkdir build && cd build
cmake .. -DBUILD_SHARED_LIBS=ON  # Для сборки как динамической библиотеки
# или
cmake .. -DBUILD_SHARED_LIBS=OFF # Для сборки как статической библиотеки
cmake --build .
Шаг 4: Добавляем CI-CD с помощью GitHub Action
Добавляем папку в корень репозитория .github/workflows
в нее добавляем первый файл yaml с содержанием.

name: Build ci-cd with dynamic and static linking

on: 
  push:
    branches:
      - main
  pull_request:
    branches:
      - main

jobs:
  build:
    runs-on: ubuntu-latest
    strategy:
      matrix:
        configuration: [Release, Debug]
        build_type: [SHARED, STATIC]
        
    steps:
      # Проверка репозитория
      - name: Checkout repository
        uses: actions/checkout@v2
      
      # Установка необходимых инструментов
      - name: Set up environment
        run: |
          sudo apt-get update
          sudo apt-get install -y build-essential cmake

      # Создание директории сборки
      - name: Create build directory
        run: mkdir -p ci-cd/build

      # Конфигурация и генерация CMake файлов
      - name: Configure CMake
        run: |
          cd ci-cd/build
          cmake .. -DCMAKE_BUILD_TYPE=${{ matrix.configuration }} -DBUILD_SHARED_LIBS=${{ matrix.build_type == 'SHARED' }}

      # Сборка проекта
      - name: Build project
        run: |
          cd ci-cd/build
          cmake --build .
Шаг 5: Добавляем генерацию файла version.h c помощью cmake.
добавляем файл шаблон в корень version.h.in

#pragma once

#cmakedefine PROJECT_VERSION_PATCH @PROJECT_VERSION_PATCH@
меняем основной CMakeLists.txt

set(PATCH_VERSION "1" CACHE INTERNAL "Patch version")
set(PROJECT_VESRION 0.0.${PATCH_VERSION})
project(HellowPack VERSION ${PROJECT_VESRION})

# Генерация файла version.h на основе шаблона version.h.in
# configure_file берет шаблон и заменяет значения в нем на реальные, полученные из переменных
configure_file(
    ${CMAKE_SOURCE_DIR}/version.h.in  # Путь к исходному файлу-шаблону
    ${CMAKE_BINARY_DIR}/version.h     # Куда вывести сгенерированный файл
)

target_include_directories(Versionlib PRIVATE "${CMAKE_BINARY_DIR}")
Шаг 6: Добавляем изменение версии с помощью GitHub action.
передаем в cmake -DPATCH_VERSION=${{ github.run_number }}
файл jaml вносим следующие изменения.

      # Конфигурация и генерация CMake файлов
      - name: Configure CMake
        run: |
          cd ci-cd/build
          cmake .. -DPATCH_VERSION=${{ github.run_number }} -DCMAKE_BUILD_TYPE=${{ matrix.configuration }} -DBUILD_SHARED_LIBS=${{ matrix.build_type == 'SHARED' }}
  

      # Сборка проекта

      # Печать версии
      - name: Print project version
        run: |
          cd ci-cd/build/app
          ./HelloApp
Шаг 7: Добавляем генерацию пакета deb. И формирование релиза.
Формируем deb только для версии release. в версиях shared и static
Возможно придется поставить права доступа в настройках Action разделе WorkFlow permision. read and write permision
изменения в CMakeList.txt

set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")
 

if(BUILD_SHARED_LIBS)
  set(CPACK_PACKAGE_FILE_NAME "HellowPack-${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}-shared")
else()
  set(CPACK_PACKAGE_FILE_NAME "HellowPack-${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}-static")
endif()

set(CPACK_PACKAGE_CONTACT rolliks@gmail.com)
include(CPack)
Создание пакета из консоли

cmake ..
cmake --build .
cpack
изменения в GitHub Action

# Условие для создания release:
      - name: Package with CPack
        if: matrix.configuration == 'Release'
        run: |
          cd ci-cd/build
          cpack -G DEB
          echo "Packages created for ${{ matrix.build_type }} ${{ matrix.configuration }}"
 

      # Генерация уникального тега
      - name: Generate unique release tag
        id: generate_tag
        run: |
          # Создаем уникальный тег на основе run_number и временной метки
          TAG_NAME="release-${{ github.run_number }}-$(date +%Y%m%d%H%M%S)"
          echo "Generated tag: $TAG_NAME"
          echo "tag_name=$TAG_NAME" >> $GITHUB_ENV  # Сохраняем его в переменной окружения
  

      # Создание релиза с уникальным тегом
      - name: Create Release
        if: matrix.configuration == 'Release'
        id: create_release
        uses: actions/create-release@v1
        env:
          GITHUB_TOKEN: ${{ secrets.GITHUB_TOKEN }}
        with:
          tag_name: ${{ env.tag_name }}    # Используем переменную окружения для тега
          release_name: "Release ${{ env.tag_name }}"  # Используем тот же тег как имя релиза
          draft: false
          prerelease: false
      - name: Upload Release Asset
        if: matrix.configuration == 'Release'
        uses: actions/upload-release-asset@v1
        env:
          GITHUB_TOKEN: ${{ secrets.GITHUB_TOKEN }}
        with:
          upload_url: ${{ steps.create_release.outputs.upload_url }}
          asset_path: ./ci-cd/build/HellowPack-0.0.${{ github.run_number }}-${{ matrix.build_type == 'SHARED' && 'shared' || 'static' }}.deb
          asset_name: HellowPack-0.0.${{ github.run_number }}-${{ matrix.build_type == 'SHARED' && 'shared' || 'static' }}.deb
          asset_content_type: application/vnd.debian.binary-package
Zero-Links
Links
