#
#  Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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
#  Description: calendaycontentwidget test project file
#

TEMPLATE = app
TARGET = unittest_calendayitem

MOC_DIR = moc

CONFIG += qtestlib
CONFIG += symbian_test
CONFIG += hb
				
INCLUDEPATH += . \
               ../../../inc/ \
               ../../../../inc/ 

DEPENDPATH  += . \
               ../../../inc/ \
               ../../../src/  

# Input
HEADERS +=	calendayitem.h \
            calendaystatusstrip.h \
			calendaycontainer.h \
			calendayutils.h
			

SOURCES +=	unittest_calendayitem.cpp \
			calendayitem.cpp \
			calendaystatusstrip.cpp 

symbian : {
	TARGET.CAPABILITY = CAP_APPLICATION
    MMP_RULES += "USERINCLUDE ."
    
    INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE
    
    LIBS += -lagendainterface
            
    BLD_INF_RULES.prj_testexports += \
        "./rom/unittest_calendayitem.iby        CORE_APP_LAYER_IBY_EXPORT_PATH(unittest_calendayitem.iby)"
}

# End of file	--Don't remove this.
