@echo off
call ..\env_cfg.bat
"%QTDIR%\lupdate" core.pro -no-obsolete -locations relative

pause
"%QTDIR%\linguist" ..\i18n\core_de.ts
"%QTDIR%\lrelease" -compress ..\i18n\core_de.ts -qm ..\i18n\core_de.qm
"%QTDIR%\lrelease" -compress ..\i18n\core_nl.ts -qm ..\i18n\core_nl.qm
pause