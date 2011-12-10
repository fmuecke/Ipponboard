@echo off
lupdate Ipponboard.pro
pause
linguist i18n\Ipponboard_de.ts
lrelease -compress i18n\Ipponboard_de.ts -qm i18n\ipponboard_de.qm
copy i18n\ipponboard_de.qm SingleTournament\release
copy i18n\ipponboard_de.qm TeamTournament\release
pause