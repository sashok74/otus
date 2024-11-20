add_executable(ip_filter main.cpp src/ip_filters.cpp)

# target_link_libraries(ip_filter PRIVATE lib)

target_include_directories(ip_filter PUBLIC ${PROJECT_SOURCE_DIR}/app/include)
