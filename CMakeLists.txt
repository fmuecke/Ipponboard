cmake_minimum_required(VERSION 3.28)

project(Ipponboard-Main VERSION 2.2.0 LANGUAGES CXX)

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

# Only do these if this is the main project, and not if it is included through add_subdirectory
#if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
#endif()

option(USE_QT5 "Use Qt5 for the build" ON)
message(STATUS "Using QT5: ${USE_QT5}")

# Check if QTDIR is configured
if(NOT DEFINED CFG_QTDIR)
    message(FATAL_ERROR "QTDIR not defined")
endif()
set(QTDIR ${CFG_QTDIR})
message(STATUS "Using: QTDIR = ${CFG_QTDIR}")
set(CMAKE_PREFIX_PATH ${CFG_QTDIR})
set(CMAKE_LIBRARY_PATH ${CFG_QTDIR}/lib)

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
