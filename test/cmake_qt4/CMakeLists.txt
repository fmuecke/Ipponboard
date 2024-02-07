cmake_minimum_required(VERSION 3.13)

project(IpponboardTest VERSION 2.0.0 LANGUAGES C CXX) 
message(STATUS "using: ${CMAKE_VS_WINDOWS_TARGET_PLATFORM_VERSION}")

find_package(Qt4 4.8.5 REQUIRED QtCore)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_PREFIX_PATH "$ENV{QTDIR}")

set(CMAKE_AUTOMOC OFF)
set(CMAKE_AUTOUIC OFF)

# compile IpponboardTest
set( SOURCES
../IpponboardTest.cpp
../TestJson.cpp
../TestScore.cpp
../TestFight.cpp
../TestTournamentMode.cpp
../TestVersionComparer.cpp
../TestRules.cpp
)

# Link the binary
add_executable(IpponboardTest ${SOURCES} )
target_link_libraries(IpponboardTest Qt4::QtCore)

# Add other data to the destination directory
#file(COPY ${TESTDATA} DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/Debug)
add_custom_command(TARGET ${PROJECT_NAME} 
					POST_BUILD
					COMMAND ${CMAKE_COMMAND} -E copy_directory ../TestData $<TARGET_FILE_DIR:${PROJECT_NAME}>/TestData)
