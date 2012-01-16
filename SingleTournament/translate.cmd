@echo off
lupdate SingleTournament.pro
pause
linguist ..\i18n\Ipponboard_de.ts
lrelease -compress ..\i18n\Ipponboard_de.ts -qm ..\i18n\ipponboard_de.qm
pause