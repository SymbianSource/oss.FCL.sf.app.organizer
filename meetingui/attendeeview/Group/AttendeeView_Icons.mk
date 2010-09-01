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
# Description:  Script to build icon svg files to get mif and mbg files.
#


# ----------------------------------------------------------------------------
# Inspect platform variables to find out root of the Z drive.
# ----------------------------------------------------------------------------
ifeq (WINS,$(findstring WINS, $(PLATFORM)))
  ZDIR=\epoc32\release\$(PLATFORM)\$(CFG)\Z
else
  ZDIR=\epoc32\data\z
endif


MIF_TARGETDIR=$(ZDIR)\resource\apps
MIF_ICONFILE=$(MIF_TARGETDIR)\attendeeview.mif
HEADERDIR=\epoc32\include
MIF_HEADERFILE=$(HEADERDIR)\attendeeview.mbg

do_nothing :
	@rem do_nothing

MAKMAKE : do_nothing

BLD : do_nothing

CLEAN : do_nothing

LIB : do_nothing

CLEANLIB : do_nothing

RESOURCE :
	mifconv $(MIF_ICONFILE) /h$(MIF_HEADERFILE) \
		/c8,8 qgn_prop_cale_organizer \
		/c8,8 qgn_prop_cale_accepted \
		/c8,8 qgn_prop_cale_tentative \
		/c8,8 qgn_prop_cale_rejected \
		/c8,8 qgn_prop_cale_notanswered
		
FREEZE : do_nothing

SAVESPACE : do_nothing

RELEASABLES :
	@echo $(MIF_HEADERFILE)&& \
	@echo $(MIF_ICONFILE)

FINAL : do_nothing

#End of file
