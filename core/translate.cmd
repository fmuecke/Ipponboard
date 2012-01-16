@echo off
lupdate core.pro
pause
linguist ..\i18n\core_de.ts
lrelease -compress ..\i18n\core_de.ts -qm ..\i18n\core_de.qm
pause