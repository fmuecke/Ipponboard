@echo off
setlocal

if not defined IPPONBOARD_ROOT_DIR (
	call "%~dp0init_env_cfg.cmd" || exit /b %errorlevel%
)

set OUTPUT_DIR=%IPPONBOARD_ROOT_DIR%\_output
if not exist "%OUTPUT_DIR%" mkdir "%OUTPUT_DIR%"
echo Using: OUTPUT_DIR=%OUTPUT_DIR%

:: derive USE_QT6 from QTDIR (set by env_cfg.bat) for setup.iss
if not defined QTDIR (
	echo Error: QTDIR not defined - check env_cfg.bat!
	exit /b 1
)
set "QT_MAJOR="
echo(%QTDIR%| findstr /i /r /c:"qt[\\/-]*6" >nul && set "QT_MAJOR=6"
if not defined QT_MAJOR echo(%QTDIR%| findstr /i /r /c:"qt[\\/-]*5" >nul && set "QT_MAJOR=5"
if not defined QT_MAJOR (
	echo Error: cannot derive Qt version from QTDIR=%QTDIR% - check env_cfg.bat!
	exit /b 1
)
set "USE_QT6="
if "%QT_MAJOR%"=="6" set "USE_QT6=1"
set "ISCC_DEFINES="
if defined USE_QT6 set "ISCC_DEFINES=/DUSE_QT6"
echo Using: QTDIR=%QTDIR% -- Qt%QT_MAJOR%, USE_QT6=%USE_QT6%

:: checking for Inno setup
if not exist "%INNO_DIR%\iscc.exe" (
	echo Error: iscc.exe not found or INNO_DIR not defined!
	exit /b 1
)
	
"%INNO_DIR%\iscc.exe" /Q /O"%OUTPUT_DIR%" %ISCC_DEFINES% "%IPPONBOARD_ROOT_DIR%\setup\setup.iss" || exit /b %errorlevel%
dir /OD "%OUTPUT_DIR%"
exit /b 0