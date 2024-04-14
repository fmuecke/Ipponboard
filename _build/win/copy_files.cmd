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

echo - msvc runtimes
set "vcRedistFiles=msvcp140.dll vcruntime140.dll"
set "vcRedistDebugFiles=msvcp140d.dll vcruntime140d.dll"

if "%RELEASE%"=="1" (
	for %%F in (%vcRedistFiles%) do (copy /Y "%VCToolsRedistDir%x86\Microsoft.VC143.CRT\%%F" "%DEST_DIR%">nul || exit /b 1)
) else (
	for %%F in (%vcRedistDebugFiles%) do (copy /Y "%VCToolsRedistDir%debug_nonredist\x86\Microsoft.VC143.DebugCRT\%%F" "%DEST_DIR%">nul || exit /b 1)
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