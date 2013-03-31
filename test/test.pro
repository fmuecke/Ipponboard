QT += testlib

win32-g++: COMPILER = mingw
contains(COMPILER, mingw) {
    QMAKE_CXXFLAGS += -std=c++0x
}


SOURCES = TestTournamentMode.cpp \
	../core/TournamentMode.cpp

# install
#target.path = $$[QT_INSTALL_EXAMPLES]/qtestlib/tutorial1
#INSTALLS += target

OTHER_FILES += \
    TournamentModes-test.ini
