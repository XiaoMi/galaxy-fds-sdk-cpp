project (galaxy-fds-sdk-cpp)

cmake_minimum_required (VERSION 2.8)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

aux_source_directory(src DIR_SRCS)

add_library (galaxy-fds-sdk-cpp ${DIR_SRCS})

target_include_directories (galaxy-fds-sdk-cpp PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_subdirectory(sample)
add_subdirectory(test)
