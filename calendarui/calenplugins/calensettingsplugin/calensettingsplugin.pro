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
# Description: This is the project file defnition for calensettingsplugin.
#


TEMPLATE = lib
TARGET = calensettingsplugin
DEPENDPATH += . \
			  ./inc \
			  ./src
INCLUDEPATH += . \
               ../../inc

CONFIG += hb plugin


symbian: {

    TARGET.EPOCALLOWDLLDATA = 1  
    TARGET.UID3 = 0x2002B338
	
    LIBS += -lcalensettings \
		-lcpframework

    deploy.path = C:
    headers.sources = qmakepluginstubs/calensettingsplugin.qtplugin
    headers.path = /resource/qt/plugins/controlpanel

    # This is for new exporting system coming in garden
    for(header, headers.sources):BLD_INF_RULES.prj_exports += "./$$header $$deploy.path$$headers.path/$$basename(header)"

    TARGET.CAPABILITY = CAP_GENERAL_DLL

    plugin.sources = calensettingsplugin.dll
    plugin.path = \resource\qt\plugins\controlpanel
    DEPLOYMENT += plugin
}

# Input
HEADERS += calensettingsplugin.h \
		   calendarsettingsview.h
		   
SOURCES += calensettingsplugin.cpp \
		   calendarsettingsview.cpp
		   

# End of file	--Don't remove this.