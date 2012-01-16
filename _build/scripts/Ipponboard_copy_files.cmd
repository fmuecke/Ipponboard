:: language file
MKDIR "%BUILD_DIR%\lang">nul
copy "%BASE_DIR%\bin\lang\core_de.qm" "%BUILD_DIR%\lang">nul
copy "%BASE_DIR%\bin\lang\Ipponboard_de.qm" "%BUILD_DIR%\lang">nul

:: sound files
MKDIR "%BUILD_DIR%\sounds">nul
copy "%BASE_DIR%\bin\sounds\buzzer.wav" "%BUILD_DIR%\sounds">nul
copy "%BASE_DIR%\bin\sounds\alarm.wav" "%BUILD_DIR%\sounds">nul
copy "%BASE_DIR%\bin\sounds\alert.wav" "%BUILD_DIR%\sounds">nul
copy "%BASE_DIR%\bin\sounds\bells.wav" "%BUILD_DIR%\sounds">nul
copy "%BASE_DIR%\bin\sounds\buzzer1.wav" "%BUILD_DIR%\sounds">nul
copy "%BASE_DIR%\bin\sounds\buzzer2.wav" "%BUILD_DIR%\sounds">nul
copy "%BASE_DIR%\bin\sounds\carhorn.wav" "%BUILD_DIR%\sounds">nul
copy "%BASE_DIR%\bin\sounds\cuckooo.wav" "%BUILD_DIR%\sounds">nul
copy "%BASE_DIR%\bin\sounds\end.wav" "%BUILD_DIR%\sounds">nul
copy "%BASE_DIR%\bin\sounds\fanfare.wav" "%BUILD_DIR%\sounds">nul
copy "%BASE_DIR%\bin\sounds\gong.wav" "%BUILD_DIR%\sounds">nul
copy "%BASE_DIR%\bin\sounds\horn.wav" "%BUILD_DIR%\sounds">nul
copy "%BASE_DIR%\bin\sounds\siren1.wav" "%BUILD_DIR%\sounds">nul
copy "%BASE_DIR%\bin\sounds\siren2.wav" "%BUILD_DIR%\sounds">nul
copy "%BASE_DIR%\bin\sounds\tada.wav" "%BUILD_DIR%\sounds">nul
copy "%BASE_DIR%\bin\sounds\windgong.wav" "%BUILD_DIR%\sounds">nul

:: GamePadDemo
copy "%BASE_DIR%\bin\GamepadDemo.exe" "%BUILD_DIR%">nul

:: 3rdParty library files
copy "%BASE_DIR%\bin\QtCore4.dll" "%BUILD_DIR%">nul
copy "%BASE_DIR%\bin\QtGui4.dll" "%BUILD_DIR%">nul
copy "%BASE_DIR%\bin\msvcp100.dll" "%BUILD_DIR%">nul
copy "%BASE_DIR%\bin\msvcr100.dll" "%BUILD_DIR%">nul

:: documentation files
copy "%BASE_DIR%\doc\Anleitung.pdf" "%BUILD_DIR%">nul
copy "%BASE_DIR%\doc\manual.pdf" "%BUILD_DIR%">nul

:: copy base files to Team Edition folder
xcopy /S /Q "%BUILD_DIR%" "%BUILD_DIR_TEAM%\"
copy "%BASE_DIR%\bin\lang\Ipponboard_team_de.qm" "%BUILD_DIR_TEAM%\lang">nul
copy "%BASE_DIR%\bin\lang\VersionSelector_de.qm" "%BUILD_DIR_TEAM%\lang">nul
xcopy /S /Q "%BASE_DIR%\bin\clubs" "%BUILD_DIR_TEAM%\clubs\"
xcopy /S /Q "%BASE_DIR%\bin\templates" "%BUILD_DIR_TEAM%\templates\"

:: Basic Edition files
copy "%BASE_DIR%\bin\categories.xml" "%BUILD_DIR%">nul

:: main programme files
copy "%BASE_DIR%\bin\Ipponboard.exe" "%BUILD_DIR%">nul
copy "%BASE_DIR%\bin\Ipponboard_team.exe" "%BUILD_DIR_TEAM%">nul
