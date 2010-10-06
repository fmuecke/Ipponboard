@echo off

IF NOT EXIST DISTRIBUTE MKDIR DISTRIBUTE
SET OUTDIR=%CD%\DISTRIBUTE\%DATE:~6,4%-%DATE:~3,2%-%DATE:~0,2%

REM call "%VS100COMNTOOLS%..\..\vc\vcvarsall.bat" x86
call "%VS90COMNTOOLS%..\..\vc\vcvarsall.bat" x86

IF "%1"=="s" GOTO SINGLE
IF "%1"=="t" GOTO TEAM
cls
echo -- Ready to build? --
echo -- Output dir: %OUTDIR%
echo (press Ctrl+C to abort) & pause>nul
echo;


REM -------
:SINGLE
REM -------
IF NOT EXIST "%OUTDIR%" MKDIR "%OUTDIR%"
echo;
echo SingleTournament
cd base
call create_versioninfo.cmd
cd ..
cd SingleTournament
echo -- cleaning up...
start /WAIT jom clean>nul
IF errorlevel 1 (
	echo failed!
	pause
)
echo -- calling qmake...
start /WAIT qmake > nul 
IF errorlevel 1 (
	echo failed!
	pause
)
echo -- building release...
start /WAIT jom release > nul
IF errorlevel 0 (
	echo -- copying files...
	copy /y release\Ipponboard.exe "%OUTDIR%\"
	copy /y Ipponboard_de.qm "%OUTDIR%\"
	copy /y Ipponboard.ini "%OUTDIR%\"
) ELSE (
	echo -- failed!
)
cd ..

IF "%1"=="s" GOTO THE_END

REM -----
:TEAM
REM -----
IF NOT EXIST "%OUTDIR%" MKDIR "%OUTDIR%"
echo;
echo TeamTournament
cd base
call create_versioninfo.cmd
cd ..
cd TeamTournament
echo -- cleaning up...
start /WAIT jom clean>nul
IF errorlevel 1 (
	echo failed!
	pause
)
echo -- calling qmake...
start /WAIT qmake > nul 
IF errorlevel 1 (
	echo failed!
	pause
)
echo -- building release...
start /WAIT jom release > nul
IF errorlevel 0 (
	echo -- done. copying files...
	copy /y release\Ipponboard_Team.exe "%OUTDIR%\"
	copy /y IpponboardT.ini "%OUTDIR%\"
	copy /y IpponboardT.ini "%OUTDIR%\"
	copy /y list_output.html "%OUTDIR%\"
	copy /y ..\base\clubs.xml "%OUTDIR%\"
) ELSE (
	echo -- failed!
)
cd ..

:THE_END

pause