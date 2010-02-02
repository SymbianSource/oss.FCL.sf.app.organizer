#
# Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies).
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
# Description:      Icons for Search
#

ifeq (WINS,$(findstring WINS, $(PLATFORM)))
ZDIR=\epoc32\release\$(PLATFORM)\$(CFG)\Z
else
ZDIR=\epoc32\data\z
endif
//ICONLIST=..\data\iconlist.txt
# ----------------------------------------------------------------------------
# Configure these
# ----------------------------------------------------------------------------

TARGETDIR=$(ZDIR)\RESOURCE\APPS
HEADERDIR=\epoc32\include
ICONTARGETFILENAME=$(TARGETDIR)\gnrlsearchuisvg.mif
HEADERFILENAME=$(HEADERDIR)\gnrlsearchuisvg.mbg

do_nothing :
	@rem do_nothing

MAKMAKE : do_nothing

#BLD : $(ICONTARGETFILENAME)
BLD : $(HEADERFILENAME)

CLEAN : do_nothing

LIB : do_nothing

CLEANLIB : do_nothing

# ----------------------------------------------------------------------------
# Configure these.
#
# NOTE 1: DO NOT DEFINE MASK FILE NAMES! They are included automatically by
# MifConv if the mask detph is defined.
#
# NOTE 2: Usually, source paths should not be included in the bitmap
# definitions. MifConv searches for the icons in all icon directories in a
# predefined order, which is currently \s60\icons, \s60\bitmaps2.
# The directory \s60\icons is included in the search only if the feature flag
# __SCALABLE_ICONS is defined.
# ----------------------------------------------------------------------------

#RESOURCE : $(ICONTARGETFILENAME) 
#RESOURCE : $(HEADERFILENAME)

#$(HEADERFILENAME) : $(ICONLIST)
#	mifconv $(ICONTARGETFILENAME) /H$(HEADERFILENAME) /F$(ICONLIST)
	
#$(ICONTARGETFILENAME) : $(ICONLIST)
#	mifconv $(ICONTARGETFILENAME) /H$(HEADERFILENAME) /F$(ICONLIST)



RESOURCE :
	mifconv $(ICONTARGETFILENAME) /h$(HEADERFILENAME) \
	      /c8,8 Skin_Green\qgn_prop_checkbox_off.svg \
        /c8,8 Skin_Green\qgn_prop_checkbox_on.svg \
        /c8,8 qgn_prop_search_indevice_title.svg
	
FREEZE : do_nothing

SAVESPACE : do_nothing

RELEASABLES :
	@echo $(HEADERFILENAME)&& \
	@echo $(ICONTARGETFILENAME)

FINAL : do_nothing
