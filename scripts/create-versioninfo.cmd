::---------------------------------------------------------
:: Copyright 2018 Florian Muecke. All rights reserved.
:: Use of this source code is governed by a BSD-style license that can be
:: found in the LICENSE.txt file.
::---------------------------------------------------------
@echo off
setlocal
set BASE_DIR=%1

echo Creating version information...
if "%BASE_DIR%" == "" (
  echo argument BASE_DIR missing for %~f0
  exit /b 1
)
echo Using: BASE_DIR=%BASE_DIR%

:: --> CHANGE VERSION HERE:
SET VER1=2
SET VER2=4
SET VER3=0
SET "TAG="
:: that's it. <--

git log -1 --format=%%h > %BASE_DIR%\.revision
SET /P REV=<%BASE_DIR%\.revision

git log -1 --format=%%ci > %BASE_DIR%\.date
SET /P REV_DATE=<%BASE_DIR%\.date

SET FILENAME_NO_EXT=%BASE_DIR%\versioninfo
IF NOT EXIST %BASE_DIR%\.buildnr (
	SET VER4=0
) ELSE (
	SET /P VER4=<%BASE_DIR%\.buildnr
	IF NOT EXIST %FILENAME_NO_EXT%.h SET /A VER4+=1 >nul
	)
)
echo %VER4% >%BASE_DIR%\.buildnr

IF EXIST %FILENAME_NO_EXT%.h (
	echo --^> version info up to date: %VER1%.%VER2%.%VER3%.%VER4%
	goto the_end
)

SET IPPONBOARD_VERSION=%VER1%.%VER2%.%VER3%.%VER4%

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
echo 	const char* const CopyrightYear = "%DATE:~-4%";>>%FILENAME_NO_EXT%.tmp
echo 	const char* const VersionStrShort = "%VER1%.%VER2%.%Ver3%";>>%FILENAME_NO_EXT%.tmp
echo 	const char* const VersionStrFull = "%VER1%.%VER2%.%VER3%.%VER4%";>>%FILENAME_NO_EXT%.tmp
if "%TAG%"=="" (
	echo 	const char* const VersionStr = "%VER1%.%VER2%.%VER3%";>>%FILENAME_NO_EXT%.tmp
) else (
	echo 	const char* const VersionStr = "%VER1%.%VER2%.%VER3%-%TAG%";>>%FILENAME_NO_EXT%.tmp
)
echo }>>%FILENAME_NO_EXT%.tmp
echo #endif  // BASE__VERSIONINFO_H_>>%FILENAME_NO_EXT%.tmp

echo -^> generating versioninfo.h
:: generate header file
del %FILENAME_NO_EXT%.h>nul 2>&1
move %FILENAME_NO_EXT%.tmp %FILENAME_NO_EXT%.h>nul

:: --
:: Update RC file
:: --
echo -^> generating RC file
SET RC_FILE=%BASE_DIR%\Ipponboard.rc
ECHO //>%RC_FILE%
ECHO // FILE IS GENERATED - DO NOT CHANGE!!>>%RC_FILE%
ECHO //>>%RC_FILE%
ECHO #include ^<winver.h^>>>%RC_FILE%
ECHO #pragma code_page(1252)>>%RC_FILE%
ECHO 101 ICON "images/ipponboard.ico">>%RC_FILE%
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
ECHO         BLOCK "040004b0">>%RC_FILE%
ECHO         BEGIN>>%RC_FILE%
ECHO             VALUE "CompanyName", "Florian M�cke">>%RC_FILE%
ECHO             VALUE "FileDescription", "Ipponboard">>%RC_FILE%
ECHO             VALUE "FileVersion", "%VER1%.%VER2%.%VER3%.%VER4%">>%RC_FILE%
ECHO             VALUE "InternalName", "Ipponboard.exe">>%RC_FILE%
ECHO             VALUE "LegalCopyright", "Copyright (C) 2010-%DATE:~-4% Florian M�cke">>%RC_FILE%
ECHO             VALUE "OriginalFilename", "Ipponboard.exe">>%RC_FILE%
ECHO             VALUE "ProductName", "Ipponboard">>%RC_FILE%
if "%TAG%"=="" (
ECHO             VALUE "ProductVersion", "%VER1%.%VER2%.%VER3%">>%RC_FILE%
) else (
ECHO             VALUE "ProductVersion", "%VER1%.%VER2%.%VER3%-%TAG%">>%RC_FILE%
)
ECHO         END>>%RC_FILE%
ECHO     END>>%RC_FILE%
ECHO     BLOCK "VarFileInfo">>%RC_FILE%
ECHO     BEGIN>>%RC_FILE%
ECHO         VALUE "Translation", 0x400, 1200>>%RC_FILE%
ECHO     END>>%RC_FILE%
ECHO END>>%RC_FILE%
ECHO /////////////////////////////////////////////////////////////////////////////>>%RC_FILE%

:the_end
