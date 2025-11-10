cmake_minimum_required(VERSION 3.28)

set(APP_NAME IpponboardNetworkTest)

include(IpponboardCatch2)

find_package(Qt6 COMPONENTS Core Network Widgets Multimedia PrintSupport REQUIRED)

if(WIN32)
    set(OUTPUT_DIR ${ROOT_DIR}/_bin/Test-Release)
    set(OUTPUT_DIR_DEBUG ${ROOT_DIR}/_bin/Test-Debug)
else()
    set(OUTPUT_DIR ${ROOT_DIR}/_bin/Test-${CMAKE_BUILD_TYPE})
    message(STATUS "Using: OUTPUT_DIR = ${OUTPUT_DIR} (${APP_NAME})")
endif()

add_executable(${APP_NAME} TestOnlineVersionChecker_network.cpp)
target_link_libraries(${APP_NAME} PRIVATE IpponboardUi Catch2::Catch2WithMain)
target_include_directories(${APP_NAME} PRIVATE ${BOOST_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/../test)
target_compile_definitions(${APP_NAME}
    PRIVATE IPPONBOARD_TESTDATA_DIR="${CMAKE_CURRENT_SOURCE_DIR}/../test/TestData")

if (MSVC)
    set_target_properties(${APP_NAME} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY_DEBUG ${OUTPUT_DIR_DEBUG}
        RUNTIME_OUTPUT_DIRECTORY_RELEASE ${OUTPUT_DIR}
        COMPILE_FLAGS "/MP"
        CMAKE_SUPPRESS_REGENERATION true
    )
    target_compile_definitions(${APP_NAME}
        PRIVATE _SILENCE_STDEXT_ARR_ITERS_DEPRECATION_WARNING)
else()
    set_target_properties(${APP_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${OUTPUT_DIR})
endif()

add_custom_command(TARGET ${APP_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_CURRENT_SOURCE_DIR}/../test/TestData"
            "$<TARGET_FILE_DIR:${APP_NAME}>/TestData")

if(MSVC)
    target_link_libraries(${APP_NAME} PRIVATE Qt6::Widgets Qt6::Multimedia Qt6::Network Qt6::PrintSupport Qt6::Core)
    ipponboard_add_windeploy(${APP_NAME})
else()
    target_link_libraries(${APP_NAME} PRIVATE Qt6::Network Qt6::Core Qt6::Multimedia Qt6::Widgets Qt6::PrintSupport)
endif()
