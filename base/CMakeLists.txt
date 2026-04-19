# This file is referenced by ../CMakeLists.txt
cmake_minimum_required(VERSION 3.28)

# CMake configuration for Ipponboard main application
set(APP_NAME Ipponboard)

find_package(Qt6 COMPONENTS Widgets PrintSupport Multimedia Network REQUIRED)
find_program(QT_LRELEASE_EXECUTABLE
    NAMES lrelease lrelease.exe
    HINTS "${QTDIR}/bin"
    REQUIRED)

set(UI_SOURCES
    AddAthleteDlg.cpp
    ClubManager.cpp
    ClubManagerDlg.cpp
    ComboBoxDelegate.cpp
    DonationManager.cpp
    ContestCategoryManager.cpp
    ContestCategoryManagerDlg.cpp
    GamepadSectionMapper.cpp
    ContestCategoryParser.cpp
    AthleteManager.cpp
    AthleteManagerDlg.cpp
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
)

set(UI_HEADERS
    AddAthleteDlg.h
    Club.h
    ClubManager.h
    ClubManagerDlg.h
    ClubParser.h
    ComboBoxDelegate.h
    DonationManager.h
    ContestCategoryManager.h
    ContestCategoryManagerDlg.h
    GamepadSectionMapper.h
    AthleteManager.h
    AthleteManager.h
    MainWindow.h
    MainWindowBase.h
    MainWindowTeam.h
    CompetitionSerialization.cpp
    CompetitionSerialization.h
    ModeManagerDlg.cpp
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
    ../util/theme_colors.h
    ../core/ControllerConfig.h
    ../core/iGoldenScoreView.h
    ../core/iView.h
)

set(UI_RESOURCES
    ipponboard.qrc
)

set(FORMS
    AddAthleteDlg.ui
    ClubManagerDlg.ui
    AthleteManagerDlg.ui
    ContestCategoryManagerDlg.ui
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
    competition_modes.config
	categories.config
	clubs.config
)

if(WIN32)
    list(PREPEND OTHER Ipponboard.rc)
endif()

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
set_target_properties(IpponboardUi PROPERTIES POSITION_INDEPENDENT_CODE ON)

target_include_directories(IpponboardUi
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${ROOT_DIR}/Widgets
        ${ROOT_DIR}/core
        ${ROOT_DIR}/util
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

target_precompile_headers(IpponboardUi
    PRIVATE "$<$<COMPILE_LANGUAGE:CXX>:${PCH_HEADER_WIDGETS}>")

if(WIN32)
    set(OUTPUT_DIR ${ROOT_DIR}/_bin/Ipponboard-Release)
    set(OUTPUT_DIR_DEBUG ${ROOT_DIR}/_bin/Ipponboard-Debug)
else()
    set(OUTPUT_DIR ${ROOT_DIR}/_bin/Ipponboard-${CMAKE_BUILD_TYPE})
    message(STATUS "Using: OUTPUT_DIR = ${OUTPUT_DIR} (${APP_NAME})")
endif()

if(APPLE)
    set(MACOS_BUNDLE_ICON "${CMAKE_CURRENT_SOURCE_DIR}/images/ipponboard.icns")

    add_executable(${APP_NAME} MACOSX_BUNDLE Main.cpp ${OTHER})
    target_sources(${APP_NAME} PRIVATE "${MACOS_BUNDLE_ICON}")
    set_source_files_properties("${MACOS_BUNDLE_ICON}" PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")
    set_target_properties(${APP_NAME} PROPERTIES
        MACOSX_BUNDLE_ICON_FILE "ipponboard.icns"
    )
else()
    add_executable(${APP_NAME} WIN32 Main.cpp ${OTHER})
endif()
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

target_precompile_headers(${APP_NAME}
    PRIVATE "$<$<COMPILE_LANGUAGE:CXX>:${PCH_HEADER_WIDGETS}>")

if(APPLE)
    set(APP_RESOURCE_DIR "$<TARGET_BUNDLE_DIR:${APP_NAME}>/Contents/Resources")

    # Copy immutable bundle resources into Contents/Resources on macOS.
    add_custom_command(TARGET ${APP_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory "${APP_RESOURCE_DIR}"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_CURRENT_SOURCE_DIR}/competition_modes.config" "${APP_RESOURCE_DIR}"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_CURRENT_SOURCE_DIR}/clubs.config" "${APP_RESOURCE_DIR}"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_CURRENT_SOURCE_DIR}/categories.config" "${APP_RESOURCE_DIR}"
        COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/clubs" "${APP_RESOURCE_DIR}/clubs"
        COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/templates" "${APP_RESOURCE_DIR}/templates"
        COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/sounds" "${APP_RESOURCE_DIR}/sounds"
        COMMAND ${CMAKE_COMMAND} -E copy_directory "${ROOT_DIR}/doc/licenses" "${APP_RESOURCE_DIR}/licenses"
    )

    add_custom_command(TARGET ${APP_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory "${APP_RESOURCE_DIR}/lang"
        COMMAND ${QT_LRELEASE_EXECUTABLE} -compress -silent ${ROOT_DIR}/i18n/de.ts -qm ${APP_RESOURCE_DIR}/lang/de.qm
        COMMAND ${QT_LRELEASE_EXECUTABLE} -compress -silent ${ROOT_DIR}/i18n/nl.ts -qm ${APP_RESOURCE_DIR}/lang/nl.qm
    )
else()
    # Copy data files
    add_custom_command(TARGET ${APP_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_CURRENT_SOURCE_DIR}/competition_modes.config" "$<TARGET_FILE_DIR:${APP_NAME}>"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_CURRENT_SOURCE_DIR}/clubs.config" "$<TARGET_FILE_DIR:${APP_NAME}>"
	    COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_CURRENT_SOURCE_DIR}/categories.config" "$<TARGET_FILE_DIR:${APP_NAME}>"
        COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/clubs" "$<TARGET_FILE_DIR:${APP_NAME}>/clubs"
        COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/templates" "$<TARGET_FILE_DIR:${APP_NAME}>/templates"
        COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/sounds" "$<TARGET_FILE_DIR:${APP_NAME}>/sounds"
        COMMAND ${CMAKE_COMMAND} -E copy_directory "${ROOT_DIR}/doc/licenses" "$<TARGET_FILE_DIR:${APP_NAME}>/licenses"
    )

    # Compile and copy translation files
    add_custom_command(TARGET ${APP_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:${APP_NAME}>/lang"
        COMMAND ${QT_LRELEASE_EXECUTABLE} -compress -silent ${ROOT_DIR}/i18n/de.ts -qm $<TARGET_FILE_DIR:${APP_NAME}>/lang/de.qm
        COMMAND ${QT_LRELEASE_EXECUTABLE} -compress -silent ${ROOT_DIR}/i18n/nl.ts -qm $<TARGET_FILE_DIR:${APP_NAME}>/lang/nl.qm
    )
endif()

if(MSVC)
    target_link_libraries(${APP_NAME} PRIVATE Qt6::Core Qt6::Widgets Qt6::PrintSupport Qt6::Multimedia Qt6::Network)
    ipponboard_add_windeploy(${APP_NAME})
else()
    target_link_libraries(${APP_NAME} PRIVATE IpponboardCore Qt6::Core Qt6::Widgets Qt6::PrintSupport Qt6::Multimedia Qt6::Network)
endif()
