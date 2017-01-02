::---------------------------------------------------------
:: Ipponboard build script
::
:: Copyright 2010-2017 Florian Muecke. All rights reserved.
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
  exit /b 1
)

SET BASE_DIR=%CD%
SET BUILD_DIR=%BASE_DIR%\_build\build_output\~tmp
SET BUILD_DIR_TEAM=%BASE_DIR%\_build\build_output\~tmp_TE

IF "%QMAKESPEC%"=="win32-msvc2013" (
	CALL "%VS120COMNTOOLS%..\..\vc\vcvarsall.bat" x86
)
IF "%QMAKESPEC%"=="win32-msvc2015" (
	CALL "%VS140COMNTOOLS%..\..\vc\vcvarsall.bat" x86
)

IF NOT EXIST "%BOOST_DIR%\boost" (
	ECHO Can't find boost. Please set "BOOST" to boost path.
	pause
	exit /b 1
)

IF NOT EXIST "%QTDIR%\qmake.exe" (
	ECHO Can't find qmake.exe. Please specify "QTDIR".
	pause
	exit /b 1
)

:menu
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
echo   (1) clean
echo   (2) build
echo   (3) rebuild
echo   (4) setup
echo   (5) run
echo   (9) all
echo   (q) quit
choice /C 123459q /N
:: value "0" is reserved!
if %errorlevel%==7 exit /b 0
if %errorlevel%==6 goto cmd_all
if %errorlevel%==5 goto cmd_run
if %errorlevel%==4 goto cmd_setup
if %errorlevel%==3 goto cmd_rebuild
if %errorlevel%==2 goto cmd_build
if %errorlevel%==1 goto cmd_clean
GOTO the_end

:cmd_run
	pushd %BASE_DIR%\bin
	ipponboard.exe
	if errorlevel 1 (
		popd
		goto the_error
	)
goto the_end

:cmd_all
	_build\stopwatch start build
	call :make_clean || goto the_error
	call :make_build || goto the_error
	call :make_setup || goto the_error
goto the_end

:cmd_setup
	_build\stopwatch start build
	call :make_setup || goto the_error
goto the_end


:cmd_build
	_build\stopwatch start build
	call :make_build || goto the_error
goto the_end

:cmd_rebuild
	_build\stopwatch start build
	call :make_clean || goto the_error
	call :make_build || goto the_error
goto the_end

:cmd_clean
	_build\stopwatch start build
	call :make_clean || goto the_error
goto the_end

::-------------------------------
:make_clean
	echo;
	echo --[clean]--
	rd /Q /S "%BASE_DIR%\bin" >nul 2>&1
	rd /Q /S "%BASE_DIR%\lib" >nul 2>&1
	if exist "%BASE_DIR%\base\versioninfo.h" del "%BASE_DIR%\base\versioninfo.h" >nul || exit /b %errorlevel%
	call :generate_makefiles || exit /b %errorlevel%
	jom /S /L clean>nul 2>&1 || exit /b %errorlevel%
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
	::call :compile VersionSelector || exit /b %errorlevel%
	call :compile GamepadDemo || exit /b %errorlevel%
exit /b 0

::-------------------------------
:make_setup
	echo;
	echo --[setup]--
	if not exist "%INNO_DIR%\iscc.exe" (
		echo Error: iscc.exe not found or INNO_DIR not defined!
		exit /b 1
	)
	CALL _build\scripts\clear_build_dir.cmd || exit /b %errorlevel%

	rem CALL _build\scripts\Ipponboard_copy_files.cmd
	rem if errorlevel 1 exit /b %errorlevel%
	
	"%INNO_DIR%\iscc.exe" /Q /O"%BASE_DIR%\_output" "%BASE_DIR%\setup\setup.iss" || exit /b %errorlevel%
	dir /OD "%BASE_DIR%\_output"
exit /b 0


:compile
	echo;
	echo -- Compiling %1
	pushd %1
	if "%2"=="debug" (
		jom /L /S /F Makefile.Debug || exit /b %errorlevel%
	) else (
		jom /L /S /F Makefile.Release || exit /b %errorlevel%
	)
	popd
exit /b 0


:generate_makefiles
	echo -- Creating makefiles
	"%QTDIR%"\qmake -recursive || exit /b %errorlevel%
exit /b 0


:the_error
echo.
echo FAILED (code=%errorlevel%)
pause
goto menu

:the_end
echo.
echo SUCCESS
_build\stopwatch stop build "Time Elapsed: {1}"
pause
goto menu
