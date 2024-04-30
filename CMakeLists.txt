cmake_minimum_required(VERSION 3.28)

project(Ipponboard-Main VERSION 2.2.0 LANGUAGES CXX)

# Define variables for the required MSVC runtime libraries
if(MSVC)
    include(InstallRequiredSystemLibraries)
    if(NOT DEFINED MSVC_REDIST_DIR)
        message(FATAL_ERROR "MSVC_REDIST_DIR not defined. See InstallRequiredSystemLibraries.cmake in Visual Studio installation.")
    endif()
endif()

# Read variables from config
file(STRINGS "env_cfg.bat" CONFIG_OUTPUT)

# Process each line to extract variables
string(REPLACE "\n" ";" CONFIG_LINES "${CONFIG_OUTPUT}")
foreach(line ${CONFIG_LINES})
    string(REGEX MATCH "^set \"([^=]+)=(.*)\"" _ ${line})
    if (CMAKE_MATCH_1 AND CMAKE_MATCH_2)
      set(CFG_${CMAKE_MATCH_1} ${CMAKE_MATCH_2})
      #message(${CMAKE_MATCH_1}=${CMAKE_MATCH_2})
    endif()
endforeach()

option(USE_QT5 "Use Qt5 for the build" ON)
message(STATUS "Using QT5: ${USE_QT5}")
if(USE_QT5)
	# Check if QTDIR is configured
	if(NOT DEFINED CFG_QTDIR)
		message(FATAL_ERROR "QTDIR not configured in env_cfg.bat")
	endif()
	set(QTDIR ${CFG_QTDIR})
else()
	# Check if QTDIR is configured
	if(NOT DEFINED CFG_QT4DIR)
		message(FATAL_ERROR "QT4DIR not configured in env_cfg.bat")
	endif()
	set(QTDIR ${CFG_QT4DIR})
endif()
message(STATUS "Using: QTDIR = ${QTDIR}")
set(CMAKE_PREFIX_PATH ${QTDIR})
set(CMAKE_LIBRARY_PATH ${QTDIR}/lib)

# Check if BOOST_DIR is configured
if(WIN32)
    if(NOT DEFINED CFG_BOOST_DIR)
        message(FATAL_ERROR "BOOST_DIR not configured in env_cfg.bat")
    endif()
    set(BOOST_DIR ${CFG_BOOST_DIR})
else()
	set(BOOST_DIR /usr/include/boost)
endif()
message(STATUS "Using: BOOST_DIR = ${BOOST_DIR}")

# Check if INNO_DIR is configured
if(WIN32)
    if(NOT DEFINED CFG_INNO_DIR)
        message(FATAL_ERROR "INNO_DIR not configured in env_cfg.bat")
    endif()
    set(INNO_DIR ${CFG_INNO_DIR})
    message(STATUS "Using: INNO_DIR = ${INNO_DIR}")
endif()

set(ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR})
message(STATUS "Using: ROOT_DIR = ${ROOT_DIR}")

# Restrict multi configurations to Release and Debug
set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "" FORCE)

# Configure c++ language support
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add targets
add_subdirectory(test ${CMAKE_BINARY_DIR}/build-Test)
add_subdirectory(base ${CMAKE_BINARY_DIR}/build-Ipponboard)
add_subdirectory(GamepadDemo ${CMAKE_BINARY_DIR}/build-GamepadDemo)
