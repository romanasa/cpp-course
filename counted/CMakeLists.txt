cmake_minimum_required(VERSION 2.8)

project(BIGINT)

set(CMAKE_CXX_STANDARD 17)
include_directories(${BIGINT_SOURCE_DIR})

add_executable(vector_testing
               vector_testing.cpp
               counted.h vector.h
               counted.cpp
               fault_injection.h
               fault_injection.cpp
               gtest/gtest-all.cc
               gtest/gtest.h
               gtest/gtest_main.cc)

if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic")
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -D_GLIBCXX_DEBUG -fsanitize=address,undefined")
endif()

target_link_libraries(vector_testing -lpthread)
