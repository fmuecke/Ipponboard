if "%BASE_DIR%"=="" (
	CD ..\..\doc
) ELSE (
	CD "%BASE_DIR%\doc"
)

xelatex.exe -quiet -interaction=nonstopmode -max-print-line=120 "Anleitung.tex"
rem run twice for references
xelatex.exe -quiet -interaction=nonstopmode -max-print-line=120 "Anleitung.tex"

xelatex.exe -quiet -interaction=nonstopmode -max-print-line=120 "manual.tex"
rem run twice for references
xelatex.exe -quiet -interaction=nonstopmode -max-print-line=120 "manual.tex"
