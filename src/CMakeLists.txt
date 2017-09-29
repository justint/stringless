# Stringless classes
add_library (MemoryManagerObj OBJECT MemoryManager.cc)
add_library (MemoryManager SHARED MemoryManager.cc)

add_library (FaceDetectorObj OBJECT FaceDetector.cc)
add_library (FaceDetector STATIC FaceDetector.cc)

add_library (Writer STATIC Writer.cc)

add_library (FrameDataObj OBJECT FrameData.cc)
add_library (FrameData SHARED FrameData.cc)

add_library (MutexObj OBJECT Mutex.cc)
add_library (Mutex SHARED Mutex.cc)

add_executable (Stringless Stringless.cc)

if (APPLE)
    target_link_libraries(Stringless MemoryManager FaceDetector Writer FrameData Mutex ${OpenCV_LIBS} dlib pthread)
else()
    target_link_libraries(Stringless MemoryManager FaceDetector Writer FrameData Mutex ${OpenCV_LIBS} dlib rt pthread)
endif()

install(TARGETS Stringless RUNTIME DESTINATION bin)
install(TARGETS MemoryManager FrameData Mutex LIBRARY DESTINATION lib)
