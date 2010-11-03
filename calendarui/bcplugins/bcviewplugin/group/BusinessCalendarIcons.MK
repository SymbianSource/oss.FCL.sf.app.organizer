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
# Description: 
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
ICONTARGETFILENAME=$(TARGETDIR)\businesscalendar.mif
HEADERFILENAME=$(HEADERDIR)\businesscalendar.mbg

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
        /1,1   qgn_indi_repeat_add.bmp           \
        /1,1   qgn_indi_repeat_exception_add.bmp \
        /c8,1  qgn_prop_todo_undone.bmp          \
        /c8,1  qgn_prop_todo_done.bmp            \
        /c8,1  qgn_indi_todo_high_add.bmp        \
        /c8,1  qgn_indi_todo_low_add.bmp         \
        /c8,1  qgn_indi_cdr_todo.bmp            \
      	/c8,8 ..\data\qgn_fs_cale_ev_acc.svg \
        /c8,8 ..\data\qgn_fs_cale_ev_canc.svg \
        /c8,8 ..\data\qgn_fs_cale_ev_decl.svg \
        /c8,8 ..\data\qgn_fs_cale_ev_notsync.svg \
        /c8,8 ..\data\qgn_fs_cale_ev_tent.svg \
        /c8,8 ..\data\qgn_fscal_month_preview_bg.svg \
        /c8,8  ..\data\qgn_fs_cale_ev_hidden_indi_up.svg \
        /c8,8  ..\data\qgn_fs_cale_ev_hidden_indi_down.svg \
        /c8,8 ..\data\qgn_fscal_indi_occasion.svg \
        /c8,8 ..\data\qgn_fscal_indi_todo.svg \
        /c8,8 ..\data\qgn_fscal_indi_memo.svg \
        /c8,8 ..\data\qgn_fs_cale_ev_act.svg \
        /c8,8 ..\data\qgn_fs_cale_ev_pass.svg \
        /c8,8 ..\data\qgn_fs_cale_bg_lsc.svg \
        /c8,8 ..\data\qgn_fs_graf_email_ctrlbar.svg \
        /c8,8 ..\data\qgn_fscal_indi_recurrence.svg \
        /c8,8 ..\data\qgn_fscal_indi_priority_low.svg \
        /c8,8 ..\data\qgn_fscal_indi_priority_high.svg \
        /c8,8 ..\data\qgn_meet_req_indi_alarm.svg \
        /c8,8 ..\data\qgn_fsca_indi_action_todo.svg \
        /c8,8 ..\data\qgn_fsca_indi_action_anni.svg \
        /c8,8 ..\data\qgn_fscal_indi_new_todo.svg \
        /c8,8 ..\data\qgn_fs_indi_priority_high.svg \
        /c8,8 ..\data\qgn_fs_indi_priority_low.svg \
        /c8,8 ..\data\qgn_fs_hscr_action_arrow_up.svg \
        /c8,8 ..\data\qgn_fs_hscr_action_arrow_down.svg \


FREEZE : do_nothing

SAVESPACE : do_nothing

RELEASABLES :
	@echo $(HEADERFILENAME)&& \
	@echo $(ICONTARGETFILENAME)

FINAL : do_nothing
