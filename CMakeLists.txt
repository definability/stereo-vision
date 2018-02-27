cmake_minimum_required(VERSION 2.6)
project(stereo-vision-all)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -Wall -O3")

add_subdirectory(src)

enable_testing()

add_subdirectory(tests)

add_subdirectory(docs)
