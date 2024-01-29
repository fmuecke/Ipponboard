:: Please configure paths first!
@echo off
IF [%1] == [] (SET DIR=.) ELSE (SET DIR=%1)

SET LOCAL_CONFIG=%DIR%\env_cfg-x86.bat

IF EXIST "%LOCAL_CONFIG%" (
  CALL "%LOCAL_CONFIG%"
  echo;
) ELSE (
  echo @echo off>"%LOCAL_CONFIG%"
  echo set QTDIR=C:\dev\inst\qt\qt-4.8.7-x86-msvc2017>>"%LOCAL_CONFIG%"
  echo set PATH=%%PATH%%;%%QTDIR%%\bin;C:\dev\inst\jom_1_1_3>>"%LOCAL_CONFIG%"
  echo set QMAKESPEC=%%QTDIR%%\mkspecs\win32-msvc2017>>"%LOCAL_CONFIG%"
  echo set BOOST_DIR=C:\dev\inst\boost_1_82_0>>"%LOCAL_CONFIG%"
  echo set INNO_DIR=C:\dev\inst\InnoSetup6>>"%LOCAL_CONFIG%"
  CALL "%LOCAL_CONFIG%"
)
