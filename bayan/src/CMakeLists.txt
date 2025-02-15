add_subdirectory(finddub)

find_package(Boost REQUIRED COMPONENTS program_options filesystem)

add_executable(bayan main.cpp dubler.cpp)  

target_link_libraries(bayan PRIVATE finddub Boost::program_options Boost::filesystem)

