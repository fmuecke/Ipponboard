# Created by and for Qt Creator This file was created for editing the project sources only.
# You may attempt to use it for building too, by modifying this file here.
cmake_minimum_required(VERSION 3.5)

project(Ipponboard VERSION 2.2.0 LANGUAGES C CXX)

if(USE_QT5)
	if(NOT DEFINED ENV{QT5DIR})
		message(FATAL_ERROR "QT5DIR not defined")
	endif()
	message(STATUS "Using: QT5DIR = $ENV{QT5DIR}")
	set(CMAKE_PREFIX_PATH $ENV{QT5DIR})
	set(CMAKE_LIBRARY_PATH $ENV{QT5DIR}/lib)
	find_package(Qt5 5.12.9 COMPONENTS Widgets XmlPatterns PrintSupport Multimedia REQUIRED)	
else()
	if(NOT DEFINED ENV{QT4DIR})
		message(FATAL_ERROR "QT4DIR not defined")
	endif()
	message(STATUS "Using: QT4DIR = $ENV{QT4DIR}")
	set(CMAKE_PREFIX_PATH $ENV{QT4DIR})
	set(CMAKE_LIBRARY_PATH $ENV{QT4DIR}/lib)
	find_package(Qt4 4.8.7 REQUIRED QtCore QtGui QtXmlPatterns)
endif()

#find_package(Qt5Core REQUIRED)
#find_package(Qt5Gui REQUIRED)
#find_package(Qt5Widgets REQUIRED)
#find_package(Qt5XmlPatterns REQUIRED)
#find_package(Qt5PrintSupport REQUIRED)
#find_package(Qt5Multimedia REQUIRED)

if(WIN32)
	if(NOT DEFINED ENV{BOOST_DIR})
		message(FATAL_ERROR "BOOST_DIR not defined")
	endif()
	set(BOOST_DIR "$ENV{BOOST_DIR}")
else()
	set(BOOST_DIR /usr/include/boost)
endif()
message(STATUS "Using: BOOST_DIR = ${BOOST_DIR}")

set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "" FORCE)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/..)
set(OUTPUT_DIR ${ROOT_DIR}/_build/bin)
set(OUTPUT_DIR_DEBUG ${ROOT_DIR}/_build/bin-debug)

message(STATUS "Using: ROOT_DIR = ${ROOT_DIR}")
message(STATUS "Using: OUTPUT_DIR = ${OUTPUT_DIR}")

set(SOURCES
    #pch.cpp
	#pch.h
	AddFighterDlg.cpp
    AddFighterDlg.h
    Club.h
    ClubManager.cpp
    ClubManager.h
    ClubManagerDlg.cpp
    ClubManagerDlg.h
    ClubParser.h
    ComboBoxDelegate.cpp
    ComboBoxDelegate.h
    DonationManager.cpp
    DonationManager.h
    FightCategoryManager.cpp
    FightCategoryManager.h
    FightCategoryManagerDlg.cpp
    FightCategoryManagerDlg.h
    FightCategoryParser.cpp
    FightCategoryParser.h
    FighterManager.cpp
    FighterManager.h
    FighterManagerDlg.cpp
    FighterManagerDlg.h
    ../util/array_helpers.h
    ../util/path_helpers.h
    ../widgets/Countdown.h
    ../widgets/ScaledImage.h
    ../widgets/ScaledText.h
    ../util/SimpleCsvFile.hpp
    ../core/Controller.h
    ../core/Enums.h
    ../core/Fight.h
    ../core/FightCategory.h
    ../core/iController.h
    ../core/iControllerCore.h
    ../core/Score.h
    ../core/StateMachine.h
    ../core/Tournament.h
    ../core/Fighter.h
    ../core/TournamentMode.h
    ../core/TournamentModel.h
    ../core/ControllerConfig.h
    ../core/Rules.h
    ipponboard.qrc
    Main.cpp
    MainWindow.cpp
    MainWindow.h
    MainWindowBase.cpp
    MainWindowBase.h
    MainWindowTeam.cpp
    MainWindowTeam.h
    ModeManagerDlg.cpp
    ModeManagerDlg.h
    #qrc_ipponboard.cpp
    ScoreScreen.cpp
    ScoreScreen.h
    SettingsDlg.cpp
    SettingsDlg.h
    SplashScreen.cpp
    SplashScreen.h
    #ui_AddFighterDlg.h
    #ui_ClubManagerDlg.h
    #ui_Countdown.h
    #ui_FightCategoryManagerDlg.h
    #ui_FighterManagerDlg.h
    #ui_MainWindow.h
    #ui_MainWindowTeam.h
    #ui_ModeManagerDlg.h
    #ui_ScoreScreen.h
    #ui_SettingsDlg.h
    #ui_SplashScreen.h
    #ui_view_horizontal.h
    #ui_view_vertical_single.h
    UpdateChecker.cpp
    UpdateChecker.h
    VersionComparer.cpp
    VersionComparer.h
    ../core/iGoldenScoreView.h
    ../core/iView.h
    ../util/json.hpp
    versioninfo.h
    View.cpp
    View.h
    ../util/jsoncpp/json.cpp
    ../widgets/ScaledImage.cpp
    ../widgets/ScaledText.cpp
    ../widgets/Countdown.cpp
    ../core/Controller.cpp
    ../core/FightCategory.cpp
    ../core/Fighter.cpp
    ../core/Fight.cpp
    ../core/Score.cpp
    ../core/StateMachine.cpp
    ../core/TournamentMode.cpp
    ../core/TournamentModel.cpp
    ../core/Rules.cpp
)

