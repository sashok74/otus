add_library(finddub finddub.cpp)

# find_package(Boost REQUIRED COMPONENTS system locale filesystem )

# target_link_libraries(${TARGET_NAME}
#     PRIVATE
#         Boost::filesystem
# )

#message(STATUS "Boost_LIBRARIES = ${Boost_LIBRARIES}")

 target_include_directories(finddub PRIVATE
     "${CMAKE_SOURCE_DIR}/include/finddub"
     "${CMAKE_BINARY_DIR}"  # для доступа к сгенерированному version.h 
 )
