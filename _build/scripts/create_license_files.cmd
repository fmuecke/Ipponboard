if "%BASE_DIR%"=="" (
	CD ..\..\doc\license
) ELSE (
	CD "%BASE_DIR%\doc\license"
)

htlatex.exe "License_de.tex">nul
copy /Y License_de.html ..\..\base\text
htlatex.exe "License_en.tex">nul
copy /Y License_en.html ..\..\base\text

SET RTFPATH=c:\development\Latex2RTF\cfg
"%RTFPATH%\..\latex2rt.exe" License_de.tex
"%RTFPATH%\..\latex2rt.exe" License_en.tex
copy /Y License_de.rtf ..\..\setup
copy /Y License_en.rtf ..\..\setup
