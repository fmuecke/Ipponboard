@echo off

cls

SET BLD=-- BUILD
IF NOT EXIST DISTRIBUTE MKDIR DISTRIBUTE
SET OUTDIR=%CD%\DISTRIBUTE\%DATE:~6,4%-%DATE:~3,2%-%DATE:~0,2%

call "%VS100COMNTOOLS%..\..\vc\vcvarsall.bat" x86
REM call "%VS90COMNTOOLS%..\..\vc\vcvarsall.bat" x86

IF "%1"=="s" GOTO SINGLE
IF "%1"=="t" GOTO TEAM
echo %BLD%: 
echo %BLD%: Output dir: %OUTDIR%
echo %BLD%: QTDIR: %QTDIR%
echo %BLD%: 
echo %BLD%: Ready to build? --
echo %BLD%: (press Ctrl+C to abort) & pause>nul
echo;

REM -------
:COMMON
REM -------
IF NOT EXIST "%OUTDIR%" MKDIR "%OUTDIR%"

echo %BLD%: updating version info...
cd %CD%\base
call create_versioninfo.cmd
cd ..

REM -------
:SINGLE
REM -------
echo;
echo %BLD%: [SingleTournament]
cd %CD%\SingleTournament
echo %BLD%: cleaning up...
jom clean>nul
IF errorlevel 1 (
	echo %BLD%: failed!
	pause
)
echo;
echo %BLD%: calling qmake...
qmake > nul 
IF errorlevel 1 (
	echo %BLD%: failed!
	pause
)
echo %BLD%: -- building release...
start /WAIT jom release
IF errorlevel 0 (
	echo %BLD%: -- copying files...
	copy /y release\Ipponboard.exe "%OUTDIR%\"
	copy /y Ipponboard_de.qm "%OUTDIR%\"
	copy /y Ipponboard.ini "%OUTDIR%\"
) ELSE (
	echo %BLD%: -- failed!
)
cd ..

IF "%1"=="s" GOTO THE_END

REM -----
:TEAM
REM -----
IF NOT EXIST "%OUTDIR%" MKDIR "%OUTDIR%"
echo;
echo %BLD%: building [TeamTournament]
cd TeamTournament
echo %BLD%: cleaning up...
start /WAIT jom clean>nul
IF errorlevel 1 (
	echo %BLD%: failed!
	pause
)
echo;
echo %BLD%: calling qmake...
qmake > nul 
IF errorlevel 1 (
	echo %BLD%: failed!
	pause
)
echo %BLD%: building release...
start /WAIT jom release > nul
IF errorlevel 0 (
	echo %BLD%: -- done. copying files...
	copy /y release\Ipponboard_team.exe "%OUTDIR%\"
	copy /y IpponboardT.ini "%OUTDIR%\"
	copy /y IpponboardT.ini "%OUTDIR%\"
	copy /y list_output.html "%OUTDIR%\"
	copy /y ..\base\clubs.xml "%OUTDIR%\"
) ELSE (
	echo %BLD%: -- failed!
)
cd ..


:THE_END

pause