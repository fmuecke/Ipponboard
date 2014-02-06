::---------------------------------------------------------
:: Ipponboard build script
::
:: Copyright 2010-2013 Florian Muecke. All rights reserved.
:: http://www.ipponboard.info (ipponboardinfo at googlemail dot com)
::
:: THIS FILE IS PART OF THE IPPONBOARD PROJECT.
:: IT MAY NOT BE DISTRIBUTED TO OR SHARED WITH THE PUBLIC IN ANY FORM!
::
:: $Id$
::---------------------------------------------------------
@echo off
SETLOCAL
SET LOCAL_CONFIG=env_cfg.bat

IF EXIST "%LOCAL_CONFIG%" (
  CALL "%LOCAL_CONFIG%"
  echo;
) ELSE (
  echo @echo off>"%LOCAL_CONFIG%"
  echo set QTDIR=c:\development\qt\qt-4.8.5-vc12xp\bin>>"%LOCAL_CONFIG%"
  echo set QMAKESPEC=win32-msvc2012>>"%LOCAL_CONFIG%"
  echo set BOOST_DIR=c:\development\boost\boost_1_55_0>>"%LOCAL_CONFIG%"
  rem echo set PATH=%QTDIR%;%PATH%>>%LOCAL_CONFIG%
  echo Please configure paths in "%LOCAL_CONFIG%" first!
  pause
  GOTO :EOF
)

SET BASE_DIR=%CD%
SET BUILD_DIR=%BASE_DIR%\_build\build_output\~tmp
SET BUILD_DIR_TEAM=%BASE_DIR%\_build\build_output\~tmp_TE

IF "%QMAKESPEC%"=="win32-msvc2012" (
	CALL "%VS120COMNTOOLS%..\..\vc\vcvarsall.bat" x86
)

IF NOT EXIST "%BOOST_DIR%\boost" (
	ECHO Can't find boost. Please set "BOOST" to boost path.
	pause
	GOTO :EOF
)

IF NOT EXIST "%QTDIR%\qmake.exe" (
	ECHO Can't find qmake.exe. Please specify "QTDIR".
	pause
	GOTO :EOF
)

cls
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
	echo;
	echo --[make clean]--
	rem del /Q "%BASE_DIR%\base\.buildnr"
	rd /Q /S "%BASE_DIR%\bin"
	rd /Q /S "%BASE_DIR%\lib"
	"%QTDIR%"\qmake -recursive
	if errorlevel 1 pause

	jom /S /L clean>nul 
	if errorlevel 1 pause
	if not "%1"=="internal" pause
GOTO :EOF

:build_clean
	echo;
	echo --[build clean]--
	CALL :make_clean internal
	CALL :build_incremental internal
	if not "%1"=="internal" pause
GOTO :EOF

:build_incremental
	echo;
	echo --[build incremental]--
	"%QTDIR%"\qmake -recursive
	if errorlevel 1 pause
	::jom /L /S /F Makefile release
	::if errorlevel 1 pause
	CALL :do_compile core
	CALL :do_compile gamepad
	CALL :do_compile SingleTournament
	CALL :do_compile TeamTournament
	::CALL :do_compile VersionSelector
	CALL :do_compile GamepadDemo
	if not "%1"=="internal" pause
GOTO :EOF

:build_setup
	echo;
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
	echo;
	echo --[build all]--
	CALL :build_clean internal
	CALL :build_setup internal
	if not "%1"=="internal" pause
GOTO :EOF

:do_compile
	echo;
	echo -- Compiling %1
	pushd %1
	jom /L /S /F Makefile.Release
	if errorlevel 1 pause
	popd
GOTO :EOF

:the_end
pause