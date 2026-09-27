@echo off

set LOCAL_CONFIG=%~dp0..\env_cfg.bat

if exist "%LOCAL_CONFIG%" (
  call "%LOCAL_CONFIG%"
  echo;
) else (
  echo @echo off > "%LOCAL_CONFIG%"
  echo :: Configure dependency paths below  >> "%LOCAL_CONFIG%"
  echo set "IPPONBOARD_ROOT_DIR=C:\dev\git\github\Ipponboard-frk-ralf" >> "%LOCAL_CONFIG%"
  echo set "QTDIR=C:\dev\tools\qt-5.15.17-x86" >> "%LOCAL_CONFIG%"  
  echo set "BOOST_DIR=C:\dev\tools\boost_1_87_0" >> "%LOCAL_CONFIG%"
  echo set "INNO_DIR=C:\dev\tools\Inno Setup 7" >> "%LOCAL_CONFIG%"
  echo Please configure dependency paths in "%LOCAL_CONFIG%" first!
  pause
  exit /b 1
)
