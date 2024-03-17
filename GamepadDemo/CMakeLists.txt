# Created by and for Qt Creator This file was created for editing the project sources only.
# You may attempt to use it for building too, by modifying this file here.
cmake_minimum_required(VERSION 3.5)

project(GamepadDemo VERSION 2.0.0  LANGUAGES C CXX)
message(STATUS "using: ${CMAKE_VS_WINDOWS_TARGET_PLATFORM_VERSION}")

find_package(Qt4 4.8.7 REQUIRED QtCore QtGui)
if($ENV{QTDIR} STREQUAL "")
    message(FATAL_ERROR "QTDIR not defined")
endif()

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_PREFIX_PATH "$ENV{QTDIR}")
set(CMAKE_LIBRARY_PATH "$ENV{QTDIR}/lib")

set(ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/..)
set(OUTPUT_DIR ${ROOT_DIR}/_build/bin)

set(SOURCES
    Main.cpp
    GamepadDemo.cpp GamepadDemo.h
    ../gamepad/gamepad.h
    ../Widgets/ScaledImage.cpp ../Widgets/ScaledImage.h
    GamepadDemo.ui
    res.qrc off.png on.png
)

if (WIN32)
    add_executable(${CMAKE_PROJECT_NAME} WIN32 ${SOURCES})
    set_target_properties(${CMAKE_PROJECT_NAME} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY_DEBUG ${OUTPUT_DIR}
		RUNTIME_OUTPUT_DIRECTORY_RELEASE ${OUTPUT_DIR}
        COMPILE_FLAGS "/Zc:wchar_t- /MP"
        CMAKE_SUPPRESS_REGENERATION true
    )
else()
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${OUTPUT_DIR})
    add_executable(${CMAKE_PROJECT_NAME} ${SOURCES})
endif()

target_link_libraries(${CMAKE_PROJECT_NAME} Qt4::QtCore Qt4::QtGui)


# Copy Qt dlls to the output directory
# TODO: fix copying of Qt dlls according to configuration (does not work RELIABLY with multi config generators)
#message("current build type: " ${CMAKE_BUILD_TYPE})
#message("current configuration: " ${CONFIG})
#if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_custom_command(TARGET ${CMAKE_PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E echo "copying Qt debug libraries to ${OUTPUT_DIR}"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different $ENV{QTDIR}/QtCored4.dll $ENV{QTDIR}/QtGuid4.dll ${OUTPUT_DIR}
    )
#else()
    add_custom_command(TARGET ${CMAKE_PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E echo "copying Qt release libraries to ${OUTPUT_DIR}"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different $ENV{QTDIR}/QtCore4.dll $ENV{QTDIR}/QtGui4.dll ${OUTPUT_DIR}
    )
#endif()


