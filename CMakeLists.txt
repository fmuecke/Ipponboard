cmake_minimum_required(VERSION 3.28)

project(Ipponboard-Main VERSION 2.2.0 LANGUAGES CXX)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# Define variables for the required MSVC runtime libraries
if(MSVC)
    if(NOT DEFINED MSVC_REDIST_DIR AND DEFINED ENV{VCToolsRedistDir})
        set(MSVC_REDIST_DIR $ENV{VCToolsRedistDir})
    endif()
    include(InstallRequiredSystemLibraries)
    if(NOT DEFINED MSVC_REDIST_DIR)
        message(FATAL_ERROR "MSVC_REDIST_DIR not defined. See InstallRequiredSystemLibraries.cmake in Visual Studio installation.")
    endif()
    include(IpponboardDeploy REQUIRED)
endif()

# Read variables from config
set(IPPONBOARD_ENV_CONFIG "${CMAKE_CURRENT_SOURCE_DIR}/env_cfg.bat")
if(NOT EXISTS "${IPPONBOARD_ENV_CONFIG}")
    message(FATAL_ERROR "Missing configuration file: ${IPPONBOARD_ENV_CONFIG}")
endif()

file(STRINGS "${IPPONBOARD_ENV_CONFIG}" CONFIG_OUTPUT)

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
    if(NOT DEFINED CFG_QTDIR)
        message(FATAL_ERROR "QTDIR not configured in env_cfg.bat")
    endif()
    set(QTDIR "${CFG_QTDIR}")
elseif(APPLE)
    if(NOT DEFINED CFG_MACOS_QTDIR)
        message(FATAL_ERROR "MACOS_QTDIR not configured in env_cfg.bat")
    endif()
    set(QTDIR "${CFG_MACOS_QTDIR}")
elseif(UNIX)
    if(NOT DEFINED CFG_LINUX_QTDIR)
        message(FATAL_ERROR "LINUX_QTDIR not configured in env_cfg.bat")
    endif()
    set(QTDIR "${CFG_LINUX_QTDIR}")
else()
    message(FATAL_ERROR "Unsupported platform")
endif()
message(STATUS "Using: QTDIR = ${QTDIR}")
list(PREPEND CMAKE_PREFIX_PATH "${QTDIR}")
set(CMAKE_LIBRARY_PATH ${QTDIR}/lib)

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

set(PCH_HEADER_STD ${ROOT_DIR}/base/pch_std.h)
set(PCH_HEADER_COMMON ${ROOT_DIR}/base/pch_core.h)
set(PCH_HEADER_CORE ${ROOT_DIR}/base/pch_core.h)
set(PCH_HEADER_WIDGETS ${ROOT_DIR}/base/pch.h)

# Restrict multi configurations to Release and Debug
get_property(IS_MULTI_CONFIG GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)
if(IS_MULTI_CONFIG)
    set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "" FORCE)
endif()

# Configure c++ language support
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
 
if (MSVC)
    add_compile_options(/MP)
endif()

set(CMAKE_AUTOGEN_PARALLEL $ENV{NUMBER_OF_PROCESSORS})

add_compile_definitions(QT_DISABLE_DEPRECATED_BEFORE=0x060900)
# Add targets
add_subdirectory(gamepad ${CMAKE_BINARY_DIR}/build-Gamepad)
add_subdirectory(core ${CMAKE_BINARY_DIR}/build-Core)
add_subdirectory(Widgets ${CMAKE_BINARY_DIR}/build-Widgets)
add_subdirectory(base ${CMAKE_BINARY_DIR}/build-Ipponboard)
add_subdirectory(test ${CMAKE_BINARY_DIR}/build-Test)
add_subdirectory(test_network ${CMAKE_BINARY_DIR}/build-TestNetwork)
