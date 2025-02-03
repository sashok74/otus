add_executable(tests test_main.cpp)

target_sources(tests PRIVATE ${PROJECT_SOURCE_DIR}/app/src/bulk.cpp)
target_include_directories(tests PRIVATE ${PROJECT_SOURCE_DIR}/app/include)

# Находим Google Test и Google Mock
find_package(GTest CONFIG REQUIRED)

# Линкуем тесты с GTest и GMock
target_link_libraries(tests PRIVATE GTest::gtest  GTest::gmock)

set(TESTS_BUILD_DIR ${CMAKE_BINARY_DIR}/tests)

# Добавляем тестирование, интеграция с cmake
add_test(NAME BulkTest COMMAND tests)

