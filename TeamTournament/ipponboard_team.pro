# -------------------------------------------------
# Project created by QtCreator 2009-12-22T21:25:43
# -------------------------------------------------
TARGET = Ipponboard_team
TEMPLATE = app
LANGUAGE = C++
CONFIG += precompile_header
DEFINES += TEAM_VIEW
DEFINES += _WIN32

# Use Precompiled headers (PCH)
# (inclusion of header in HEADERS section is not required!)
PRECOMPILED_HEADER  = ../base/pch.h

SOURCES = ../base/clubmanager.cpp \
	../base/clubmanagerdlg.cpp \
	../base/controller.cpp \
	../base/mainwindow.cpp \
	../base/score.cpp \
	../base/settingsdlg.cpp \
	../base/statemachine.cpp \
	../base/tournamentmodel.cpp \
	../base/view.cpp \
	../gamepad/gamepad.cpp \
	../widgets/scaledimage.cpp \
	../widgets/scaledtext.cpp \
	main.cpp \
	scorescreen.cpp

HEADERS = ../base/pch.h \
	../base/clubmanager.h \
	../base/clubmanagerdlg.h \
	../base/controller.h \
	../base/enums.h \
	../base/icontroller.h \
	../base/icontrollercore.h \
	../base/iview.h \
	../base/mainwindow.h \
	../base/score.h \
	../base/settingsdlg.h \
	../base/statemachine.h \
	../base/tournament.h \
	../base/tournamentmodel.h \
	../base/view.h \
	../gamepad/gamepad.h \
	../util/qstring_serialization.h \
	../widgets/scaledimage.h \
	../widgets/scaledtext.h \
	scorescreen.h \
    ../util/helpers.h

FORMS = ../base/clubmanagerdlg.ui \
	mainwindow.ui \
	scorescreen.ui \
	../base/settingsdlg.ui \
	../base/view_horizontal.ui \
	view_vertical_team.ui

OTHER_FILES +=

RESOURCES += ../base/ipponboard.qrc

INCLUDEPATH += $$quote($$(BOOST))

QMAKE_LIBS += -lshell32
QMAKE_LIBS += -lWinmm
QMAKE_LIBDIR += $$quote($$(BOOST)/lib) \
	$$quote($$(BOOST)/stage/lib)

TRANSLATIONS = ../i18n/ipponboard_en.ts \
	../i18n/ipponboard_de.ts

win32:RC_FILE = ../base/ipponboard.rc
