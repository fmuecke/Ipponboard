@echo off
lupdate -verbose VersionSelector.pro
pause
linguist ..\i18n\VersionSelector_de.ts
lrelease -compress ..\i18n\VersionSelector_de.ts -qm ..\i18n\VersionSelector_de.qm
pause