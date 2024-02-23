cmake_minimum_required(VERSION 3.6)

project(Ipponboard VERSION 2.0.0 LANGUAGES C CXX) 
set (CMAKE_CXX_STANDARD 17)
set (CMAKE_CXX_STANDARD_REQUIRED ON)
set (CMAKE_CXX_EXTENSIONS OFF)
set (CMAKE_AUTOMOC ON)
set (CMAKE_AUTOUIC ON)

set (CMAKE_PREFIX_PATH $ENV{QTDIR})
set(CMAKE_LIBRARY_PATH $ENV{QTDIR}/lib )
set (XMLQUERY $ENV{QTDIR}/include/QtXmlPatterns)

message(STATUS "Using CMAKE_PREFIX_PATH=${CMAKE_PREFIX_PATH}, CMAKE_LIBRARY_PATH=${CMAKE_LIBRARY_PATH} XMLQUERY=${XMLQUERY}")

find_package(Qt4 4.8.5 REQUIRED QtCore QtGui QtXmlPatterns)

set (BASE_DIR ../..)

#####################################################
# compile Ipponboard
#####################################################
set( SOURCES
${BASE_DIR}/core/Controller.cpp ${BASE_DIR}/core/Controller.h
${BASE_DIR}/core/ControllerConfig.h
${BASE_DIR}/core/Enums.h
${BASE_DIR}/core/Fight.cpp ${BASE_DIR}/core/Fight.h
${BASE_DIR}/core/FightCategory.cpp ${BASE_DIR}/core/FightCategory.h
${BASE_DIR}/core/Fighter.cpp ${BASE_DIR}/core/Fighter.h
${BASE_DIR}/core/Rules.cpp ${BASE_DIR}/core/Rules.h
${BASE_DIR}/core/Score.cpp ${BASE_DIR}/core/Score.h
${BASE_DIR}/core/StateMachine.cpp ${BASE_DIR}/core/StateMachine.h
${BASE_DIR}/core/Tournament.h
${BASE_DIR}/core/TournamentMode.cpp ${BASE_DIR}/core/TournamentMode.h
${BASE_DIR}/core/TournamentModel.cpp ${BASE_DIR}/core/TournamentModel.h
${BASE_DIR}/core/iController.h
${BASE_DIR}/core/iControllerCore.h
${BASE_DIR}/core/iGoldenScoreView.h
${BASE_DIR}/core/iView.h
${BASE_DIR}/util/SimpleCsvFile.hpp
${BASE_DIR}/util/array_helpers.h
${BASE_DIR}/util/json.hpp
${BASE_DIR}/util/jsoncpp/json.cpp
${BASE_DIR}/util/path_helpers.h
${BASE_DIR}/util/screen_helpers.h
${BASE_DIR}/Widgets/Countdown.cpp ${BASE_DIR}/Widgets/Countdown.h ${BASE_DIR}/Widgets/Countdown.ui
${BASE_DIR}/Widgets/ScaledImage.cpp ${BASE_DIR}/Widgets/ScaledImage.h
${BASE_DIR}/Widgets/ScaledText.cpp ${BASE_DIR}/Widgets/ScaledText.h
${BASE_DIR}/base/AddFighterDlg.cpp ${BASE_DIR}/base/AddFighterDlg.h ${BASE_DIR}/base/AddFighterDlg.ui
${BASE_DIR}/base/ClubManager.cpp ${BASE_DIR}/base/ClubManager.h
${BASE_DIR}/base/ClubManagerDlg.cpp ${BASE_DIR}/base/ClubManagerDlg.h ${BASE_DIR}/base/ClubManagerDlg.ui
${BASE_DIR}/base/ClubParser.h
${BASE_DIR}/base/DonationManager.cpp ${BASE_DIR}/base/DonationManager.h
${BASE_DIR}/base/FightCategoryManager.cpp ${BASE_DIR}/base/FightCategoryManager.h
${BASE_DIR}/base/FightCategoryManagerDlg.cpp ${BASE_DIR}/base/FightCategoryManagerDlg.h ${BASE_DIR}/base/FightCategoryManagerDlg.ui
${BASE_DIR}/base/FightCategoryParser.cpp ${BASE_DIR}/base/FightCategoryParser.h
${BASE_DIR}/base/FighterManager.cpp ${BASE_DIR}/base/FighterManager.h
${BASE_DIR}/base/FighterManagerDlg.cpp ${BASE_DIR}/base/FighterManagerDlg.h ${BASE_DIR}/base/FighterManagerDlg.ui
${BASE_DIR}/base/Main.cpp
${BASE_DIR}/base/MainWindow.cpp ${BASE_DIR}/base/MainWindow.h ${BASE_DIR}/base/MainWindow.ui
${BASE_DIR}/base/MainWindowBase.cpp ${BASE_DIR}/base/MainWindowBase.h
${BASE_DIR}/base/MainWindowTeam.cpp ${BASE_DIR}/base/MainWindowTeam.h ${BASE_DIR}/base/MainWindowTeam.ui
${BASE_DIR}/base/ModeManagerDlg.cpp ${BASE_DIR}/base/ModeManagerDlg.h ${BASE_DIR}/base/ModeManagerDlg.ui
${BASE_DIR}/base/ScoreScreen.cpp ${BASE_DIR}/base/ScoreScreen.h ${BASE_DIR}/base/ScoreScreen.ui
${BASE_DIR}/base/SettingsDlg.cpp ${BASE_DIR}/base/SettingsDlg.h ${BASE_DIR}/base/SettingsDlg.ui
${BASE_DIR}/base/SplashScreen.cpp ${BASE_DIR}/base/SplashScreen.h ${BASE_DIR}/base/SplashScreen.ui
${BASE_DIR}/base/UpdateChecker.cpp ${BASE_DIR}/base/UpdateChecker.h
${BASE_DIR}/base/VersionComparer.cpp ${BASE_DIR}/base/VersionComparer.h
${BASE_DIR}/base/View.cpp ${BASE_DIR}/base/View.h
${BASE_DIR}/base/pch.h
${BASE_DIR}/base/view_horizontal.ui
${BASE_DIR}/base/view_vertical_single.ui
${BASE_DIR}/base/qrc_ipponboard.cpp
)

