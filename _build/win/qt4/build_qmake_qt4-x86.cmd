::---------------------------------------------------------
:: Copyright 2018 Florian Muecke. All rights reserved.
:: Use of this source code is governed by a BSD-style license that can be
:: found in the LICENSE.txt file.
::---------------------------------------------------------
@echo off
SETLOCAL

call _create_env_cfg.cmd x86|| exit /b %errorlevel%

SET BASE_DIR=..\..\..
if "%2"=="debug" (SET BIN_DIR=%BASE_DIR%\bin\Debug) else (SET BIN_DIR=%BASE_DIR%\bin\Release)

IF NOT EXIST "%BIN_DIR%" mkdir "%BIN_DIR%"

rem check for compiler
cl > nul 2>&1
IF ERRORLEVEL 1 (
	ECHO Can't find C++ compiler tools. Please run create_env_cfg-x86.cmd from within Visual Studio Developer Command Prompt 
		pause	
		exit /b 1
	)
)

IF NOT EXIST "%BOOST_DIR%\boost" (
	ECHO Can't find boost. Please set "BOOST" to boost path.
	pause
	exit /b 1
)

IF NOT EXIST "%QTDIR%\bin\qmake.exe" (
	ECHO Can't find qmake.exe. Please specify "QTDIR".
	pause
	exit /b 1
)


:menu
::cls
echo;
echo Current config:
echo;
echo   QMAKESPEC : %QMAKESPEC%
echo   QTDIR     : %QTDIR%
echo   BOOST_DIR : %BOOST_DIR%
echo;
echo;
echo Select build mode:
echo;
echo   (1) make makefiles
echo   (2) clean
echo   (3) build
echo   (4) rebuild
echo   (5) run
echo   (6) build doc
echo   (7) setup
echo   (8) all
echo   (q) quit
choice /C 12345678q /N
:: value "0" is reserved!
if %errorlevel%==9 exit /b 0
if %errorlevel%==8 goto cmd_all
if %errorlevel%==7 goto cmd_setup
if %errorlevel%==6 goto cmd_build_doc
if %errorlevel%==5 goto cmd_run
if %errorlevel%==4 goto cmd_rebuild
if %errorlevel%==3 goto cmd_build
if %errorlevel%==2 goto cmd_clean
if %errorlevel%==1 goto cmd_make_makefiles
GOTO the_end

:cmd_make_makefiles
	%~dp0.\_stopwatch start build
	call :generate_makefiles || goto the_error
goto the_end

:cmd_clean
	%~dp0.\_stopwatch start build
	call :make_clean || goto the_error
goto the_end

:cmd_build
	%~dp0.\_stopwatch start build
	call :make_build || goto the_error
goto the_end

:cmd_rebuild
	%~dp0.\_stopwatch start build
	call :make_clean || goto the_error
	call :make_build || goto the_error
goto the_end

:cmd_run
	pushd "%BIN_DIR%"
	ipponboard.exe
	if errorlevel 1 (
		popd
		goto the_error
	)
	popd
goto menu

:cmd_build_doc
	%~dp0.\_stopwatch start build
	call :make_doc || goto the_error
goto the_end

:cmd_setup
	%~dp0.\_stopwatch start build
	call :make_setup || goto the_error
goto the_end

:cmd_all
	%~dp0.\_stopwatch start build
	call :make_clean || goto the_error
	call :make_build || goto the_error
	call :make_setup || goto the_error
goto the_end

::-------------------------------
:generate_makefiles
	pushd "%BASE_DIR%"
	echo --[Creating makefiles]--
	qmake -recursive || exit /b %errorlevel%
	popd
exit /b 0

::-------------------------------
:make_clean
	echo;
	echo --[clean]--
	rd /Q /S "%BIN_DIR%" >nul 2>&1
	if exist "%BASE_DIR%\base\versioninfo.h" del "%BASE_DIR%\base\versioninfo.h" >nul || exit /b %errorlevel%

	call :clean test || exit /b %errorlevel%
	call :clean base || exit /b %errorlevel%
	call :clean GamepadDemo || exit /b %errorlevel%
exit /b 0

::-------------------------------
:clean
	echo;
	echo -- Cleaning %1
	pushd %BASE_DIR%\%1
	jom /S /L clean>nul 2>&1 || exit /b %errorlevel%
	popd
exit /b 0

::-------------------------------
:make_build
	echo;
	echo --[build]--
	call :compile test debug || exit /b %errorlevel%
		pushd "%BASE_DIR%\test\bin"
		IpponboardTest.exe & popd 
		if errorlevel 1 exit /b 1
	call :compile base || exit /b %errorlevel%
	call :compile GamepadDemo || exit /b %errorlevel%
	::call ::make_doc || exit /b %errorlevel%
	::call %~dp0.\_copy_files.cmd -release || exit /b %errorlevel%
exit /b 0

::-------------------------------
:compile
	echo;
	echo -- Compiling %1
	pushd %BASE_DIR%\%1
	if "%2"=="debug" (
		jom /L /S /F Makefile.Debug /J 1 || exit /b %errorlevel%
	) else (
		jom /L /S /F Makefile.Release /J 1 || exit /b %errorlevel%
	)
	popd
exit /b 0

::-------------------------------
:make_doc
	echo;
	echo -- building doc
	call _build_doc.cmd || exit /b %errorlevel%
exit /b 0

::-------------------------------
:make_setup
	echo;
	echo --[setup]--
	if not exist "%INNO_DIR%\iscc.exe" (
		echo Error: iscc.exe not found or INNO_DIR not defined!
		exit /b 1
	)
	
	"%INNO_DIR%\iscc.exe" /Q /O"%BIN_DIR%" "%BASE_DIR%\setup\setup.iss" || exit /b %errorlevel%
	dir /OD "%BIN_DIR%"
exit /b 0

::-------------------------------
:the_error
echo.
echo FAILED (code=%errorlevel%)
pause
goto menu

::-------------------------------
:the_end
echo.
echo SUCCESS
%~dp0.\_stopwatch stop build "Time Elapsed: {1}"
pause
goto menu
