::---------------------------------------------------------
:: Copyright 2018 Florian Muecke. All rights reserved.
:: Use of this source code is governed by a BSD-style license that can be
:: found in the LICENSE.txt file.
::---------------------------------------------------------
@echo off
SET ICONSET=2
SET DEST=.

IF "%ICONSET%"=="1" GOTO LABEL_SILK
IF "%ICONSET%"=="2" GOTO LABEL_FUGUE
IF "%ICONSET%"=="3" GOTO LABEL_FREE_APP_ICONS
IF "%ICONSET%"=="4" GOTO LABEL_DIAGONA
GOTO THE_END

:LABEL_SILK
SET SOURCE=..\..\..\3rdparty\icons\silk_v013\icons
copy %SOURCE%\add.png %DEST%\add.png
copy %SOURCE%\cross.png %DEST%\delete.png
copy %SOURCE%\delete.png %DEST%\deny.png
copy %SOURCE%\door_in.png %DEST%\exit.png
copy %SOURCE%\house_link.png %DEST%\homepage.png
copy %SOURCE%\email.png %DEST%\mail.png
copy %SOURCE%\accept.png %DEST%\accept.png
copy %SOURCE%\application_double.png %DEST%\secondary_view.png
copy %SOURCE%\comments_add.png %DEST%\feedback.png
REM copy %SOURCE%\cog.png %DEST%\settings.png
copy %SOURCE%\wrench.png %DEST%\settings.png
copy %SOURCE%\control_play_blue.png %DEST%\play.png
REM copy %SOURCE%\group_edit.png %DEST%\category_edit.png
copy %SOURCE%\group.png %DEST%\category.png
copy %SOURCE%\printer.png %DEST%\printer.png
copy %SOURCE%\sound.png %DEST%\test_sound.png
copy %SOURCE%\arrow_down.png %DEST%\arrow_down.png
copy %SOURCE%\arrow_up.png %DEST%\arrow_up.png
copy %SOURCE%\information.png %DEST%\about.png
REM copy %SOURCE%\joystick.png %DEST%\controls.png
copy %SOURCE%\controller.png %DEST%\controller.png
REM copy %SOURCE%\hourglass.png %DEST%\time.png
REM copy %SOURCE%\clock.png %DEST%\clock.png
copy %SOURCE%\status_online.png %DEST%\fighter_blue.png
copy %SOURCE%\status_offline.png %DEST%\fighter_white.png
REM copy %SOURCE%\exclamation.png %DEST%\reset.png
copy DUMMY.png %DEST%\keyboard.png
copy page_white_acrobat.png %DEST%\pdf.png
GOTO THE_END


:LABEL_FUGUE
SET SOURCE=..\..\..\3rdparty\icons\fugue\icons
copy %SOURCE%\symbols\plus.png %DEST%\add.png
copy %SOURCE%\symbols\cross.png %DEST%\delete.png
copy DUMMY.png %DEST%\exit.png
copy %SOURCE%\web\home.png %DEST%\homepage.png
copy %SOURCE%\mail\mail.png %DEST%\mail.png
REM copy %SOURCE%\tick-circle.png %DEST%\accept.png
copy %SOURCE%\symbols\tick.png %DEST%\accept.png
REM copy %SOURCE%\minus-octagon.png %DEST%\deny.png
copy %SOURCE%\symbols\slash.png %DEST%\deny.png
copy %SOURCE%\computer\monitor.png %DEST%\secondary_view.png
copy %SOURCE%\document\blue-document-task.png %DEST%\feedback.png
REM copy %SOURCE%\screwdriwer.png %DEST%\settings.png
copy %SOURCE%\tools\wrench-screwdriver.png %DEST%\settings.png
copy %SOURCE%\media\control.png %DEST%\play.png
copy %SOURCE%\people\users.png %DEST%\category.png
copy %SOURCE%\computer\printer.png %DEST%\printer.png
copy %SOURCE%\media\speaker-volume.png %DEST%\test_sound.png
copy %SOURCE%\arrows\arrow-270-medium.png %DEST%\arrow_down.png
copy %SOURCE%\arrows\arrow-090-medium.png %DEST%\arrow_up.png
copy %SOURCE%\arrow_circle.png %DEST%\arrow_circle.png
copy %SOURCE%\symbols\information-frame.png %DEST%\about.png
copy %SOURCE%\media\joystick.png %DEST%\controller.png
::copy %SOURCE%\tag-label.png %DEST%\fighter_blue.png
::copy %SOURCE%\tag.png %DEST%\fighter_white.png
copy %SOURCE%\people\user.png %DEST%\fighter_blue.png
copy %SOURCE%\people\user-white.png %DEST%\fighter_white.png
copy %SOURCE%\keyboard.png %DEST%\keyboard.png
copy %SOURCE%\document\document-pdf.png %DEST%\pdf.png
copy %SOURCE%\application\application-list.png %DEST%\list.png
copy %SOURCE%\arrows\arrow.png %DEST%\right.png
copy %SOURCE%\arrows\arrow-180.png %DEST%\left.png
copy %SOURCE%\edit\edit-color.png %DEST%\color_fg.png
copy %SOURCE%\imaging\paint-can-color.png %DEST%\color_bg.png
REM copy %SOURCE%\imaging\paint-can-color.png %DEST%\color_bg.png
copy %SOURCE%\layout\tables.png %DEST%\copy_cells.png
copy %SOURCE%\clipboard-paste.png %DEST%\paste.png
copy %SOURCE%\layout\table-delete.png %DEST%\clear_cells.png
copy %SOURCE%\layout\table-import.png %DEST%\import_list.png
copy %SOURCE%\layout\table-export.png %DEST%\export_list.png
copy %SOURCE%\media\disk-black.png %DEST%\save.png
copy %SOURCE%\gear.png %DEST%\options.png

