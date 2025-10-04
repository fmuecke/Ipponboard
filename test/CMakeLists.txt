# This file is referenced by ../CMakeLists.txt
cmake_minimum_required(VERSION 3.28)

# CMake configuration for Ipponboard tests
set(APP_NAME IpponboardTest)

find_package(Qt5 5.15.2 COMPONENTS Core XmlPatterns Network Widgets Multimedia REQUIRED)

if(LINUX)
    # common include directories
    #include_directories(${Qt5Widgets_INCLUDE_DIRS})
    #include_directories(${Qt5PrintSupport_INCLUDE_DIRS})
    include_directories(${Qt5XmlPatterns_INCLUDE_DIRS})
    #include_directories(${Qt5Multimedia_INCLUDE_DIRS})
    add_compile_options(-fPIC -fpermissive)
else()
    include_directories(${Qt5XmlPatterns_INCLUDE_DIRS})
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
    TestOnlineVersionChecker.cpp
    Test-path_helpers.cpp
    TestVersionComparer.cpp
    TestStateMachine.cpp
    TestTournamentModel.cpp
    TestView.cpp
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
        COMMAND ${CMAKE_COMMAND} -E copy_if_different ${MSVC_REDIST_DIR}$<IF:$<CONFIG:Release>,/x86/Microsoft.VC143.CRT/concrt140.dll,/debug_nonredist/x86/Microsoft.VC143.DebugCRT/concrt140d.dll> "$<TARGET_FILE_DIR:${APP_NAME}>"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different ${MSVC_REDIST_DIR}$<IF:$<CONFIG:Release>,/x86/Microsoft.VC143.CRT/msvcp140.dll,/debug_nonredist/x86/Microsoft.VC143.DebugCRT/msvcp140d.dll> "$<TARGET_FILE_DIR:${APP_NAME}>"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different ${MSVC_REDIST_DIR}$<IF:$<CONFIG:Release>,/x86/Microsoft.VC143.CRT/msvcp140_1.dll,/debug_nonredist/x86/Microsoft.VC143.DebugCRT/msvcp140_1d.dll> "$<TARGET_FILE_DIR:${APP_NAME}>"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different ${MSVC_REDIST_DIR}$<IF:$<CONFIG:Release>,/x86/Microsoft.VC143.CRT/msvcp140_2.dll,/debug_nonredist/x86/Microsoft.VC143.DebugCRT/msvcp140_2d.dll> "$<TARGET_FILE_DIR:${APP_NAME}>"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different ${MSVC_REDIST_DIR}$<IF:$<CONFIG:Release>,/x86/Microsoft.VC143.CRT/msvcp140_atomic_wait.dll,/debug_nonredist/x86/Microsoft.VC143.DebugCRT/msvcp140d_atomic_wait.dll> "$<TARGET_FILE_DIR:${APP_NAME}>"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different ${MSVC_REDIST_DIR}$<IF:$<CONFIG:Release>,/x86/Microsoft.VC143.CRT/msvcp140_codecvt_ids.dll,/debug_nonredist/x86/Microsoft.VC143.DebugCRT/msvcp140d_codecvt_ids.dll> "$<TARGET_FILE_DIR:${APP_NAME}>"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different ${MSVC_REDIST_DIR}$<IF:$<CONFIG:Release>,/x86/Microsoft.VC143.CRT/vcruntime140.dll,/debug_nonredist/x86/Microsoft.VC143.DebugCRT/vcruntime140d.dll> "$<TARGET_FILE_DIR:${APP_NAME}>"
    )

    # Copy required QT runtime dlls
    target_link_libraries(${APP_NAME} PRIVATE Qt5::Widgets Qt5::XmlPatterns Qt5::Multimedia Qt5::Network)
    add_custom_command(TARGET ${APP_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different ${QTDIR}/bin/$<IF:$<CONFIG:Release>,Qt5Core.dll,Qt5Cored.dll> "$<TARGET_FILE_DIR:${APP_NAME}>"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different ${QTDIR}/bin/$<IF:$<CONFIG:Release>,Qt5Gui.dll,Qt5Guid.dll> "$<TARGET_FILE_DIR:${APP_NAME}>"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different ${QTDIR}/bin/$<IF:$<CONFIG:Release>,Qt5Network.dll,Qt5Networkd.dll> "$<TARGET_FILE_DIR:${APP_NAME}>"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different ${QTDIR}/bin/$<IF:$<CONFIG:Release>,Qt5Widgets.dll,Qt5Widgetsd.dll> "$<TARGET_FILE_DIR:${APP_NAME}>"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different ${QTDIR}/bin/$<IF:$<CONFIG:Release>,Qt5XmlPatterns.dll,Qt5XmlPatternsd.dll> "$<TARGET_FILE_DIR:${APP_NAME}>"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different ${QTDIR}/bin/$<IF:$<CONFIG:Release>,Qt5Multimedia.dll,Qt5Multimediad.dll> "$<TARGET_FILE_DIR:${APP_NAME}>"
        COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:${APP_NAME}>/plugins/audio"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different ${QTDIR}/plugins/audio/$<IF:$<CONFIG:Release>,qtaudio_windows.dll,qtaudio_windowsd.dll> "$<TARGET_FILE_DIR:${APP_NAME}>/plugins/audio"
        COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:${APP_NAME}>/plugins/platforms"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different ${QTDIR}/plugins/platforms/$<IF:$<CONFIG:Release>,qwindows.dll,qwindowsd.dll> "$<TARGET_FILE_DIR:${APP_NAME}>/plugins/platforms"
        COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:${APP_NAME}>/plugins/styles"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different ${QTDIR}/plugins/styles/$<IF:$<CONFIG:Release>,qwindowsvistastyle.dll,qwindowsvistastyled.dll> "$<TARGET_FILE_DIR:${APP_NAME}>/plugins/styles"
    )
else()
    target_link_libraries(${APP_NAME} PRIVATE Qt5::XmlPatterns Qt5::Network Qt5::Core Qt5::Multimedia)
#    find_package(OpenSSL REQUIRED)
#    target_link_libraries(${APP_NAME} PRIVATE OpenSSL::SSL OpenSSL::Crypto)
endif()






