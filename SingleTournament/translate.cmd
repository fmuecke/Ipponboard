@echo off
lupdate ipponboard.pro
pause
linguist Ipponboard_de.ts
lrelease -compress Ipponboard_de.ts -qm ..\i18n\ipponboard_de.qm
pause