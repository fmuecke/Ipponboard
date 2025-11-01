# This file is referenced by ../CMakeLists.txt
cmake_minimum_required(VERSION 3.28)

# CMake configuration for Ipponboard main application
set(APP_NAME Ipponboard)

find_package(Qt6 COMPONENTS Widgets PrintSupport Multimedia Network REQUIRED)

if(LINUX)
    # common include directories
    include_directories(${ROOT_DIR}/Widgets)

    add_compile_options(-fPIC -fpermissive)
endif()

set(UI_SOURCES
    AddFighterDlg.cpp
    ClubManager.cpp
    ClubManagerDlg.cpp
    ComboBoxDelegate.cpp
    DonationManager.cpp
    FightCategoryManager.cpp
    FightCategoryManagerDlg.cpp
    GamepadSectionMapper.cpp
    FightCategoryParser.cpp
    FighterManager.cpp
    FighterManagerDlg.cpp
    MainWindow.cpp
    MainWindowBase.cpp
    MainWindowTeam.cpp
    ModeManagerDlg.cpp
    OnlineVersionChecker.cpp
    ScoreScreen.cpp
    SettingsDlg.cpp
    SplashScreen.cpp
    VersionComparer.cpp
    View.cpp
    ../util/jsoncpp/json.cpp
)

set(UI_HEADERS
    AddFighterDlg.h
    Club.h
    ClubManager.h
    ClubManagerDlg.h
    ClubParser.h
    ComboBoxDelegate.h
    DonationManager.h
    FightCategoryManager.h
    FightCategoryManagerDlg.h
    GamepadSectionMapper.h
    FighterManager.h
    FighterManagerDlg.h
    MainWindow.h
    MainWindowBase.h
    MainWindowTeam.h
    ModeManagerDlg.h
    OnlineVersionChecker.h
    ScoreScreen.h
    SettingsDlg.h
    SplashScreen.h
    VersionComparer.h
    View.h
    ../util/SimpleCsvFile.hpp
    ../util/array_helpers.h
    ../util/path_helpers.h
    ../util/qt_helpers.hpp
    ../util/json.hpp
    ../core/ControllerConfig.h
    ../core/iGoldenScoreView.h
    ../core/iView.h
)

set(UI_RESOURCES
    ipponboard.qrc
)

set(FORMS
    AddFighterDlg.ui
    ClubManagerDlg.ui
    FighterManagerDlg.ui
    FightCategoryManagerDlg.ui
    MainWindow.ui
    MainWindowTeam.ui
    ModeManagerDlg.ui
    ScoreScreen.ui
    SettingsDlg.ui
    SplashScreen.ui
    view_horizontal.ui
    view_vertical_single.ui
    view_vertical_team.ui
)

set(OTHER
    Ipponboard.rc
    TournamentModes.ini
	clubs.config
)

source_group("UI Forms" FILES ${FORMS})
source_group("Other Files" FILES ${OTHER})

add_library(IpponboardUi STATIC
    ${UI_SOURCES}
    ${UI_HEADERS}
    ${FORMS}
    ${UI_RESOURCES}
)

set_target_properties(IpponboardUi PROPERTIES
    AUTOMOC ON
    AUTOUIC ON
    AUTORCC ON
)

target_include_directories(IpponboardUi
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${ROOT_DIR}/Widgets
        ${ROOT_DIR}/core
        ${ROOT_DIR}/util
        ${BOOST_DIR}
)

target_link_libraries(IpponboardUi
    PUBLIC
        IpponboardCore
        IpponboardWidgets
        IpponboardGamepad
        Qt6::Widgets
        Qt6::PrintSupport
        Qt6::Multimedia
        Qt6::Network
)

if(WIN32)
    set(OUTPUT_DIR ${ROOT_DIR}/_bin/Ipponboard-Release)
    set(OUTPUT_DIR_DEBUG ${ROOT_DIR}/_bin/Ipponboard-Debug)
else()
    set(OUTPUT_DIR ${ROOT_DIR}/_bin/Ipponboard-${CMAKE_BUILD_TYPE})
    message(STATUS "Using: OUTPUT_DIR = ${OUTPUT_DIR} (${APP_NAME})")
endif()

add_executable(${APP_NAME} WIN32 Main.cpp ${OTHER})
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${APP_NAME})

if(MSVC)
    set_target_properties(${APP_NAME} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY_DEBUG ${OUTPUT_DIR_DEBUG}
        RUNTIME_OUTPUT_DIRECTORY_RELEASE ${OUTPUT_DIR}
        COMPILE_FLAGS "/MP /W4"
        CMAKE_SUPPRESS_REGENERATION true
    )
    target_compile_definitions(${APP_NAME} PRIVATE _SILENCE_STDEXT_ARR_ITERS_DEPRECATION_WARNING)
