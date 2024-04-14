# This file is referenced by ../CMakeLists.txt
cmake_minimum_required(VERSION 3.28)

# CMake configuration for Gamepad Demo
set(APP_NAME GamepadDemo)

if(USE_QT5)
    find_package(Qt5 5.12.9 COMPONENTS Core Gui Widgets REQUIRED)
else()
    find_package(Qt4 4.8.7 REQUIRED QtCore QtGui)
endif()

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

if(USE_QT5)
    set(OUTPUT_DIR ${ROOT_DIR}/_bin/${APP_NAME}-Release)
    set(OUTPUT_DIR_DEBUG ${ROOT_DIR}/_bin/${APP_NAME}-Debug)
else()
    set(OUTPUT_DIR ${ROOT_DIR}/_bin/${APP_NAME}-Release-Qt4)
    set(OUTPUT_DIR_DEBUG ${ROOT_DIR}/_bin/${APP_NAME}-Debug-Qt4)
endif()
message(STATUS "Using: OUTPUT_DIR = ${OUTPUT_DIR} (${APP_NAME})")

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

add_executable(${APP_NAME} WIN32 ${SOURCES})
set_target_properties(${APP_NAME} PROPERTIES
	RUNTIME_OUTPUT_DIRECTORY_DEBUG ${OUTPUT_DIR_DEBUG}
	RUNTIME_OUTPUT_DIRECTORY_RELEASE ${OUTPUT_DIR}
	COMPILE_FLAGS "/Zc:wchar_t- /MP"
	CMAKE_SUPPRESS_REGENERATION true
)

# Copy required Qt runtime dlls
if(USE_QT5)
    target_link_libraries(${APP_NAME} PRIVATE Qt5::Widgets)
    add_custom_command(TARGET ${APP_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different ${QTDIR}/bin/$<IF:$<CONFIG:Release>,Qt5Core.dll,Qt5Cored.dll> "$<TARGET_FILE_DIR:${APP_NAME}>"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different ${QTDIR}/bin/$<IF:$<CONFIG:Release>,Qt5Gui.dll,Qt5Guid.dll> "$<TARGET_FILE_DIR:${APP_NAME}>"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different ${QTDIR}/bin/$<IF:$<CONFIG:Release>,Qt5Widgets.dll,Qt5Widgetsd.dll> "$<TARGET_FILE_DIR:${APP_NAME}>"
		COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:${APP_NAME}>/plugins/platforms"
		COMMAND ${CMAKE_COMMAND} -E copy_if_different ${QTDIR}/plugins/platforms/$<IF:$<CONFIG:Release>,qwindows.dll,qwindowsd.dll> "$<TARGET_FILE_DIR:${APP_NAME}>/plugins/platforms"
    )
else()
    target_link_libraries(${APP_NAME} Qt4::QtCore Qt4::QtGui)
    add_custom_command(TARGET ${APP_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different ${QTDIR}/bin/$<IF:$<CONFIG:Release>,QtCore4.dll,QtCored4.dll> "$<TARGET_FILE_DIR:${APP_NAME}>"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different ${QTDIR}/bin/$<IF:$<CONFIG:Release>,QtGui4.dll,QtGuid4.dll> "$<TARGET_FILE_DIR:${APP_NAME}>"
    )
endif()
