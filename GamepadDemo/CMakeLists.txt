# This file is referenced by ../CMakeLists.txt
cmake_minimum_required(VERSION 3.28)

# CMake configuration for Gamepad Demo
set(APP_NAME GamepadDemo)

find_package(Qt5 5.15.2 COMPONENTS Core Gui Widgets Gamepad REQUIRED)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

if(WIN32)
    set(OUTPUT_DIR ${ROOT_DIR}/_bin/${APP_NAME}-Release)
    set(OUTPUT_DIR_DEBUG ${ROOT_DIR}/_bin/${APP_NAME}-Debug)
else()
    set(OUTPUT_DIR ${ROOT_DIR}/_bin/${APP_NAME}-${CMAKE_BUILD_TYPE})
endif()

set(SOURCES
    Main.cpp
    GamepadDemo.cpp
    GamepadDemo.h
    GamepadDemo.ui
    res.qrc
    ../Widgets/ScaledImage.cpp
    ../Widgets/ScaledImage.h
    off.png
    on.png
)

if(WIN32)
    add_executable(${APP_NAME} WIN32 ${SOURCES})
    set_target_properties(${APP_NAME} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY_DEBUG ${OUTPUT_DIR_DEBUG}
        RUNTIME_OUTPUT_DIRECTORY_RELEASE ${OUTPUT_DIR}
        CMAKE_SUPPRESS_REGENERATION true
    )
else()
    add_executable(${APP_NAME} ${SOURCES})
    set_target_properties(${APP_NAME} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${OUTPUT_DIR}
    )
endif()

# Copy msvc runtime dlls
if(MSVC)
	add_custom_command(TARGET ${APP_NAME} POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_if_different ${MSVC_REDIST_DIR}$<IF:$<CONFIG:Release>,/x86/Microsoft.VC143.CRT/concrt140.dll,/debug_nonredist/x86/Microsoft.VC143.DebugCRT/concrt140d.dll> "$<TARGET_FILE_DIR:${APP_NAME}>"
		COMMAND ${CMAKE_COMMAND} -E copy_if_different ${MSVC_REDIST_DIR}$<IF:$<CONFIG:Release>,/x86/Microsoft.VC143.CRT/msvcp140.dll,/debug_nonredist/x86/Microsoft.VC143.DebugCRT/msvcp140d.dll> "$<TARGET_FILE_DIR:${APP_NAME}>"
		COMMAND ${CMAKE_COMMAND} -E copy_if_different ${MSVC_REDIST_DIR}$<IF:$<CONFIG:Release>,/x86/Microsoft.VC143.CRT/msvcp140_1.dll,/debug_nonredist/x86/Microsoft.VC143.DebugCRT/msvcp140_1d.dll> "$<TARGET_FILE_DIR:${APP_NAME}>"
		COMMAND ${CMAKE_COMMAND} -E copy_if_different ${MSVC_REDIST_DIR}$<IF:$<CONFIG:Release>,/x86/Microsoft.VC143.CRT/msvcp140_2.dll,/debug_nonredist/x86/Microsoft.VC143.DebugCRT/msvcp140_2d.dll> "$<TARGET_FILE_DIR:${APP_NAME}>"
		COMMAND ${CMAKE_COMMAND} -E copy_if_different ${MSVC_REDIST_DIR}$<IF:$<CONFIG:Release>,/x86/Microsoft.VC143.CRT/msvcp140_atomic_wait.dll,/debug_nonredist/x86/Microsoft.VC143.DebugCRT/msvcp140d_atomic_wait.dll> "$<TARGET_FILE_DIR:${APP_NAME}>"
		COMMAND ${CMAKE_COMMAND} -E copy_if_different ${MSVC_REDIST_DIR}$<IF:$<CONFIG:Release>,/x86/Microsoft.VC143.CRT/msvcp140_codecvt_ids.dll,/debug_nonredist/x86/Microsoft.VC143.DebugCRT/msvcp140d_codecvt_ids.dll> "$<TARGET_FILE_DIR:${APP_NAME}>"
		COMMAND ${CMAKE_COMMAND} -E copy_if_different ${MSVC_REDIST_DIR}$<IF:$<CONFIG:Release>,/x86/Microsoft.VC143.CRT/vcruntime140.dll,/debug_nonredist/x86/Microsoft.VC143.DebugCRT/vcruntime140d.dll> "$<TARGET_FILE_DIR:${APP_NAME}>"
	)
endif()

# Copy required runtime dlls
target_link_libraries(${APP_NAME}
    PRIVATE
        IpponboardGamepad
        Qt5::Core
        Qt5::Gui
        Qt5::Widgets
        Qt5::Gamepad
)

if(WIN32)
    add_custom_command(TARGET ${APP_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different ${QTDIR}/bin/$<IF:$<CONFIG:Release>,Qt5Core.dll,Qt5Cored.dll> "$<TARGET_FILE_DIR:${APP_NAME}>"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different ${QTDIR}/bin/$<IF:$<CONFIG:Release>,Qt5Gui.dll,Qt5Guid.dll> "$<TARGET_FILE_DIR:${APP_NAME}>"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different ${QTDIR}/bin/$<IF:$<CONFIG:Release>,Qt5Widgets.dll,Qt5Widgetsd.dll> "$<TARGET_FILE_DIR:${APP_NAME}>"
        COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:${APP_NAME}>/plugins/platforms"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different ${QTDIR}/plugins/platforms/$<IF:$<CONFIG:Release>,qwindows.dll,qwindowsd.dll> "$<TARGET_FILE_DIR:${APP_NAME}>/plugins/platforms"
    )
endif()
