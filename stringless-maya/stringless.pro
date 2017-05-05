include(qtconfig)
TARGET = stringless
CONFIG += designer
HEADERS += src/StringlessMayaDevice.h src/StringlessMayaCmd.h
SOURCES += src/StringlessMayaMain.cc src/StringlessMayaDevice.cc src/StringlessMayaCmd.cc
FORMS += stringless.ui
LIBS += -lOpenMayaUI -lReader -lMutex -lMemoryManager -lFrameData
RESOURCES += stringless.qrc
