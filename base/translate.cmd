@echo off
call ..\env_cfg.bat

"%QTDIR%\lupdate" base.pro
pause
"%QTDIR%\linguist" ..\i18n\de.ts
"%QTDIR%\lrelease" -compress ..\i18n\de.ts -qm ..\i18n\de.qm
pause