cmake_minimum_required(VERSION 2.6)
project(stereo-vision)

set(disparity_graph_src disparity_graph.cpp)
add_library(libdisparity-graph STATIC ${disparity_graph_src})
target_include_directories(
    libdisparity-graph PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)
