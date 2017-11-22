set(CMAKE_VERBOSE_MAKEFILE 1)
set(CMAKE_MACOSX_RPATH 1)
cmake_minimum_required(VERSION 2.8)

project(stringless)
include(ext/dlib/dlib/cmake)

find_package(OpenCV REQUIRED)

add_subdirectory(src)

option(BUILD_MAYA "Enable building stringless-maya plugin" ON)
if(BUILD_MAYA)
    add_subdirectory(stringless-maya)
endif()
