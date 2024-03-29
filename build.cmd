::---------------------------------------------------------
:: Copyright 2018 Florian Muecke. All rights reserved.
:: Use of this source code is governed by a BSD-style license that can be
:: found in the LICENSE.txt file.
::---------------------------------------------------------
@echo off
SETLOCAL
SET LOCAL_CONFIG=env_cfg.bat

IF EXIST "%LOCAL_CONFIG%" (
  CALL "%LOCAL_CONFIG%"
  echo;
) ELSE (
  echo @echo off>"%LOCAL_CONFIG%"
  echo set QTDIR=c:\devtools\qt\qt-4.8.7-vc14\bin\>>"%LOCAL_CONFIG%"
  echo set QMAKESPEC=win32-msvc2015>>"%LOCAL_CONFIG%"
  echo set BOOST_DIR=c:\devtools\boost_1_81_0>>"%LOCAL_CONFIG%"
  echo set "MSVC_CMD=C:\Program Files\Microsoft Visual Studio\2022\Professional\VC\Auxiliary\Build\vcvars32.bat">>"%LOCAL_CONFIG%"
  echo set INNO_DIR=c:\devtools\inno setup 5>>"%LOCAL_CONFIG%"
  rem echo set PATH=%QTDIR%;%PATH%>>%LOCAL_CONFIG%
  echo Please configure paths in "%LOCAL_CONFIG%" first!
  pause
  exit /b 1
)

SET BASE_DIR=%~dp0.
SET BIN_DIR=%BASE_DIR%\bin
IF NOT EXIST "%BIN_DIR%" mkdir "%BIN_DIR%"

rem check for compiler
call "%MSVC_CMD%"
cl > nul 2>&1
IF ERRORLEVEL 1 (
	ECHO Can't find C++ compiler tools. Please run build.cmd from within Visual Studio x86 Developer Command Prompt 
		pause	
		exit /b 1
	)
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
echo   (6) build doc
echo   (9) all
echo   (q) quit
choice /C 1234569q /N
:: value "0" is reserved!
if %errorlevel%==8 exit /b 0
if %errorlevel%==7 goto cmd_all
if %errorlevel%==6 goto cmd_build_doc
if %errorlevel%==5 goto cmd_run
if %errorlevel%==4 goto cmd_setup
if %errorlevel%==3 goto cmd_rebuild
if %errorlevel%==2 goto cmd_build
if %errorlevel%==1 goto cmd_clean
GOTO the_end

:cmd_run
	pushd "%BIN_DIR%"
	ipponboard.exe
	if errorlevel 1 (
		popd
		goto the_error
	)
	popd
goto menu

:cmd_all
	%BASE_DIR%\_build\stopwatch start build
	call :make_clean || goto the_error
	call :make_build || goto the_error
	call :make_setup || goto the_error
goto the_end

:cmd_build_doc
	%BASE_DIR%\_build\stopwatch start build
	call :make_doc || goto the_error
goto the_end

:cmd_setup
	%BASE_DIR%\_build\stopwatch start build
	call :make_setup || goto the_error
goto the_end


:cmd_build
	%BASE_DIR%\_build\stopwatch start build
	call :make_build || goto the_error
goto the_end

:cmd_rebuild
	%BASE_DIR%\_build\stopwatch start build
	call :make_clean || goto the_error
	call :make_build || goto the_error
goto the_end

:cmd_clean
	%BASE_DIR%\_build\stopwatch start build
	call :make_clean || goto the_error
goto the_end

::-------------------------------
:make_clean
	echo;
	echo --[clean]--
	rd /Q /S "%BIN_DIR%" >nul 2>&1
	rd /Q /S "%BASE_DIR%\lib" >nul 2>&1
	if exist "%BASE_DIR%\base\versioninfo.h" del "%BASE_DIR%\base\versioninfo.h" >nul || exit /b %errorlevel%
	::if NOT exist "%BASE_DIR%\base\versioninfo.h" echo Removed %BASE_DIR%\base\versioninfo.h
	jom /S /L clean>nul 2>&1 || exit /b %errorlevel%
	call :generate_makefiles || exit /b %errorlevel%
	jom /S /L clean>nul 2>&1 || exit /b %errorlevel%
	::if exist "%BASE_DIR%\base\versioninfo.h" echo Created %BASE_DIR%\base\versioninfo.h
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
	call ::make_doc || exit /b %errorlevel%
	call "%BASE_DIR%\base\copy_files.cmd" -release || exit /b %errorlevel%
exit /b 0

::-------------------------------
:make_doc
	echo;
	echo -- building doc
	call "%BASE_DIR%\doc\build_doc.cmd" || exit /b %errorlevel%
exit /b 0

::-------------------------------
:make_setup
	echo;
	echo --[setup]--
	if not exist "%INNO_DIR%\iscc.exe" (
		echo Error: iscc.exe not found or INNO_DIR not defined!
		exit /b 1
	)
	
	"%INNO_DIR%\iscc.exe" /Q /O"%BASE_DIR%\_output" "%BASE_DIR%\setup\setup.iss" || exit /b %errorlevel%
	dir /OD "%BASE_DIR%\_output"
exit /b 0


:compile
	echo;
	echo -- Compiling %1
	pushd %BASE_DIR%\%1
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
%BASE_DIR%\_build\stopwatch stop build "Time Elapsed: {1}"
pause
goto menu
