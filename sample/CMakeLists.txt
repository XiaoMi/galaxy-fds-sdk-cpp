cmake_minimum_required (VERSION 2.8)

aux_source_directory(. TEST_DIR_SRCS)

add_executable (sample ${TEST_DIR_SRCS})

target_link_libraries(sample LINK_PUBLIC galaxy-fds-sdk-cpp PocoNet PocoNetSSL
  PocoFoundation PocoJSON crypto)
