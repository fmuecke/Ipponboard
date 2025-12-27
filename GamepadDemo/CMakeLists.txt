# This file is referenced by ../CMakeLists.txt
cmake_minimum_required(VERSION 3.28)

# CMake configuration for Gamepad Demo
set(APP_NAME GamepadDemo)

find_package(Qt5 5.12.9 COMPONENTS Core Gui Widgets REQUIRED)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(OUTPUT_DIR ${ROOT_DIR}/_bin/${APP_NAME}-Release)
set(OUTPUT_DIR_DEBUG ${ROOT_DIR}/_bin/${APP_NAME}-Debug)
#message(STATUS "Using: OUTPUT_DIR = ${OUTPUT_DIR} (${APP_NAME})")

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
    COMPILE_FLAGS "/MP /W4"
    CMAKE_SUPPRESS_REGENERATION true
)

# Copy msvc runtime dlls
if(MSVC)
	add_custom_command(TARGET ${APP_NAME} POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_if_different ${MSVC_REDIST_DIR}$<IF:$<CONFIG:Release>,/x86/Microsoft.VC145.CRT/concrt140.dll,/debug_nonredist/x86/Microsoft.VC143.DebugCRT/concrt140d.dll> "$<TARGET_FILE_DIR:${APP_NAME}>"
		COMMAND ${CMAKE_COMMAND} -E copy_if_different ${MSVC_REDIST_DIR}$<IF:$<CONFIG:Release>,/x86/Microsoft.VC145.CRT/msvcp140.dll,/debug_nonredist/x86/Microsoft.VC143.DebugCRT/msvcp140d.dll> "$<TARGET_FILE_DIR:${APP_NAME}>"
		COMMAND ${CMAKE_COMMAND} -E copy_if_different ${MSVC_REDIST_DIR}$<IF:$<CONFIG:Release>,/x86/Microsoft.VC145.CRT/msvcp140_1.dll,/debug_nonredist/x86/Microsoft.VC143.DebugCRT/msvcp140_1d.dll> "$<TARGET_FILE_DIR:${APP_NAME}>"
		COMMAND ${CMAKE_COMMAND} -E copy_if_different ${MSVC_REDIST_DIR}$<IF:$<CONFIG:Release>,/x86/Microsoft.VC145.CRT/msvcp140_2.dll,/debug_nonredist/x86/Microsoft.VC143.DebugCRT/msvcp140_2d.dll> "$<TARGET_FILE_DIR:${APP_NAME}>"
		COMMAND ${CMAKE_COMMAND} -E copy_if_different ${MSVC_REDIST_DIR}$<IF:$<CONFIG:Release>,/x86/Microsoft.VC145.CRT/msvcp140_atomic_wait.dll,/debug_nonredist/x86/Microsoft.VC143.DebugCRT/msvcp140d_atomic_wait.dll> "$<TARGET_FILE_DIR:${APP_NAME}>"
		COMMAND ${CMAKE_COMMAND} -E copy_if_different ${MSVC_REDIST_DIR}$<IF:$<CONFIG:Release>,/x86/Microsoft.VC145.CRT/msvcp140_codecvt_ids.dll,/debug_nonredist/x86/Microsoft.VC143.DebugCRT/msvcp140d_codecvt_ids.dll> "$<TARGET_FILE_DIR:${APP_NAME}>"
		COMMAND ${CMAKE_COMMAND} -E copy_if_different ${MSVC_REDIST_DIR}$<IF:$<CONFIG:Release>,/x86/Microsoft.VC145.CRT/vcruntime140.dll,/debug_nonredist/x86/Microsoft.VC143.DebugCRT/vcruntime140d.dll> "$<TARGET_FILE_DIR:${APP_NAME}>"
	)
endif()

# Copy required runtime dlls
target_link_libraries(${APP_NAME} PRIVATE Qt5::Widgets)
add_custom_command(TARGET ${APP_NAME} POST_BUILD
    # Copy Qt5 runtime dlls
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${QTDIR}/bin/$<IF:$<CONFIG:Release>,Qt5Core.dll,Qt5Cored.dll> "$<TARGET_FILE_DIR:${APP_NAME}>"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${QTDIR}/bin/$<IF:$<CONFIG:Release>,Qt5Gui.dll,Qt5Guid.dll> "$<TARGET_FILE_DIR:${APP_NAME}>"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${QTDIR}/bin/$<IF:$<CONFIG:Release>,Qt5Widgets.dll,Qt5Widgetsd.dll> "$<TARGET_FILE_DIR:${APP_NAME}>"
    COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:${APP_NAME}>/plugins/platforms"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${QTDIR}/plugins/platforms/$<IF:$<CONFIG:Release>,qwindows.dll,qwindowsd.dll> "$<TARGET_FILE_DIR:${APP_NAME}>/plugins/platforms"
)