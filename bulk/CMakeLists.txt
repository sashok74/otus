cmake_minimum_required(VERSION 3.25)
option(BUILD_SHARED_LIBS "Build using shared libraries" ON)

set(PATCH_VERSION "1" CACHE INTERNAL "Patch version")
set(PROJECT_VESRION 0.0.${PATCH_VERSION})
project(bulk VERSION ${PROJECT_VESRION})
enable_testing()

add_subdirectory(app)
add_subdirectory(tests)

set_target_properties(bulk tests PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)

if (NOT EXISTS "${CMAKE_BINARY_DIR}/conan_toolchain.cmake")
   message(FATAL_ERROR "Conan toolchain file not found. Make sure to run `conan install` first.")
endif()
include(${CMAKE_BINARY_DIR}/conan_toolchain.cmake)


install(TARGETS bulk RUNTIME DESTINATION bin)
set(CPACK_GENERATOR DEB)
set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")

# исли никаких библиотек не будте то можно убарть это разделение
if(BUILD_SHARED_LIBS)
  set(CPACK_PACKAGE_FILE_NAME "bulk-${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}-shared")
else()
  set(CPACK_PACKAGE_FILE_NAME "bulk-${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}-static")
endif()

set(CPACK_PACKAGE_CONTACT rolliks@gmail.com)

include(CPack)
