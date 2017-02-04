SET (CMAKE_VERBOSE_MAKEFILE 1)

add_library (MemoryManager STATIC MemoryManager.cc)
add_library (Writer STATIC Writer.cc)
add_library (FrameData STATIC FrameData.cc)
add_library (Mutex STATIC Mutex.cc)
add_executable (Stringless Stringless.cc)
target_link_libraries(Stringless MemoryManager Writer FrameData Mutex rt pthread)

# mem_test is old testing code, uncomment the below line and comment everything
# else if you want to compile it
#add_subdirectory (mem_test)