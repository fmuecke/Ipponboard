@echo off
call ..\env_cfg.bat

"%QTDIR%\lupdate" -verbose VersionSelector.pro
pause
"%QTDIR%\linguist" ..\i18n\VersionSelector_de.ts
"%QTDIR%\lrelease" -compress ..\i18n\VersionSelector_de.ts -qm ..\i18n\VersionSelector_de.qm
pause