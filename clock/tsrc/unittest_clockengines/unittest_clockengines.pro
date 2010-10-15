#
# Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
# All rights reserved.
# This component and the accompanying materials are made available
# under the terms of the License "Symbian Foundation License v1.0"
# which accompanies this distribution, and is available
# at the URL "http://www.symbianfoundation.org/legal/sfl-v10.html".
#
# Initial Contributors:
# Nokia Corporation - initial contribution.
#
# Contributors:
# 
# Description: 
# Project specification file for settingsutility unit test.
#


TEMPLATE = app
TARGET = utclockengines
CONFIG += qtestlib
CONFIG += symbian_test

DEPENDPATH += \
		. \
		./src

INCLUDEPATH += . \
               ../../clockengines \
               ../../clockengines/inc \
               ../../clockengines/clockserver/client/inc \
               ../../clockengines/clockserver/server/inc \
               ../../clockengines/clocktimezoneresolver/inc \
               ../../clockengines/clocktimesourceinterface/inc \
               ../../clockui/inc \
               ../../inc               

symbian: {

    LIBS += -leuser \
         -lclockserverclient \
         -lclocktimesourceinterface \
         -lclocktimezoneresolver
         
    BLD_INF_RULES.prj_testexports += \
		"./rom/unittest_clockengines.iby		CORE_APP_LAYER_IBY_EXPORT_PATH(unittest_clockengines.iby)" 		
    
    TARGET.EPOCHEAPSIZE = 0x200000 0x4000000
    TARGET.CAPABILITY = CAP_GENERAL_DLL
    INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE
}

SOURCES += \
		unittest_clockengines.cpp \


HEADERS += \
		./inc/unittest_clockengines.h

# End of file	--Don't remove this.
