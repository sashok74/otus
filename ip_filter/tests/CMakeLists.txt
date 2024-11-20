add_executable(tests test_main.cpp)

target_sources(tests PRIVATE ${PROJECT_SOURCE_DIR}/app/src/ip_filters.cpp)
target_include_directories(tests PUBLIC ${PROJECT_SOURCE_DIR}/app/include)

find_package(GTest CONFIG REQUIRED)
target_link_libraries(tests PRIVATE GTest::gtest GTest::gtest_main)

set(TESTS_BUILD_DIR ${CMAKE_BINARY_DIR}/tests)

# Копируем файлы данных в папку со сборкой тестов
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/ip_filter.tsv
     DESTINATION ${TESTS_BUILD_DIR})
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/ip_filter_result.tsv
     DESTINATION ${TESTS_BUILD_DIR})

# Добавляем тестирование, интеграция с cmake
add_test(NAME IPSortTest COMMAND tests)