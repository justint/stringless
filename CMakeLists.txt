SET (CMAKE_VERBOSE_MAKEFILE 1)
cmake_minimum_required(VERSION 3.6)

project (stringless)

find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

add_subdirectory (src)

# mem_test is old testing code, uncomment the below line and comment everything
# else if you want to compile it
#add_subdirectory (mem_test)
