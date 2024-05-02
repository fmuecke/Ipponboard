@echo off

set LOCAL_CONFIG=%~dp0..\..\env_cfg.bat

if exist "%LOCAL_CONFIG%" (
  call "%LOCAL_CONFIG%"
  echo;
) else (
  echo @echo off > "%LOCAL_CONFIG%"
  echo :: Configure dependency paths below  >> "%LOCAL_CONFIG%"
  echo set "IPPONBOARD_ROOT_DIR=c:\dev\_cpp\Ipponboard" >> "%LOCAL_CONFIG%"
  echo set "QMAKESPEC=win32-msvc2015" >> "%LOCAL_CONFIG%"
  echo set "QTDIR=c:\devtools\qt5\Qt5.15.13-x86-msvc2022" >> "%LOCAL_CONFIG%"  
  echo set "BOOST_DIR=c:\devtools\boost_1_81_0" >> "%LOCAL_CONFIG%"
  echo set "INNO_DIR=c:\Program Files (x86)\Inno Setup 6\" >> "%LOCAL_CONFIG%"
  echo Please configure dependency paths in "%LOCAL_CONFIG%" first!
  pause
  exit /b 1
)
