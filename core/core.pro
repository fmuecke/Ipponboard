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

TARGET = core

build_pass:CONFIG(debug, debug|release) {
    TARGET = $$join(TARGET,,,_d)
}

win32-g++: COMPILER = mingw
contains(COMPILER, mingw) {
    QMAKE_CXXFLAGS += -std=c++11
    #QMAKE_CXXFLAGS += -std=c++0x
}


HEADERS = ../base/pch.h \
    ../util/path_helpers.h \
    ../util/qstring_serialization.h \
    controlconfig.h \
    controller.h \
    enums.h \
    fightcategory.h \
    icontroller.h \
    icontrollercore.h \
    iview.h \
    score.h \
    statemachine.h \
    tournament.h \
    tournamentmodel.h

SOURCES = controller.cpp \
    fightcategory.cpp \
    score.cpp \
    statemachine.cpp \
    tournamentmodel.cpp
    

#OTHER_FILES +=

TRANSLATIONS = ../i18n/core_de.ts