GOTO THE_END

:LABEL_FREE_APP_ICONS
SET SOURCE=..\..\..\3rdparty\icons\free-application-icons\png\16x16
copy %SOURCE%\Create.png %DEST%\add.png
copy %SOURCE%\No-Entry.png %DEST%\delete.png
copy %SOURCE%\Exit.png %DEST%\exit.png
copy %SOURCE%\Home.png %DEST%\homepage.png
copy %SOURCE%\E-mail.png %DEST%\mail.png
copy %SOURCE%\Yes.png %DEST%\accept.png
copy %SOURCE%\No.png %DEST%\deny.png
copy "%SOURCE%\Display 16x16.png" %DEST%\secondary_view.png
copy %SOURCE%\Comment.png %DEST%\feedback.png
copy %SOURCE%\Equipment.png %DEST%\settings.png
copy %SOURCE%\Play.png %DEST%\play.png
copy %SOURCE%\People.png %DEST%\category.png
copy %SOURCE%\Print.png %DEST%\printer.png
copy %SOURCE%\Sound.png %DEST%\test_sound.png
copy %SOURCE%\Up.png %DEST%\arrow_down.png
copy %SOURCE%\Down.png %DEST%\arrow_up.png
copy %SOURCE%\Info.png %DEST%\about.png
copy %SOURCE%\Application.png %DEST%\controller.png
copy "%SOURCE%\Blue tag.png" %DEST%\fighter_blue.png
copy "%SOURCE%\Black tag.png" %DEST%\fighter_white.png
copy DUMMY.png %DEST%\keyboard.png
copy DUMMY.png %DEST%\pdf.png
GOTO THE_END

:LABEL_DIAGONA
SET SOURCE=..\..\..\3rdparty\icons\diagona\icons\16
copy %SOURCE%\103.png %DEST%\add.png
copy %SOURCE%\101.png %DEST%\delete.png
copy DUMMY.png %DEST%\exit.png
copy %SOURCE%\045.png %DEST%\homepage.png
copy %SOURCE%\004.png %DEST%\mail.png
copy %SOURCE%\102.png %DEST%\accept.png
copy %SOURCE%\150.png %DEST%\deny.png
copy %SOURCE%\068.png %DEST%\secondary_view.png
copy %SOURCE%\028.png %DEST%\feedback.png
copy %SOURCE%\086.png %DEST%\settings.png
copy %SOURCE%\131.png %DEST%\play.png
copy %SOURCE%\084.png %DEST%\category.png
copy %SOURCE%\062.png %DEST%\printer.png
copy %SOURCE%\056.png %DEST%\test_sound.png
copy %SOURCE%\107.png %DEST%\arrow_down.png
copy %SOURCE%\108.png %DEST%\arrow_up.png
copy %SOURCE%\043.png %DEST%\about.png
copy %SOURCE%\041.png %DEST%\controller.png
copy %SOURCE%\006.png %DEST%\fighter_blue.png
copy %SOURCE%\005.png %DEST%\fighter_white.png
copy DUMMY.png %DEST%\keyboard.png
copy DUMMY.png %DEST%\pdf.png
GOTO THE_END



:LABEL_XXX
SET SOURCE=..\..\..\3rdparty\icons\XXX\icons
copy %SOURCE%\.png %DEST%\add.png
copy %SOURCE%\.png %DEST%\delete.png
copy %SOURCE%\.png %DEST%\exit.png
copy %SOURCE%\.png %DEST%\homepage.png
copy %SOURCE%\.png %DEST%\mail.png
copy %SOURCE%\.png %DEST%\accept.png
copy %SOURCE%\.png %DEST%\deny.png
copy %SOURCE%\.png %DEST%\secondary_view.png
copy %SOURCE%\.png %DEST%\feedback.png
copy %SOURCE%\.png %DEST%\settings.png
copy %SOURCE%\.png %DEST%\play.png
copy %SOURCE%\.png %DEST%\category.png
copy %SOURCE%\.png %DEST%\printer.png
copy %SOURCE%\.png %DEST%\test_sound.png
copy %SOURCE%\.png %DEST%\arrow_down.png
copy %SOURCE%\.png %DEST%\arrow_up.png
copy %SOURCE%\.png %DEST%\about.png
copy %SOURCE%\.png %DEST%\controller.png
copy %SOURCE%\.png %DEST%\fighter_blue.png
copy %SOURCE%\.png %DEST%\fighter_white.png
copy DUMMY.png %DEST%\keyboard.png
copy DUMMY.png %DEST%\pdf.png
GOTO THE_END

:THE_END
pause