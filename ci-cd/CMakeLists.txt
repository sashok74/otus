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

if(BUILD_SHARED_LIBS)
    install(TARGETS Versionlib LIBRARY DESTINATION lib)
endif()
install(TARGETS HelloApp RUNTIME DESTINATION bin)

set(CPACK_GENERATOR DEB)

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