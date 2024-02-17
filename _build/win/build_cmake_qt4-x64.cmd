@echo off
setlocal
call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat" || exit /b %errorlevel%
call _create_env_cfg.cmd x64 || exit /b %errorlevel%

set BASE_DIR=%~dp0..\..
call _create_versioninfo.cmd %BASE_DIR%\base

:: Create the QT resource file
rcc.exe -name ipponboard %BASE_DIR%\base\ipponboard.qrc -o %BASE_DIR%\base\qrc_ipponboard.cpp || exit /b %errorlevel%

:: make Release Makefiles
cmake -S "..\_cmake_qt4" -B "..\_build_cmake_qt4" -G "Visual Studio 17 2022" -A x64 -DCMAKE_BUILD_TYPE=Release

:: make tests
cmake --build ..\_build_cmake_qt4 --config Release --target IpponboardTest
pushd ..\_build_cmake_qt4\test\Release
IpponboardTest
popd
if errorlevel 1 exit /b 1

:: Build Release
cmake --build ..\_build_cmake_qt4 --config Release --target Ipponboard 
cmake --build ..\_build_cmake_qt4 --target setup 

:: build Debug
::cmake -S "..\_cmake_qt4" -B "..\_build_cmake_qt4" -G "Visual Studio 17 2022" -A x64 -DCMAKE_BUILD_TYPE=Debug
::cmake --build ..\_build_cmake_qt4 --config Debug --target Ipponboard_Debug

pause
