cmake_minimum_required(VERSION 3.28)

project(IpponboardGamepad LANGUAGES CXX)

set(SOURCES
    Gamepad.cpp
)

if(WIN32)
    list(APPEND SOURCES GamepadWin.cpp GamepadWin.h)
else()
    list(APPEND SOURCES GamepadLinux.cpp GamepadLinux.h)
endif()

add_library(IpponboardGamepad STATIC ${SOURCES})

target_include_directories(IpponboardGamepad
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

if(WIN32)
    target_link_libraries(IpponboardGamepad PRIVATE winmm)
endif()

set_target_properties(IpponboardGamepad PROPERTIES
    POSITION_INDEPENDENT_CODE ON
)
