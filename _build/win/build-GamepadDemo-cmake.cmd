@echo off
setlocal
call %~dp0init_env_cfg.cmd  || exit /b %errorlevel%

SET CONFIG=Release
if not "%1"=="" SET CONFIG=%1

if "%2"=="clean" SET CLEAN=--clean-first

set IPPONBOARD_ROOT_DIR=%~dp0..\..

:: Create makefiles
cmake -S "%IPPONBOARD_ROOT_DIR%\GamepadDemo" -B "%IPPONBOARD_ROOT_DIR%\_build\build-GamepadDemo" -G "Visual Studio 17 2022" -A Win32 || exit /b %errorlevel%

:: Build Release
cmake --build "%IPPONBOARD_ROOT_DIR%\_build\build-GamepadDemo" --clean-first --config %CONFIG% %CLEAN%
