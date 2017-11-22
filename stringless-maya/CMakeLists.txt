cmake_minimum_required(VERSION 2.8)
project(stringless-maya)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/CMake)
set(CMAKE_CXX_FLAGS "--std=c++11")
set(CMAKE_MACOSX_RPATH 1)

set(SOURCE_FILES "src/StringlessMayaMain.cc"
    "src/StringlessMayaDevice.cc" "src/StringlessMayaCmd.cc"
    "../src/MemoryManager.cc" "../src/FrameData.cc"
    "../src/Mutex.cc" "../src/Reader.cc"
    "src/StringlessMayaDevice.h" "src/StringlessMayaCmd.h"
)

find_package(Maya REQUIRED)

add_library(stringless-maya SHARED ${SOURCE_FILES})
target_link_libraries(stringless-maya PRIVATE Maya::Maya)
target_include_directories(stringless-maya PRIVATE Maya::Maya "../src" "src")
MAYA_PLUGIN(stringless-maya)
