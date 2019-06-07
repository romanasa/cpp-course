cmake_minimum_required(VERSION 3.10)
project(lib)

set(CMAKE_CXX_STANDARD 14)

set(SOURCE_LIB huffman.cpp trie.cpp)
set(HEADER_LIB huffman.h trie.h)

add_library(lib STATIC ${SOURCE_LIB} ${HEADER_LIB})

add_executable(testing test.cpp
        gtest/gtest-all.cc
        gtest/gtest.h
        gtest/gtest_main.cc)
target_link_libraries(testing lib -pthread)