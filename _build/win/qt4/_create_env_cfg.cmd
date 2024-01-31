:: Please configure paths first!
@echo off
if [%1] == [] (
	echo Fehlende Argumente: _create_env_cfg.cmd x86/x64 [outputDir]
	exit /b 1
)

if [%1] == [x86] (set ARCH=x86) else if [%1] == [x64] (set ARCH=x64)
if [%2] == [] (set DIR=.) else (set DIR=%1)

set LOCAL_CONFIG=%DIR%\_env_cfg-%ARCH%.cmd

if EXIST "%LOCAL_CONFIG%" (
  call "%LOCAL_CONFIG%"
  echo;
) else (
  echo @echo off>"%LOCAL_CONFIG%"
  echo set QTDIR=C:\dev\inst\qt\qt-4.8.7-%ARCH%-msvc2017>>"%LOCAL_CONFIG%"
  echo set PATH=%%PATH%%;%%QTDIR%%\bin;C:\dev\inst\jom_1_1_3>>"%LOCAL_CONFIG%"
  echo set QMAKESPEC=%%QTDIR%%\mkspecs\win32-msvc2017>>"%LOCAL_CONFIG%"
  echo set BOOST_DIR=C:\dev\inst\boost_1_82_0>>"%LOCAL_CONFIG%"
  echo set INNO_DIR=C:\dev\inst\InnoSetup6>>"%LOCAL_CONFIG%"
  CALL "%LOCAL_CONFIG%"
)
