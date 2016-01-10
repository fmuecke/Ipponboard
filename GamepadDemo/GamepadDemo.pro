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

QMAKE_LFLAGS += /SUBSYSTEM:WINDOWS,5.01
win32-g++: COMPILER = mingw
win32-msvc2013: COMPILER = msvc
win32-msvc2015: COMPILER = msvc

contains(COMPILER, mingw) {
    QMAKE_CXXFLAGS += -std=c++11
    # get rid of some nasty boost warnings
    QMAKE_CXXFLAGS += -Wno-unused-local-typedefs
    #QMAKE_CXXFLAGS += -std=c++0x
}

contains(COMPILER, msvc) {
    QMAKE_CXX += /FS
    DEFINES += "WINVER=0x0501"
    DEFINES += WIN32 _WIN32_WINNT=0x0501
}

SOURCES += Main.cpp GamepadDemo.cpp ../Widgets/ScaledImage.cpp
HEADERS += GamepadDemo.h ../Widgets/ScaledImage.h
FORMS   += GamepadDemo.ui
RESOURCES += res.qrc

INCLUDEPATH += $$(BOOST_DIR) ../gamepad

QMAKE_LIBDIR += $$(BOOST_DIR)/stage/lib ../lib
