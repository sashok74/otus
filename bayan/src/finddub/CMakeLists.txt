add_library(finddub finddub.cpp)

target_include_directories(finddub PUBLIC
    "${CMAKE_SOURCE_DIR}/include/finddub"
    "${CMAKE_BINARY_DIR}"  # для доступа к сгенерированному version.h 
)
