#
# Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
# Description: Project file for LunarChinese plugin module.
#


TEMPLATE = lib
CONFIG += hb
TARGET = calenlunarchineseplugin
DEPENDPATH +=  ./inc ./src ./data
INCLUDEPATH +=  ./inc 
INCLUDEPATH += ../inc ../../inc
INCLUDEPATH += /epoc32/include/ecom
INCLUDEPATH += /orbit/include
INCLUDEPATH += /epoc32/include/osextensions/stdapis/stlport \
						   /sf/mw/hb/include/hbservices \
						   /sf/mw/hb/include/hbservices/private \
						   /sf/mw/hb/include/hbcore \
						   /sf/mw/hb/hbcore/private



BLD_INF_RULES.prj_exports += \
"./data/solaritems  /epoc32/data/z/data/solaritems" 

# MMP Rules
mmpRules = \
"$${LITERAL_HASH}include <platform_paths.hrh>" \
#"$${LITERAL_HASH}include <../inc/CalenRegionalPluginUids.h>" \
"$${LITERAL_HASH}include <data_caging_paths.hrh>" \
"APP_LAYER_SYSTEMINCLUDE" \
	\
"SOURCEPATH      ./data" \
"START RESOURCE hb_calenlunarchineseplugin.rss" \
"TARGET hb_calenlunarchineseplugin.rsc" \
"TARGETPATH ECOM_RESOURCE_DIR" \
"LANGUAGE_IDS" \
"END" \
\
"START RESOURCE  ./data/calenlunarchineseplugindata.rss" \
"HEADER" \
"TARGETPATH      RESOURCE_FILES_DIR" \
"LANGUAGE_IDS" \
"END"

MMP_RULES +=mmpRules
symbian: {

TARGET.UID2 = 0x10009D8D #SYMBIAN_ECOM_PLUGIN_UID 
TARGET.UID3 = 0x102823A8 #CALENLUNARCHINESEPLUGIN_DLL_UID 
TARGET.CAPABILITY = CAP_ECOM_PLUGIN 

}
# Input
HEADERS += ./inc/CalenLunarChinesePlugin.h 

SOURCES +=  ./src/CalenLunarChinesePlugin.cpp \
            ./src/CalenLunarChinesePluginProxy.cpp 
          
LIBS +=    -lcalinterimapi \
			-lcone \
            -lecom \
            -leikcdlg \
            -leikctl \
            -leikcoctl \
            -leikcore \
            -leuser \
            -lgdi \
            -lbafl \
            -lplatformenv \
            -lefsrv \
            -lCommonEngine \
            -lccon \
            -lestor \
            -lcentralrepository \
            -lCdlEngine \
            -laknskins \
	    -laknicon \
	    -legul \
	    -lflogger \	 
		-lavkon \
            -lcalenregionalutil

# End of file	--Don't remove this.