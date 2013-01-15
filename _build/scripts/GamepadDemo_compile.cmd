REM call "%VS100COMNTOOLS%..\..\vc\vcvarsall.bat" x86 >nul
cd "%BASE_DIR%\GamepadDemo"
"%QTDIR%\qmake.exe" > nul 
jom /L /S release
IF errorlevel 1 (
	echo %0: -- failed!
	goto exit /b 1
)
