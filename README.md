# build
1. To build fds-client, you need to compile Poco(complete edition) & gtest manually. 
2. After poco and gtest are built, you can use CMake to build the client itself: 
```
$ mkdir build
$ cd build
$ cmake .. -DPOCO_INCLUDE=/my/poco/include -DPOCO_LIB=/my/poco/lib -DGTEST_INCLUDE=/my/gtest/include -DGTEST_LIB=/my/gtest/lib
$ make
```
