@echo off

IF EXIST "%INNO_DIR%\iscc.exe" (
	GOTO COMPILE_SETUP
) ELSE (
	echo Error: iscc.exe not found or INNO_DIR not defined!
	goto :EOF
)

:COMPILE_SETUP
"%INNO_DIR%\iscc.exe" /Q setup\setup.iss

pause