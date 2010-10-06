# -------------------------------------------------
# Project created by QtCreator 2010-02-19T22:00:18
# -------------------------------------------------
TARGET = GamepadDemo

TEMPLATE = app

SOURCES += main.cpp \
    gamepaddemo.cpp \
    ../widgets/scaledimage.cpp \
    ../gamepad/gamepad.cpp

HEADERS += gamepaddemo.h \
    ../widgets/scaledimage.h \
    ../gamepad/gamepad.h

FORMS += gamepaddemo.ui

RESOURCES += res.qrc

INCLUDEPATH += $$(BOOST)

LIBPATH += $$(BOOST)/lib
