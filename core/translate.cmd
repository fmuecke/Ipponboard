@echo off
call ..\env_cfg.bat
"%QTDIR%\lupdate" core.pro
pause
"%QTDIR%\linguist" ..\i18n\core_de.ts
"%QTDIR%\lrelease" -compress ..\i18n\core_de.ts -qm ..\i18n\core_de.qm
pause