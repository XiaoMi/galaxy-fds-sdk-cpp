cmake_minimum_required (VERSION 2.8)

aux_source_directory(. TEST_DIR_SRCS)

add_executable (testrunner ${TEST_DIR_SRCS})

target_link_libraries(testrunner LINK_PUBLIC gtest gtest_main 
  galaxy-fds-sdk-cpp PocoNet PocoFoundation PocoNetSSL PocoJSON crypto -lpthread -lm)
