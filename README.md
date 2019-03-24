# TAVL
TAVL is a template version of AVL-tree sequence class for C++ metaprogramming.

[![Build Status](https://travis-ci.org/wxr001/TAVL.svg?branch=master)](https://travis-ci.org/wxr001/TAVL)
[![Build status](https://ci.appveyor.com/api/projects/status/hsuwd04xtc7lv3wl?svg=true)](https://ci.appveyor.com/project/wxr001/tavl)


C++17 is needed.

## Notes for benchmarks: 
These benchmarks are used to test the memory cost between type_list and TAVL when the number of types is very large (2000 for example).
Compiling test_tavl.cpp cost 6.5GB+ memory and 10+ mins in my laptop.
Compiling test_list.cpp failed because of  memory exhausted.

## Do Not Try to Compile those benchmarks unless you have enough memory resource.
