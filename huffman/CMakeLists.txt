cmake_minimum_required(VERSION 3.10)
project(huffman)

set(CMAKE_CXX_STANDARD 14)
set(SOURCE_EXE main.cpp)
set(CMAKE_CXX_FLAGS_RELEASE  ${CMAKE_CXX_FLAGS_RELEASE})

include_directories(lib)

add_executable(huffman ${SOURCE_EXE})

add_subdirectory(lib)

target_link_libraries(huffman lib)