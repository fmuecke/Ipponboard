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

TRANSLATIONS = ../i18n/ipponboard_en.ts \
	../i18n/ipponboard_de.ts

win32:RC_FILE = ../base/ipponboard.rc
