# -------------------------------------------------
# Project created by QtCreator 2009-12-22T21:25:43
# -------------------------------------------------
TARGET = Ipponboard

TEMPLATE = app

LANGUAGE = C++

CONFIG += precompile_header

# Use Precompiled headers (PCH)
# (inclusion of header in HEADERS section is not required!)
PRECOMPILED_HEADER = ../base/pch.h

SOURCES = ../base/clubmanager.cpp \
	../base/clubmanagerdlg.cpp \
	../base/controller.cpp \
	../gamepad/gamepad.cpp \
	main.cpp \
    ../base/mainwindow.cpp \
    ../widgets/scaledimage.cpp \
    ../widgets/scaledtext.cpp \
    ../base/score.cpp \
    ../base/settingsdlg.cpp \
    ../base/statemachine.cpp \
    ../base/tournamentmodel.cpp \
    ../base/view.cpp \
	../base/fightcategorymanagerdlg.cpp \
	../base/fightcategorymanager.cpp \
	../base/fightcategory.cpp \
    ../widgets/splashscreen.cpp

HEADERS = ../base/pch.h \
	../base/clubmanager.h \
	../base/clubmanagerdlg.h \
	../base/controller.h \
	../base/enums.h \
	../gamepad/gamepad.h \
	../base/icontroller.h \
	../base/icontrollercore.h \
	../base/iview.h \
    ../base/mainwindow.h \
    ../util/qstring_serialization.h \
    ../widgets/scaledimage.h \
    ../widgets/scaledtext.h \
    ../base/score.h \
    ../base/settingsdlg.h \
    ../base/statemachine.h \
    ../base/tournament.h \
    ../base/tournamentmodel.h \
    ../base/view.h \
	../base/fightcategorymanagerdlg.h \
	../base/fightcategorymanager.h \
	../base/fightcategory.h \
    ../widgets/splashscreen.h \
    ../util/helpers.h \
    ../base/controlconfig.h

FORMS = ../base/clubmanagerdlg.ui \
	mainwindow.ui \
	../base/settingsdlg.ui \
    view_vertical_single.ui \
    ../base/view_horizontal.ui \
	../base/fightcategorymanagerdlg.ui \
    ../widgets/splashscreen.ui

OTHER_FILES += 

RESOURCES += ../base/ipponboard.qrc

INCLUDEPATH += $$quote($$(BOOST))

DEFINES += _WIN32

QMAKE_LIBS += -lshell32
QMAKE_LIBS += -lWinmm

#QMAKE_LIBS += -lboost_serialization
#QMAKE_LIBS += -lboost_system
#QMAKE_LIBS += -lboost_filesystem

QMAKE_LIBDIR += $$quote($$(BOOST)/lib) \
    $$quote($$(BOOST)/stage/lib)

TRANSLATIONS = ../i18n/Ipponboard_en.ts \
    ../i18n/Ipponboard_de.ts

win32:RC_FILE = ../base/ipponboard.rc
