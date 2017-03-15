include(qtconfig)
TARGET = stringless
CONFIG += designer
HEADERS += src/StringlessWindow.h
SOURCES += src/StringlessWindow.cc
FORMS += stringless.ui
LIBS += -lOpenMayaUI -lReader -lMutex -lMemoryManager
RESOURCES += stringless.qrc
