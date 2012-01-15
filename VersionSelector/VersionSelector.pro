#-------------------------------------------------
#
# Project created by QtCreator 2012-01-15T01:53:19
#
#-------------------------------------------------

QT += core gui
CONFIG += windows

CONFIG(release, release|debug) {
    TARGET = VersionSelector
    #QMAKE_LIBS += -lshell32 -lWinmm -lgamepad -lcore
}

CONFIG(debug, release|debug) {
    TARGET = VersionSelector_d
    #QMAKE_LIBS += -lshell32 -lWinmm -lgamepad_d -lcore_d
}

TEMPLATE = app

DESTDIR = ../bin

SOURCES += main.cpp maindlg.cpp
HEADERS += maindlg.h
FORMS   += maindlg.ui

TRANSLATIONS = ../i18n/VersionSelector_de.ts

win32:RC_FILE = versionselector.rc

QMAKE_POST_LINK += copy_files.cmd