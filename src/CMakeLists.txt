cmake_minimum_required(VERSION 2.6)
project(stereo-vision)

set(disparity_graph_src disparity_graph.cpp)
set(matrix_src matrix.cpp)
add_library(libdisparity-graph STATIC ${disparity_graph_src})
add_library(libmatrix STATIC ${matrix_src})
target_include_directories(
    libdisparity-graph PUBLIC
    libmatrix PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)
