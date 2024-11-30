cmake_minimum_required(VERSION 3.10)
project(HACLExample)

set(CMAKE_CXX_STANDARD 11)

# Adjust this path to your HACL* installation
include_directories(../hacl-star/dist/gcc-compatible/)

# Add the executable
add_executable(example main.cpp)

# Link the HACL libraries (add other required libraries if needed)
target_link_libraries(example Hacl_Hash_Blake2b)
