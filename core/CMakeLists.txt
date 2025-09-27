cmake_minimum_required(VERSION 3.28)

project(IpponboardCore LANGUAGES CXX)

find_package(Qt5 5.15.2 COMPONENTS Core XmlPatterns Multimedia Widgets REQUIRED)

set(CORE_SOURCES
    Controller.cpp
    Fight.cpp
    FightCategory.cpp
    Fighter.cpp
    Rules.cpp
    TimerService.cpp
    TournamentNavigator.cpp
    TournamentRepository.cpp
    Score.cpp
    StateMachine.cpp
    TournamentModel.cpp
    TournamentMode.cpp
)

add_library(IpponboardCore STATIC ${CORE_SOURCES})
set_target_properties(IpponboardCore PROPERTIES
    AUTOMOC ON
)

target_include_directories(IpponboardCore PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${BOOST_DIR})
target_link_libraries(IpponboardCore PUBLIC Qt5::Core Qt5::XmlPatterns Qt5::Multimedia Qt5::Widgets)
