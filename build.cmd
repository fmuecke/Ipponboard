::---------------------------------------------------------
:: Ipponboard build script
::
:: Copyright 2010-2012 Florian Muecke. All rights reserved.
:: http://www.ipponboard.info (ipponboardinfo at googlemail dot com)
::
:: THIS FILE IS PART OF THE IPPONBOARD PROJECT.
:: IT MAY NOT BE DISTRIBUTED TO OR SHARED WITH THE PUBLIC IN ANY FORM!
::
:: $Id$
::---------------------------------------------------------
@echo off
SETLOCAL
SET BASE_DIR=%CD%
SET BUILD_DIR=%BASE_DIR%\_build\build_output\~tmp
SET BUILD_DIR_TEAM=%BASE_DIR%\_build\build_output\~tmp_TE

IF "%VS100COMNTOOLS%"=="" (
  CALL "%VS90COMNTOOLS%..\..\vc\vcvarsall.bat" x86
) ELSE (
  CALL "%VS100COMNTOOLS%..\..\vc\vcvarsall.bat" x86
)

echo;
echo Select build mode:
echo   (1) make clean
echo   (2) clean build
echo   (3) incremental build
echo   (4) setup only
echo   (9) everything
echo   (q) quit
choice /C 12349q /N
:: value "0" is reserved!
if %errorlevel%==6 goto :eof
if %errorlevel%==5 goto build_all
if %errorlevel%==4 goto build_setup
if %errorlevel%==3 goto build_incremental
if %errorlevel%==2 goto build_clean
if %errorlevel%==1 goto make_clean
GOTO the_end

:make_clean
	echo --[make clean]--
	rem del /Q "%BASE_DIR%\base\.buildnr"
	rd /Q /S "%BASE_DIR%\bin"
	rd /Q /S "%BASE_DIR%\lib"
	qmake -recursive
	if errorlevel 1 pause
	jom clean
	if errorlevel 1 pause
	if not "%1"=="internal" pause
GOTO :EOF

:build_clean
	echo --[build clean]--
	CALL :make_clean internal
	CALL :build_incremental internal
	if not "%1"=="internal" pause
GOTO :EOF

:build_incremental
	echo --[build incremental]--
	qmake -recursive
	if errorlevel 1 pause
	jom release
	if errorlevel 1 pause
	if not "%1"=="internal" pause
GOTO :EOF

:build_setup
	echo --[setup only]--
	if not exist "%INNO_DIR%\iscc.exe" (
		echo Error: iscc.exe not found or INNO_DIR not defined!
		GOTO :EOF
	)
	CALL _build\scripts\clear_build_dir.cmd
	if errorlevel 1 pause
	CALL _build\scripts\Ipponboard_copy_files.cmd
	if errorlevel 1 pause
	"%INNO_DIR%\iscc.exe" /Q /O"%BASE_DIR%\_build\build_output" "%BASE_DIR%\setup\setup.iss"
	if errorlevel 1 pause
	"%INNO_DIR%\iscc.exe" /Q /O"%BASE_DIR%\_build\build_output" "%BASE_DIR%\setup\setup_team.iss"
	if errorlevel 1 pause
	if errorlevel 0 dir /OD "%BASE_DIR%\_build\build_output"
	if not "%1"=="internal" pause
GOTO :EOF

:build_all
	echo --[build all]--
	CALL :make_clean internal
	CALL :build_clean internal
	CALL :build_setup internal
	if not "%1"=="internal" pause
GOTO :EOF

:the_end
pause