@echo off
setlocal
call %~dp0init_env_cfg.cmd  || exit /b %errorlevel%

set CONFIG=release
if not "%1"=="" SET CONFIG=%1
if "%2"=="clean" SET CLEAN=--clean-first

set BIN_DIR=%IPPONBOARD_ROOT_DIR%\_build\bin-test

:: create version information
call create-versioninfo.cmd %IPPONBOARD_ROOT_DIR%\base || exit /b %errorlevel%

if defined CLEAN (
	if "%CONFIG%"=="release" (
		if exist "%BIN_DIR%" (
			rd /Q /S "%BIN_DIR%" >nul 2>&1
		)
	) else (
		if exist "%BIN_DIR%-debug" (
			rd /Q /S "%BIN_DIR%-debug" >nul 2>&1
		)
	)
)

echo Creating makefiles...
cmake -S "%IPPONBOARD_ROOT_DIR%\test" -B "%IPPONBOARD_ROOT_DIR%\_build\build-Test" -G "Visual Studio 17 2022" -A Win32 -DUSE_QT5=ON || exit /b %errorlevel%

echo Building makefiles...
cmake --build "%IPPONBOARD_ROOT_DIR%\_build\build-Test" --config %CONFIG% %CLEAN%
::--clean-first

:: copy testdata and run tests
echo Executing tests...
if "%CONFIG%"=="release" (
	xcopy /Y /Q /I %IPPONBOARD_ROOT_DIR%\test\TestData %BIN_DIR%\TestData >nul 
	xcopy /Y /Q /I %QT5DIR%\bin\Qt5Core.dll %BIN_DIR%>nul 
	pushd %BIN_DIR%\
	%BIN_DIR%\Test-Ipponboard.exe
	popd
) else (
	xcopy /Y /Q /I %IPPONBOARD_ROOT_DIR%\test\TestData %BIN_DIR%-debug\TestData >nul 
	xcopy /Y /Q /I %QT5DIR%\bin\Qt5Cored.dll %BIN_DIR%-debug>nul 
	pushd %BIN_DIR%-debug\
	%BIN_DIR%-debug\Test-Ipponboard.exe
	popd
)	
