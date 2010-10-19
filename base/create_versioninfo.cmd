@echo off
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
echo #ifndef VERSIONINFO_HEADER_INCLUDED>>%FILENAME_NO_EXT%.tmp
echo #define VERSIONINFO_HEADER_INCLUDED>>%FILENAME_NO_EXT%.tmp
echo namespace VersionInfo>>%FILENAME_NO_EXT%.tmp
echo {>>%FILENAME_NO_EXT%.tmp
echo 	const char* const Revision = "$WCREV$";>>%FILENAME_NO_EXT%.tmp
echo 	const char* const Date = "$WCNOW$";>>%FILENAME_NO_EXT%.tmp
echo }>>%FILENAME_NO_EXT%.tmp
echo #endif // VERSIONINFO_HEADER_INCLUDED>>%FILENAME_NO_EXT%.tmp

echo -^> generating version info header
REM generate header file
subwcrev .\ versioninfo.tmp versioninfo.h>nul
del versioninfo.tmp
