# This file is referenced by ../CMakeLists.txt
cmake_minimum_required(VERSION 3.28)

# CMake configuration for Ipponboard tests
set(APP_NAME IpponboardTest)

if(USE_QT5)
	find_package(Qt5 5.12.9 COMPONENTS Widgets XmlPatterns PrintSupport Multimedia REQUIRED)
else()
	find_package(Qt4 4.8.7 REQUIRED QtCore)
endif()

set(CMAKE_AUTOUIC OFF)
set(CMAKE_AUTOMOC OFF)
set(CMAKE_AUTORCC OFF)

if(USE_QT5)
	set(OUTPUT_DIR ${ROOT_DIR}/_bin/Test-Release)
	set(OUTPUT_DIR_DEBUG ${ROOT_DIR}/_bin/Test-Debug)
else()
	set(OUTPUT_DIR ${ROOT_DIR}/_bin/Test-Release-Qt4)
	set(OUTPUT_DIR_DEBUG ${ROOT_DIR}/_bin/Test-Debug-Qt4)
endif()
message(STATUS "Using: OUTPUT_DIR = ${OUTPUT_DIR} (${APP_NAME})")

set(SOURCES
	IpponboardTest.cpp
	TestFight.cpp
	TestJson.cpp
	TestRules.cpp
	TestScore.cpp
	TestTournamentMode.cpp
	TestVersionComparer.cpp
)

add_executable(${APP_NAME} ${SOURCES})

if (WIN32)
    set_target_properties(${APP_NAME} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY_DEBUG ${OUTPUT_DIR_DEBUG}
        RUNTIME_OUTPUT_DIRECTORY_RELEASE ${OUTPUT_DIR}
        COMPILE_FLAGS "/Zc:wchar_t- /MP"
        CMAKE_SUPPRESS_REGENERATION true
    )
else()
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${OUTPUT_DIR})
endif()

# Copy testdata
add_custom_command(TARGET ${APP_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/TestData" "$<TARGET_FILE_DIR:${APP_NAME}>/TestData")

# Copy msvc runtime dlls
if(MSVC)
	add_custom_command(TARGET ${APP_NAME} POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_if_different ${MSVC_REDIST_DIR}$<IF:$<CONFIG:Release>,/x86/Microsoft.VC143.CRT/concrt140.dll,/debug_nonredist/x86/Microsoft.VC143.DebugCRT/concrt140d.dll> "$<TARGET_FILE_DIR:${APP_NAME}>"
		COMMAND ${CMAKE_COMMAND} -E copy_if_different ${MSVC_REDIST_DIR}$<IF:$<CONFIG:Release>,/x86/Microsoft.VC143.CRT/msvcp140.dll,/debug_nonredist/x86/Microsoft.VC143.DebugCRT/msvcp140d.dll> "$<TARGET_FILE_DIR:${APP_NAME}>"
		COMMAND ${CMAKE_COMMAND} -E copy_if_different ${MSVC_REDIST_DIR}$<IF:$<CONFIG:Release>,/x86/Microsoft.VC143.CRT/msvcp140_1.dll,/debug_nonredist/x86/Microsoft.VC143.DebugCRT/msvcp140_1d.dll> "$<TARGET_FILE_DIR:${APP_NAME}>"
		COMMAND ${CMAKE_COMMAND} -E copy_if_different ${MSVC_REDIST_DIR}$<IF:$<CONFIG:Release>,/x86/Microsoft.VC143.CRT/msvcp140_2.dll,/debug_nonredist/x86/Microsoft.VC143.DebugCRT/msvcp140_2d.dll> "$<TARGET_FILE_DIR:${APP_NAME}>"
		COMMAND ${CMAKE_COMMAND} -E copy_if_different ${MSVC_REDIST_DIR}$<IF:$<CONFIG:Release>,/x86/Microsoft.VC143.CRT/msvcp140_atomic_wait.dll,/debug_nonredist/x86/Microsoft.VC143.DebugCRT/msvcp140d_atomic_wait.dll> "$<TARGET_FILE_DIR:${APP_NAME}>"
		COMMAND ${CMAKE_COMMAND} -E copy_if_different ${MSVC_REDIST_DIR}$<IF:$<CONFIG:Release>,/x86/Microsoft.VC143.CRT/msvcp140_codecvt_ids.dll,/debug_nonredist/x86/Microsoft.VC143.DebugCRT/msvcp140d_codecvt_ids.dll> "$<TARGET_FILE_DIR:${APP_NAME}>"
		COMMAND ${CMAKE_COMMAND} -E copy_if_different ${MSVC_REDIST_DIR}$<IF:$<CONFIG:Release>,/x86/Microsoft.VC143.CRT/vcruntime140.dll,/debug_nonredist/x86/Microsoft.VC143.DebugCRT/vcruntime140d.dll> "$<TARGET_FILE_DIR:${APP_NAME}>"
	)
endif()

# Copy required runtime dlls
if(USE_QT5)
	target_link_libraries(${APP_NAME} PRIVATE Qt5::Widgets)
	add_custom_command(TARGET ${APP_NAME} POST_BUILD
		# Copy Qt5 runtime dlls
		COMMAND ${CMAKE_COMMAND} -E copy_if_different ${QTDIR}/bin/$<IF:$<CONFIG:Release>,Qt5Core.dll,Qt5Cored.dll> "$<TARGET_FILE_DIR:${APP_NAME}>"
	)
else()
	target_link_libraries(${APP_NAME} PRIVATE Qt4::QtCore)
	add_custom_command(TARGET ${APP_NAME} POST_BUILD
		# Copy Qt4 runtime dlls
		COMMAND ${CMAKE_COMMAND} -E copy_if_different ${QTDIR}/bin/$<IF:$<CONFIG:Release>,QtCore4.dll,QtCored4.dll> "$<TARGET_FILE_DIR:${APP_NAME}>"
	)
endif()
