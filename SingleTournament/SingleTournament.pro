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
#PRECOMPILED_HEADER = ../base/pch.h
#disabled due to mingw reasons

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
win32-msvc2012: COMPILER = msvc

contains(COMPILER, mingw) {
	#QMAKE_CXXFLAGS += -std=c++0x
	QMAKE_CXXFLAGS += -std=c++11
	# get rid of some nasty boost warnings
	QMAKE_CXXFLAGS += -Wno-unused-local-typedefs
	QMAKE_LIBS += -lboost_serialization-mgw48-mt-1_53
	QMAKE_LIBS += -lboost_system-mgw48-mt-1_53
	#QMAKE_LIBS += -lboost_filesystem-mgw48-mt-1_53

	# copy all needed files to destdir
	QMAKE_POST_LINK += copy_files.cmd
}

contains(COMPILER, msvc) {
    QMAKE_CXX += /FS
    DEFINES += "WINVER=0x0501"
    DEFINES += WIN32 _WIN32_WINNT=0x0501
    #QMAKE_LIBS += -llibboost_serialization-vc100-mt-1_50
    #QMAKE_LIBS += -llibboost_system-vc100-mt-1_50
    #QMAKE_LIBS += -llibboost_filesystem-vc100-mt-1_50

    # remove unneccessary output files
    QMAKE_POST_LINK += del /Q ..\\bin\\$${TARGET}.exp ..\\bin\\$${TARGET}.lib

    # copy all needed files to destdir
    QMAKE_POST_LINK += & copy_files.cmd
}

SOURCES = Main.cpp \
    ../base/ClubManager.cpp \
    ../base/ClubManagerDlg.cpp \
    ../base/MainWindowBase.cpp \
    ../base/SettingsDlg.cpp \
    ../base/View.cpp \
    ../base/FightCategoryManagerDlg.cpp \
    ../base/FightCategoryManager.cpp \
    ../Widgets/ScaledImage.cpp \
    ../Widgets/ScaledText.cpp \
    ../Widgets/SplashScreen.cpp \
    ../Widgets/Countdown.cpp \

HEADERS = ../base/pch.h \
    ../base/ClubManager.h \
    ../base/ClubManagerDlg.h \
    ../base/MainWindowBase.h \
    ../base/SettingsDlg.h \
    ../base/View.h \
    ../base/FightCategoryManagerDlg.h \
    ../base/FightCategoryManager.h \
    ../Widgets/Countdown.h \
    ../Widgets/SplashScreen.h \
    ../Widgets/ScaledImage.h \
    ../Widgets/ScaledText.h \
    ../util/path_helpers.h \
    ../util/qstring_serialization.h

FORMS = ../base/ClubManagerDlg.ui \
    MainWindow.ui \
    View_vertical_single.ui \
    ../base/SettingsDlg.ui \
    ../base/View_horizontal.ui \
    ../base/FightCategoryManagerDlg.ui \
    ../Widgets/SplashScreen.ui \
    ../Widgets/Countdown.ui

#OTHER_FILES +=

RESOURCES += ../base/Ipponboard.qrc

TRANSLATIONS = ../i18n/Ipponboard_de.ts

win32:RC_FILE = ../base/ipponboard.rc
