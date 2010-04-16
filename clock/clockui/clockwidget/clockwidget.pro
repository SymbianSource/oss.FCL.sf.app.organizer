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
# Description:  Project definition file for clockui
# 

TEMPLATE = lib
TARGET = clockwidgetproto
DEFINES += CLOCKWIDGET_LIB
CONFIG += hb

DEPENDPATH += . \
			  ./src \
			  ./inc \
			  ../../data
INCLUDEPATH += . \
			   ../../clockmw/clocktimezone/inc \
			   ../../clockmw/clocksettingsutility/inc \
			   ../../inc

symbian: {
	TARGET.CAPABILITY = ALL -TCB
	TARGET.EPOCALLOWDLLDATA = 1
	TARGET.UID3 = 0x2002E6B4

	LIBS += -ltimezoneclient \
			-lclocksettingsutility
}

SOURCES += skinnableclock.cpp \
		   analogclock.cpp \
		   digitalclock.cpp

HEADERS += skinnableclock.h \
		   analogclock.h \
		   digitalclock.h

RESOURCES += ../../data/clockwidget.qrc

# End of file	--Don't remove this.
