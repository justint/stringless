add_custom_target(
    moveMayaQtFiles
    COMMAND cp ${CMAKE_CURRENT_SOURCE_DIR}/../{buildconfig,Makefile.qt,qtconfig,stringless.pro} ${CMAKE_CURRENT_SOURCE_DIR}/../build/Debug/
)

add_custom_target(
    moveSrcFiles
    COMMAND ${CMAKE_CURRENT_SOURCE_DIR} src && cp -a ${CMAKE_CURRENT_SOURCE_DIR}/../src/. ${CMAKE_CURRENT_SOURCE_DIR}/../build/Debug/src/
)

add_custom_target(
    setQtEnvVars
    COMMAND export QMAKESPEC=/usr/local/qt/mkspecs/linux-g++ && export PATH=$PATH:/local/qmake/bin
)

add_executable (hello hello.cc)

add_dependencies (hello moveMayaQtFiles moveSrcFiles setQtEnvVars)

target_link_libraries(hello pipe pthread OpenMaya Foundation)