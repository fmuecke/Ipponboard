TEMPLATE = lib
CONFIG = staticlib
SOURCES = gamepad.cpp
HEADERS = gamepad.h

INCLUDEPATH += $$(BOOST)

QMAKE_LIBDIR += $$(BOOST)/lib
QMAKE_LIBS += -lWinmm