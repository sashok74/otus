cmake_minimum_required(VERSION 3.25)
option(BUILD_SHARED_LIBS "Build using shared libraries" ON)

set(PATCH_VERSION "1" CACHE INTERNAL "Patch version")
set(PROJECT_VESRION 0.0.${PATCH_VERSION})
project(HellowPack VERSION ${PROJECT_VESRION})

add_subdirectory(lib)
add_subdirectory(app)

set_target_properties(HelloApp Versionlib PROPERTIES
    CXX_STANDARD 14
    CXX_STANDARD_REQUIRED ON
)

# Генерация файла version.h на основе шаблона version.h.in
# configure_file берет шаблон и заменяет значения в нем на реальные, полученные из переменных
configure_file(
    ${CMAKE_SOURCE_DIR}/version.h.in  # Путь к исходному файлу-шаблону
    ${CMAKE_BINARY_DIR}/version.h     # Куда вывести сгенерированный файл
)

target_include_directories(HelloApp PRIVATE "${CMAKE_BINARY_DIR}")
target_include_directories(Versionlib PRIVATE "${CMAKE_BINARY_DIR}")

