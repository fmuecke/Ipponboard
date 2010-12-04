@echo off

cls

SET BLD=-- BUILD
IF NOT EXIST DISTRIBUTE MKDIR DISTRIBUTE
SET OUTDIR=%CD%\DISTRIBUTE\_build_output
SET FINALDIR=%CD%\DISTRIBUTE\%DATE:~6,4%-%DATE:~3,2%-%DATE:~0,2%

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
echo %BLD%: cleaning up "%OUTDIR%"...
IF EXIST "%OUTDIR%" RD /S /Q "%OUTDIR%"
MKDIR "%OUTDIR%"

echo %BLD%: updating version info...
cd "%CD%\base"
call create_versioninfo.cmd
cd ..

CALL :COPY_GENERAL_FILES

REM -------
:SINGLE
REM -------
echo;
echo %BLD%: [SingleTournament]
cd "%CD%\SingleTournament"
echo %BLD%: cleaning up...
jom clean>nul
IF errorlevel 1 (
	echo %BLD%: failed!
	goto THE_END
)
echo;
echo %BLD%: calling qmake...
qmake > nul 
IF errorlevel 1 (
	echo %BLD%: failed!
	goto THE_END
)
echo %BLD%: -- building release...
start /WAIT jom release
IF errorlevel 1 (
	echo %BLD%: -- failed!
	goto THE_END	
)
cd ..
CALL :COPY_IPPONBOARD_SINGLE_FILES


IF "%1"=="s" GOTO THE_END

REM -----
:TEAM
REM -----
IF NOT EXIST "%OUTDIR%" MKDIR "%OUTDIR%"
echo;
echo %BLD%: building [TeamTournament]
cd "%CD%\TeamTournament"
echo %BLD%: cleaning up...
start /WAIT jom clean>nul
IF errorlevel 1 (
	echo %BLD%: failed!
	goto THE_END
)
echo;
echo %BLD%: calling qmake...
qmake > nul 
IF errorlevel 1 (
	echo %BLD%: failed!
	goto THE_END
)
echo %BLD%: building release...
start /WAIT jom release > nul
IF errorlevel 1 (
	echo %BLD%: -- failed!
	goto THE_END
)
cd ..
CALL :COPY_IPPONBOARD_TEAM_FILES

GOTO THE_END

REM
REM --- functions ---
REM
:COPY_GENERAL_FILES
	echo %BLD%: -- copying general files...
	copy /y "%CD%\DISTRIBUTE\_dlls\QtCore4.dll" "%OUTDIR%\"
	copy /y "%CD%\DISTRIBUTE\_dlls\QtGui4.dll" "%OUTDIR%\"
	copy /y "%CD%\base\images\ipponboard.ico" "%OUTDIR%\"
GOTO :EOF

:COPY_IPPONBOARD_SINGLE_FILES
	echo %BLD%: -- copying SingleTournament files...
	copy /y "%CD%\SingleTournament\release\Ipponboard.exe" "%OUTDIR%\"
	copy /y "%CD%\i18n\Ipponboard_de.qm" "%OUTDIR%\"
	copy /y "%CD%\SingleTournament\Ipponboard.ini" "%OUTDIR%\"	
GOTO :EOF

:COPY_IPPONBOARD_TEAM_FILES
	echo %BLD%: -- copying TeamTournament files...
	copy /y "%CD%\TeamTournament\release\Ipponboard_team.exe" "%OUTDIR%\"
	copy /y "%CD%\TeamTournament\IpponboardT.ini" "%OUTDIR%\"
	copy /y "%CD%\TeamTournament\IpponboardT.ini" "%OUTDIR%\"
	copy /y "%CD%\TeamTournament\list_output.html" "%OUTDIR%\"
	copy /y "%CD%\base\clubs.xml" "%OUTDIR%\"
GOTO :EOF

:THE_END

pause
