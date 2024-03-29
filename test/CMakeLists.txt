# Created by and for Qt Creator This file was created for editing the project sources only.
# You may attempt to use it for building too, by modifying this file here.
cmake_minimum_required(VERSION 3.5)

option(USE_QT5 "Use Qt5 for the build" OFF)

project(Test-Ipponboard VERSION 2.2.0 LANGUAGES C CXX)
message(STATUS "Using: ${CMAKE_VS_WINDOWS_TARGET_PLATFORM_VERSION} SDK")

if(USE_QT5)
	if(NOT DEFINED ENV{QT5DIR})
		message(FATAL_ERROR "QT5DIR not defined")
	endif()
	set(CMAKE_PREFIX_PATH $ENV{QT5DIR})
	set(CMAKE_LIBRARY_PATH $ENV{QT5DIR}/lib)
	find_package(Qt5 5.12.9 COMPONENTS Widgets XmlPatterns PrintSupport Multimedia REQUIRED)	
else()
	set(CMAKE_PREFIX_PATH $ENV{QT4DIR})
	set(CMAKE_LIBRARY_PATH $ENV{QT4DIR}/lib)
	find_package(Qt4 4.8.7 REQUIRED QtCore)
endif()

set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "" FORCE)

set(CMAKE_AUTOUIC OFF)
set(CMAKE_AUTOMOC OFF)
set(CMAKE_AUTORCC OFF)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/..)
set(OUTPUT_DIR ${ROOT_DIR}/_build/bin-test)
set(OUTPUT_DIR_DEBUG ${ROOT_DIR}/_build/bin-test-debug)

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
	target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE Qt5::Widgets Qt5::XmlPatterns Qt5::PrintSupport Qt5::Multimedia)
else()
	target_link_libraries(${CMAKE_PROJECT_NAME} Qt4::QtCore)
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