# Stringless classes
add_library (MemoryManager STATIC MemoryManager.cc)
add_library (Writer STATIC Writer.cc)
add_library (FrameData STATIC FrameData.cc)
add_library (Mutex STATIC Mutex.cc)

add_executable (Stringless Stringless.cc)

target_link_libraries(Stringless MemoryManager Writer FrameData Mutex ${OpenCV_LIBS} dlib rt pthread)
