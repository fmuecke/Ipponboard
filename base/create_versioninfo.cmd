@echo off
:: --> CHANGE VERSION HERE:
SET VER1=1
SET VER2=0
SET VER3=1
IF NOT EXIST ..\base\.buildnr (
	SET VER4=0
) ELSE (
	SET /P VER4=<..\base\.buildnr
	SET /A VER4+=1 >nul
)
echo %VER4% >..\base\.buildnr
:: that's it. <--

hg parents --template {rev} > ..\base\.revision
SET /P REV=<..\base\.revision

hg parents --template {date^|localdate^|isodate} > ..\base\.date
SET /P REV_DATE=<..\base\.date

SET IPPONBOARD_VERSION=%VER1%.%VER2%.%VER3%.%VER4%
SET FILENAME_NO_EXT=..\base\versioninfo

rem subwcrev>nul
rem if %errorlevel%==1 goto do_it
rem echo Subversion not found!
rem goto :EOF

:do_it
echo -^> writing version info template: v=%IPPONBOARD_VERSION%
:: write template
echo //>%FILENAME_NO_EXT%.tmp
echo // THIS FILE IS GENERATED - DO NOT MODIFY!>>%FILENAME_NO_EXT%.tmp
echo //>>%FILENAME_NO_EXT%.tmp
echo #ifndef BASE__VERSIONINFO_H_>>%FILENAME_NO_EXT%.tmp
echo #define BASE__VERSIONINFO_H_>>%FILENAME_NO_EXT%.tmp
echo namespace VersionInfo>>%FILENAME_NO_EXT%.tmp
echo {>>%FILENAME_NO_EXT%.tmp
echo 	const char* const Revision = "%REV%";>>%FILENAME_NO_EXT%.tmp
echo 	const char* const Date = "%REV_DATE%";>>%FILENAME_NO_EXT%.tmp
echo 	const char* const VersionStr = "%VER1%.%VER2%.%VER3%";>>%FILENAME_NO_EXT%.tmp
echo 	const char* const VersionStrFull = "%VER1%.%VER2%.%VER3%.%VER4%";>>%FILENAME_NO_EXT%.tmp
echo }>>%FILENAME_NO_EXT%.tmp
echo #endif  // BASE__VERSIONINFO_H_>>%FILENAME_NO_EXT%.tmp

echo -^> generating version info header
:: generate header file
del %FILENAME_NO_EXT%.h>nul
move %FILENAME_NO_EXT%.tmp %FILENAME_NO_EXT%.h>nul

