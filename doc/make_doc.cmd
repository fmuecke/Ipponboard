@echo off
setlocal

echo copying license files...
if not exist %~dp0bin mkdir %~dp0bin
robocopy /mir /nfl /njs /njh /ndl /np licenses %~dp0bin\Licenses || goto error

echo creating Anleitung.html
pandoc -s manual-de.md -o %~dp0bin\Anleitung.html --toc --css=Ipponboard.css --number-sections --self-contained || goto error

echo creating Manual.html
pandoc -s manual-en.md -o %~dp0bin\Manual.html --toc --css=Ipponboard.css --number-sections --self-contained || goto error


echo creating CHANGELOG.html
if exist "..\CHANGELOG.md" (
	pandoc -s ..\CHANGELOG.md -o %~dp0bin\Changes.html --css=Ipponboard.css --self-contained || goto error
) else (
    echo skipped
)
goto :eof

:error
echo Something bad happened!
pause

