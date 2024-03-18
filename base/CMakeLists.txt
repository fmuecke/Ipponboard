# Created by and for Qt Creator This file was created for editing the project sources only.
# You may attempt to use it for building too, by modifying this file here.

cmake_minimum_required(VERSION 3.5)
project(Ipponboard VERSION 2.2.0 LANGUAGES C CXX)
message(STATUS "using: ${CMAKE_VS_WINDOWS_TARGET_PLATFORM_VERSION}")

find_package(Qt4 4.8.7 REQUIRED QtCore QtGui QtXmlPatterns)

if(NOT DEFINED ENV{QTDIR})
    message(FATAL_ERROR "QTDIR not defined")
endif()
if(NOT DEFINED ENV{BOOST_DIR})
    message(FATAL_ERROR "BOOST_DIR not defined")
endif()

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
#set(CMAKE_PREFIX_PATH "$ENV{QTDIR}")
#set(CMAKE_LIBRARY_PATH "$ENV{QTDIR}/lib")

set(ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/..)
set(OUTPUT_DIR ${ROOT_DIR}/_build/bin)

set(SOURCES
    AddFighterDlg.cpp
    AddFighterDlg.h
    #categories.json
    #categories.xml
    Club.h
    ClubManager.cpp
    ClubManager.h
    ClubManagerDlg.cpp
    ClubManagerDlg.h
    ClubParser.h
    #clubs.json
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
    Ipponboard.rc
    Main.cpp
    MainWindow.cpp
    MainWindow.h
    MainWindowBase.cpp
    MainWindowBase.h
    MainWindowTeam.cpp
    MainWindowTeam.h
    ModeManagerDlg.cpp
    ModeManagerDlg.h
    pch.h
    #qrc_ipponboard.cpp
    ScoreScreen.cpp
    ScoreScreen.h
    SettingsDlg.cpp
    SettingsDlg.h
    SplashScreen.cpp
    SplashScreen.h
    #TournamentModes.ini
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

add_executable(${CMAKE_PROJECT_NAME} WIN32 ${SOURCES} ${FORMS})
set_target_properties(${CMAKE_PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY_DEBUG ${OUTPUT_DIR}
            RUNTIME_OUTPUT_DIRECTORY_RELEASE ${OUTPUT_DIR}
    COMPILE_FLAGS "/Zc:wchar_t- /MP"
    CMAKE_SUPPRESS_REGENERATION true
)

target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE $ENV{BOOST_DIR})
target_link_libraries(${CMAKE_PROJECT_NAME} Qt4::QtCore Qt4::QtGui Qt4::QtXmlPatterns)

# Copy Qt dlls to the output directory
# TODO: fix copying of Qt dlls according to configuration (does not work RELIABLY with multi config generators)
#message("current build type: " ${CMAKE_BUILD_TYPE})
#message("current configuration: " ${CONFIG})
#if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_custom_command(TARGET ${CMAKE_PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E echo "copying Qt debug-AND-release libraries to ${OUTPUT_DIR}"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different $ENV{QTDIR}/bin/QtCored4.dll $ENV{QTDIR}/bin/QtGuid4.dll $ENV{QTDIR}/bin/QtNetworkd4.dll $ENV{QTDIR}/bin/QtXmlPatternsd4.dll ${OUTPUT_DIR}
       COMMAND ${CMAKE_COMMAND} -E copy_if_different $ENV{QTDIR}/bin/QtCore4.dll $ENV{QTDIR}/bin/QtGui4.dll $ENV{QTDIR}/bin/QtNetwork4.dll $ENV{QTDIR}/bin/QtXmlPatterns4.dll ${OUTPUT_DIR}
    )
#else()
#endif()
