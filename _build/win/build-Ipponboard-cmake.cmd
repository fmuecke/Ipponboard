@echo off
setlocal
call %~dp0init_env_cfg.cmd  || exit /b %errorlevel%

SET CONFIG=release
if not "%1"=="" SET CONFIG=%1
if "%2"=="clean" SET CLEAN=--clean-first

set BIN_DIR=%IPPONBOARD_ROOT_DIR%\_bin\%CONFIG%

:: create version information
call create-versioninfo.cmd %IPPONBOARD_ROOT_DIR%\base || exit /b %errorlevel%

if defined CLEAN (
	if exist "%BIN_DIR%" (
		rd /Q /S "%BIN_DIR%" >nul 2>&1
	)
)

echo Creating makefiles...
cmake -S "%IPPONBOARD_ROOT_DIR%\base" -B "%IPPONBOARD_ROOT_DIR%\_build\build-Ipponboard" -G "Visual Studio 17 2022" -A Win32 -DUSE_QT5=ON || exit /b %errorlevel%

echo Building makefiles...
cmake --build "%IPPONBOARD_ROOT_DIR%\_build\build-Ipponboard" --config %CONFIG% %CLEAN%