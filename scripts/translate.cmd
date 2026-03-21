@echo off
call %~dp0..\env_cfg.bat

"%QTDIR%\bin\lupdate" -no-obsolete -locations relative -no-recursive -sort-messages %IPPONBOARD_ROOT_DIR%\base %IPPONBOARD_ROOT_DIR%\core %IPPONBOARD_ROOT_DIR%\widgets -ts %IPPONBOARD_ROOT_DIR%\i18n\de.ts -ts %IPPONBOARD_ROOT_DIR%\i18n\nl.ts

pause

"%QTDIR%\bin\linguist" %IPPONBOARD_ROOT_DIR%\i18n\de.ts %IPPONBOARD_ROOT_DIR%\i18n\nl.ts
"%QTDIR%\bin\lrelease" -compress %IPPONBOARD_ROOT_DIR%\i18n\de.ts -qm %IPPONBOARD_ROOT_DIR%\i18n\de.qm
"%QTDIR%\bin\lrelease" -compress %IPPONBOARD_ROOT_DIR%\i18n\nl.ts -qm %IPPONBOARD_ROOT_DIR%\i18n\nl.qm
