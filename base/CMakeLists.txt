# This file is referenced by ../CMakeLists.txt
cmake_minimum_required(VERSION 3.28)

# CMake configuration for Ipponboard main application
set(APP_NAME Ipponboard)

if(USE_QT5)
	find_package(Qt5 5.15.2 COMPONENTS Widgets XmlPatterns PrintSupport Multimedia REQUIRED)	
else()
	find_package(Qt4 4.8.7 REQUIRED QtCore QtGui QtXmlPatterns)
endif()

if(LINUX)
    # common include directories
    include_directories(${ROOT_DIR}/Widgets)
    include_directories(${Qt5Widgets_INCLUDE_DIRS})
    include_directories(${Qt5PrintSupport_INCLUDE_DIRS})
    include_directories(${Qt5XmlPatterns_INCLUDE_DIRS})
    include_directories(${Qt5Multimedia_INCLUDE_DIRS})
    add_compile_options(-fPIC -fpermissive)
endif()

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

if(USE_QT5)
    set(OUTPUT_DIR ${ROOT_DIR}/_bin/Ipponboard-Release)
    set(OUTPUT_DIR_DEBUG ${ROOT_DIR}/_bin/Ipponboard-Debug)
else()
    set(OUTPUT_DIR ${ROOT_DIR}/_bin/Ipponboard-Release-Qt4)
    set(OUTPUT_DIR_DEBUG ${ROOT_DIR}/_bin/Ipponboard-Debug-Qt4)
endif()
if(LINUX)
    message(STATUS "Using: OUTPUT_DIR = ${OUTPUT_DIR} (${APP_NAME})")
endif()

set(SOURCES
    #pch.cpp
    #pch.h
    AddFighterDlg.cpp
    AddFighterDlg.h
    Club.h
    ClubManager.cpp
    ClubManager.h
    ClubManagerDlg.cpp
    ClubManagerDlg.h
    ClubParser.h
    ComboBoxDelegate.cpp
    ComboBoxDelegate.h
    DonationManager.cpp
    DonationManager.h
    FightCategoryManager.cpp
    FightCategoryManager.h
    FightCategoryManagerDlg.cpp
    FightCategoryManagerDlg.h
    FightCategoryParser.cpp
    FightCategoryParser.h
    FighterManager.cpp
    FighterManager.h
    FighterManagerDlg.cpp
    FighterManagerDlg.h
    ../util/array_helpers.h
    ../util/path_helpers.h
    ../Widgets/Countdown.h
    ../Widgets/ScaledImage.h
    ../Widgets/ScaledText.h
    ../util/SimpleCsvFile.hpp
    ../core/Controller.h
    ../core/Enums.h
    ../core/Fight.h
    ../core/FightCategory.h
    ../core/iController.h
    ../core/iControllerCore.h
    ../core/Score.h
    ../core/StateMachine.h
    ../core/Tournament.h
    ../core/Fighter.h
    ../core/TournamentMode.h
    ../core/TournamentModel.h
    ../core/ControllerConfig.h
    ../core/Rules.h
    ipponboard.qrc
    Main.cpp
    MainWindow.cpp
    MainWindow.h
    MainWindowBase.cpp
    MainWindowBase.h
    MainWindowTeam.cpp
    MainWindowTeam.h
    ModeManagerDlg.cpp
    ModeManagerDlg.h
    #qrc_ipponboard.cpp
    ScoreScreen.cpp
    ScoreScreen.h
    SettingsDlg.cpp
    SettingsDlg.h
    SplashScreen.cpp
    SplashScreen.h
    #ui_AddFighterDlg.h
    #ui_ClubManagerDlg.h
    #ui_Countdown.h
    #ui_FightCategoryManagerDlg.h
    #ui_FighterManagerDlg.h
    #ui_MainWindow.h
    #ui_MainWindowTeam.h
    #ui_ModeManagerDlg.h
    #ui_ScoreScreen.h
    #ui_SettingsDlg.h
    #ui_SplashScreen.h
    #ui_view_horizontal.h
    #ui_view_vertical_single.h
    UpdateChecker.cpp
    UpdateChecker.h
    VersionComparer.cpp
    VersionComparer.h
    ../core/iGoldenScoreView.h
    ../core/iView.h
    ../util/json.hpp
    versioninfo.h
    View.cpp
    View.h
    ../util/jsoncpp/json.cpp
    ../util/qt_helpers.hpp
    ../Widgets/ScaledImage.cpp
    ../Widgets/ScaledText.cpp
    ../Widgets/Countdown.cpp
    ../core/Controller.cpp
    ../core/FightCategory.cpp
    ../core/Fighter.cpp
    ../core/Fight.cpp
    ../core/Score.cpp
    ../core/StateMachine.cpp
    ../core/TournamentMode.cpp
    ../core/TournamentModel.cpp
    ../core/Rules.cpp
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
    ../Widgets/Countdown.ui
)

set(OTHER
    Ipponboard.rc
    TournamentModes.ini
    categories.json
    #categories.xml
    clubs.json
)

source_group("UI Forms" FILES ${FORMS})
source_group("Other Files" FILES ${OTHER})

add_executable(${APP_NAME} WIN32 ${SOURCES} ${FORMS} ${OTHER})
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${APP_NAME})

if(MSVC)
    set_target_properties(${APP_NAME} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY_DEBUG ${OUTPUT_DIR_DEBUG}
        RUNTIME_OUTPUT_DIRECTORY_RELEASE ${OUTPUT_DIR}
        COMPILE_FLAGS "/Zc:wchar_t- /MP /W4"
        CMAKE_SUPPRESS_REGENERATION true
    )
