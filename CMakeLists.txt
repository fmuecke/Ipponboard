cmake_minimum_required(VERSION 3.28)

project(Ipponboard-Main VERSION 2.2.0 LANGUAGES CXX)

# Define a compiler database for clang validation via my IDE
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Erstellt den Symlink sofort während der CMake-Generierung
if(CMAKE_EXPORT_COMPILE_COMMANDS AND NOT WIN32)
    execute_process(
        COMMAND ${CMAKE_COMMAND} -E create_symlink
            "${CMAKE_BINARY_DIR}/compile_commands.json"
            "${CMAKE_SOURCE_DIR}/compile_commands.json"
    )
endif()

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
    if(NOT DEFINED CFG_QTDIR)
        message(FATAL_ERROR "QTDIR not configured in env_cfg.bat")
    endif()
    set(QTDIR ${CFG_QTDIR})
else()
    if(NOT DEFINED CFG_LINUX_QTDIR)
        message(FATAL_ERROR "LINUX_QTDIR not configured in env_cfg.bat")
    endif()
    set(QTDIR ${CFG_LINUX_QTDIR})
endif()
message(STATUS "Using: QTDIR = ${QTDIR}")
set(CMAKE_PREFIX_PATH ${QTDIR})
set(CMAKE_LIBRARY_PATH ${QTDIR}/lib)

set(USE_QT6_DEFAULT OFF)
if(QTDIR MATCHES "qt[\\/-]*6")
    set(USE_QT6_DEFAULT ON)
endif()
option(USE_QT6 "Use Qt6 instead of Qt5" ${USE_QT6_DEFAULT})

# Qt6 is only available as 64-bit, Qt5 only as 32-bit
if(USE_QT6 AND NOT CMAKE_SIZEOF_VOID_P EQUAL 8)
	message(FATAL_ERROR "Qt6 requires a 64-bit build.")
endif()
if(NOT USE_QT6 AND NOT CMAKE_SIZEOF_VOID_P EQUAL 4)
	message(FATAL_ERROR "Qt5 requires a 32-bit build.")
endif()

# Add targets
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

include(${CMAKE_CURRENT_SOURCE_DIR}/FixQtImportedConfigs.cmake)

# Add targets
add_subdirectory(test ${CMAKE_BINARY_DIR}/build-Test)
add_subdirectory(base ${CMAKE_BINARY_DIR}/build-Ipponboard)
if(MSVC)
add_subdirectory(GamepadDemo ${CMAKE_BINARY_DIR}/build-GamepadDemo)
endif()
