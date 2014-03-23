# -------------------------------------------------
# Project created by QtCreator 2010-02-19T22:00:18
# -------------------------------------------------
TEMPLATE = app
TARGET = GamepadDemo
DESTDIR = ../bin

build_pass:CONFIG(release, debug|release) {
	QMAKE_LIBS += -lgamepad -lwinmm
}

build_pass:CONFIG(debug, debug|release) {
    TARGET = $$join(TARGET,,,_d)
	QMAKE_LIBS += -lgamepad_d -lwinmm
} 

SOURCES += Main.cpp GamepadDemo.cpp ../widgets/scaledimage.cpp
HEADERS += GamepadDemo.h ../widgets/scaledimage.h
FORMS   += GamepadDemo.ui
RESOURCES += res.qrc

INCLUDEPATH += $$(BOOST_DIR) ../gamepad

QMAKE_LIBDIR += $$(BOOST_DIR)/stage/lib ../lib
