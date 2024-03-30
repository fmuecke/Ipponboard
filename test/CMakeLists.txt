# Created by and for Qt Creator This file was created for editing the project sources only.
# You may attempt to use it for building too, by modifying this file here.
cmake_minimum_required(VERSION 3.28)

option(USE_QT5 "Use Qt5 for the build" OFF)

project(Test-Ipponboard VERSION 2.2.0 LANGUAGES C CXX)

if(NOT DEFINED ENV{QTDIR})
	message(FATAL_ERROR "QTDIR not defined")
endif()
message(STATUS "Using: QTDIR = $ENV{QTDIR}")
set(CMAKE_PREFIX_PATH $ENV{QTDIR})
set(CMAKE_LIBRARY_PATH $ENV{QTDIR}/lib)

if(USE_QT5)
	find_package(Qt5 5.12.9 COMPONENTS Widgets XmlPatterns PrintSupport Multimedia REQUIRED)	
else()
	find_package(Qt4 4.8.7 REQUIRED QtCore)
endif()

set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "" FORCE)

set(CMAKE_AUTOUIC OFF)
set(CMAKE_AUTOMOC OFF)
set(CMAKE_AUTORCC OFF)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/..)
set(OUTPUT_DIR ${ROOT_DIR}/_bin/test-Release)
set(OUTPUT_DIR_DEBUG ${ROOT_DIR}/_bin/test-Debug)

set(SOURCES
	IpponboardTest.cpp
	TestFight.cpp
	TestJson.cpp
	TestRules.cpp
	TestScore.cpp
	TestTournamentMode.cpp
	TestVersionComparer.cpp
)

add_executable(${CMAKE_PROJECT_NAME} ${SOURCES})

if(USE_QT5)
	target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE Qt5::Widgets)
else()
	target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE Qt4::QtCore)
endif()

if (WIN32)
    set_target_properties(${CMAKE_PROJECT_NAME} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY_DEBUG ${OUTPUT_DIR_DEBUG}
        RUNTIME_OUTPUT_DIRECTORY_RELEASE ${OUTPUT_DIR}
        COMPILE_FLAGS "/Zc:wchar_t- /MP"
        CMAKE_SUPPRESS_REGENERATION true
    )
else()
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${OUTPUT_DIR})
endif()

# copy testdata
add_custom_command(TARGET ${CMAKE_PROJECT_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/TestData" "$<TARGET_FILE_DIR:${CMAKE_PROJECT_NAME}>/TestData")

# copy Qt runtime dlls
if(USE_QT5)
	add_custom_command(TARGET ${CMAKE_PROJECT_NAME} POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_if_different $ENV{QTDIR}/bin/$<IF:$<CONFIG:Release>,Qt5Core.dll,Qt5Cored.dll> "$<TARGET_FILE_DIR:${CMAKE_PROJECT_NAME}>"
	)
else()
	add_custom_command(TARGET ${CMAKE_PROJECT_NAME} POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_if_different $ENV{QTDIR}/bin/$<IF:$<CONFIG:Release>,QtCore4.dll,QtCored4.dll> "$<TARGET_FILE_DIR:${CMAKE_PROJECT_NAME}>"
	)
endif()