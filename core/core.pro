TEMPLATE = lib
LANGUAGE = C++
CONFIG += precompile_header static exceptions
DEFINES += _WIN32

# Use Precompiled headers (PCH)
# (inclusion of header in HEADERS section is not required!)
PRECOMPILED_HEADER = ../base/pch.h

INCLUDEPATH += $$quote($$(BOOST))

QMAKE_LIBDIR += $$quote($$(BOOST)/lib) \
    $$quote($$(BOOST)/stage/lib)

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

CONFIG(__GNUC__) {
    QMAKE_CXXFLAGS += -std=c++0x
}


HEADERS = ../base/pch.h \
    ../base/controller.h \
    ../base/enums.h \
    ../base/icontroller.h \
    ../base/icontrollercore.h \
    ../base/iview.h \
    ../base/score.h \
    ../base/statemachine.h \
    ../base/tournament.h \
    ../base/fightcategory.h \
    ../base/tournamentmodel.h \
    ../base/controlconfig.h \
    ../util/helpers.h \
    ../util/qstring_serialization.h

SOURCES = ../base/controller.cpp \
    ../base/score.cpp \
    ../base/statemachine.cpp \
    ../base/tournamentmodel.cpp \
    ../base/fightcategory.cpp

#OTHER_FILES +=
