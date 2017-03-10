include(qtconfig)
TARGET = stringless
CONFIG += designer
HEADERS += src/stringless.h
SOURCES += src/stringless.cc
FORMS += stringless.ui
LIBS += -lOpenMayaUI
RESOURCES += stringless.qrc
