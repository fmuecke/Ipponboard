cmake_minimum_required(VERSION 3.13)

project(Ipponboard VERSION 1.0 LANGUAGES C CXX) 
message(STATUS "using: ${CMAKE_VS_WINDOWS_TARGET_PLATFORM_VERSION}")

find_package(Qt5 5.15.2 REQUIRED Core Gui XmlPatterns)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_PREFIX_PATH "$ENV{QTDIR}")

set (BOOST_DIR D:\\dev\\inst\\boost_1_82_0)
set (XMLQUERY C:\\devtools\\qt\\qt-4.8.7-x86-vs2017\\include\\QtXmlPatterns)

set( SOURCES
../core/Controller.cpp ../core/Controller.h
../core/ControllerConfig.h
../core/Enums.h
../core/Fight.cpp ../core/Fight.h
../core/FightCategory.cpp ../core/FightCategory.h
../core/Fighter.cpp ../core/Fighter.h
../core/Rules.cpp ../core/Rules.h
../core/Score.cpp ../core/Score.h
../core/StateMachine.cpp ../core/StateMachine.h
../core/Tournament.h
../core/TournamentMode.cpp ../core/TournamentMode.h
../core/TournamentModel.cpp ../core/TournamentModel.h
../core/iController.h
../core/iControllerCore.h
../core/iGoldenScoreView.h
../core/iView.h
../util/SimpleCsvFile.hpp
../util/array_helpers.h
../util/json.hpp
../util/jsoncpp/json.cpp
../util/path_helpers.h
../widgets/Countdown.cpp ../widgets/Countdown.h ../widgets/Countdown.ui
../widgets/ScaledImage.cpp ../widgets/ScaledImage.h
../widgets/ScaledText.cpp ../widgets/ScaledText.h
../base/AddFighterDlg.cpp ../base/AddFighterDlg.h ../base/AddFighterDlg.ui
../base/ClubManager.cpp ../base/ClubManager.h
../base/ClubManagerDlg.cpp ../base/ClubManagerDlg.h ../base/ClubManagerDlg.ui
../base/ClubParser.h
../base/DonationManager.cpp ../base/DonationManager.h
../base/FightCategoryManager.cpp ../base/FightCategoryManager.h
../base/FightCategoryManagerDlg.cpp ../base/FightCategoryManagerDlg.h ../base/FightCategoryManagerDlg.ui
../base/FightCategoryParser.cpp ../base/FightCategoryParser.h
../base/FighterManager.cpp ../base/FighterManager.h
../base/FighterManagerDlg.cpp ../base/FighterManagerDlg.h ../base/FighterManagerDlg.ui
../base/Main.cpp
../base/MainWindow.cpp ../base/MainWindow.h ../base/MainWindow.ui
../base/MainWindowBase.cpp ../base/MainWindowBase.h
../base/MainWindowTeam.cpp ../base/MainWindowTeam.h ../base/MainWindowTeam.ui
../base/ModeManagerDlg.cpp ../base/ModeManagerDlg.h ../base/ModeManagerDlg.ui
../base/ScoreScreen.cpp ../base/ScoreScreen.h ../base/ScoreScreen.ui
../base/SettingsDlg.cpp ../base/SettingsDlg.h ../base/SettingsDlg.ui
../base/SplashScreen.cpp ../base/SplashScreen.h ../base/SplashScreen.ui
../base/UpdateChecker.cpp ../base/UpdateChecker.h
../base/VersionComparer.cpp ../base/VersionComparer.h
../base/View.cpp ../base/View.h
../base/pch.h
../base/view_horizontal.ui
../base/view_vertical_single.ui
)

#set(CMAKE_INCLUDE_CURRENT_DIR ON)
include_directories(base ${BOOST_DIR} ${XMLQUERY} )
add_executable(Ipponboard ${SOURCES} )
target_link_libraries(Ipponboard Qt5::Core Qt5::Gui Qt5::XmlPatterns)
