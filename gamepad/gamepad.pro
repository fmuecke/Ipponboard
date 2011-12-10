TEMPLATE = lib
DESTDIR = ../lib
CONFIG += staticlib

Release:TARGET = gamepad
Debug:TARGET = gamepad_d

CONFIG(__GNUC__) {
	QMAKE_CXXFLAGS += -std=c++0x
}

SOURCES = gamepad.cpp
HEADERS = gamepad.h

INCLUDEPATH += $$(BOOST)

QMAKE_LIBDIR += $$(BOOST)/lib
QMAKE_LIBS += -lWinmm