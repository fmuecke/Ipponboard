@echo off
REM -- language file
call ..\env_cfg.bat

SET DEST=..\bin
SET THIRDPARTY=..\..\3rdParty

rem (mkdir is recursive) if not exist "%DEST%" mkdir "%DEST%">nul
if not exist "%DEST%\lang" mkdir "%DEST%\lang">nul
"%QTDIR%\lrelease" -compress -silent ..\i18n\core_de.ts -qm ..\i18n\core_de.qm
"%QTDIR%\lrelease" -compress -silent ..\i18n\Ipponboard_de.ts -qm ..\i18n\Ipponboard_de.qm
copy /Y "..\i18n\core_de.qm" "%DEST%\lang">nul
copy /Y "..\i18n\Ipponboard_de.qm" "%DEST%\lang">nul

REM -- sounds
if not exist "%DEST%\sounds" mkdir "%DEST%\sounds">nul
copy /Y "..\base\sounds\buzzer.wav" "%DEST%\sounds">nul
copy /Y "%THIRDPARTY%\sounds\*.wav" "%DEST%\sounds">nul

REM -- binaries
copy /Y "%THIRDPARTY%\qt\4.8.6-vc120xp\*.dll" "%DEST%">nul
copy /Y "%THIRDPARTY%\msvc120\*.dll" "%DEST%">nul

REM -- doc
rem copy /Y "%DEST%\doc\Anleitung.pdf" "%DEST%">nul
rem copy /Y "%DEST%\doc\manual.pdf" "%DEST%">nul

REM -- categories
copy /Y "..\base\categories.json" "%DEST%">nul

REM -- programme
REM if not exist "%DEST%\Ipponboard.exe" (
	REM echo ERROR: Ipponboard.exe not found
	REM pause
	REM exit 1
REM )

copy /Y "..\CHANGELOG.txt" "%DEST%">nul