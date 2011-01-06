REM -- main programme files
copy "%BASE_DIR%\SingleTournament\release\Ipponboard.exe" "%BUILD_DIR%">nul
REM copy "%BASE_DIR%\SingleTournament\Ipponboard.ini" "%BUILD_DIR%">nul
REM copy "%BASE_DIR%\SingleTournament\categories.xml" "%BUILD_DIR%">nul

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
copy "%BASE_DIR%\..\3rdParty\qt-4.6.2\QtCore4.dll" "%BUILD_DIR%">nul
copy "%BASE_DIR%\..\3rdParty\qt-4.6.2\QtGui4.dll" "%BUILD_DIR%">nul
copy "%BASE_DIR%\..\3rdParty\msvc100\plain\msvcp100.dll" "%BUILD_DIR%">nul
copy "%BASE_DIR%\..\3rdParty\msvc100\plain\msvcr100.dll" "%BUILD_DIR%">nul

REM -- Documentation files
copy "%BASE_DIR%\doc\Anleitung.pdf" "%BUILD_DIR%">nul
copy "%BASE_DIR%\doc\manual.pdf" "%BUILD_DIR%">nul