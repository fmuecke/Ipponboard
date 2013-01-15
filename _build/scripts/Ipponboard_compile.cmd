REM call "%VS100COMNTOOLS%..\..\vc\vcvarsall.bat" x86 >nul
cd "%BASE_DIR%\SingleTournament"
"%QTDIR%\qmake.exe" > nul 
jom /L /S release
IF errorlevel 1 (
	echo %0: -- failed!
	exit /b 1
)

cd "%BASE_DIR%\TeamTournament"
"%QTDIR%\qmake.exe" > nul 
jom /L /S release
IF errorlevel 1 (
	echo %0: -- failed!
	exit /b 1
)
