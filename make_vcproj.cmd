@echo off
if "%LIB%"=="" (
	echo Visual Studio build environment not loaded
	pause
	goto :EOF
)

cd core
qmake -tp vc
cd ..

cd gamepad
qmake -tp vc
cd ..

cd gamepaddemo
qmake -tp vc
cd ..

cd SingleTournament
qmake -tp vc
cd ..

cd TeamTournament
qmake -tp vc
cd ..

qmake -tp vc
