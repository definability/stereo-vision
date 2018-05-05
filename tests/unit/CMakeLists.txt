cmake_minimum_required(VERSION 2.6)
project(stereo-vision-unit-tests)

file(GLOB SRCS *.cpp)
add_executable(testunit ${SRCS})
target_link_libraries(
    testunit

    disparity-graph
    matrix
    labeling
    disparity-finder
    bf-disparity-finder
    diffusion-disparity-finder

    libgtest
    libgmock
)

add_test(
    NAME testunit
    COMMAND testunit
)
