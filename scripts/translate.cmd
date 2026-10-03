@echo off
call %~dp0..\env_cfg.bat

:: Resolve the Qt tool directory: %QTDIR%\bin or the kit directory below
set "QT_BIN_DIR=%QTDIR%\bin"
if exist "%QT_BIN_DIR%\lupdate.exe" goto tools_found
for %%k in (msvc2022_64 msvc2019_64 mingw_64 win64_msvc2022_64 win64_mingw) do (
  if exist "%QTDIR%\%%k\bin\lupdate.exe" (
    set "QT_BIN_DIR=%QTDIR%\%%k\bin"
    goto tools_found
  )
)
echo ERROR: lupdate not found in %QTDIR%\bin or %QTDIR%\^<kit^>\bin. QTDIR must point at the Qt installation configured in env_cfg.bat.
exit /b 1

:tools_found
if not exist "%QT_BIN_DIR%\linguist.exe" (
  echo ERROR: linguist not found in %QT_BIN_DIR%.
  exit /b 1
)
if not exist "%QT_BIN_DIR%\lrelease.exe" (
  echo ERROR: lrelease not found in %QT_BIN_DIR%.
  exit /b 1
)

"%QT_BIN_DIR%\lupdate" -no-obsolete -locations none -no-recursive -sort-messages "%IPPONBOARD_ROOT_DIR%\base" "%IPPONBOARD_ROOT_DIR%\core" "%IPPONBOARD_ROOT_DIR%\Widgets" -ts "%IPPONBOARD_ROOT_DIR%\i18n\de.ts" -ts "%IPPONBOARD_ROOT_DIR%\i18n\nl.ts"
if errorlevel 1 exit /b 1

pause

"%QT_BIN_DIR%\linguist" "%IPPONBOARD_ROOT_DIR%\i18n\de.ts" "%IPPONBOARD_ROOT_DIR%\i18n\nl.ts"
if errorlevel 1 exit /b 1
"%QT_BIN_DIR%\lrelease" -compress "%IPPONBOARD_ROOT_DIR%\i18n\de.ts" -qm "%IPPONBOARD_ROOT_DIR%\i18n\de.qm"
if errorlevel 1 exit /b 1
"%QT_BIN_DIR%\lrelease" -compress "%IPPONBOARD_ROOT_DIR%\i18n\nl.ts" -qm "%IPPONBOARD_ROOT_DIR%\i18n\nl.qm"
if errorlevel 1 exit /b 1
exit /b 0
