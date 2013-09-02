TEMPLATE = lib
DESTDIR = ../lib
CONFIG += staticlib

Release:TARGET = gamepad
Debug:TARGET = gamepad_d

SOURCES = gamepad.cpp
HEADERS = gamepad.h

INCLUDEPATH += $$(BOOST_DIR)

QMAKE_LIBDIR += $$(BOOST_DIR)/stage/lib
QMAKE_LIBS += -lWinmm