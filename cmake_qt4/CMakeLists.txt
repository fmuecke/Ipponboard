cmake_minimum_required(VERSION 3.13)

project(Ipponboard VERSION 2.0.0 LANGUAGES C CXX) 
message(STATUS "using: ${CMAKE_VS_WINDOWS_TARGET_PLATFORM_VERSION}")

find_package(Qt4 4.8.7 REQUIRED QtCore QtGui QtXmlPatterns)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_PREFIX_PATH "$ENV{QTDIR}")
set (BOOST_DIR C:/dev/inst/boost_1_82_0)
set (XMLQUERY $ENV{QTDIR}/include/QtXmlPatterns)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

# compile Ipponboard
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

include_directories(base ${BOOST_DIR} ${XMLQUERY} )
#set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ../bin)

# create a debug binary
add_executable(Ipponboard_Debug ${SOURCES} )
set_target_properties(Ipponboard_Debug PROPERTIES
    OUTPUT_NAME Ipponboard_Debug
    DEBUG_POSTFIX _d
	RUNTIME_OUTPUT_DIRECTORY "../bin"
)
target_link_libraries(Ipponboard_Debug Qt4::QtCore Qt4::QtGui Qt4::QtXmlPatterns)

# create a binary
add_executable(Ipponboard ${SOURCES} ../base/Ipponboard.rc)
set_target_properties(Ipponboard PROPERTIES
	RUNTIME_OUTPUT_DIRECTORY "../bin"
)
target_link_libraries(Ipponboard Qt4::QtCore Qt4::QtGui Qt4::QtXmlPatterns)

# compile Testsuites
add_subdirectory(../test/cmake_qt4 ../test/_build_cmake)
add_subdirectory(../GamepadDemo/cmake_qt4 ../GamepadDemo/_build_cmake)

if(WIN32)
	add_custom_command(TARGET Ipponboard PRE_BUILD
		COMMAND call "../base/create_versioninfo.cmd" )

	add_custom_command(TARGET Ipponboard POST_BUILD
		COMMAND call "../base/copy_files.cmd" -release )

	add_custom_command(TARGET Ipponboard_Debug POST_BUILD
		COMMAND call "../base/copy_files.cmd" -debug )

	add_custom_target(make-setup
		COMMAND call "../doc/build_doc.cmd" 
		COMMAND cmd /c "%INNO_DIR%/iscc.exe" /Q /O../bin ../setup/setup.iss )
endif (WIN32)