set(FORMS
    AddFighterDlg.ui
    ClubManagerDlg.ui
    FighterManagerDlg.ui
    FightCategoryManagerDlg.ui
    MainWindow.ui
    MainWindowTeam.ui
    ModeManagerDlg.ui
    ScoreScreen.ui
    SettingsDlg.ui
    SplashScreen.ui
    view_horizontal.ui
    view_vertical_single.ui
    view_vertical_team.ui
    ../widgets/Countdown.ui
)

set(OTHER
    Ipponboard.rc
    TournamentModes.ini
    categories.json
    #categories.xml
    clubs.json
)

source_group("UI Forms" FILES ${FORMS})
source_group("Other Files" FILES ${OTHER})

#include_directories(${Qt5Core_INCLUDE_DIRS})
#include_directories(${Qt5Gui_INCLUDE_DIRS})
#include_directories(${Qt5Widgets_INCLUDE_DIRS})
#include_directories(${Qt5PrintSupport_INCLUDE_DIRS})
#include_directories(${Qt5XmlPatterns_INCLUDE_DIRS})
#include_directories(${Qt5Multimedia_INCLUDE_DIRS})

add_executable(${CMAKE_PROJECT_NAME} WIN32 ${SOURCES} ${FORMS} ${OTHER})
set_target_properties(${CMAKE_PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY_DEBUG ${OUTPUT_DIR_DEBUG}
    RUNTIME_OUTPUT_DIRECTORY_RELEASE ${OUTPUT_DIR}
    COMPILE_FLAGS "/Zc:wchar_t- /MP"
    CMAKE_SUPPRESS_REGENERATION true
)

#target_precompile_headers(${CMAKE_PROJECT_NAME} PRIVATE pch.h)
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE ${BOOST_DIR})
if(USE_QT5)
	target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE Qt5::Widgets Qt5::XmlPatterns Qt5::PrintSupport Qt5::Multimedia)
	# copying of Qt dlls according to configuration (does not work RELIABLY with multi config generators)
	# therefore logic handled in separate batch file
	add_custom_command(TARGET ${CMAKE_PROJECT_NAME} POST_BUILD
		COMMAND call "${ROOT_DIR}/_build/win/copy_files-qt5.cmd" ${OUTPUT_DIR} $<CONFIG>
	)
else()
	target_link_libraries(${CMAKE_PROJECT_NAME} Qt4::QtCore Qt4::QtGui Qt4::QtXmlPatterns)
	# copying of Qt dlls according to configuration (does not work RELIABLY with multi config generators)
	# therefore logic handled in separate batch file
	add_custom_command(TARGET ${CMAKE_PROJECT_NAME} POST_BUILD
		COMMAND call "${ROOT_DIR}/_build/win/copy_files.cmd" ${OUTPUT_DIR} $<CONFIG>
	)
endif()
