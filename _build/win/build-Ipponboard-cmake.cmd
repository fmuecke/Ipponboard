@echo off
setlocal
call %~dp0init_env_cfg.cmd  || exit /b %errorlevel%

set ROOT_DIR=%~dp0..\..

:: Create makefiles
cmake -S "%ROOT_DIR%\base" -B "%ROOT_DIR%\_build\build-Ipponboard" -G "Visual Studio 17 2022" -A Win32 || exit /b %errorlevel%

:: Build Release
cmake --build "%ROOT_DIR%\_build\build-Ipponboard" --clean-first --config Release

pause
