#------------------------------------------------------------------------------
# This app depends on:
#   - gamepad
#   - core
# These need to be build first.

TEMPLATE = app
LANGUAGE = C++
CONFIG += windows precompile_header
DEFINES += _WIN32

# Use Precompiled headers (PCH)
# (inclusion of header in HEADERS section is not required!)
PRECOMPILED_HEADER = ../base/pch.h

INCLUDEPATH += $$quote($$(BOOST_DIR))

QMAKE_LIBDIR += $$quote($$(BOOST_DIR)/stage/lib) \
    ../lib

DESTDIR = ../bin

CONFIG(release, release|debug) {
    TARGET = Ipponboard
    QMAKE_LIBS += -lgamepad -lcore -lshell32 -lWinmm
}

CONFIG(debug, release|debug) {
    TARGET = Ipponboard_d
    QMAKE_LIBS += -lgamepad_d -lcore_d -lshell32 -lWinmm
}


# Auto select compiler 
win32-g++: COMPILER = mingw 
win32-msvc2010: COMPILER = msvc

contains(COMPILER, mingw) {
    QMAKE_CXXFLAGS += -std=c++0x
	#QMAKE_CXXFLAGS += -std=c++11
	QMAKE_LIBS += -lboost_serialization-mgw46-mt-1_50
	QMAKE_LIBS += -lboost_system-mgw46-mt-1_50
	#QMAKE_LIBS += -lboost_filesystem-mgw46-mt-1_50
}

contains(COMPILER, msvc) {
	#QMAKE_LIBS += -llibboost_serialization-vc100-mt-1_50
	#QMAKE_LIBS += -llibboost_system-vc100-mt-1_50
	#QMAKE_LIBS += -llibboost_filesystem-vc100-mt-1_50
}

SOURCES = main.cpp \
    mainwindow.cpp \
    ../base/clubmanager.cpp \
    ../base/clubmanagerdlg.cpp \
    ../base/settingsdlg.cpp \
    ../base/view.cpp \
    ../base/fightcategorymanagerdlg.cpp \
    ../base/fightcategorymanager.cpp \
    ../base/MainWindowBase.cpp \
    ../core/fighter.cpp \
    ../widgets/scaledimage.cpp \
    ../widgets/scaledtext.cpp \
    ../widgets/splashscreen.cpp \
    ../widgets/countdown.cpp

HEADERS = ../base/pch.h \
    mainwindow.h \
    ../base/clubmanager.h \
    ../base/clubmanagerdlg.h \
    ../base/MainWindowBase.h \
    ../base/settingsdlg.h \
    ../base/view.h \
    ../base/fightcategorymanagerdlg.h \
    ../base/fightcategorymanager.h \
    ../core/fighter.h \
    ../util/path_helpers.h \
    ../util/qstring_serialization.h \
    ../widgets/countdown.h \
    ../widgets/splashscreen.h \
    ../widgets/scaledimage.h \
    ../widgets/scaledtext.h

FORMS = mainwindow.ui \
    view_vertical_single.ui \
	../base/clubmanagerdlg.ui \
    ../base/settingsdlg.ui \
    ../base/view_horizontal.ui \
    ../base/fightcategorymanagerdlg.ui \
    ../widgets/splashscreen.ui \
    ../widgets/countdown.ui

#OTHER_FILES +=

RESOURCES += ../base/ipponboard.qrc

TRANSLATIONS = ../i18n/Ipponboard_de.ts

win32:RC_FILE = ../base/ipponboard.rc

# remove unneccessary output files
QMAKE_POST_LINK += del /Q ..\\bin\\$${TARGET}.exp
QMAKE_POST_LINK += && del /Q ..\\bin\\$${TARGET}.lib

# copy all needed files to destdir
QMAKE_POST_LINK += && copy_files.cmd
