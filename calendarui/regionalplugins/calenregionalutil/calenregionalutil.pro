#
# Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
# Description:  Project definition file for Calendar regional utils
#

TEMPLATE = lib
TARGET = calenregionalutil

BLD_INF_RULES.prj_exports += \
"./inc/calenextrarowformatter.h			|../inc/calenextrarowformatter.h" \
"./inc/calenlunarinfo.h					|../inc/calenlunarinfo.h" \
"./inc/calenlunarinfoprovider.h     		|../inc/calenlunarinfoprovider.h" \
"./inc/calenlunarlocalizedinfo.h   			|../inc/calenlunarlocalizedinfo.h" \
"./inc/calenlunarlocalizer.h   			|../inc/calenlunarlocalizer.h" \
"./inc/calenlunarpanic.h   				|../inc/calenlunarpanic.h" \
"./inc/CalenLunarPaths.h				|../inc/CalenLunarPaths.h" \
"./inc/CalenRegionalPluginUids.h			|../inc/CalenRegionalPluginUids.h" \
"./inc/CalenSolarTerms.h				|../inc/CalenSolarTerms.h"

# MMP Rules
mmpRules = \
"$${LITERAL_HASH}include <data_caging_paths.hrh>" \
"START RESOURCE ./data/calenregionalutil.rss" \
"HEADER" \
"TARGETPATH RESOURCE_FILES_DIR" \
"LANGUAGE_IDS" \
"END"

MMP_RULES +=mmpRules

symbian: {
TARGET.UID3 = 0x20022EDB	
TARGET.CAPABILITY = CAP_GENERAL_DLL

INCLUDEPATH += ./inc \
							 ../inc \
					     ../../inc \
							/epoc32/include \
				      /epoc32/include/platform \
				    /epoc32/include/platform/app \
 						/epoc32/include/platform/loc \
						/epoc32/include/app/exported \
						/epoc32/include/app \
					 /epoc32/include/platform/mw/loc \
					 /epoc32/include/platform/app/loc \
					 /epoc32/include/platform/loc/sc \
					 /epoc32/include/platform/mw/loc/sc \
					 /epoc32/include/platform/app/loc/sc \
						/epoc32/include/platform/mw \
						/epoc32/include/mw


LIBS += -leuser \
				-lcalinterimapi \
				-lecom \
				-lavkon \
				-lcone \
				-lgdi \
				-lbafl \
				-lplatformenv \
				-lefsrv \
				-lCommonEngine \
				-lccon \
				-lestor \
				-lcentralrepository \
				-lCdlEngine
		
}

SOURCES += ./src/calenextrarowformatter.cpp \
		   		 ./src/calenlunarinfo.cpp \
		   		 ./src/calenlunarinfoprovider.cpp \
		   		 ./src/calenlunarlocalizedinfo.cpp \
		   		 ./src/calenlunarlocalizer.cpp \
		   		 ./src/calensolarterms.cpp 

# End of file	--Don't remove this.