:: --
:: Update RC file
:: --
SET RC_FILE=..\base\Ipponboard.rc
SET RC_FILE2=..\base\Ipponboard_team.rc
ECHO //>%RC_FILE%
ECHO //>%RC_FILE2%
ECHO // FILE IS GENERATED - DO NOT CHANGE!!>>%RC_FILE%
ECHO // FILE IS GENERATED - DO NOT CHANGE!!>>%RC_FILE2%
ECHO //>>%RC_FILE%
ECHO //>>%RC_FILE2%
ECHO #include ^<winver.h^>>>%RC_FILE%
ECHO #include ^<winver.h^>>>%RC_FILE2%
ECHO #pragma code_page(1252)>>%RC_FILE%
ECHO #pragma code_page(1252)>>%RC_FILE2%
ECHO IDI_MAIN_ICON	ICON	"images/ipponboard.ico">>%RC_FILE%
ECHO IDI_MAIN_ICON	ICON	"images/ipponboard_team.ico">>%RC_FILE2%
ECHO /////////////////////////////////////////////////////////////////////////////>>%RC_FILE%
ECHO /////////////////////////////////////////////////////////////////////////////>>%RC_FILE2%
ECHO VS_VERSION_INFO VERSIONINFO>>%RC_FILE%
ECHO VS_VERSION_INFO VERSIONINFO>>%RC_FILE2%
ECHO FILEVERSION %VER1%,%VER2%,%VER3%,%VER4% >>%RC_FILE%
ECHO FILEVERSION %VER1%,%VER2%,%VER3%,%VER4% >>%RC_FILE2%
ECHO PRODUCTVERSION %VER1%,%VER2%,%VER3%,%VER4% >>%RC_FILE%
ECHO PRODUCTVERSION %VER1%,%VER2%,%VER3%,%VER4% >>%RC_FILE2%
ECHO FILEFLAGSMASK 0x3fL>>%RC_FILE%
ECHO FILEFLAGSMASK 0x3fL>>%RC_FILE2%
ECHO #ifdef _DEBUG>>%RC_FILE%
ECHO #ifdef _DEBUG>>%RC_FILE2%
ECHO FILEFLAGS 0x1L>>%RC_FILE%
ECHO FILEFLAGS 0x1L>>%RC_FILE2%
ECHO #else>>%RC_FILE%
ECHO #else>>%RC_FILE2%
ECHO FILEFLAGS 0x0L>>%RC_FILE%
ECHO FILEFLAGS 0x0L>>%RC_FILE2%
ECHO #endif>>%RC_FILE%
ECHO #endif>>%RC_FILE2%
ECHO FILEOS 0x40004L>>%RC_FILE%
ECHO FILEOS 0x40004L>>%RC_FILE2%
ECHO FILETYPE 0x1L>>%RC_FILE%
ECHO FILETYPE 0x1L>>%RC_FILE2%
ECHO FILESUBTYPE 0x0L>>%RC_FILE%
ECHO FILESUBTYPE 0x0L>>%RC_FILE2%
ECHO BEGIN>>%RC_FILE%
ECHO BEGIN>>%RC_FILE2%
ECHO     BLOCK "StringFileInfo">>%RC_FILE%
ECHO     BLOCK "StringFileInfo">>%RC_FILE2%
ECHO     BEGIN>>%RC_FILE%
ECHO     BEGIN>>%RC_FILE2%
ECHO         BLOCK "040704b0">>%RC_FILE%
ECHO         BLOCK "040704b0">>%RC_FILE2%
ECHO         BEGIN>>%RC_FILE%
ECHO         BEGIN>>%RC_FILE2%
ECHO             VALUE "CompanyName", "Florian Mücke\0">>%RC_FILE%
ECHO             VALUE "CompanyName", "Florian Mücke\0">>%RC_FILE2%
ECHO             VALUE "FileDescription", "Ipponboard\0">>%RC_FILE%
ECHO             VALUE "FileDescription", "Ipponboard Team Edition\0">>%RC_FILE2%
ECHO             VALUE "FileVersion", "%VER1%.%VER2%.%VER3%.%VER4%\0">>%RC_FILE%
ECHO             VALUE "FileVersion", "%VER1%.%VER2%.%VER3%.%VER4%\0">>%RC_FILE2%
ECHO             VALUE "InternalName", "Ipponboard.exe\0">>%RC_FILE%
ECHO             VALUE "InternalName", "Ipponboard_team.exe\0">>%RC_FILE2%
ECHO             VALUE "LegalCopyright", "Copyright (C) 2010-2012 Florian Mücke\0">>%RC_FILE%
ECHO             VALUE "LegalCopyright", "Copyright (C) 2010-2012 Florian Mücke\0">>%RC_FILE2%
ECHO             VALUE "OriginalFilename", "Ipponboard.exe\0">>%RC_FILE%
ECHO             VALUE "OriginalFilename", "Ipponboard_team.exe\0">>%RC_FILE2%
ECHO             VALUE "ProductName", "Ipponboard\0">>%RC_FILE%
ECHO             VALUE "ProductName", "Ipponboard Team Edition\0">>%RC_FILE2%
ECHO             VALUE "ProductVersion", "%VER1%.%VER2%.%VER3%.%VER4%\0">>%RC_FILE%
ECHO             VALUE "ProductVersion", "%VER1%.%VER2%.%VER3%.%VER4%\0">>%RC_FILE2%
ECHO         END>>%RC_FILE%
ECHO         END>>%RC_FILE2%
ECHO     END>>%RC_FILE%
ECHO     END>>%RC_FILE2%
ECHO END>>%RC_FILE%
ECHO END>>%RC_FILE2%
ECHO /////////////////////////////////////////////////////////////////////////////>>%RC_FILE%
ECHO /////////////////////////////////////////////////////////////////////////////>>%RC_FILE2%
