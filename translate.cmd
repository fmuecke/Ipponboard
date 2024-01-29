@echo off
call env_cfg-x86.bat

"%QTDIR%\lupdate" base core widgets -locations relative -no-recursive -ts i18n\de.ts -ts i18n\nl.ts

pause

"%QTDIR%\linguist" i18n\de.ts i18n\nl.ts
"%QTDIR%\lrelease" -compress i18n\de.ts -qm i18n\de.qm
"%QTDIR%\lrelease" -compress i18n\nl.ts -qm i18n\nl.qm

pause