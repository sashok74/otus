add_library(Versionlib SHARED  src/lib.cpp )

target_include_directories(Versionlib PUBLIC  ${CMAKE_CURRENT_SOURCE_DIR}/include)

set_target_properties(Versionlib PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS true)