:: Please configure paths first!
@echo off
if [%1] == [] (
	echo Fehlende Argumente: _create_env_cfg.cmd x86/x64
	exit /b 1
)

if [%1] == [x86] (set ARCH=x86) else if [%1] == [x64] (set ARCH=x64)

set LOCAL_CONFIG=_env_cfg-%ARCH%.cmd

if EXIST "%LOCAL_CONFIG%" (
  call "%LOCAL_CONFIG%"
  echo;
) else (
  echo create file=%LOCAL_CONFIG%
  echo @echo off>"%LOCAL_CONFIG%"
  echo set QTDIR=C:\dev\inst\qt\qt-4.8.7-%ARCH%-msvc2017>>"%LOCAL_CONFIG%"
  echo set PATH=%%PATH%%;C:\dev\inst\qt\qt-4.8.7-%ARCH%-msvc2017\bin;C:\dev\inst\jom_1_1_3>>"%LOCAL_CONFIG%"
  echo set QMAKESPEC=C:\dev\inst\qt\qt-4.8.7-x64-msvc2017\mkspecs\win32-msvc2017>>"%LOCAL_CONFIG%"
  echo set BOOST_DIR=C:\dev\inst\boost_1_82_0>>"%LOCAL_CONFIG%"
  echo set INNO_DIR=C:\dev\inst\InnoSetup6>>"%LOCAL_CONFIG%"
  echo set REDIST_DIR_x86=C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Redist\MSVC\14.38.33135\x86\Microsoft.VC143.CRT>>"%LOCAL_CONFIG%"
  echo set REDIST_DIR_x64=C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Redist\MSVC\14.38.33135\x64\Microsoft.VC143.CRT>>"%LOCAL_CONFIG%"
  echo set REDIST_DIR_x86d=C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Redist\MSVC\14.38.33135\debug_nonredist\x86\Microsoft.VC143.DebugCRT>>"%LOCAL_CONFIG%"
  echo set REDIST_DIR_x64d=C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Redist\MSVC\14.38.33135\debug_nonredist\x64\Microsoft.VC143.DebugCRT>>"%LOCAL_CONFIG%"
 CALL "%LOCAL_CONFIG%"
)
