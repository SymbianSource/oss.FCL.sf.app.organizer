#
# Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies).
# All rights reserved.
# This component and the accompanying materials are made available
# under the terms of "Eclipse Public License v1.0"
# which accompanies this distribution, and is available
# at the URL "http://www.eclipse.org/legal/epl-v10.html".
#
# Initial Contributors:
# Nokia Corporation - initial contribution.
#
# Contributors:
#
# Description:  File to load necessary calendar icon while building. These icons are used by calendar UI component.
#

ifeq (WINS,$(findstring WINS, $(PLATFORM)))
ZDIR=\epoc32\release\$(PLATFORM)\$(CFG)\Z
else
ZDIR=\epoc32\data\z
endif

# ----------------------------------------------------------------------------
# 
# ----------------------------------------------------------------------------

TARGETDIR=$(ZDIR)\resource\apps
HEADERDIR=\epoc32\include
ICONTARGETFILENAME=$(TARGETDIR)\calendar.mif
HEADERFILENAME=$(HEADERDIR)\calendar.mbg

do_nothing :
	@rem do_nothing

MAKMAKE : do_nothing

BLD : do_nothing

CLEAN : do_nothing

LIB : do_nothing

CLEANLIB : do_nothing

# ----------------------------------------------------------------------------
# NOTE 1: DO NOT DEFINE MASK FILE NAMES! They are included automatically by
# MifConv if the mask detph is defined.
#
# NOTE 2: Usually, source paths should not be included in the bitmap
# definitions. MifConv searches for the icons in all icon directories in a
# predefined order, which is currently \s60\icons, \s60\bitmaps2.
# The directory \s60\icons is included in the search only if the feature flag
# __SCALABLE_ICONS is defined.
# ----------------------------------------------------------------------------

RESOURCE :
	mifconv $(ICONTARGETFILENAME) /h$(HEADERFILENAME) \
        /c8,1  qgn_indi_cdr_birthday.bmp         \
        /c8,1  qgn_indi_cdr_meeting.bmp          \
        /c8,1  qgn_indi_cdr_reminder.bmp         \
        /1,1   qgn_indi_cdr_event_mixed.bmp      \
        /1     qgn_indi_item_not_shown.bmp       \
        /c8,8   qgn_indi_repeat_add.bmp           \
        /1,1   qgn_indi_repeat_exception_add.bmp \
        /c8,1  qgn_prop_todo_undone.bmp          \
        /c8,1  qgn_prop_todo_done.bmp            \
        /c8,1  qgn_indi_todo_high_add.bmp        \
        /c8,1  qgn_indi_todo_low_add.bmp         \
        /c8,1  qgn_indi_cdr_todo.bmp             \
	/c8,8  qgn_indi_cale_tb_day.svg          \
	/c8,8  qgn_indi_cale_tb_left.svg         \
	/c8,8  qgn_indi_cale_tb_month.svg        \
	/c8,8  qgn_indi_cale_tb_new_entry.svg    \
	/c8,8  qgn_indi_cale_tb_right.svg        \
	/c8,8  qgn_indi_cale_tb_todo.svg         \
	/c8,8  qgn_indi_cale_tb_week.svg	\
	/c8,8  qgn_indi_cale_tb_meeting_new.svg	\
	/c8,8  qgn_indi_cale_tb_todo_new.svg 		\
	/c8,8  qgn_indi_mg_tb_edit.svg          \
	/c8,8  qgn_indi_mg_tb_delete.svg         \
	/c8,8  qgn_indi_tb_mce.svg              \
	/c8,8  qgn_menu_maps.svg       \
	/c8,8  qgn_indi_cdr_birthday.svg		       \
	/c8,8  qgn_indi_cdr_birthday_missed_new.svg    \
	/c8,8  qgn_indi_cdr_meeting.svg				   \
	/c8,8  qgn_indi_cdr_meeting_missed_new.svg     \
	/c8,8  qgn_indi_cdr_reminder.svg               \
	/c8,8  qgn_indi_cdr_reminder_missed_new.svg    \
	/c8,8  qgn_indi_cdr_todo_undone.svg            \
	/c8,8  qgn_indi_cdr_todo_undone_missed_new.svg \
	/c8,8  qgn_indi_tb_cale_alarm_dismiss.svg	   \
	/c8,8  qgn_indi_tb_cale_alarm_dismiss_all.svg


FREEZE : do_nothing

SAVESPACE : do_nothing

RELEASABLES :
	@echo $(HEADERFILENAME)&& \
	@echo $(ICONTARGETFILENAME)

FINAL : do_nothing
