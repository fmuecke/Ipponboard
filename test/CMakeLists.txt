# This file is referenced by ../CMakeLists.txt
cmake_minimum_required(VERSION 3.28)

# CMake configuration for Ipponboard tests
set(APP_NAME IpponboardTest)

find_package(Qt6 COMPONENTS Core Network Widgets Multimedia PrintSupport REQUIRED)

if(LINUX)
    add_compile_options(-fPIC -fpermissive)
endif()

if(WIN32)
    set(OUTPUT_DIR ${ROOT_DIR}/_bin/Test-Release)
    set(OUTPUT_DIR_DEBUG ${ROOT_DIR}/_bin/Test-Debug)
else()
    set(OUTPUT_DIR ${ROOT_DIR}/_bin/Test-${CMAKE_BUILD_TYPE})
    message(STATUS "Using: OUTPUT_DIR = ${OUTPUT_DIR} (${APP_NAME})")
endif()

set(SOURCES
    IpponboardTest.cpp
    TestFight.cpp
    TestJson.cpp
    TestController.cpp
    TestRules.cpp
    TestScore.cpp
    TestTournamentMode.cpp
    TestInputBindingResolver.cpp
    TestRawInputCapture.cpp
    TestGamepadSectionMapper.cpp
    TestGamepadSections.cpp
    TestOnlineVersionChecker.cpp
    Test-path_helpers.cpp
    TestVersionComparer.cpp
    TestStateMachine.cpp
    TestTournamentModel.cpp
    TestView.cpp
    ../util/catch2/catch_amalgamated.cpp
)

set_property(SOURCE IpponboardTest.cpp PROPERTY SKIP_AUTOMOC ON)
add_executable(${APP_NAME} ${SOURCES})
target_link_libraries(${APP_NAME} PRIVATE IpponboardUi)
target_include_directories(${APP_NAME} PRIVATE ${BOOST_DIR})
target_compile_definitions(${APP_NAME} PRIVATE IPPON_TEST_DATA_DIR="${CMAKE_CURRENT_SOURCE_DIR}/TestData")

if (MSVC)
    set_target_properties(${APP_NAME} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY_DEBUG ${OUTPUT_DIR_DEBUG}
        RUNTIME_OUTPUT_DIRECTORY_RELEASE ${OUTPUT_DIR}
        COMPILE_FLAGS "/MP"
        CMAKE_SUPPRESS_REGENERATION true
    )
    target_compile_definitions(${APP_NAME} PRIVATE _SILENCE_STDEXT_ARR_ITERS_DEPRECATION_WARNING)    
else()
    set_target_properties(${APP_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${OUTPUT_DIR})
endif()

# Copy testdata
add_custom_command(TARGET ${APP_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/TestData" "$<TARGET_FILE_DIR:${APP_NAME}>/TestData")

if(MSVC)
    # Copy msvc runtime dlls
    add_custom_command(TARGET ${APP_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different ${MSVC_REDIST_DIR}$<IF:$<CONFIG:Release>,/x64/Microsoft.VC143.CRT/concrt140.dll,/debug_nonredist/x64/Microsoft.VC143.DebugCRT/concrt140d.dll> "$<TARGET_FILE_DIR:${APP_NAME}>"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different ${MSVC_REDIST_DIR}$<IF:$<CONFIG:Release>,/x64/Microsoft.VC143.CRT/msvcp140.dll,/debug_nonredist/x64/Microsoft.VC143.DebugCRT/msvcp140d.dll> "$<TARGET_FILE_DIR:${APP_NAME}>"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different ${MSVC_REDIST_DIR}$<IF:$<CONFIG:Release>,/x64/Microsoft.VC143.CRT/msvcp140_1.dll,/debug_nonredist/x64/Microsoft.VC143.DebugCRT/msvcp140_1d.dll> "$<TARGET_FILE_DIR:${APP_NAME}>"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different ${MSVC_REDIST_DIR}$<IF:$<CONFIG:Release>,/x64/Microsoft.VC143.CRT/msvcp140_2.dll,/debug_nonredist/x64/Microsoft.VC143.DebugCRT/msvcp140_2d.dll> "$<TARGET_FILE_DIR:${APP_NAME}>"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different ${MSVC_REDIST_DIR}$<IF:$<CONFIG:Release>,/x64/Microsoft.VC143.CRT/msvcp140_atomic_wait.dll,/debug_nonredist/x64/Microsoft.VC143.DebugCRT/msvcp140d_atomic_wait.dll> "$<TARGET_FILE_DIR:${APP_NAME}>"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different ${MSVC_REDIST_DIR}$<IF:$<CONFIG:Release>,/x64/Microsoft.VC143.CRT/msvcp140_codecvt_ids.dll,/debug_nonredist/x64/Microsoft.VC143.DebugCRT/msvcp140d_codecvt_ids.dll> "$<TARGET_FILE_DIR:${APP_NAME}>"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different ${MSVC_REDIST_DIR}$<IF:$<CONFIG:Release>,/x64/Microsoft.VC143.CRT/vcruntime140.dll,/debug_nonredist/x64/Microsoft.VC143.DebugCRT/vcruntime140d.dll> "$<TARGET_FILE_DIR:${APP_NAME}>"
    )

    # Deploy Qt runtime
    target_link_libraries(${APP_NAME} PRIVATE Qt6::Widgets Qt6::Multimedia Qt6::Network Qt6::PrintSupport Qt6::Core)
    ipponboard_add_windeploy(${APP_NAME})
else()
    target_link_libraries(${APP_NAME} PRIVATE Qt6::Network Qt6::Core Qt6::Multimedia Qt6::Widgets Qt6::PrintSupport)
#    find_package(OpenSSL REQUIRED)
#    target_link_libraries(${APP_NAME} PRIVATE OpenSSL::SSL OpenSSL::Crypto)
endif()
