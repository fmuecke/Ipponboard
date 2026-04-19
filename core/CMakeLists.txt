cmake_minimum_required(VERSION 3.28)

project(IpponboardCore LANGUAGES CXX)

find_package(Qt6 COMPONENTS Core Multimedia Widgets REQUIRED)

set(CORE_SOURCES
    Controller.cpp
    Contest.cpp
    FightCategory.cpp
    Athlete.cpp
    Rules.cpp
    TimerService.cpp
    CompetitionNavigator.cpp
    CompetitionRepository.cpp
    Score.cpp
    StateMachine.cpp
    CompetitionModel.cpp
    CompetitionMode.cpp
)

add_library(IpponboardCore STATIC ${CORE_SOURCES})
set_target_properties(IpponboardCore PROPERTIES
    AUTOMOC ON
)

target_include_directories(IpponboardCore PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(IpponboardCore PUBLIC Qt6::Core Qt6::Multimedia Qt6::Widgets)

target_precompile_headers(IpponboardCore
    PRIVATE "$<$<COMPILE_LANGUAGE:CXX>:${PCH_HEADER_CORE}>")
