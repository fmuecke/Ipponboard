@echo off
REM -- language file
SET DEST=..\bin
SET THIRDPARTY=..\..\3rdParty

rem (mkdir is recursive) if not exist "%DEST%" mkdir "%DEST%">nul
if not exist "%DEST%\lang" mkdir "%DEST%\lang">nul
lrelease -compress -silent ..\i18n\core_de.ts -qm ..\i18n\core_de.qm
lrelease -compress -silent ..\i18n\Ipponboard_team_de.ts -qm ..\i18n\Ipponboard_team_de.qm
lrelease -compress -silent ..\i18n\VersionSelector_de.ts -qm ..\i18n\VersionSelector_de.qm
copy /Y "..\i18n\core_de.qm" "%DEST%\lang">nul
copy /Y "..\i18n\Ipponboard_team_de.qm" "%DEST%\lang">nul
copy /Y "..\i18n\VersionSelector_de.qm" "%DEST%\lang">nul

REM -- sounds
if not exist "%DEST%\sounds" mkdir "%DEST%\sounds">nul
copy /Y "..\base\sounds\buzzer.wav" "%DEST%\sounds">nul
copy /Y "%THIRDPARTY%\sounds\*.wav" "%DEST%\sounds">nul

REM -- binaries
IF "%QMAKESPEC%"=="win32-g++" (
	copy /Y "%THIRDPARTY%\Qt\4.8.4-mingw\*.dll" "%DEST%">nul
) ELSE (
	copy /Y "%THIRDPARTY%\Qt\4.8.3-vc100\*.dll" "%DEST%">nul
	copy /Y "%THIRDPARTY%\msvc100\*.dll" "%DEST%">nul
)

REM -- doc
rem copy /Y "%DEST%\doc\Anleitung.pdf" "%DEST%">nul
rem copy /Y "%DEST%\doc\manual.pdf" "%DEST%">nul

REM -- tournament modes
copy /y TournamentModes.ini %DEST%>nul

REM -- clubs
if not exist "%DEST%\clubs" mkdir "%DEST%\clubs">nul
copy /Y "clubs\clubs.xml" "%DEST%\clubs">nul
copy /Y "clubs\*.png" "%DEST%\clubs">nul

REM -- templates
if not exist "%DEST%\templates" mkdir "%DEST%\templates">nul
copy /Y "templates\*.*" "%DEST%\templates">nul

REM -- programme
REM if not exist "%DEST%\Ipponboard_team.exe" (
	REM echo ERROR: Ipponboard_team.exe not found
	REM pause
	REM exit 1
REM )

copy /Y "..\CHANGELOG.txt" "%DEST%">nul