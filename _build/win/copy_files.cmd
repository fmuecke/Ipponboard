::---------------------------------------------------------
:: Copyright 2018 Florian Muecke. All rights reserved.
:: Use of this source code is governed by a BSD-style license that can be
:: found in the LICENSE.txt file.
::---------------------------------------------------------
@echo off
setlocal

set ROOT_DIR=%1
if "%ROOT_DIR%" == "" (
  echo argument ROOT_DIR missing for %~f0
  exit /b 1
) else (
  echo using ROOT_DIR=%ROOT_DIR%
)

call %~dp0init_env_cfg.cmd

if not defined VCToolsRedistDir (
	echo "VCToolsRedistDir not defined, starting VS developer command line..." 
	call "%MSVC_CMD%"
)
if "%VCToolsRedistDir%"=="" echo "VCToolsRedistDir still not defined" && exit /b 1

set ROOT_DIR=%~dp0..\
set DEST=%ROOT_DIR%\_build\bin
if "%2"=="-release" set RELEASE=1

echo Copying files...
echo  languages
rem (mkdir is recursive) if not exist "%DEST%" mkdir "%DEST%">nul
if not exist "%DEST%\lang" mkdir "%DEST%\lang">nul
"%QTDIR%\bin\lrelease" -compress -silent %ROOT_DIR%\i18n\de.ts -qm %ROOT_DIR%\i18n\de.qm || exit /b 1
copy /Y "%ROOT_DIR%\i18n\de.qm" "%DEST%\lang">nul || exit /b 1
"%QTDIR%\bin\lrelease" -compress -silent %ROOT_DIR%\i18n\nl.ts -qm %ROOT_DIR%\i18n\nl.qm || exit /b 1
copy /Y "%ROOT_DIR%\i18n\nl.qm" "%DEST%\lang">nul || exit /b 1

echo  sounds
if not exist "%DEST%\sounds" mkdir "%DEST%\sounds">nul
copy /Y "%ROOT_DIR%\base\sounds\buzzer.wav" "%DEST%\sounds">nul || exit /b 1
:: copy /Y "%THIRDPARTY%\sounds\*.wav" "%DEST%\sounds">nul
:: if errorlevel 1 exit /b 1

echo  QT and C^+^+ runtimes
if defined RELEASE (
if not exist "%QTDIR%\bin\QtCore4.dll" echo not found: "%QTDIR%\bin\QtCore4.dll" && exit /b 1
	copy /Y "%QTDIR%\bin\QtCore4.dll" "%DEST%">nul || exit /b 1
	copy /Y "%QTDIR%\bin\QtGui4.dll" "%DEST%">nul || exit /b 1
	copy /Y "%QTDIR%\bin\QtNetwork4.dll" "%DEST%">nul || exit /b 1
	copy /Y "%QTDIR%\bin\QtXmlPatterns4.dll" "%DEST%">nul || exit /b 1
	if not exist "%VCToolsRedistDir%x86\Microsoft.VC143.CRT\msvcp140.dll" echo not found: "%VCToolsRedistDir%x86\Microsoft.VC143.CRT\msvcp140.dll" && exit /b 1
	copy /Y "%VCToolsRedistDir%x86\Microsoft.VC143.CRT\msvcp140.dll" "%DEST%">nul || exit /b 1
	if not exist "%VCToolsRedistDir%x86\Microsoft.VC143.CRT\vcruntime140.dll" echo not found: "%VCToolsRedistDir%x86\Microsoft.VC143.CRT\vcruntime140.dll" && exit /b 1
	copy /Y "%VCToolsRedistDir%x86\Microsoft.VC143.CRT\vcruntime140.dll" "%DEST%">nul || exit /b 1
) else (
	if not exist "%QTDIR%\bin\QtCored4.dll" echo %QTDIR%\bin\QtCored4.dll not found && exit /b 1
	copy /Y "%QTDIR%\bin\QtCored4.dll" "%DEST%">nul || exit /b 1
	copy /Y "%QTDIR%\bin\QtGuid4.dll" "%DEST%">nul || exit /b 1
	copy /Y "%QTDIR%\bin\QtNetworkd4.dll" "%DEST%">nul || exit /b 1
	copy /Y "%QTDIR%\bin\QtXmlPatternsd4.dll" "%DEST%">nul || exit /b 1
	if not exist "%VCToolsRedistDir%debug_nonredist\x86\Microsoft.VC143.DebugCRT\msvcp140d.dll" echo not found: "%VCToolsRedistDir%debug_nonredist\x86\Microsoft.VC143.DebugCRT\msvcp140d.dll" && exit /b 1
	copy /Y "%VCToolsRedistDir%debug_nonredist\x86\Microsoft.VC143.DebugCRT\msvcp140d.dll" "%DEST%">nul || exit /b 1
	if not exist "%VCToolsRedistDir%debug_nonredist\x86\Microsoft.VC143.DebugCRT\vcruntime140d.dll" echo not found: "%VCToolsRedistDir%debug_nonredist\x86\Microsoft.VC143.DebugCRT\vcruntime140d.dll" && exit /b 1
	copy /Y "%VCToolsRedistDir%debug_nonredist\x86\Microsoft.VC143.DebugCRT\vcruntime140d.dll" "%DEST%">nul || exit /b 1
)

REM -- doc
rem copy /Y "%DEST%\doc\Anleitung.pdf" "%DEST%">nul
rem copy /Y "%DEST%\doc\manual.pdf" "%DEST%">nul

echo  Categories and modes
copy /Y "%ROOT_DIR%\base\categories.json" "%DEST%">nul || exit /b 1
copy /Y TournamentModes.ini "%DEST%">nul || exit /b 1

echo  Clubs
if not exist "%DEST%\clubs" mkdir "%DEST%\Clubs">nul
copy /Y "clubs.json" "%DEST%">nul || exit /b 1
copy /Y "clubs\*.png" "%DEST%\Clubs">nul || exit /b 1

echo  Templates
if not exist "%DEST%\templates" mkdir "%DEST%\templates">nul
copy /Y "templates\*.*" "%DEST%\Templates">nul || exit /b 1

REM -- programme
REM if not exist "%DEST%\Ipponboard.exe" (
	REM echo ERROR: Ipponboard.exe not found
	REM pause
	REM exit 1
REM )

exit /b 0