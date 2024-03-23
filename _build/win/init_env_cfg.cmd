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
  echo set "QTDIR=c:\devtools\qt\qt-4.8.7-vc14" >> "%LOCAL_CONFIG%"
  echo set "BOOST_DIR=c:\devtools\boost_1_81_0" >> "%LOCAL_CONFIG%"
  echo set "MSVC_DIR=C:\Program Files\Microsoft Visual Studio\2022\Professional\VC" >> "%LOCAL_CONFIG%"
  echo set "MSVC_CMD=%%MSVC_DIR%%\Auxiliary\Build\vcvars32.bat" >> "%LOCAL_CONFIG%"
  echo set "INNO_DIR=c:\devtools\inno setup 5" >> "%LOCAL_CONFIG%"
  
  echo echo Using: IPPONBOARD_ROOT_DIR=%%IPPONBOARD_ROOT_DIR%% >> "%LOCAL_CONFIG%"
  echo echo Using: QMAKESPEC=%%QMAKESPEC%% >> "%LOCAL_CONFIG%"
  echo echo Using: QTDIR=%%QTDIR%% >> "%LOCAL_CONFIG%"
  echo echo Using: BOOST_DIR=%%BOOST_DIR%% >> "%LOCAL_CONFIG%"
  echo echo Using: MSVC_DIR=%%MSVC_DIR%% >> "%LOCAL_CONFIG%"
  echo echo Using: MSVC_CMD=%%MSVC_CMD%% >> "%LOCAL_CONFIG%"
  echo echo Using: INNO_DIR=%%INNO_DIR%% >> "%LOCAL_CONFIG%"
  
  echo Please configure dependency paths in "%LOCAL_CONFIG%" first!
  pause
  exit /b 1
)
