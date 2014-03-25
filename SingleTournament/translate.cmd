@echo off
call ..\env_cfg.bat

"%QTDIR%\lupdate" SingleTournament.pro
pause
"%QTDIR%\linguist" ..\i18n\Ipponboard_de.ts
"%QTDIR%\lrelease" -compress ..\i18n\Ipponboard_de.ts -qm ..\i18n\ipponboard_de.qm
pause