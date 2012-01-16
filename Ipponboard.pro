TEMPLATE = subdirs
CONFIG += ordered
SUBDIRS += core gamepad SingleTournament TeamTournament VersionSelector GamePadDemo
SingleTournament.depends = core gamepad
TeamTournament.depends = core gamepad