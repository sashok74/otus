add_executable(tests test_main.cpp)

# Добавляем исходные файлы, если они нужны
target_sources(tests PRIVATE ${PROJECT_SOURCE_DIR}/src/dubler.cpp)

# Подключаем директорию include
target_include_directories(tests PRIVATE ${PROJECT_SOURCE_DIR}/include)

# Линкуем тесты с GTest, GMock и Boost, предоставленным Conan

# Находим Google Test и Google Mock
find_package(GTest CONFIG REQUIRED)
target_link_libraries(tests PRIVATE GTest::gtest GTest::gmock)

find_package(Boost REQUIRED COMPONENTS filesystem system)
target_link_libraries(tests PRIVATE Boost::filesystem Boost::system)

# Добавляем тест для CTest
add_test(NAME BayanTest COMMAND tests)



