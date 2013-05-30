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
	QMAKE_LIBS += -lboost_serialization-mgw46-mt-1_50
	QMAKE_LIBS += -lboost_system-mgw46-mt-1_50
	#QMAKE_LIBS += -lboost_filesystem-mgw46-mt-1_50

	# copy all needed files to destdir
	QMAKE_POST_LINK += copy_files.cmd
}

contains(COMPILER, msvc) {
	#QMAKE_LIBS += -llibboost_serialization-vc100-mt-1_50
	#QMAKE_LIBS += -llibboost_system-vc100-mt-1_50
	#QMAKE_LIBS += -llibboost_filesystem-vc100-mt-1_50

	# remove unneccessary output files
	QMAKE_POST_LINK += del /Q ..\\bin\\$${TARGET}.exp ..\\bin\\$${TARGET}.lib

	# copy all needed files to destdir
        QMAKE_POST_LINK += & copy_files.cmd
}

SOURCES = Main.cpp \
    MainWindow.cpp \
    ../base/AddFighterDlg.cpp \
    ../base/ClubManager.cpp \
    ../base/ClubManagerDlg.cpp \
    ../base/SettingsDlg.cpp \
    ../base/View.cpp \
    ../base/FightCategoryManagerDlg.cpp \
    ../base/FightCategoryManager.cpp \
    ../base/MainWindowBase.cpp \
    ../base/FighterManager.cpp \
    ../base/FighterManagerDlg.cpp \
    ../widgets/ScaledImage.cpp \
    ../widgets/ScaledText.cpp \
    ../widgets/SplashScreen.cpp \
    ../widgets/Countdown.cpp

HEADERS = ../base/pch.h \
    MainWIndow.h \
    ../base/AddFighterDlg.h \
    ../base/ClubManager.h \
    ../base/ClubManagerDlg.h \
    ../base/MainWindowBase.h \
    ../base/SettingsDlg.h \
    ../base/View.h \
    ../base/FightCategoryManagerDlg.h \
    ../base/FightCategoryManager.h \
    ../base/FighterManager.h \
    ../base/FighterManagerDlg.h \
    ../util/path_helpers.h \
    ../util/qstring_serialization.h \
    ../widgets/Countdown.h \
    ../widgets/SplashScreen.h \
    ../widgets/ScaledImage.h \
    ../widgets/ScaledText.h \
    ../util/SimpleCsvFile.hpp 

FORMS = MainWindow.ui \
    view_vertical_single.ui \
    ../base/AddFighterDlg.ui \
    ../base/ClubManagerDlg.ui \
    ../base/SettingsDlg.ui \
    ../base/FightCategoryManagerDlg.ui \
    ../base/FighterManagerDlg.ui \
    ../base/view_horizontal.ui \
    ../widgets/SplashScreen.ui \
    ../widgets/Countdown.ui

#OTHER_FILES +=
RESOURCES += ../base/ipponboard.qrc
TRANSLATIONS = ../i18n/Ipponboard_de.ts

win32:RC_FILE = ../base/ipponboard.rc
