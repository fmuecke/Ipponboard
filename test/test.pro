QT += testlib

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

SOURCES = TestTournamentMode.cpp \
	../core/TournamentMode.cpp

# install
#target.path = $$[QT_INSTALL_EXAMPLES]/qtestlib/tutorial1
#INSTALLS += target

OTHER_FILES += \
    TournamentModes-test.ini
