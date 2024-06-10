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

# Check if QTDIR is configured
if(MSVC)
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
else()
    set(USE_QT5 ON)

    #find_package(Qt5 COMPONENTS Core REQUIRED)
    #get_target_property(QT_CORE_LIBRARY Qt5::Core LOCATION)
    #get_filename_component(QT_BASE_DIR "${QT_CORE_LIBRARY}" DIRECTORY)
    #get_filename_component(QT_BASE_DIR "${QT_BASE_DIR}" DIRECTORY)
    #set(QTDIR ${QT_BASE_DIR})

    # Check if QTDIR is configured
    if(NOT DEFINED CFG_LINUX_QTDIR)
        message(FATAL_ERROR "LINUX_QTDIR not configured in env_cfg.bat")
    endif()
    set(QTDIR ${CFG_LINUX_QTDIR})
endif()
message(STATUS "Using: QTDIR = ${QTDIR}")
set(CMAKE_PREFIX_PATH ${QTDIR})
set(CMAKE_LIBRARY_PATH ${QTDIR}/lib)

# Check if BOOST_DIR is configured
if(MSVC)
    if(NOT DEFINED CFG_BOOST_DIR)
        message(FATAL_ERROR "BOOST_DIR not configured in env_cfg.bat")
    endif()
    set(BOOST_DIR ${CFG_BOOST_DIR})
else()
    if(NOT DEFINED BOOST_DIR)
	set(BOOST_DIR ${CFG_LINUX_BOOST_DIR})
	endif()
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
set(CMAKE_CXX_EXTENSIONS OFF)

# Add targets
add_subdirectory(test ${CMAKE_BINARY_DIR}/build-Test)
add_subdirectory(base ${CMAKE_BINARY_DIR}/build-Ipponboard)
if(MSVC)
add_subdirectory(GamepadDemo ${CMAKE_BINARY_DIR}/build-GamepadDemo)
endif()
