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
:: Prefer the values of an existing configuration as suggestions
if exist "%LOCAL_CONFIG%" call "%LOCAL_CONFIG%"
set "CFG_QTDIR=%QTDIR%"
set "CFG_IPPONBOARD_ROOT_DIR=%IPPONBOARD_ROOT_DIR%"
set "CFG_INNO_DIR=%INNO_DIR%"
set "CFG_CLANGFORMAT=%CLANGFORMAT_BINARY%"
set "IPPONBOARD_ROOT_DIR="
set "QTDIR="
set "INNO_DIR="
set "CLANGFORMAT_BINARY="
if not defined CFG_QTDIR set "CFG_QTDIR=C:\Qt\6.9.2\msvc2022_64"
set /p "QTDIR=Qt installation directory [%CFG_QTDIR%]: "
if not defined QTDIR set "QTDIR=%CFG_QTDIR%"
if not defined CFG_IPPONBOARD_ROOT_DIR set "CFG_IPPONBOARD_ROOT_DIR=%CD%"
set /p "IPPONBOARD_ROOT_DIR=Ipponboard root directory [%CFG_IPPONBOARD_ROOT_DIR%]: "
if not defined IPPONBOARD_ROOT_DIR set "IPPONBOARD_ROOT_DIR=%CFG_IPPONBOARD_ROOT_DIR%"
if not defined CFG_INNO_DIR set "CFG_INNO_DIR=c:\Program Files (x86)\Inno Setup 6"
set /p "INNO_DIR=Inno Setup directory [%CFG_INNO_DIR%]: "
if not defined INNO_DIR set "INNO_DIR=%CFG_INNO_DIR%"
for /f "delims=" %%i in ('where clang-format 2^>nul') do if not defined CFG_CLANGFORMAT set "CFG_CLANGFORMAT=%%i"
if not defined CFG_CLANGFORMAT set "CFG_CLANGFORMAT=clang-format"
set /p "CLANGFORMAT_BINARY=clang-format executable [%CFG_CLANGFORMAT%]: "
if not defined CLANGFORMAT_BINARY set "CLANGFORMAT_BINARY=%CFG_CLANGFORMAT%"
set "CFG_QTDIR="
set "CFG_IPPONBOARD_ROOT_DIR="
set "CFG_INNO_DIR="
set "CFG_CLANGFORMAT="
echo @echo off > "%LOCAL_CONFIG%"
echo :: Configure dependency paths below >> "%LOCAL_CONFIG%"
echo set "IPPONBOARD_ROOT_DIR=%IPPONBOARD_ROOT_DIR%" >> "%LOCAL_CONFIG%"
echo set "QTDIR=%QTDIR%" >> "%LOCAL_CONFIG%"
echo set "INNO_DIR=%INNO_DIR%" >> "%LOCAL_CONFIG%"
echo set "CLANGFORMAT_BINARY=%CLANGFORMAT_BINARY%" >> "%LOCAL_CONFIG%"
echo Created "%LOCAL_CONFIG%".
exit /b 0
