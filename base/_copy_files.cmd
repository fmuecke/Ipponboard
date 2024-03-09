@echo off
setlocal

set ROOT=%~dp0..
set THIRDPARTY=..\..\3rdParty
if "%1"=="-release" set RELEASE=1
if defined RELEASE (set DEST=%ROOT%\_build\bin\Release) else (set DEST=%ROOT%\_build\bin\Debug)

echo Copying files to %DEST% using QTDIR=%QTDIR%

echo  languages
rem (mkdir is recursive) if not exist "%DEST%" mkdir "%DEST%">nul
if not exist "%DEST%\lang" mkdir "%DEST%\lang">nul
"%QTDIR%\bin\lrelease" -compress -silent %ROOT%\i18n\de.ts -qm %ROOT%\i18n\de.qm || exit /b 1
copy /Y "%ROOT%\i18n\de.qm" "%DEST%\lang">nul || exit /b 1
"%QTDIR%\bin\lrelease" -compress -silent %ROOT%\i18n\nl.ts -qm %ROOT%\i18n\nl.qm || exit /b 1
copy /Y "%ROOT%\i18n\nl.qm" "%DEST%\lang">nul || exit /b 1

echo  sounds
if not exist "%DEST%\sounds" mkdir "%DEST%\sounds">nul
copy /Y "%ROOT%\base\sounds\*.wav" "%DEST%\sounds">nul || exit /b 1
copy /Y "%THIRDPARTY%\sounds\*.wav" "%DEST%\sounds">nul
:: if errorlevel 1 exit /b 1

echo  QT and C^+^+ runtimes
if defined RELEASE (
	copy /Y "%QTDIR%\bin\QtCore4.dll" "%DEST%">nul || exit /b 1
	copy /Y "%QTDIR%\bin\QtGui4.dll" "%DEST%">nul || exit /b 1
	copy /Y "%QTDIR%\bin\QtNetwork4.dll" "%DEST%">nul || exit /b 1
	copy /Y "%QTDIR%\bin\QtXmlPatterns4.dll" "%DEST%">nul || exit /b 1

	copy /Y "%REDIST_DIR_x86%\msvcp140.dll" "%DEST%">nul
	copy /Y "%REDIST_DIR_x86%\vcruntime140.dll" "%DEST%">nul 
	copy /Y "%REDIST_DIR_x64%\msvcp140.dll" "%DEST%">nul
	copy /Y "%REDIST_DIR_x64%\vcruntime140.dll" "%DEST%">nul
) else (
	copy /Y "%QTDIR%\bin\QtCored4.dll" "%DEST%">nul || exit /b 1
	copy /Y "%QTDIR%\bin\QtGuid4.dll" "%DEST%">nul || exit /b 1
	copy /Y "%QTDIR%\bin\QtNetworkd4.dll" "%DEST%">nul || exit /b 1
	copy /Y "%QTDIR%\bin\QtXmlPatternsd4.dll" "%DEST%">nul || exit /b 1

	copy /Y "%REDIST_DIR_x86d%\msvcp140d.dll" "%DEST%">nul
	copy /Y "%REDIST_DIR_x86d%\vcruntime140d.dll" "%DEST%">nul
	copy /Y "%REDIST_DIR_x64d%\msvcp140d.dll" "%DEST%">nul
	copy /Y "%REDIST_DIR_x64d%\vcruntime140d.dll" "%DEST%">nul
)

REM -- doc
rem copy /Y "%DEST%\doc\Anleitung.pdf" "%DEST%">nul
rem copy /Y "%DEST%\doc\manual.pdf" "%DEST%">nul

echo  Categories and modes
copy /Y "%ROOT%\base\categories.json" "%DEST%">nul || exit /b 1
copy /Y "%ROOT%\base\TournamentModes.ini" "%DEST%">nul || exit /b 1

echo  Clubs
if not exist "%DEST%\clubs" mkdir "%DEST%\Clubs">nul
copy /Y "%ROOT%\base\clubs.json" "%DEST%">nul || exit /b 1
copy /Y "%ROOT%\base\clubs\*.png" "%DEST%\Clubs">nul || exit /b 1

echo  Templates
if not exist "%DEST%\templates" mkdir "%DEST%\templates">nul
copy /Y "%ROOT%\base\templates\*.*" "%DEST%\Templates">nul || exit /b 1

REM -- programme
REM if not exist "%DEST%\Ipponboard.exe" (
	REM echo ERROR: Ipponboard.exe not found
	REM pause
	REM exit 1
REM )
endlocal
exit /b 0