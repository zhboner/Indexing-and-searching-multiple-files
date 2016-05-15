cmake_minimum_required(VERSION 3.4)
project(3)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR})

set(SOURCE_FILES main.cpp)
add_executable(a3search ${SOURCE_FILES})