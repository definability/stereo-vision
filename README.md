# Stereo Vision

[![Build Status](https://api.travis-ci.org/char-lie/stereo-vision.svg?branch=master)](https://travis-ci.org/char-lie/stereo-vision)
[![Codacy Badge](https://api.codacy.com/project/badge/Grade/3327c65e99614089818a06dede511750)](https://www.codacy.com/app/char-lie/stereo-vision?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=char-lie/stereo-vision&amp;utm_campaign=Badge_Grade)
[![CodeDocs](https://codedocs.xyz/char-lie/stereo-vision.svg)](https://codedocs.xyz/char-lie/stereo-vision/)

Implementation of algorithms for stereo vision.

The project was commercialised and copyrighted in another place,
so this repository is abandoned and archived.
You may be interested in https://github.com/definability/python_scanline_stereo.

# Development

## Build

It's recommended to build the project in separate `build` folder

```bash
mkdir -p build && cd build
```

The project uses [CMake](https://cmake.org/) and it's very easy in use.
Just execute the following code from the `build` directory

```bash
cmake .. && cmake --build .
```

## Unit tests

The project uses [GTest](https://github.com/google/googletest) framework.
To run tests you can execute unit tests from `build` directory of built project

```bash
make test
```

You can use `ctest` as well.
For verbose output with colors use

```bash
GTEST_COLOR=1 ctest -V
```

## Autobuild

Go to your `build` directory and execute `watch`

```bash
watch --color "cmake .. && cmake --build . && GTEST_COLOR=1 ctest -V | tail"
```
