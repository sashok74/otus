cmake_minimum_required(VERSION 3.25)
option(BUILD_SHARED_LIBS "Build using shared libraries" ON)

set(PATCH_VERSION "1" CACHE INTERNAL "Patch version")
set(PROJECT_VESRION 0.0.${PATCH_VERSION})
project(HellowPack VERSION ${PROJECT_VESRION})
enable_testing()

include_directories(${CMAKE_SOURCE_DIR}/include)
add_subdirectory(src)
add_subdirectory(tests)

set_target_properties(bayan finddub tests PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)

if (NOT EXISTS "${CMAKE_BINARY_DIR}/conan_toolchain.cmake")
   message(FATAL_ERROR "Conan toolchain file not found. Make sure to run `conan install` first.")
endif()
include(${CMAKE_BINARY_DIR}/conan_toolchain.cmake)

# Генерация файла version.h на основе шаблона version.h.in
# configure_file берет шаблон и заменяет значения в нем на реальные, полученные из переменных
configure_file(
    ${CMAKE_SOURCE_DIR}/include/finddub/version.h.in  # Шаблон
    ${CMAKE_BINARY_DIR}/version.h             # Генерируемый файл
)

# Сообщаем библиотекам, где искать сгенерированный version.h
target_include_directories(tests PRIVATE "${CMAKE_BINARY_DIR}")

if(BUILD_SHARED_LIBS)
    install(TARGETS finddub LIBRARY DESTINATION lib)
endif()
install(TARGETS bayan RUNTIME DESTINATION bin)

set(CPACK_GENERATOR DEB)

set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")

if(BUILD_SHARED_LIBS)
  set(CPACK_PACKAGE_FILE_NAME "bayan-${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}-shared")
else()
  set(CPACK_PACKAGE_FILE_NAME "bayan-${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}-static")
endif()

set(CPACK_PACKAGE_CONTACT rolliks@gmail.com)

# Можно подключить CPack для генерации пакетов (опционально)
include(CPack)