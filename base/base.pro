#------------------------------------------------------------------------------
QT += xmlpatterns

# This app depends on:
#   - core
# These need to be build first.
#PRE_TARGETDEPS += ../core

TEMPLATE = app
LANGUAGE = C++
CONFIG += windows precompile_header
DEFINES += _WIN32

# Use Precompiled headers (PCH)
# (inclusion of header in HEADERS section is not required!)
#PRECOMPILED_HEADER  = ../base/pch.h
#disabled for mingw!

INCLUDEPATH += $$quote($$(BOOST_DIR))

QMAKE_LIBDIR += $$quote($$(BOOST_DIR)/stage/lib) \
    ../lib

DESTDIR = ../bin

#----------------------------------------------------------
# Create our custom prebuild target for the release build
#----------------------------------------------------------
prebuild.commands = create_versioninfo.cmd
QMAKE_EXTRA_TARGETS += prebuild
# Hook our prebuild target in between qmake's Makefile update and the actual project target.
prebuildhook.depends = prebuild
CONFIG(release, debug|release):prebuildhook.target = Makefile.Release
QMAKE_EXTRA_TARGETS += prebuildhook

CONFIG(debug, release|debug) {
    TARGET = Ipponboard_d
    QMAKE_LIBS += -lshell32
}

CONFIG(release, release|debug) {
    TARGET = Ipponboard
    QMAKE_LIBS += -lshell32
}

QMAKE_LFLAGS += /SUBSYSTEM:WINDOWS,5.01

# Auto select compiler 
win32-g++: COMPILER = mingw
win32-msvc2013: COMPILER = msvc
win32-msvc2015: COMPILER = msvc

contains(COMPILER, mingw) {
	#QMAKE_CXXFLAGS += -std=c++0x
	QMAKE_CXXFLAGS += -std=c++11
	# get rid of some nasty boost warnings
    QMAKE_CXXFLAGS += -Wno-unused-local-typedef

	# copy all needed files to destdir
	QMAKE_POST_LINK += copy_files.cmd
}

contains(COMPILER, msvc) {
    QMAKE_CXX += /FS /MP
    DEFINES += "WINVER=0x0501"
    DEFINES += WIN32 _WIN32_WINNT=0x0501

    # remove unneccessary output files
    QMAKE_POST_LINK += del /Q ..\\bin\\$${TARGET}.exp ..\\bin\\$${TARGET}.lib

    # copy all needed files to destdir
    QMAKE_POST_LINK += & copy_files.cmd
}

HEADERS = pch.h \
    MainWindow.h \
	MainWindowTeam.h \
    AddFighterDlg.h \
    ClubManager.h \
    ClubManagerDlg.h \
    MainWindowBase.h \
	ModeManagerDlg.h \
    SettingsDlg.h \
	ScoreScreen.h \
    View.h \
    FightCategoryManagerDlg.h \
    FightCategoryManager.h \
    FighterManager.h \
    FighterManagerDlg.h \
    ../util/path_helpers.h \
    ../util/qstring_serialization.h \
    ../widgets/Countdown.h \
    SplashScreen.h \
    ../widgets/ScaledImage.h \
    ../widgets/ScaledText.h \
    ../util/SimpleCsvFile.hpp \
	../core/Controller.h \
	../core/Enums.h \
	../core/EnumStrings.h \
	../core/Fight.h \
	../core/FightCategory.h \
	../core/iController.h \
	../core/iControllerCore.h \
	../core/iView.h \
	../core/Score.h \
	../core/StateMachine.h \
	../core/Tournament.h \
	../core/Fighter.h \
	../core/TournamentMode.h \
	../core/TournamentModel.h \
	../core/ControllerConfig.h \
    DonationManager.h \
    ../util/array_helpers.h

SOURCES = Main.cpp \
    Main.cpp \
    MainWindow.cpp \
    MainWindowTeam.cpp \
    ../util/jsoncpp/json.cpp \
	AddFighterDlg.cpp \
    ClubManager.cpp \
    ClubManagerDlg.cpp \
    SettingsDlg.cpp \
    View.cpp \
    FightCategoryManagerDlg.cpp \
    FightCategoryManager.cpp \
    MainWindowBase.cpp \
	ModeManagerDlg.cpp \
    FighterManager.cpp \
    FighterManagerDlg.cpp \
	ScoreScreen.cpp \
    ../widgets/ScaledImage.cpp \
    ../widgets/ScaledText.cpp \
    SplashScreen.cpp \
    ../widgets/Countdown.cpp \
	../core/Controller.cpp \
	../core/FightCategory.cpp \
	../core/Fighter.cpp \
	../core/Fight.cpp \
	../core/Score.cpp \
	../core/StateMachine.cpp \
	../core/TournamentMode.cpp \
	../core/TournamentModel.cpp \
    DonationManager.cpp

FORMS = MainWindow.ui \
    MainWindowTeam.ui \
    view_vertical_single.ui \
    AddFighterDlg.ui \
    ClubManagerDlg.ui \
	ScoreScreen.ui \
    SettingsDlg.ui \
	ModeManagerDlg.ui \
    FightCategoryManagerDlg.ui \
    FighterManagerDlg.ui \
    view_horizontal.ui \
    SplashScreen.ui \
    ../widgets/Countdown.ui

OTHER_FILES += \
    TournamentModes.ini

RESOURCES += ipponboard.qrc
TRANSLATIONS = ../i18n/de.ts \
    ../i18n/nl.ts

win32:RC_FILE = ipponboard.rc
