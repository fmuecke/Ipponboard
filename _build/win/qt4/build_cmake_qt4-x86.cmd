@echo off
setlocal
call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars32.bat" || exit /b %errorlevel%
call _create_env_cfg.cmd x86 || exit /b %errorlevel%

set BASE_DIR=%~dp0..\..\..
call "%BASE_DIR%\base\_create_versioninfo.cmd"

:: Create the QT resource file
rcc.exe -name ipponboard %BASE_DIR%\base\ipponboard.qrc -o %BASE_DIR%\base\qrc_ipponboard.cpp || exit /b %errorlevel%

:: now build
cmake -S "_cmake_qt4" -B "_build_cmake_qt4" -G "Visual Studio 17 2022" -A Win32
cmake --build _build_cmake_qt4 --config Release --target Ipponboard
cmake --build _build_cmake_qt4 --target setup 
pause
