cmake_minimum_required(VERSION 3.28)
project(GamepadDemo VERSION 2.2.0 LANGUAGES C CXX)

option(USE_QT5 "Use Qt5 for the build" OFF)
if(NOT DEFINED ENV{QTDIR})
        message(FATAL_ERROR "QTDIR not defined")
endif()
message(STATUS "Using: QTDIR = $ENV{QTDIR}")

if(USE_QT5)
    set(CMAKE_PREFIX_PATH $ENV{QTDIR})
    set(CMAKE_LIBRARY_PATH $ENV{QTDIR}/lib)
    find_package(Qt5 5.12.9 COMPONENTS Core Gui Widgets REQUIRED)
else()
    set(CMAKE_PREFIX_PATH $ENV{QTDIR})
    set(CMAKE_LIBRARY_PATH $ENV{QTDIR}/lib)
    find_package(Qt4 4.8.7 REQUIRED QtCore QtGui)
endif()

set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "" FORCE)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/..)
set(OUTPUT_DIR ${ROOT_DIR}/_bin/GamepadDemo-Release)
set(OUTPUT_DIR_DEBUG ${ROOT_DIR}/_bin/GamepadDemo-Debug)

set(SOURCES
    Main.cpp
    GamepadDemo.cpp
	GamepadDemo.h
    ../gamepad/gamepad.h
    ../Widgets/ScaledImage.cpp
    ../Widgets/ScaledImage.h
    GamepadDemo.ui
    res.qrc
    off.png
    on.png
)

add_executable(${CMAKE_PROJECT_NAME} WIN32 ${SOURCES})
set_target_properties(${CMAKE_PROJECT_NAME} PROPERTIES
	RUNTIME_OUTPUT_DIRECTORY_DEBUG ${OUTPUT_DIR_DEBUG}
	RUNTIME_OUTPUT_DIRECTORY_RELEASE ${OUTPUT_DIR}
	COMPILE_FLAGS "/Zc:wchar_t- /MP"
	CMAKE_SUPPRESS_REGENERATION true
)

if(USE_QT5)
    target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE Qt5::Widgets)
else()
    target_link_libraries(${CMAKE_PROJECT_NAME} Qt4::QtCore Qt4::QtGui)
endif()

# copy Qt runtime dlls
if(USE_QT5)
    add_custom_command(TARGET ${CMAKE_PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different $ENV{QTDIR}/bin/$<IF:$<CONFIG:Release>,Qt5Core.dll,Qt5Cored.dll> "$<TARGET_FILE_DIR:${CMAKE_PROJECT_NAME}>"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different $ENV{QTDIR}/bin/$<IF:$<CONFIG:Release>,Qt5Gui.dll,Qt5Guid.dll> "$<TARGET_FILE_DIR:${CMAKE_PROJECT_NAME}>"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different $ENV{QTDIR}/bin/$<IF:$<CONFIG:Release>,Qt5Widgets.dll,Qt5Widgetsd.dll> "$<TARGET_FILE_DIR:${CMAKE_PROJECT_NAME}>"
		COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:${CMAKE_PROJECT_NAME}>/plugins/platforms"
		COMMAND ${CMAKE_COMMAND} -E copy_if_different $ENV{QT5DIR}/plugins/platforms/$<IF:$<CONFIG:Release>,qwindows.dll,qwindowsd.dll> "$<TARGET_FILE_DIR:${CMAKE_PROJECT_NAME}>/plugins/platforms"
    )
else()
    add_custom_command(TARGET ${CMAKE_PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different $ENV{QTDIR}/bin/$<IF:$<CONFIG:Release>,QtCore4.dll,QtCored4.dll> "$<TARGET_FILE_DIR:${CMAKE_PROJECT_NAME}>"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different $ENV{QTDIR}/bin/$<IF:$<CONFIG:Release>,QtGui4.dll,QtGuid4.dll> "$<TARGET_FILE_DIR:${CMAKE_PROJECT_NAME}>"
    )
endif()
