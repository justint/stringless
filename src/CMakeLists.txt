# Stringless classes
add_library (MemoryManager SHARED MemoryManager.cc)
add_library (FaceDetector STATIC FaceDetector.cc)
add_library (Writer STATIC Writer.cc)
add_library (FrameData SHARED FrameData.cc)
add_library (Mutex SHARED Mutex.cc)
add_library (Reader SHARED Reader.cc)

add_executable (Stringless Stringless.cc)

target_link_libraries(Stringless MemoryManager FaceDetector Writer FrameData Mutex ${OpenCV_LIBS} dlib rt pthread)

install(TARGETS Stringless RUNTIME DESTINATION bin)
install(TARGETS MemoryManager FrameData Mutex Reader LIBRARY DESTINATION lib)