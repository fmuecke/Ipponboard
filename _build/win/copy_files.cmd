::---------------------------------------------------------
:: Copyright 2018 Florian Muecke. All rights reserved.
:: Use of this source code is governed by a BSD-style license that can be
:: found in the LICENSE.txt file.
::---------------------------------------------------------
@echo off
setlocal

set DEST_DIR=%1
set CONFIG=%2

if "%CONFIG%" == "" (
  echo argument CONFIG missing for %~f0. Please specify "Debug" or "Release".
  exit /b 1
)
if "%DEST_DIR%" == "" (
  echo argument DEST_DIR missing for %~f0
  exit /b 1
)

if "%CONFIG%"=="Debug" set DEST_DIR=%DEST_DIR%-debug
if "%CONFIG%"=="debug" set DEST_DIR=%DEST_DIR%-debug
if "%CONFIG%"=="release" set RELEASE=1
if "%CONFIG%"=="Release" set RELEASE=1
echo Using: CONFIG=%CONFIG%
echo Using: DEST_DIR=%DEST_DIR%

call %~dp0init_env_cfg.cmd > nul
if not defined IPPONBOARD_ROOT_DIR (
  echo IPPONBAORD_ROOT_DIR not defined %~f0 - maybe %~dp0init_env_cfg.cmd not configured?
  exit /b 1
)

if not defined VCToolsRedistDir (
	echo VCToolsRedistDir not defined, starting VS developer command line...
	call "%MSVC_CMD%" > nul
)
if "%VCToolsRedistDir%"=="" echo "VCToolsRedistDir still not defined" && exit /b 1

echo Copying files...
echo - languages
rem (mkdir is recursive) if not exist "%DEST_DIR%" mkdir "%DEST_DIR%">nul
if not exist "%DEST_DIR%\lang" mkdir "%DEST_DIR%\lang">nul
"%QTDIR%\bin\lrelease" -compress -silent %IPPONBOARD_ROOT_DIR%\i18n\de.ts -qm %IPPONBOARD_ROOT_DIR%\i18n\de.qm || exit /b 1
copy /Y "%IPPONBOARD_ROOT_DIR%\i18n\de.qm" "%DEST_DIR%\lang">nul || exit /b 1
"%QTDIR%\bin\lrelease" -compress -silent %IPPONBOARD_ROOT_DIR%\i18n\nl.ts -qm %IPPONBOARD_ROOT_DIR%\i18n\nl.qm || exit /b 1
copy /Y "%IPPONBOARD_ROOT_DIR%\i18n\nl.qm" "%DEST_DIR%\lang">nul || exit /b 1

echo - sounds
if not exist "%DEST_DIR%\sounds" mkdir "%DEST_DIR%\sounds">nul
copy /Y "%IPPONBOARD_ROOT_DIR%\base\sounds\buzzer.wav" "%DEST_DIR%\sounds">nul || exit /b 1
:: copy /Y "%THIRDPARTY%\sounds\*.wav" "%DEST_DIR%\sounds">nul
:: if errorlevel 1 exit /b 1

echo - QT and C^+^+ runtimes
if defined RELEASE (
if not exist "%QTDIR%\bin\QtCore4.dll" echo not found: "%QTDIR%\bin\QtCore4.dll" && exit /b 1
	copy /Y "%QTDIR%\bin\QtCore4.dll" "%DEST_DIR%">nul || exit /b 1
	copy /Y "%QTDIR%\bin\QtGui4.dll" "%DEST_DIR%">nul || exit /b 1
	copy /Y "%QTDIR%\bin\QtNetwork4.dll" "%DEST_DIR%">nul || exit /b 1
	copy /Y "%QTDIR%\bin\QtXmlPatterns4.dll" "%DEST_DIR%">nul || exit /b 1
	if not exist "%VCToolsRedistDir%x86\Microsoft.VC143.CRT\msvcp140.dll" echo not found: "%VCToolsRedistDir%x86\Microsoft.VC143.CRT\msvcp140.dll" && exit /b 1
	copy /Y "%VCToolsRedistDir%x86\Microsoft.VC143.CRT\msvcp140.dll" "%DEST_DIR%">nul || exit /b 1
	if not exist "%VCToolsRedistDir%x86\Microsoft.VC143.CRT\vcruntime140.dll" echo not found: "%VCToolsRedistDir%x86\Microsoft.VC143.CRT\vcruntime140.dll" && exit /b 1
	copy /Y "%VCToolsRedistDir%x86\Microsoft.VC143.CRT\vcruntime140.dll" "%DEST_DIR%">nul || exit /b 1
) else (
	if not exist "%QTDIR%\bin\QtCored4.dll" echo %QTDIR%\bin\QtCored4.dll not found && exit /b 1
	copy /Y "%QTDIR%\bin\QtCored4.dll" "%DEST_DIR%">nul || exit /b 1
	copy /Y "%QTDIR%\bin\QtGuid4.dll" "%DEST_DIR%">nul || exit /b 1
	copy /Y "%QTDIR%\bin\QtNetworkd4.dll" "%DEST_DIR%">nul || exit /b 1
	copy /Y "%QTDIR%\bin\QtXmlPatternsd4.dll" "%DEST_DIR%">nul || exit /b 1
	if not exist "%VCToolsRedistDir%debug_nonredist\x86\Microsoft.VC143.DebugCRT\msvcp140d.dll" echo not found: "%VCToolsRedistDir%debug_nonredist\x86\Microsoft.VC143.DebugCRT\msvcp140d.dll" && exit /b 1
	copy /Y "%VCToolsRedistDir%debug_nonredist\x86\Microsoft.VC143.DebugCRT\msvcp140d.dll" "%DEST_DIR%">nul || exit /b 1
	if not exist "%VCToolsRedistDir%debug_nonredist\x86\Microsoft.VC143.DebugCRT\vcruntime140d.dll" echo not found: "%VCToolsRedistDir%debug_nonredist\x86\Microsoft.VC143.DebugCRT\vcruntime140d.dll" && exit /b 1
	copy /Y "%VCToolsRedistDir%debug_nonredist\x86\Microsoft.VC143.DebugCRT\vcruntime140d.dll" "%DEST_DIR%">nul || exit /b 1
)

echo - Categories and modes
copy /Y "%IPPONBOARD_ROOT_DIR%\base\categories.json" "%DEST_DIR%">nul || exit /b 1
copy /Y "%IPPONBOARD_ROOT_DIR%\base\TournamentModes.ini" "%DEST_DIR%" > nul || exit /b 1

echo - Clubs
if not exist "%DEST_DIR%\clubs" mkdir "%DEST_DIR%\Clubs">nul
copy /Y "%IPPONBOARD_ROOT_DIR%\base\clubs.json" "%DEST_DIR%">nul || exit /b 1
copy /Y "%IPPONBOARD_ROOT_DIR%\base\clubs\*.png" "%DEST_DIR%\Clubs">nul || exit /b 1

echo - Templates
if not exist "%DEST_DIR%\templates" mkdir "%DEST_DIR%\templates">nul
copy /Y "%IPPONBOARD_ROOT_DIR%\base\templates\*.*" "%DEST_DIR%\Templates">nul || exit /b 1

exit /b 0