#
# Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
# Description:  Project definition file for calendar views.
#
TEMPLATE = lib

TARGET = calenviews

DEPENDPATH += ./resources \
			  ./inc \
			  ./src

DEFINES += CALENVIEWS_DLL

INCLUDEPATH += ./inc \
			   ../inc \
			   ../controller/inc \
			   ../cenrep

							
CONFIG += hb

symbian: { 
    LIBS += -leuser \
			-lcalinterimapi \
			-lcalencommonutils \
			-lcalensettings \
			-lagendainterface \
			-lxqsettingsmanager \
			-lnoteseditor
    TARGET.UID3 = 0x10282F26
    TARGET.CAPABILITY = CAP_GENERAL_DLL

		TARGET.EPOCALLOWDLLDATA = 1
		INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE
    }

# Input
HEADERS += calenmonthview.h \
		   calenmonthdata.h \
		   calenmonthgrid.h \
		   calengriditemprototype.h \
		   calennativeview.h \
		   calendayview.h \
		   caleneventlistviewitem.h \
		   calendayviewwidget.h \
		   calenpreviewpane.h \
		   calensettingsview.h \
		   calenthicklinesdrawer.h \
		   calenpluginlabel.h
	
SOURCES += calenmonthview.cpp \
           calenmonthdata.cpp \
           calenmonthgrid.cpp \
           calengriditemprototype.cpp \
           calennativeview.cpp \
           calendayview.cpp \
           caleneventlistviewitem.cpp \
           calendayviewwidget.cpp \
           calenpreviewpane.cpp \
           calensettingsview.cpp \
           calenthicklinesdrawer.cpp \
		   calenpluginlabel.cpp

RESOURCES += calenresources.qrc

# End of file	--Don't remove this.

