DESTDIR = ../lib
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

CONFIG(release, release|debug)
{
    TARGET = core
    #OBJECTS_DIR = Release/.obj
    #MOC_DIR = Release/.moc
    #RCC_DIR = Release/.rcc
    #UI_DIR = Release/.ui
}

CONFIG(debug, release|debug)
{
    TARGET = core_d
	#OBJECTS_DIR = Debug/.obj
    #MOC_DIR = Debug/.moc
    #RCC_DIR = Debug/.rcc
    #UI_DIR = Debug/.ui
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

OTHER_FILES += 
