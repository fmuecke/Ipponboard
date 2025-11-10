cmake_minimum_required(VERSION 3.28)

project(IpponboardCore LANGUAGES CXX)

find_package(Qt6 COMPONENTS Core Multimedia Widgets REQUIRED)

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
target_link_libraries(IpponboardCore PUBLIC Qt6::Core Qt6::Multimedia Qt6::Widgets)
