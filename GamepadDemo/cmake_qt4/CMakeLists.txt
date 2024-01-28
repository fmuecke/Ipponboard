cmake_minimum_required(VERSION 3.13)

project(GamepadDemo VERSION 2.0.0 LANGUAGES C CXX) 
message(STATUS "using: ${CMAKE_VS_WINDOWS_TARGET_PLATFORM_VERSION}")

find_package(Qt4 4.8.7 REQUIRED QtCore QtGui)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_PREFIX_PATH "$ENV{QTDIR}")

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

set( SOURCES
../Main.cpp
../GamepadDemo.cpp ../GamepadDemo.h ../GamepadDemo.ui
../../Widgets/ScaledImage.cpp ../../Widgets/ScaledImage.h
)

add_executable(GamepadDemo ${SOURCES} )
target_link_libraries(GamepadDemo Qt4::QtCore Qt4::QtGui)
