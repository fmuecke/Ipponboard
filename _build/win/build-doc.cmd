@echo off
setlocal

set BASE_DIR=%~dp0..\..\doc

if "%BASE_DIR%" == "" (
  echo argument BASE_DIR missing for %~f0
  exit /b 1
) 
echo Using: BASE_DIR=%BASE_DIR%

set OUTPUT_DIR=%~dp0..\..\_bin\Ipponboard-Release
if not exist "%OUTPUT_DIR%" mkdir "%OUTPUT_DIR%"
echo Using: OUTPUT_DIR=%OUTPUT_DIR%

:: checking for pandoc
pandoc --version >nul 2>&1
if errorlevel 1 (
  echo ERROR: Pandoc not found!
  echo Pandoc is required to build the html help. You can get it from https://pandoc.org/.
  exit /b 1
)

echo Creating Docs...
pandoc -s "%BASE_DIR%\USER_MANUAL-DE.md" -o "%OUTPUT_DIR%\Anleitung.html" --metadata=title:Anleitung --css="%BASE_DIR%\Ipponboard.css" --resource-path="%BASE_DIR%" --self-contained || exit /b %errorlevel%
pandoc -s "%BASE_DIR%\USER_MANUAL-EN.md" -o "%OUTPUT_DIR%\User-Manual.html" --metadata=title:"User Manual" --css="%BASE_DIR%\Ipponboard.css" --resource-path="%BASE_DIR%" --self-contained || exit /b %errorlevel%
pandoc -s "%BASE_DIR%\..\CHANGELOG.md" -o "%OUTPUT_DIR%\CHANGELOG.html" --css="%BASE_DIR%\Ipponboard.css" --self-contained || exit /b %errorlevel%

echo Copying license files...
robocopy /mir /nfl /njs /njh /ndl /np "%BASE_DIR%\licenses" "%OUTPUT_DIR%\licenses" >nul || exit /b %errorlevel%

pause