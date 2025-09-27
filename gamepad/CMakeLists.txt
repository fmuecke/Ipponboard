cmake_minimum_required(VERSION 3.28)

project(IpponboardGamepad LANGUAGES CXX)

set(SOURCES
    Gamepad.cpp
)

add_library(IpponboardGamepad STATIC ${SOURCES})

target_include_directories(IpponboardGamepad
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

if(WIN32)
    target_link_libraries(IpponboardGamepad PRIVATE winmm)
else()
    find_package(Qt5 5.15.2 COMPONENTS Gamepad REQUIRED)
    target_link_libraries(IpponboardGamepad
        PUBLIC
            Qt5::Gamepad
    )
endif()

set_target_properties(IpponboardGamepad PROPERTIES
    POSITION_INDEPENDENT_CODE ON
)
