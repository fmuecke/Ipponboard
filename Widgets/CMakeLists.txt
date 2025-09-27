cmake_minimum_required(VERSION 3.28)

project(IpponboardWidgets LANGUAGES CXX)

find_package(Qt5 5.15.2 COMPONENTS Widgets REQUIRED)

add_library(IpponboardWidgets STATIC
    ScaledImage.cpp
    ScaledText.cpp
)

set_target_properties(IpponboardWidgets PROPERTIES
    AUTOMOC ON
    AUTOUIC OFF
    AUTORCC OFF
)

target_include_directories(IpponboardWidgets
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(IpponboardWidgets
    PUBLIC
        Qt5::Widgets
)
