@echo off
setlocal

if [%1] == [] (
	echo Fehlendes Argument: _build_doc.cmd BASE_DIR
	exit /b 1
) else (
	set BASE_DIR=%1
	echo BASE_DIR=%BASE_DIR%
)

set OUTPUT_DIR=%BASE_DIR%\bin
set BASE_DIR=%BASE_DIR%/doc
if not exist "%OUTPUT_DIR%" mkdir "%OUTPUT_DIR%"

:: checking for pandoc
pandoc --version >nul 2>&1
if errorlevel 1 (
	echo ERROR: Pandoc not found!
	echo Pandoc is required to build the html help. You can get it from https://pandoc.org/.
	exit /b 1
)

echo creating Anleitung.html...
pandoc -s "%BASE_DIR%\manual-de.md" -o "%OUTPUT_DIR%\Anleitung.html" --toc --css="%BASE_DIR%\Ipponboard.css" --resource-path="%BASE_DIR%" --number-sections --self-contained || exit /b %errorlevel%

echo creating Manual.html...
pandoc -s "%BASE_DIR%\USER_MANUAL-EN.md" -o "%OUTPUT_DIR%\User-Manual.html" -T "Ipponboard Manual" --css="%BASE_DIR%\Ipponboard.css" --resource-path="%BASE_DIR%" --self-contained || exit /b %errorlevel%

echo creating CHANGELOG.html...
pandoc -s "%BASE_DIR%\..\CHANGELOG.md" -o "%OUTPUT_DIR%\CHANGELOG.html" --css="%BASE_DIR%\Ipponboard.css" --self-contained || exit /b %errorlevel%

echo copying license files...
robocopy /mir /nfl /njs /njh /ndl /np "%BASE_DIR%\licenses" "%OUTPUT_DIR%\Licenses" >nul || exit /b %errorlevel%
