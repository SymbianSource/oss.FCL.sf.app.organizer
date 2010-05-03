#
#  Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
#  All rights reserved.
#  This component and the accompanying materials are made available
#  under the terms of "Eclipse Public License v1.0"
#  which accompanies this distribution, and is available
#  at the URL "http://www.eclipse.org/legal/epl-v10.html".
#  
#  Initial Contributors:
#  Nokia Corporation - initial contribution.
#  
#  Contributors: 
#  
#  Description: This is the project specification file for the calendar exe.
#  

TEMPLATE = app
TARGET = calendar
DEPENDPATH += ./src \
 		./inc \
		../controller/data
		
INCLUDEPATH += ..\controller\inc \
							 ..\inc

CONFIG += hb


symbian: { 

	BLD_INF_RULES.prj_exports += \
	"../rom/calendar.iby					CORE_APP_LAYER_IBY_EXPORT_PATH(calendar.iby)" \
	"../rom/calendarresources.iby			LANGUAGE_APP_LAYER_IBY_EXPORT_PATH(calendarresources.iby)" \
	"../rom/calenregionalutil.iby			LANGUAGE_APP_LAYER_IBY_EXPORT_PATH(calenregionalutil.iby)" \
	"../rom/calenlunarchinese_variant.iby	CUSTOMER_APP_LAYER_IBY_EXPORT_PATH(calenlunarchinese_variant.iby)" \
	# Generic configuration interface for component cenrep settings  
	"../conf/calendarUI.confml				APP_LAYER_CONFML(calendarUI.confml)" \
	"../conf/calendarUI_101F874B.crml		APP_LAYER_CRML(calendarUI_101F874B.crml)" 	

	LIBS += -lcalencontroller \
		-lxqservice \ 
		-lxqserviceutil

    TARGET.UID3 = 0x10005901
    
    TARGET.CAPABILITY = CAP_APPLICATION \
                		NetworkControl
    TARGET.EPOCSTACKSIZE = 0x14000
    TARGET.EPOCHEAPSIZE = 0x200000  0x1000000

    	
    INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE

    CONFIG += service
    SERVICE.FILE = calendar_conf.xml

    RESOURCES += calendar.qrc

    TRANSLATIONS += calendar.ts

}

# Input
HEADERS += calenserviceprovider.h

SOURCES += main.cpp \
	     calenserviceprovider.cpp	

# End of file	--Don't remove this.