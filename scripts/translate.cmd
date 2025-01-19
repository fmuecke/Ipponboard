@echo off
call %~dp0..\env_cfg.bat

"%QTDIR%\bin\lupdate" %IPPONBOARD_ROOT_DIR%\base %IPPONBOARD_ROOT_DIR%\core %IPPONBOARD_ROOT_DIR%\widgets -locations relative -no-recursive -no-ui-lines -ts %IPPONBOARD_ROOT_DIR%\i18n\de.ts -ts %IPPONBOARD_ROOT_DIR%\i18n\nl.ts

pause

"%QTDIR%\bin\linguist" %IPPONBOARD_ROOT_DIR%\i18n\de.ts %IPPONBOARD_ROOT_DIR%\i18n\nl.ts
"%QTDIR%\bin\lrelease" -compress %IPPONBOARD_ROOT_DIR%\i18n\de.ts -qm %IPPONBOARD_ROOT_DIR%\i18n\de.qm
"%QTDIR%\bin\lrelease" -compress %IPPONBOARD_ROOT_DIR%\i18n\nl.ts -qm %IPPONBOARD_ROOT_DIR%\i18n\nl.qm