else()
    set_target_properties(${APP_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${OUTPUT_DIR})
endif()

target_link_libraries(${APP_NAME}
    PRIVATE
        IpponboardUi
)

# Copy data files
add_custom_command(TARGET ${APP_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_CURRENT_SOURCE_DIR}/TournamentModes.ini" "$<TARGET_FILE_DIR:${APP_NAME}>"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_CURRENT_SOURCE_DIR}/clubs.config" "$<TARGET_FILE_DIR:${APP_NAME}>"
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/clubs" "$<TARGET_FILE_DIR:${APP_NAME}>/clubs"
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/templates" "$<TARGET_FILE_DIR:${APP_NAME}>/templates"
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/sounds" "$<TARGET_FILE_DIR:${APP_NAME}>/sounds"
)

if(MSVC)
    # Compile and copy translation files
    add_custom_command(TARGET ${APP_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:${APP_NAME}>/lang"
        COMMAND ${QTDIR}/bin/lrelease.exe -compress -silent ${ROOT_DIR}/i18n/de.ts -qm $<TARGET_FILE_DIR:${APP_NAME}>/lang/de.qm
        COMMAND ${QTDIR}/bin/lrelease.exe -compress -silent ${ROOT_DIR}/i18n/nl.ts -qm $<TARGET_FILE_DIR:${APP_NAME}>/lang/nl.qm
    )

    # Copy msvc runtime dlls (64-bit)
    add_custom_command(TARGET ${APP_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different ${MSVC_REDIST_DIR}$<IF:$<CONFIG:Release>,/x64/Microsoft.VC143.CRT/concrt140.dll,/debug_nonredist/x64/Microsoft.VC143.DebugCRT/concrt140d.dll> "$<TARGET_FILE_DIR:${APP_NAME}>"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different ${MSVC_REDIST_DIR}$<IF:$<CONFIG:Release>,/x64/Microsoft.VC143.CRT/msvcp140.dll,/debug_nonredist/x64/Microsoft.VC143.DebugCRT/msvcp140d.dll> "$<TARGET_FILE_DIR:${APP_NAME}>"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different ${MSVC_REDIST_DIR}$<IF:$<CONFIG:Release>,/x64/Microsoft.VC143.CRT/msvcp140_1.dll,/debug_nonredist/x64/Microsoft.VC143.DebugCRT/msvcp140_1d.dll> "$<TARGET_FILE_DIR:${APP_NAME}>"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different ${MSVC_REDIST_DIR}$<IF:$<CONFIG:Release>,/x64/Microsoft.VC143.CRT/msvcp140_2.dll,/debug_nonredist/x64/Microsoft.VC143.DebugCRT/msvcp140_2d.dll> "$<TARGET_FILE_DIR:${APP_NAME}>"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different ${MSVC_REDIST_DIR}$<IF:$<CONFIG:Release>,/x64/Microsoft.VC143.CRT/msvcp140_atomic_wait.dll,/debug_nonredist/x64/Microsoft.VC143.DebugCRT/msvcp140d_atomic_wait.dll> "$<TARGET_FILE_DIR:${APP_NAME}>"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different ${MSVC_REDIST_DIR}$<IF:$<CONFIG:Release>,/x64/Microsoft.VC143.CRT/msvcp140_codecvt_ids.dll,/debug_nonredist/x64/Microsoft.VC143.DebugCRT/msvcp140d_codecvt_ids.dll> "$<TARGET_FILE_DIR:${APP_NAME}>"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different ${MSVC_REDIST_DIR}$<IF:$<CONFIG:Release>,/x64/Microsoft.VC143.CRT/vcruntime140.dll,/debug_nonredist/x64/Microsoft.VC143.DebugCRT/vcruntime140d.dll> "$<TARGET_FILE_DIR:${APP_NAME}>"
    )

    target_link_libraries(${APP_NAME} PRIVATE Qt6::Core Qt6::Widgets Qt6::PrintSupport Qt6::Multimedia Qt6::Network)
    ipponboard_add_windeploy(${APP_NAME})
else()
    # Compile and copy translation files
    add_custom_command(TARGET ${APP_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:${APP_NAME}>/lang"
        COMMAND ${QTDIR}/bin/lrelease -compress -silent ${ROOT_DIR}/i18n/de.ts -qm $<TARGET_FILE_DIR:${APP_NAME}>/lang/de.qm
        COMMAND ${QTDIR}/bin/lrelease -compress -silent ${ROOT_DIR}/i18n/nl.ts -qm $<TARGET_FILE_DIR:${APP_NAME}>/lang/nl.qm
    )

    target_link_libraries(${APP_NAME} PRIVATE IpponboardCore Qt6::Core Qt6::Widgets Qt6::PrintSupport Qt6::Multimedia Qt6::Network)
endif()
