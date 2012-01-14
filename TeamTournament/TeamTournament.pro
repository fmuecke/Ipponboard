TEMPLATE = app
LANGUAGE = C++
CONFIG += windows precompile_header
DEFINES += _WIN32 TEAM_VIEW

# Use Precompiled headers (PCH)
# (inclusion of header in HEADERS section is not required!)
PRECOMPILED_HEADER  = ../base/pch.h

INCLUDEPATH += $$quote($$(BOOST))

QMAKE_LIBDIR += $$quote($$(BOOST)/lib) \
        $$quote($$(BOOST)/stage/lib) \
        ../lib

DESTDIR = ../bin

CONFIG(release, release|debug) {
    TARGET = Ipponboard_team
        QMAKE_LIBS += -lshell32 -lWinmm -lgamepad -lcore
}

CONFIG(debug, release|debug) {
    TARGET = Ipponboard_team_d
        QMAKE_LIBS += -lshell32 -lWinmm -lgamepad_d -lcore_d
}

CONFIG(__GNUC__) {
        QMAKE_CXXFLAGS += -std=c++0x
        QMAKE_LIBS += -lboost_serialization
        QMAKE_LIBS += -lboost_system
        QMAKE_LIBS += -lboost_filesystem
}

HEADERS = ../base/pch.h \
        ../base/clubmanager.h \
        ../base/clubmanagerdlg.h \
        #../base/controller.h \
        #../base/enums.h \
        #../base/icontroller.h \
        #../base/icontrollercore.h \
        #../base/iview.h \
        #../base/score.h \
        #../base/statemachine.h \
        #../base/tournament.h \
        #../base/tournamentmodel.h \
        ../widgets/scaledimage.h \
        ../widgets/scaledtext.h \
    ../util/helpers.h \
        ../util/qstring_serialization.h \
        ../base/mainwindow.h \
        ../base/settingsdlg.h \
        ../base/view.h \
        scorescreen.h

SOURCES = ../base/clubmanager.cpp \
        ../base/clubmanagerdlg.cpp \
        ../base/mainwindow.cpp \
        ../base/settingsdlg.cpp \
        ../base/view.cpp \
        ../widgets/scaledimage.cpp \
        ../widgets/scaledtext.cpp \
        main.cpp \
        scorescreen.cpp
        #../base/controller.cpp \
        #../base/score.cpp \
        #../base/statemachine.cpp \
        #../base/tournamentmodel.cpp \

FORMS = ../base/clubmanagerdlg.ui \
        mainwindow.ui \
        scorescreen.ui \
        view_vertical_team.ui \
        ../base/settingsdlg.ui
        #../base/view_horizontal.ui

OTHER_FILES +=

RESOURCES += ../base/ipponboard.qrc

TRANSLATIONS = ../i18n/ipponboard_team_de.ts

win32:RC_FILE = ../base/ipponboard.rc

clubs.path = $${DESTDIR}/clubs
clubs.files = clubs/clubs.xml
clubs.files += clubs/*.png
i18n.path = $${DESTDIR}/lang
i18n.files = ../i18n/ipponboard_team_de.qm
binaries.path = $${DESTDIR}
binaries.files += ../../3rdParty/msvc100/*.dll
binaries.files += ../../3rdParty/Qt-4.7.4-vc100/*.dll
sounds.path = $${DESTDIR}/sounds
sounds.files += ../../3rdParty/sounds/*.wav
INSTALLS += clubs i18n binaries sounds

QMAKE_POST_LINK += del /Q ..\\bin\\$${TARGET}.exp &&
QMAKE_POST_LINK += del /Q ..\\bin\\$${TARGET}.lib
