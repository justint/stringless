SET (CMAKE_VERBOSE_MAKEFILE 1)
cmake_minimum_required(VERSION 3.6)

project (stringless)

include (ext/dlib/dlib/cmake)

find_package(OpenCV REQUIRED)

add_subdirectory (src)
        
#file(COPY ${CMAKE_BINARY_DIR}/src/libMemoryManager.so 
#          ${CMAKE_BINARY_DIR}/src/libFrameData.so 
#          ${CMAKE_BINARY_DIR}/src/libMutex.so 
#          ${CMAKE_BINARY_DIR}/src/libReader.so 
#    DESTINATION lib)
        
#file (COPY ${CMAKE_BINARY_DIR}/src/Stringless
#      DESTINATION bin)
