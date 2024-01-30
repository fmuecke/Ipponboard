:: Please configure paths first!
@echo off
IF [%1] == [] (
	echo Fehlende Argumente: _create_env_cfg.cmd x86/x64 [outputDir]
	exit /b 1
)

IF [%1] == [x86] (SET ARCH=x86) ELSE IF [%1] == [x64] (SET ARCH=x64)
IF [%2] == [] (SET DIR=.) ELSE (SET DIR=%1)

SET LOCAL_CONFIG=%DIR%\_env_cfg-%ARCH%.cmd

IF EXIST "%LOCAL_CONFIG%" (
  CALL "%LOCAL_CONFIG%"
  echo;
) ELSE (
  echo @echo off>"%LOCAL_CONFIG%"
  echo set QTDIR=C:\dev\inst\qt\qt-4.8.7-%ARCH%-msvc2017>>"%LOCAL_CONFIG%"
  echo set PATH=%%PATH%%;%%QTDIR%%\bin;C:\dev\inst\jom_1_1_3>>"%LOCAL_CONFIG%"
  echo set QMAKESPEC=%%QTDIR%%\mkspecs\win32-msvc2017>>"%LOCAL_CONFIG%"
  echo set BOOST_DIR=C:\dev\inst\boost_1_82_0>>"%LOCAL_CONFIG%"
  echo set INNO_DIR=C:\dev\inst\InnoSetup6>>"%LOCAL_CONFIG%"
  CALL "%LOCAL_CONFIG%"
)
