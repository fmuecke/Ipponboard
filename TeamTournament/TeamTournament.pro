#------------------------------------------------------------------------------
# This app depends on:
#   - gamepad
#   - core
# These need to be build first.
#PRE_TARGETDEPS += ../gamepad ../core

TEMPLATE = app
LANGUAGE = C++
CONFIG += windows precompile_header
DEFINES += _WIN32 TEAM_VIEW

# Use Precompiled headers (PCH)
# (inclusion of header in HEADERS section is not required!)
PRECOMPILED_HEADER  = ../base/pch.h

INCLUDEPATH += $$quote($$(BOOST_DIR))

QMAKE_LIBDIR += $$quote($$(BOOST_DIR)/stage/lib) \
    ../lib

DESTDIR = ../bin

CONFIG(release, release|debug) {
    TARGET = Ipponboard_team
    QMAKE_LIBS += -lgamepad -lcore -lshell32 -lwinmm
}

CONFIG(debug, release|debug) {
    TARGET = Ipponboard_team_d
    QMAKE_LIBS += -lgamepad_d -lcore_d -lshell32 -lwinmm 
}

# Auto select compiler
win32-g++: COMPILER = mingw
win32-msvc2010: COMPILER = msvc

contains(COMPILER, mingw) {
    QMAKE_CXXFLAGS += -std=c++0x
    QMAKE_LIBS += -lboost_serialization-mgw46-mt-1_50
	QMAKE_LIBS += -lboost_system-mgw46-mt-1_50
	QMAKE_LIBS += -lboost_filesystem-mgw46-mt-1_50

	# copy all needed files to destdir
	QMAKE_POST_LINK += copy_files.cmd
}


contains(COMPILER, msvc) {
        # remove unneccessary output files
	QMAKE_POST_LINK += del /Q ..\\bin\\$${TARGET}.exp ..\\bin\\$${TARGET}.lib

	# copy all needed files to destdir
        QMAKE_POST_LINK += & copy_files.cmd
}

HEADERS = MainWindowTeam.h \
    scorescreen.h \
    ../base/pch.h \
    ../base/clubmanager.h \
    ../base/clubmanagerdlg.h \
    ../base/ComboBoxDelegate.h \
    ../base/FighterManager.h \
    ../base/FighterManagerDlg.h \
    ../base/mainwindowbase.h \
    ../base/settingsdlg.h \
    ../base/view.h \
    ../util/path_helpers.h \
    ../util/qstring_serialization.h \
    ../widgets/countdown.h \
    ../widgets/scaledimage.h \
    ../widgets/scaledtext.h \
	../widgets/splashscreen.h

SOURCES = main.cpp \
    scorescreen.cpp \
    MainWindowTeam.cpp \
    ../base/clubmanager.cpp \
    ../base/clubmanagerdlg.cpp \
    ../base/FighterManager.cpp \
    ../base/FighterManagerDlg.cpp \
    ../base/mainwindowbase.cpp \
    ../base/settingsdlg.cpp \
    ../base/view.cpp \
    ../widgets/countdown.cpp \
    ../widgets/scaledimage.cpp \
    ../widgets/scaledtext.cpp \
    ../widgets/splashscreen.cpp \
	../base/ComboBoxDelegate.cpp

FORMS = ../base/clubmanagerdlg.ui \
    mainwindow.ui \
    scorescreen.ui \
    ../base/settingsdlg.ui \
    ../base/FighterManagerDlg.ui \
    ../base/view_horizontal.ui \
    ../widgets/splashscreen.ui \
    ../widgets/countdown.ui

OTHER_FILES += \
    TournamentModes.ini

RESOURCES += ../base/ipponboard.qrc
TRANSLATIONS = ../i18n/ipponboard_team_de.ts

win32:RC_FILE = ../base/Ipponboard_team.rc
