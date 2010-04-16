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
# Description:  FTU datetime wizard project file
#

TEMPLATE      	= lib

CONFIG       	+= plugin hb
TARGET        	= ftudatetimewizard

HEADERS += ./inc/*.h
SOURCES += ./src/*.cpp

RESOURCES += resources/ftudatetimewizard.qrc

plugin.sources 	= ftudatetimewizard.dll
plugin.path 	= \resource\qt\plugins\FTU
 
pluginstub.sources = resource/*.manifest
pluginstub.path = /fturesources/plugins/wizardproviders 

DEPLOYMENT 	+= plugin pluginstub

CONFIG(debug, debug|release) {
    DESTDIR = debug
    LIBS += -lftuwizardmodel
} else {
    DESTDIR = release
    LIBS += -lftuwizardmodel
}

CONFIG += plugin debug_and_release

QT += xml sql

DEFINES += FTUDATETIMEWIZARD_LIB

coverage:DEFINES += COVERAGE_MEASUREMENT

DEPENDPATH +=   ./inc \
                ./src

INCLUDEPATH +=  ./inc/ \
	  	../clockmw/clocksettingsutility/inc \
	  	../clockmw/clocktimezone/inc \
	  	../clockengines/inc \
	  	../clockengines/clockserver/server/inc \
	  	../clockui/clockcityselectionlist/inc \
	    ../inc   
symbian: {
    TARGET.UID3 = 0x2002C30C
    TARGET.EPOCALLOWDLLDATA=1
    TARGET.CAPABILITY = ALL -TCB
    TARGET.CAPABILITY = CAP_GENERAL_DLL
    INCLUDEPATH +=  $$MW_LAYER_SYSTEMINCLUDE
	
	LIBS += -leuser \
			-lclocksettingsutility \
			-lclockserverclient \
			-ltimezoneclient \
			-lxqsettingsmanager \
			-lclockcityselectionlist

}    

# End of file	--Don't remove this.