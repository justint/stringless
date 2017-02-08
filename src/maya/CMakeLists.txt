add_executable(Reader Reader.cc)
include_directories(${CMAKE_SOURCE_DIR}/src) # include files from src
target_link_libraries(Reader MemoryManager Mutex FrameData rt pthread)

install(TARGETS Reader RUNTIME DESTINATION bin)