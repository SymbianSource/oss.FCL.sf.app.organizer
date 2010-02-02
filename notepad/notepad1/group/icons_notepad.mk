#
# Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
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
# Description: Icons makefile for project notepad
#

ifeq (WINS,$(findstring WINS, $(PLATFORM)))
ZDIR=\epoc32\release\$(PLATFORM)\$(CFG)\z
else
ZDIR=\epoc32\data\z
endif

# ----------------------------------------------------------------------------
TARGETDIR=$(ZDIR)\system\data
HEADERDIR=\epoc32\include
ICONTARGETFILENAME=$(TARGETDIR)\npdlib.mif
HEADERFILENAME=$(HEADERDIR)\npdlib.mbg

do_nothing :
	@rem do_nothing

MAKMAKE : do_nothing

BLD : do_nothing

CLEAN : do_nothing

LIB : do_nothing

CLEANLIB : do_nothing

# ----------------------------------------------------------------------------
RESOURCE : 
	mifconv $(ICONTARGETFILENAME) /h$(HEADERFILENAME) \
        /c8,8 qsn_fr_notepad_side_t.bmp \
        /c8,8 qsn_fr_notepad_cont_side_t.bmp \
        /c8,1 qgn_prop_mce_template.bmp \
        /c8,8 qsn_fr_notepad_corner_tl.bmp \
        /c8,8 qsn_fr_notepad_corner_tr.bmp

FREEZE : do_nothing

SAVESPACE : do_nothing

RELEASABLES :
	@echo $(HEADERFILENAME)&& \
	@echo $(ICONTARGETFILENAME)

FINAL : do_nothing
