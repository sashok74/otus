add_executable(HelloApp main.cpp)

target_link_libraries(HelloApp PRIVATE Versionlib)

target_include_directories(HelloApp PRIVATE ../lib/include)