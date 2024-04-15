@echo off
setlocal
call %~dp0init_env_cfg.cmd  || exit /b %errorlevel%

SET CONFIG=release
if not "%1"=="" SET CONFIG=%1
if "%2"=="clean" SET CLEAN=--clean-first

set BUILD_DIR=%IPPONBOARD_ROOT_DIR%\_build\build-Ipponboard
set BIN_DIR=%IPPONBOARD_ROOT_DIR%\_bin\%CONFIG%
set TEST_BIN_DIR=%IPPONBOARD_ROOT_DIR%\_bin\Test-%CONFIG%

if defined CLEAN (
	rd /Q /S "%BUILD_DIR%" >nul 2>&1
	rd /Q /S "%BIN_DIR%" >nul 2>&1
	rd /Q /S "%TEST_BIN_DIR%" >nul 2>&1
)

:: Create version information
call create-versioninfo.cmd %IPPONBOARD_ROOT_DIR%\base || exit /b %errorlevel%

echo Creating makefiles...
cmake -S "%IPPONBOARD_ROOT_DIR%" -B "%BUILD_DIR%" -G "Visual Studio 17 2022" -A Win32 || exit /b %errorlevel%

echo Building makefiles...
cmake --build "%IPPONBOARD_ROOT_DIR%\_build\build-Ipponboard" --config %CONFIG% %CLEAN%

echo Executing tests...
pushd %TEST_BIN_DIR%
IpponboardTest.exe
popd