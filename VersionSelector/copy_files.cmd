@echo off
REM -- language file
SET DEST=..\bin
SET THIRDPARTY=..\..\3rdParty

rem (mkdir is recursive) if not exist "%DEST%" mkdir "%DEST%">nul
if not exist "%DEST%\lang" mkdir "%DEST%\lang">nul
copy /Y "..\i18n\VersionSelector_de.qm" "%DEST%\lang">nul

REM -- binaries
copy /Y "%THIRDPARTY%\Qt-4.7.4-vc100\*.dll" "%DEST%">nul
copy /Y "%THIRDPARTY%\msvc100\*.dll" "%DEST%">nul

REM -- programme
if not exist "%DEST%\VersionSelector.exe" (
	echo ERROR: VersionSelector.exe not found
	pause
	exit 1
)
