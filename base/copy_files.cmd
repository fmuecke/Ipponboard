@echo off
setlocal
REM -- language file
call %~dp0..\env_cfg.bat

set ROOT=%~dp0..\
set DEST=%ROOT%\bin
set THIRDPARTY=..\..\3rdParty
if "%1"=="-release" set RELEASE=1

echo Copying files...
echo  languages
rem (mkdir is recursive) if not exist "%DEST%" mkdir "%DEST%">nul
if not exist "%DEST%\lang" mkdir "%DEST%\lang">nul
"%QTDIR%\lrelease" -compress -silent %ROOT%\i18n\de.ts -qm %ROOT%\i18n\de.qm || exit /b 1
copy /Y "%ROOT%\i18n\de.qm" "%DEST%\lang">nul || exit /b 1
"%QTDIR%\lrelease" -compress -silent %ROOT%\i18n\nl.ts -qm %ROOT%\i18n\nl.qm || exit /b 1
copy /Y "%ROOT%\i18n\nl.qm" "%DEST%\lang">nul || exit /b 1

echo  sounds
if not exist "%DEST%\sounds" mkdir "%DEST%\sounds">nul
copy /Y "%ROOT%\base\sounds\buzzer.wav" "%DEST%\sounds">nul || exit /b 1
copy /Y "%THIRDPARTY%\sounds\*.wav" "%DEST%\sounds">nul
:: if errorlevel 1 exit /b 1

echo  QT and C^+^+ runtimes
if defined RELEASE (
	copy /Y "%QTDIR%\QtCore4.dll" "%DEST%">nul || exit /b 1
	copy /Y "%QTDIR%\QtGui4.dll" "%DEST%">nul || exit /b 1
	copy /Y "%QTDIR%\QtNetwork4.dll" "%DEST%">nul || exit /b 1
	copy /Y "%QTDIR%\QtXmlPatterns4.dll" "%DEST%">nul || exit /b 1
	copy /Y "c:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\redist\x86\Microsoft.VC140.CRT\msvcp140.dll" "%DEST%">nul || exit /b 1
	copy /Y "c:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\redist\x86\Microsoft.VC140.CRT\vcruntime140.dll" "%DEST%">nul || exit /b 1
) else (
	copy /Y "%QTDIR%\QtCored4.dll" "%DEST%">nul || exit /b 1
	copy /Y "%QTDIR%\QtGuid4.dll" "%DEST%">nul || exit /b 1
	copy /Y "%QTDIR%\QtNetworkd4.dll" "%DEST%">nul || exit /b 1
	copy /Y "%QTDIR%\QtXmlPatternsd4.dll" "%DEST%">nul || exit /b 1
	copy /Y "c:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\redist\debug_nonredist\x86\Microsoft.VC140.DebugCRT\\msvcp140d.dll" "%DEST%">nul || exit /b 1
	copy /Y "c:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\redist\debug_nonredist\x86\Microsoft.VC140.DebugCRT\vcruntime140d.dll" "%DEST%">nul || exit /b 1
)

REM -- doc
rem copy /Y "%DEST%\doc\Anleitung.pdf" "%DEST%">nul
rem copy /Y "%DEST%\doc\manual.pdf" "%DEST%">nul

echo  Categories and modes
copy /Y "%ROOT%\base\categories.json" "%DEST%">nul || exit /b 1
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