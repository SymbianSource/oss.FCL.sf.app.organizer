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
# Description: pro file for settingsutility unit test.
#


TEMPLATE = app
TARGET = utalarmclient
CONFIG += qtestlib
CONFIG += symbian_test
DEPENDPATH += .
INCLUDEPATH += . \
			   				../../inc \
               	../../clockmw/clockalarms/inc

# Input
SOURCES += unittest_alarmclient.cpp

symbian: {
		BLD_INF_RULES.prj_testexports += \
					"./rom/unittest_alarmclient.iby		CORE_APP_LAYER_IBY_EXPORT_PATH(unittest_alarmclient.iby)"
					
    LIBS += -leuser \
            -lclockalarmclient

    INCLUDEPATH += /epoc32/include/osextensions/stdapis/stlport
		INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE
    TARGET.EPOCHEAPSIZE = 0x200000 0x4000000
    TARGET.CAPABILITY = CAP_GENERAL_DLL
}

# End of file	--Don't remove this.
