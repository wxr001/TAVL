cmake_minimum_required(VERSION 3.8)

# test c++ standard
if(NOT CMAKE_CXX_STANDARD OR CMAKE_CXX_STANDARD LESS 17)
    set(CMAKE_CXX_STANDARD 17)
endif()
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

project(TAVL)

add_executable(test tests/test.cpp)
target_include_directories(test PRIVATE .)
