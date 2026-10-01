@echo off

set LOCAL_CONFIG=%~dp0..\env_cfg.bat

if "%~1"=="" goto load_config
if /i "%~1"=="ud" goto user_defined
echo ERROR: unknown profile "%~1" (use ud)
exit /b 1

:load_config
if exist "%LOCAL_CONFIG%" (
  call "%LOCAL_CONFIG%"
  echo;
) else (
  echo ERROR: missing "%LOCAL_CONFIG%". Create it with build.ps1 -Profile ud or init_env_cfg.cmd ud.
  exit /b 1
)
exit /b 0

:user_defined
:: Interactive configuration (profile "ud")
set "IPPONBOARD_ROOT_DIR="
set "QTDIR="
set "INNO_DIR="
set "CLANGFORMAT_BINARY="
set /p "QTDIR=Qt installation directory [C:\Qt\6.9.2\msvc2022_64]: "
if not defined QTDIR set "QTDIR=C:\Qt\6.9.2\msvc2022_64"
set /p "IPPONBOARD_ROOT_DIR=Ipponboard root directory [c:\dev\_cpp\Ipponboard]: "
if not defined IPPONBOARD_ROOT_DIR set "IPPONBOARD_ROOT_DIR=c:\dev\_cpp\Ipponboard"
set /p "INNO_DIR=Inno Setup directory [c:\Program Files (x86)\Inno Setup 6]: "
if not defined INNO_DIR set "INNO_DIR=c:\Program Files (x86)\Inno Setup 6"
for /f "delims=" %%i in ('where clang-format 2^>nul') do if not defined CLANGFORMAT_BINARY set "CLANGFORMAT_BINARY=%%i"
if not defined CLANGFORMAT_BINARY set "CLANGFORMAT_BINARY=clang-format"
set /p "CLANGFORMAT_BINARY=clang-format executable [%CLANGFORMAT_BINARY%]: "
if not defined CLANGFORMAT_BINARY set "CLANGFORMAT_BINARY=clang-format"
echo @echo off > "%LOCAL_CONFIG%"
echo :: Configure dependency paths below >> "%LOCAL_CONFIG%"
echo set "IPPONBOARD_ROOT_DIR=%IPPONBOARD_ROOT_DIR%" >> "%LOCAL_CONFIG%"
echo set "QTDIR=%QTDIR%" >> "%LOCAL_CONFIG%"
echo set "INNO_DIR=%INNO_DIR%" >> "%LOCAL_CONFIG%"
echo set "CLANGFORMAT_BINARY=%CLANGFORMAT_BINARY%" >> "%LOCAL_CONFIG%"
echo Created "%LOCAL_CONFIG%".
exit /b 0
