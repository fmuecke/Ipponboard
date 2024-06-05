@echo off
setlocal

if not defined IPPONBOARD_ROOT_DIR (
	call "%~dp0init_env_cfg.cmd" || exit /b %errorlevel%
)

set OUTPUT_DIR=%IPPONBOARD_ROOT_DIR%\_output
if not exist "%OUTPUT_DIR%" mkdir "%OUTPUT_DIR%"
echo Using: OUTPUT_DIR=%OUTPUT_DIR%

:: checking for Inno setup
if not exist "%INNO_DIR%\iscc.exe" (
	echo Error: iscc.exe not found or INNO_DIR not defined!
	exit /b 1
)
	
"%INNO_DIR%\iscc.exe" /Q /O"%OUTPUT_DIR%" "%IPPONBOARD_ROOT_DIR%\setup\setup.iss" || exit /b %errorlevel%
dir /OD "%OUTPUT_DIR%"
exit /b 0