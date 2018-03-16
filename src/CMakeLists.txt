cmake_minimum_required(VERSION 2.6)
project(stereo-vision)

set(disparity_graph_src disparity_graph.cpp)
set(matrix_src matrix.cpp)
set(labeling_src labeling.cpp)
set(bf_disparity_finder_src bf_disparity_finder.cpp)
set(disparity_finder_src disparity_finder.cpp)

add_library(libdisparity-graph STATIC ${disparity_graph_src})
add_library(libmatrix STATIC ${matrix_src})
add_library(liblabeling STATIC ${labeling_src})
add_library(libbf_disparity_finder STATIC ${bf_disparity_finder_src})
add_library(libdisparity_finder STATIC ${disparity_finder_src})

target_include_directories(
    libdisparity-graph PUBLIC
    libmatrix PUBLIC
    liblabeling PUBLIC
    libbf_disparity_finder PUBLIC
    libdisparity_finder PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)
