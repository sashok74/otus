add_subdirectory(finddub)
add_executable(bayan main.cpp)
target_link_libraries(bayan PRIVATE finddub)
#target_include_directories(bayan PRIVATE ${CMAKE_BINARY_DIR})