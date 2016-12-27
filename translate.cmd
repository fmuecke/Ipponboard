@echo off
call env_cfg.bat

"%QTDIR%\lupdate" base\base.pro -no-obsolete -locations relative
pause

"%QTDIR%\linguist" i18n\de.ts
"%QTDIR%\lrelease" -compress i18n\de.ts -qm i18n\de.qm
"%QTDIR%\lrelease" -compress i18n\nl.ts -qm i18n\nl.qm

pause