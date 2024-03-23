@echo off
setlocal
call %~dp0init_env_cfg.cmd  || exit /b %errorlevel%

SET CONFIG=Release
if not "%1"=="" SET CONFIG=%1
if "%2"=="clean" SET CLEAN=--clean-first

:: create version information
call create-versioninfo.cmd %IPPONBOARD_ROOT_DIR%\base || exit /b %errorlevel%

echo Creating makefiles...
::cmake -S "%IPPONBOARD_ROOT_DIR%\test" -B "%IPPONBOARD_ROOT_DIR%\_build\build-Test" -G "Visual Studio 17 2022" -A Win32 || exit /b %errorlevel%
cmake -S "%IPPONBOARD_ROOT_DIR%\base" -B "%IPPONBOARD_ROOT_DIR%\_build\build-Ipponboard" -G "Visual Studio 17 2022" -A Win32 || exit /b %errorlevel%

echo Building makefiles...
::cmake --build "%IPPONBOARD_ROOT_DIR%\_build\build-Test" --config %CONFIG% %CLEAN%
cmake --build "%IPPONBOARD_ROOT_DIR%\_build\build-Ipponboard" --config %CONFIG% %CLEAN%
::--clean-first

:: copy testdata and run tests
echo Executing tests...
if "%CONFIG%"=="Release" (
	xcopy /Y /Q /I %IPPONBOARD_ROOT_DIR%\test\TestData %IPPONBOARD_ROOT_DIR%\_build\bin\TestData >nul 
	xcopy /Y /Q /I %QTDIR%\bin\QtCore4.dll %IPPONBOARD_ROOT_DIR%\_build\bin>nul 
	pushd %IPPONBOARD_ROOT_DIR%\_build\bin\
	%IPPONBOARD_ROOT_DIR%\_build\bin\Test-Ipponboard.exe
	popd
) else (
	xcopy /Y /Q /I %IPPONBOARD_ROOT_DIR%\test\TestData %IPPONBOARD_ROOT_DIR%\_build\bin-debug\TestData >nul 
	xcopy /Y /Q /I %QTDIR%\bin\QtCored4.dll %IPPONBOARD_ROOT_DIR%\_build\bin-debug>nul 
	pushd %IPPONBOARD_ROOT_DIR%\_build\bin-debug\
	%IPPONBOARD_ROOT_DIR%\_build\bin-debug\Test-Ipponboard.exe
	popd
)	
