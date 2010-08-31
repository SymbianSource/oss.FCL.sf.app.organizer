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
TARGET = unittest_calendayview

MOC_DIR = moc

CONFIG += qtestlib
CONFIG += symbian_test
CONFIG += hb
				
INCLUDEPATH += . \
               ../../../inc/ \
               ../../../../../inc/

DEPENDPATH  += . \
               ../../../inc/ \
               ../../../src/ \
               ../../../../../inc/

# Input
HEADERS +=	hbeffect.h \
			calendateutils.h \
			calenservices.h \
			calencontext.h \
			calennotificationhandler.h \
			calenview.h \
			calennativeview.h \
			hb_calencommands.hrh \
			calendocloader.h \
			calendarcrkeys.h \
			calenuid.h \
			calenpluginlabel.h \
			calendaymodelmanager.h \
			calendaycontentwidget.h \
			calendaycontentscrollarea.h \
			calendayhourscrollarea.h \
			calendayutils.h \
			calendateutils.h \
			calendayview.h \
			calencommon.h

SOURCES +=	unittest_calendayview.cpp \
			calendayview.cpp 

symbian : {
	TARGET.CAPABILITY = CAP_APPLICATION
    MMP_RULES += "USERINCLUDE ."
    
    INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE
    
        LIBS += -lagendainterface \
    		-lxqsettingsmanager
}

# End of file	--Don't remove this.
