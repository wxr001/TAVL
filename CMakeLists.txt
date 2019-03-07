cmake_minimum_required(VERSION 3.8)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(TAVL)

add_executable(test tests/test.cpp)
target_include_directories(test PRIVATE .)
