@echo off
call _env_cfg-x86.cmd

pushd ..\..

"lupdate" base core widgets -locations relative -no-recursive -ts i18n\de.ts -ts i18n\nl.ts

pause

"linguist" i18n\de.ts i18n\nl.ts
"lrelease" -compress i18n\de.ts -qm i18n\de.qm
"lrelease" -compress i18n\nl.ts -qm i18n\nl.qm

pause

popd