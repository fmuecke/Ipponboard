@echo off
lupdate TeamTournament.pro
pause
linguist ..\i18n\Ipponboard_team_de.ts
lrelease -compress ..\i18n\Ipponboard_team_de.ts -qm ..\i18n\ipponboard_team_de.qm
pause