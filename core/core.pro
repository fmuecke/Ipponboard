TEMPLATE = lib
LANGUAGE = C++
CONFIG += precompile_header static exceptions
DEFINES += _WIN32

# Use Precompiled headers (PCH)
# (inclusion of header in HEADERS section is not required!)
PRECOMPILED_HEADER = ../base/pch.h

INCLUDEPATH += $$quote($$(BOOST_DIR))

QMAKE_LIBDIR += $$quote($$(BOOST_DIR)/stage/lib)

DESTDIR = ../lib

#----------------------------------------------------------
# Create our custom prebuild target for the release build
#----------------------------------------------------------
prebuild.commands = ..\\base\\create_versioninfo.cmd
QMAKE_EXTRA_TARGETS += prebuild
# Hook our prebuild target in between qmake's Makefile update and the actual project target.
prebuildhook.depends = prebuild
CONFIG(release, debug|release):prebuildhook.target = Makefile.Release
QMAKE_EXTRA_TARGETS += prebuildhook

TARGET = core

build_pass:CONFIG(debug, debug|release) {
    TARGET = $$join(TARGET,,,_d)
}

win32-g++: COMPILER = mingw
contains(COMPILER, mingw) {
    #QMAKE_CXXFLAGS += -std=c++11
	QMAKE_CXXFLAGS += -std=c++0x
}


HEADERS = ../base/pch.h \
    ../util/path_helpers.h \
    ../util/qstring_serialization.h \
    Controller.h \
    Enums.h \
    FightCategory.h \
    iController.h \
    iControllerCore.h \
    iView.h \
    Score.h \
    StateMachine.h \
    Tournament.h \
    Fighter.h \
	TournamentMode.h \
	TournamentModel.h \
	ControllerConfig.h \
	Fight.h

SOURCES = Controller.cpp \
    FightCategory.cpp \
    Score.cpp \
    StateMachine.cpp \
	TournamentMode.cpp \
    TournamentModel.cpp \
    Fighter.cpp
    
#OTHER_FILES +=

TRANSLATIONS = ../i18n/core_de.ts
