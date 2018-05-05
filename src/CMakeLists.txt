cmake_minimum_required(VERSION 2.6)
project(stereo-vision)

set(disparity_graph_src disparity_graph.cpp)
set(matrix_src matrix.cpp)
set(labeling_src labeling.cpp)
set(disparity_finder_src disparity_finder.cpp)
set(bf_disparity_finder_src bf_disparity_finder.cpp)

add_library(disparity-graph STATIC ${disparity_graph_src})
add_library(matrix STATIC ${matrix_src})
add_library(labeling STATIC ${labeling_src})
add_library(disparity-finder STATIC ${disparity_finder_src})
add_library(bf-disparity-finder STATIC ${bf_disparity_finder_src})

target_include_directories(
    disparity-graph PUBLIC
    matrix PUBLIC
    labeling PUBLIC
    disparity-finder PUBLIC
    bf-disparity-finder PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)