if(WIN32)
	message(STATUS "using: ${CMAKE_VS_WINDOWS_TARGET_PLATFORM_VERSION}")
	set(BOOST_DIR C:/dev/inst/boost_1_82_0)
else()
	set(BOOST_DIR /usr/include/boost)
endif()

include_directories(${BASE_DIR}/Widgets ${BOOST_DIR} ${XMLQUERY} )

#####################################################
# Target Ipponboard: create a binary
#####################################################
if (CMAKE_BUILD_TYPE STREQUAL "Release")
	if (WIN32)
		add_executable(Ipponboard ${SOURCES} ${BASE_DIR}/gamepad/gamepad.h ${BASE_DIR}/base/Ipponboard.rc)
		set_target_properties(Ipponboard PROPERTIES
			RUNTIME_OUTPUT_DIRECTORY "${BASE_DIR}/_build/bin"
			LINK_FLAGS_RELEASE "/SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup"
			CMAKE_SUPPRESS_REGENERATION true	
			COMPILE_FLAGS -D_WITH_GAMEPAD_
		)

		add_custom_command(TARGET Ipponboard POST_BUILD
			COMMAND call "${BASE_DIR}/base/_copy_files.cmd" -release 
		)

		#####################################################
		# Target setup
		#####################################################
		add_custom_target(setup
				COMMAND call "../win/_build_doc.cmd" ${BASE_DIR}
				COMMAND cmd /c "%INNO_DIR%/iscc.exe" /Q /O${BASE_DIR}/_build/bin ${BASE_DIR}/setup/setup.iss 
		)

		target_link_libraries(Ipponboard Qt4::QtCore Qt4::QtGui Qt4::QtXmlPatterns)
	else()
		#if (CMAKE_COMPILER_IS_GNUCC AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS 11.1.0)
		#	message(FATAL_ERROR "Require at least gcc-11.1.0, because c++-17 is needed")
		#endif()

		add_executable(Ipponboard ${SOURCES} ${BASE_DIR}/base/Ipponboard.rc)
		set_target_properties(Ipponboard PROPERTIES
			RUNTIME_OUTPUT_DIRECTORY "${BASE_DIR}/_build/bin/Release"
		)

		target_link_libraries(Ipponboard Qt4::QtCore Qt4::QtGui Qt4::QtXmlPatterns X11 Xrandr)
	endif()
endif()

#####################################################
# Target Ipponboard_Debug: create a debug binary
#####################################################
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
	if (WIN32)
		add_executable(Ipponboard_Debug ${SOURCES} ${BASE_DIR}/gamepad/gamepad.h ${BASE_DIR}/base/Ipponboard.rc)
		set_target_properties(Ipponboard_Debug PROPERTIES
			RUNTIME_OUTPUT_DIRECTORY "${BASE_DIR}/_build/bin"
			OUTPUT_NAME Ipponboard
			#LINK_FLAGS_DEBUG "/SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup"
			#CMAKE_SUPPRESS_REGENERATION true	
		)

		add_custom_command(TARGET Ipponboard_Debug POST_BUILD
			# the following environment variable is needed, if using VStudio GUI (solution)
			COMMAND set QTDIR="c:\\dev\\inst\\qt\\qt-4.8.7-x64-msvc2017"
			COMMAND call "${BASE_DIR}/base/_copy_files.cmd" -debug 
		)

		target_link_libraries(Ipponboard_Debug Qt4::QtCore Qt4::QtGui Qt4::QtXmlPatterns)
	else()
		#if (CMAKE_COMPILER_IS_GNUCC AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS 11.1.0)
		#	message(FATAL_ERROR "Require at least gcc-11.1.0, because c++-17 is needed")
		#endif()

		add_executable(Ipponboard_Debug ${SOURCES} ${BASE_DIR}/base/Ipponboard.rc)
		set_target_properties(Ipponboard_Debug PROPERTIES
			RUNTIME_OUTPUT_DIRECTORY "${BASE_DIR}/_build/bin/Debug"
			OUTPUT_NAME Ipponboard
		)

		target_link_libraries(Ipponboard_Debug Qt4::QtCore Qt4::QtGui Qt4::QtXmlPatterns X11 Xrandr)
	endif()
endif()

# create the qt resource file
#add_custom_command(TARGET ${PROJECT_NAME} PRE_BUILD
#	COMMAND rcc -name ipponboard "%BASE_DIR%/base/ipponboard.qrc" -o "%BASE_DIR%/base/qrc_ipponboard.cpp"
#)

#####################################################
# Target run ALL
#####################################################
#add_custom_target(run ALL
#	COMMAND rcc -name ipponboard "%BASE_DIR%/base/ipponboard.qrc" -o "%BASE_DIR%/base/qrc_ipponboard.cpp"
#)

# compile Testsuites
add_subdirectory(${BASE_DIR}/test/cmake_qt4 ${BASE_DIR}/_build/_build_cmake_qt4/test)
#add_subdirectory(${BASE_DIR}/GamepadDemo/cmake_qt4 ${BASE_DIR}/_build/_build_cmake_qt4/GamepadDemo)
