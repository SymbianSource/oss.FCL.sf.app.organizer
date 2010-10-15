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
TARGET = utsettingsutility
CONFIG += qtestlib
CONFIG += symbian_test

DEPENDPATH += .
INCLUDEPATH += . \
				../../clockmw/clocksettingsutility/inc \
				../../inc

symbian: {
		BLD_INF_RULES.prj_testexports += \
					"./rom/unittest_settingsutility.iby		CORE_APP_LAYER_IBY_EXPORT_PATH(unittest_settingsutility.iby)"
		INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE
		
		LIBS += -leuser \
						-lclocksettingsutility 

		TARGET.EPOCHEAPSIZE = 0x200000 0x4000000
		TARGET.CAPABILITY = CAP_GENERAL_DLL
}

SOURCES += unittest_settingsutility.cpp

# End of file	--Don't remove this.
