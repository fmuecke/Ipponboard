REM -- language file
copy "%BASE_DIR%\i18n\Ipponboard_de.qm" "%BUILD_DIR%">nul
copy "%BASE_DIR%\i18n\Ipponboard_en.qm" "%BUILD_DIR%">nul

REM -- sound files
MKDIR "%BUILD_DIR%\sounds">nul
copy "%BASE_DIR%\base\sounds\buzzer.wav" "%BUILD_DIR%\sounds">nul
copy "%BASE_DIR%\..\3rdParty\sounds\alarm.wav" "%BUILD_DIR%\sounds">nul
copy "%BASE_DIR%\..\3rdParty\sounds\alert.wav" "%BUILD_DIR%\sounds">nul
copy "%BASE_DIR%\..\3rdParty\sounds\bells.wav" "%BUILD_DIR%\sounds">nul
copy "%BASE_DIR%\..\3rdParty\sounds\buzzer1.wav" "%BUILD_DIR%\sounds">nul
copy "%BASE_DIR%\..\3rdParty\sounds\buzzer2.wav" "%BUILD_DIR%\sounds">nul
copy "%BASE_DIR%\..\3rdParty\sounds\carhorn.wav" "%BUILD_DIR%\sounds">nul
copy "%BASE_DIR%\..\3rdParty\sounds\cuckooo.wav" "%BUILD_DIR%\sounds">nul
copy "%BASE_DIR%\..\3rdParty\sounds\end.wav" "%BUILD_DIR%\sounds">nul
copy "%BASE_DIR%\..\3rdParty\sounds\fanfare.wav" "%BUILD_DIR%\sounds">nul
copy "%BASE_DIR%\..\3rdParty\sounds\gong.wav" "%BUILD_DIR%\sounds">nul
copy "%BASE_DIR%\..\3rdParty\sounds\horn.wav" "%BUILD_DIR%\sounds">nul
copy "%BASE_DIR%\..\3rdParty\sounds\siren1.wav" "%BUILD_DIR%\sounds">nul
copy "%BASE_DIR%\..\3rdParty\sounds\siren2.wav" "%BUILD_DIR%\sounds">nul
copy "%BASE_DIR%\..\3rdParty\sounds\tada.wav" "%BUILD_DIR%\sounds">nul
copy "%BASE_DIR%\..\3rdParty\sounds\windgong.wav" "%BUILD_DIR%\sounds">nul

REM -- GamePadDemo
copy "%BASE_DIR%\GamepadDemo\release\GamepadDemo.exe" "%BUILD_DIR%">nul

REM -- 3rdParty library files
copy "%BASE_DIR%\..\3rdParty\Qt-4.7.1-vc100\QtCore4.dll" "%BUILD_DIR%">nul
copy "%BASE_DIR%\..\3rdParty\Qt-4.7.1-vc100\QtGui4.dll" "%BUILD_DIR%">nul
copy "%BASE_DIR%\..\3rdParty\msvc100\msvcp100.dll" "%BUILD_DIR%">nul
copy "%BASE_DIR%\..\3rdParty\msvc100\msvcr100.dll" "%BUILD_DIR%">nul

REM -- Documentation files
copy "%BASE_DIR%\doc\Anleitung.pdf" "%BUILD_DIR%">nul
copy "%BASE_DIR%\doc\manual.pdf" "%BUILD_DIR%">nul

REM -- Basic Edition files
copy "%BASE_DIR%\base\categories.xml" "%BUILD_DIR%">nul

REM -- copy base files to Team Edition folder
xcopy /S "%BUILD_DIR%\*.*" "%BUILD_DIR_TEAM%"
MKDIR "%BUILD_DIR_TEAM%\templates">nul
copy "%BASE_DIR%\TeamTournament\templates\*.html" "%BUILD_DIR_TEAM%\templates">nul
copy "%BASE_DIR%\TeamTournament\templates\*.png" "%BUILD_DIR_TEAM%\templates">nul
MKDIR "%BUILD_DIR_TEAM%\clubs">nul
copy "%BASE_DIR%\base\emblems\*.png" "%BUILD_DIR_TEAM%\clubs">nul
copy "%BASE_DIR%\base\clubs.xml" "%BUILD_DIR_TEAM%\clubs">nul

REM -- main programme files
copy "%BASE_DIR%\SingleTournament\release\Ipponboard.exe" "%BUILD_DIR%">nul
copy "%BASE_DIR%\TeamTournament\release\Ipponboard_team.exe" "%BUILD_DIR_TEAM%">nul
