@echo off
REM --> CHANGE VERSION HERE:
SET VER1=0
SET VER2=9
SET VER3=9
IF NOT EXIST buildnr (
	SET VER4=0
) ELSE (
	SET /P VER4=<buildnr
	SET /A VER4+=1 >nul
)
echo %VER4% >buildnr
REM that's it. <--

SET IPPONBOARD_VERSION=%VER1%.%VER2%.%VER3%.%VER4%
SET FILENAME_NO_EXT=versioninfo
subwcrev>nul
if %errorlevel%==1 goto do_it
echo Subversion not found!
goto :EOF

:do_it
echo -^> writing version info template
REM write template
echo //>%FILENAME_NO_EXT%.tmp
echo // THIS FILE IS GENERATED - DO NOT MODIFY!>>%FILENAME_NO_EXT%.tmp
echo //>>%FILENAME_NO_EXT%.tmp
echo #ifndef BASE__VERSIONINFO_H_>>%FILENAME_NO_EXT%.tmp
echo #define BASE__VERSIONINFO_H_>>%FILENAME_NO_EXT%.tmp
echo namespace VersionInfo>>%FILENAME_NO_EXT%.tmp
echo {>>%FILENAME_NO_EXT%.tmp
echo 	const char* const Revision = "$WCREV$";>>%FILENAME_NO_EXT%.tmp
echo 	const char* const Date = "$WCNOW$";>>%FILENAME_NO_EXT%.tmp
echo 	const char* const VersionStr = "%VER1%.%VER2%.%VER3%";>>%FILENAME_NO_EXT%.tmp
echo 	const char* const VersionStrFull = "%VER1%.%VER2%.%VER3%.%VER4%";>>%FILENAME_NO_EXT%.tmp
echo }>>%FILENAME_NO_EXT%.tmp
echo #endif  // BASE__VERSIONINFO_H_>>%FILENAME_NO_EXT%.tmp

echo -^> generating version info header
REM generate header file
subwcrev .\ versioninfo.tmp versioninfo.h>nul
del versioninfo.tmp

REM --
REM -- Update RC file
REM --
SET RC_FILE=Ipponboard.rc
ECHO //>%RC_FILE%
ECHO // FILE IS GENERATED - DO NOT CHANGE!!>>%RC_FILE%
ECHO //>>%RC_FILE%
ECHO #include ^<winver.h^>>>%RC_FILE%
ECHO #pragma code_page(1252)>>%RC_FILE%
ECHO IDI_MAIN_ICON	ICON	"images/ipponboard.ico">>%RC_FILE%
ECHO /////////////////////////////////////////////////////////////////////////////>>%RC_FILE%
ECHO VS_VERSION_INFO VERSIONINFO>>%RC_FILE%
ECHO FILEVERSION %VER1%,%VER2%,%VER3%,%VER4% >>%RC_FILE%
ECHO PRODUCTVERSION %VER1%,%VER2%,%VER3%,%VER4% >>%RC_FILE%
ECHO FILEFLAGSMASK 0x3fL>>%RC_FILE%
ECHO #ifdef _DEBUG>>%RC_FILE%
ECHO FILEFLAGS 0x1L>>%RC_FILE%
ECHO #else>>%RC_FILE%
ECHO FILEFLAGS 0x0L>>%RC_FILE%
ECHO #endif>>%RC_FILE%
ECHO FILEOS 0x40004L>>%RC_FILE%
ECHO FILETYPE 0x1L>>%RC_FILE%
ECHO FILESUBTYPE 0x0L>>%RC_FILE%
ECHO BEGIN>>%RC_FILE%
ECHO     BLOCK "StringFileInfo">>%RC_FILE%
ECHO     BEGIN>>%RC_FILE%
ECHO         BLOCK "040704b0">>%RC_FILE%
ECHO         BEGIN>>%RC_FILE%
ECHO             VALUE "CompanyName", "Florian Mücke\0">>%RC_FILE%
ECHO             VALUE "FileDescription", "TODO: ***File description***\0">>%RC_FILE%
ECHO             VALUE "FileVersion", "%VER1%.%VER2%.%VER3%.%VER4%\0">>%RC_FILE%
ECHO             VALUE "InternalName", "Ipponboard.exe\0">>%RC_FILE%
ECHO             VALUE "LegalCopyright", "Copyright (C) 2010-2012 Florian Mücke\0">>%RC_FILE%
ECHO             VALUE "OriginalFilename", "Ipponboard.exe\0">>%RC_FILE%
ECHO             VALUE "ProductName", "Ipponboard\0">>%RC_FILE%
ECHO             VALUE "ProductVersion", "%VER1%.%VER2%.%VER3%.%VER4%\0">>%RC_FILE%
ECHO         END>>%RC_FILE%
ECHO     END>>%RC_FILE%
ECHO END>>%RC_FILE%
ECHO /////////////////////////////////////////////////////////////////////////////>>%RC_FILE%
