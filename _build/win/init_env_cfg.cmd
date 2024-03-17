@echo off

SET LOCAL_CONFIG=%~dp0..\..\env_cfg.bat

IF EXIST "%LOCAL_CONFIG%" (
  CALL "%LOCAL_CONFIG%"
  echo;
) ELSE (
  echo @echo off>"%LOCAL_CONFIG%"
  echo set "QMAKESPEC=win32-msvc2015" >> "%LOCAL_CONFIG%"
  echo set "QTDIR=c:\devtools\qt\qt-4.8.7-vc14\bin\" >> "%LOCAL_CONFIG%"
  echo set "BOOST_DIR=c:\devtools\boost_1_81_0" >> "%LOCAL_CONFIG%"
  echo set "MSVC_DIR=C:\Program Files\Microsoft Visual Studio\2022\Professional\VC" >> "%LOCAL_CONFIG%"
  echo set "MSVC_CMD=%%MSVC_DIR%%\Auxiliary\Build\vcvars32.bat" >> "%LOCAL_CONFIG%"
  echo set "INNO_DIR=c:\devtools\inno setup 5" >> "%LOCAL_CONFIG%"
  ::::echo set PATH=%QTDIR%;%PATH%>>%LOCAL_CONFIG%
  echo Please configure dependency paths in "%LOCAL_CONFIG%" first!
  pause
  exit /b 1
)
