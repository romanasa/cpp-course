cmake_minimum_required(VERSION 3.8)
set(CMAKE_CXX_FLAGS "-pthread -Wall -std=c++17 -pedantic ${CMAKE_CXX_FLAGS}")
set(CMAKE_CXX_FLAGS_DEBUG "-fsanitize=address,undefined -D_GLIBCXX_DEBUG ${CMAKE_CXX_FLAGS_DEBUG}")
set(CMAKE_CXX_FLAGS_COVERAGE "-g --coverage")
set(LINK_FLAGS "-pthread ${LINK_FLAGS}")

include_directories(.)
add_subdirectory(gtest)
include_directories(${CMAKE_SOURCE_DIR}/gtest)

add_library(counted counted.h counted.cpp fault_injection.h fault_injection.cpp gtest/gtest-all.cc gtest/gtest_main.cc)

add_executable(std std.cpp)
target_link_libraries(std counted)

add_executable(set set.cpp set.h)
target_link_libraries(set counted gtest -lpthread)
