IF EXIST "%INNO_DIR%\iscc.exe" (
	GOTO COMPILE_SETUP
) ELSE (
	echo Error: iscc.exe not found or INNO_DIR not defined!
	goto :EOF
)

:COMPILE_SETUP
cd "%BASE_DIR%"
"%INNO_DIR%\iscc.exe" /Q /O"%BASE_DIR%\_build\build_output" "%BASE_DIR%\setup\setup.iss" 