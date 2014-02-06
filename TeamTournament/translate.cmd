@echo off
call ..\env_cfg.bat

"%QTDIR%\lupdate" TeamTournament.pro
pause
"%QTDIR%\linguist" ..\i18n\Ipponboard_team_de.ts
"%QTDIR%\lrelease" -compress ..\i18n\Ipponboard_team_de.ts -qm ..\i18n\ipponboard_team_de.qm
pause