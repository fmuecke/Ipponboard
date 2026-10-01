@echo off

set LOCAL_CONFIG=%~dp0..\env_cfg.bat

if /i "%~1"=="ud" goto user_defined

if exist "%LOCAL_CONFIG%" (
  call "%LOCAL_CONFIG%"
  echo;
) else (
  echo @echo off > "%LOCAL_CONFIG%"
  echo :: Configure dependency paths below  >> "%LOCAL_CONFIG%"
  echo set "IPPONBOARD_ROOT_DIR=c:\dev\_cpp\Ipponboard" >> "%LOCAL_CONFIG%"
  echo set "QTDIR=C:\Qt\6.9.2\msvc2022_64" >> "%LOCAL_CONFIG%"  
  echo set "INNO_DIR=c:\Program Files (x86)\Inno Setup 6" >> "%LOCAL_CONFIG%"
  echo Please configure dependency paths in "%LOCAL_CONFIG%" first!
  pause
  exit /b 1
)
exit /b 0

:user_defined
:: Interactive configuration (profile "ud")
set "IPPONBOARD_ROOT_DIR="
set "QTDIR="
set "INNO_DIR="
set /p "QTDIR=Qt installation directory [C:\Qt\6.9.2\msvc2022_64]: "
if not defined QTDIR set "QTDIR=C:\Qt\6.9.2\msvc2022_64"
set /p "IPPONBOARD_ROOT_DIR=Ipponboard root directory [c:\dev\_cpp\Ipponboard]: "
if not defined IPPONBOARD_ROOT_DIR set "IPPONBOARD_ROOT_DIR=c:\dev\_cpp\Ipponboard"
set /p "INNO_DIR=Inno Setup directory [c:\Program Files (x86)\Inno Setup 6]: "
if not defined INNO_DIR set "INNO_DIR=c:\Program Files (x86)\Inno Setup 6"
echo @echo off > "%LOCAL_CONFIG%"
echo :: Configure dependency paths below >> "%LOCAL_CONFIG%"
echo set "IPPONBOARD_ROOT_DIR=%IPPONBOARD_ROOT_DIR%" >> "%LOCAL_CONFIG%"
echo set "QTDIR=%QTDIR%" >> "%LOCAL_CONFIG%"
echo set "INNO_DIR=%INNO_DIR%" >> "%LOCAL_CONFIG%"
echo Created "%LOCAL_CONFIG%".
exit /b 0
