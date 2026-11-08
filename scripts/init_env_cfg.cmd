@echo off

set LOCAL_CONFIG=%~dp0..\env_cfg.bat

if exist "%LOCAL_CONFIG%" (
  call "%LOCAL_CONFIG%"
  echo;
) else (
  echo @echo off > "%LOCAL_CONFIG%"
  echo :: Configure dependency paths below  >> "%LOCAL_CONFIG%"
  echo set "IPPONBOARD_ROOT_DIR=c:\dev\_cpp\Ipponboard" >> "%LOCAL_CONFIG%"
  echo set "QTDIR=C:\Qt\6.9.2\msvc2022_64" >> "%LOCAL_CONFIG%"
  echo set "INNO_DIR=c:\Program Files (x86)\Inno Setup 6" >> "%LOCAL_CONFIG%"
  echo set "CLANGFORMAT=C:\dev\tools\libclang_23.1.1-vs2022_64\bin\clang-format.exe" >> "%LOCAL_CONFIG%"
  echo Please configure dependency paths in "%LOCAL_CONFIG%" first!
  pause
  exit /b 1
)
