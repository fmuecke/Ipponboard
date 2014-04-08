TEMPLATE = lib
LANGUAGE = C++
CONFIG += precompile_header static exceptions
DEFINES += _WIN32

# Use Precompiled headers (PCH)
# (inclusion of header in HEADERS section is not required!)
#PRECOMPILED_HEADER = ../base/pch.h
#disabled due to mingw reasons

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

#TARGET = core
#
#build_pass:CONFIG(debug, debug|release) {
#    TARGET = $$join(TARGET,,,_d)
#}
Release:TARGET = core
Debug:TARGET = core_d

win32-g++: COMPILER = mingw
win32-msvc2012: COMPILER = msvc
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

HEADERS = ../base/pch.h \
    ../util/path_helpers.h \
    ../util/qstring_serialization.h \
    Controller.h \
    Enums.h \
    EnumStrings.h \
	Fight.h \
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
	ControllerConfig.h

SOURCES = Controller.cpp \
    FightCategory.cpp \
    Fighter.cpp \
    Fight.cpp \
    Score.cpp \
    StateMachine.cpp \
	TournamentMode.cpp \
    TournamentModel.cpp
    
#OTHER_FILES +=

TRANSLATIONS = ../i18n/core_de.ts
