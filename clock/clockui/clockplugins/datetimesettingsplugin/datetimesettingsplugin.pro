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
# Description: 
# Project specification file for datetimesettingsplugin.
#

TEMPLATE = lib
TARGET = datetimesettingsplugin
DEPENDPATH += . \
			  ./inc \
			  ./src
INCLUDEPATH += . \
               ../../clocksettingsview/inc

CONFIG += hb plugin

LIBS += -lcpframework

# Input
HEADERS += datetimesettingsplugin.h \
		   datetimesettingsview.h
		   
SOURCES += datetimesettingsplugin.cpp \
		   datetimesettingsview.cpp

symbian: {
    TARGET.EPOCALLOWDLLDATA = 1  
    TARGET.UID3 = 0x102818E9
	
	LIBS += -lclocksettingsview
}

symbian {
    deploy.path = C:
    headers.sources = qmakepluginstubs/datetimesettingsplugin.qtplugin
    headers.path = /resource/qt/plugins/controlpanel

    # This is for new exporting system coming in garden
    for(header, headers.sources):BLD_INF_RULES.prj_exports += "./$$header $$deploy.path$$headers.path/$$basename(header)"
}
			   
TARGET.CAPABILITY = CAP_GENERAL_DLL

plugin.sources = datetimesettingsplugin.dll
plugin.path = \resource\qt\plugins\controlpanel
DEPLOYMENT += plugin