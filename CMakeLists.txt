cmake_minimum_required(VERSION 3.8)

# test c++ standard
if(NOT CMAKE_CXX_STANDARD OR CMAKE_CXX_STANDARD LESS 17)
    set(CMAKE_CXX_STANDARD 17)
endif()
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

project(TAVL)

add_executable(test tests/test.cpp)
add_executable(test_len tests/test.cpp)
target_compile_definitions(test_len PRIVATE TAVL_IS_SAME_LEN_IMPL)
target_include_directories(test PRIVATE .)
target_include_directories(test_len PRIVATE .)