else()
    set_target_properties(${APP_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${OUTPUT_DIR})
endif()

#target_precompile_headers(${APP_NAME} PRIVATE pch.h)
target_include_directories(${APP_NAME} PRIVATE ${BOOST_DIR})

# Copy data files
add_custom_command(TARGET ${APP_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_CURRENT_SOURCE_DIR}/categories.json" "$<TARGET_FILE_DIR:${APP_NAME}>"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_CURRENT_SOURCE_DIR}/TournamentModes.ini" "$<TARGET_FILE_DIR:${APP_NAME}>"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_CURRENT_SOURCE_DIR}/clubs.json" "$<TARGET_FILE_DIR:${APP_NAME}>"
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

    if(USE_QT5)
        # Copy Qt5 runtime dlls
        target_link_libraries(${APP_NAME} PRIVATE Qt5::Widgets Qt5::XmlPatterns Qt5::PrintSupport Qt5::Multimedia)
        add_custom_command(TARGET ${APP_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different ${QTDIR}/bin/$<IF:$<CONFIG:Release>,Qt5Core.dll,Qt5Cored.dll> "$<TARGET_FILE_DIR:${APP_NAME}>"
            COMMAND ${CMAKE_COMMAND} -E copy_if_different ${QTDIR}/bin/$<IF:$<CONFIG:Release>,Qt5Gui.dll,Qt5Guid.dll> "$<TARGET_FILE_DIR:${APP_NAME}>"
            COMMAND ${CMAKE_COMMAND} -E copy_if_different ${QTDIR}/bin/$<IF:$<CONFIG:Release>,Qt5Multimedia.dll,Qt5Multimediad.dll> "$<TARGET_FILE_DIR:${APP_NAME}>"
            COMMAND ${CMAKE_COMMAND} -E copy_if_different ${QTDIR}/bin/$<IF:$<CONFIG:Release>,Qt5Network.dll,Qt5Networkd.dll> "$<TARGET_FILE_DIR:${APP_NAME}>"
            COMMAND ${CMAKE_COMMAND} -E copy_if_different ${QTDIR}/bin/$<IF:$<CONFIG:Release>,Qt5PrintSupport.dll,Qt5PrintSupportd.dll> "$<TARGET_FILE_DIR:${APP_NAME}>"
            COMMAND ${CMAKE_COMMAND} -E copy_if_different ${QTDIR}/bin/$<IF:$<CONFIG:Release>,Qt5Widgets.dll,Qt5Widgetsd.dll> "$<TARGET_FILE_DIR:${APP_NAME}>"
            COMMAND ${CMAKE_COMMAND} -E copy_if_different ${QTDIR}/bin/$<IF:$<CONFIG:Release>,Qt5XmlPatterns.dll,Qt5XmlPatternsd.dll> "$<TARGET_FILE_DIR:${APP_NAME}>"
            COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:${APP_NAME}>/plugins/platforms"
            COMMAND ${CMAKE_COMMAND} -E copy_if_different ${QTDIR}/plugins/platforms/$<IF:$<CONFIG:Release>,qwindows.dll,qwindowsd.dll> "$<TARGET_FILE_DIR:${APP_NAME}>/plugins/platforms"
            COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:${APP_NAME}>/plugins/printsupport"
            COMMAND ${CMAKE_COMMAND} -E copy_if_different ${QTDIR}/plugins/printsupport/$<IF:$<CONFIG:Release>,windowsprintersupport.dll,windowsprintersupportd.dll> "$<TARGET_FILE_DIR:${APP_NAME}>/plugins/printsupport"
            COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:${APP_NAME}>/plugins/styles"
            COMMAND ${CMAKE_COMMAND} -E copy_if_different ${QTDIR}/plugins/styles/$<IF:$<CONFIG:Release>,qwindowsvistastyle.dll,qwindowsvistastyled.dll> "$<TARGET_FILE_DIR:${APP_NAME}>/plugins/styles"
        )
    else()
        # Copy Qt4 runtime dlls
        target_link_libraries(${APP_NAME} Qt4::QtCore Qt4::QtGui Qt4::QtXmlPatterns)
        add_custom_command(TARGET ${APP_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different ${QTDIR}/bin/$<IF:$<CONFIG:Release>,QtCore4.dll,QtCored4.dll> "$<TARGET_FILE_DIR:${APP_NAME}>"
            COMMAND ${CMAKE_COMMAND} -E copy_if_different ${QTDIR}/bin/$<IF:$<CONFIG:Release>,QtGui4.dll,QtGuid4.dll> "$<TARGET_FILE_DIR:${APP_NAME}>"
            COMMAND ${CMAKE_COMMAND} -E copy_if_different ${QTDIR}/bin/$<IF:$<CONFIG:Release>,QtNetwork4.dll,QtNetworkd4.dll> "$<TARGET_FILE_DIR:${APP_NAME}>"
            COMMAND ${CMAKE_COMMAND} -E copy_if_different ${QTDIR}/bin/$<IF:$<CONFIG:Release>,QtXmlPatterns4.dll,QtXmlPatternsd4.dll> "$<TARGET_FILE_DIR:${APP_NAME}>"
        )
    endif()
else()
    target_link_libraries(${APP_NAME} PRIVATE Qt5::Widgets Qt5::XmlPatterns Qt5::PrintSupport Qt5::Multimedia)
endif()
