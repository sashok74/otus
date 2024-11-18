add_executable(tests test_main.cpp)
target_link_libraries(tests PRIVATE Versionlib)

target_include_directories(tests PRIVATE ../lib/include)
find_package(GTest CONFIG REQUIRED)
target_link_libraries(tests PRIVATE GTest::gtest GTest::gtest_main)

# Добавляем тестирование, интеграция с cmake
add_test(NAME VersionTest COMMAND tests)