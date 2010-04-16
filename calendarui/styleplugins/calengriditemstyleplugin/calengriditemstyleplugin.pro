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
# Description: Project definition file for calengriditemstyleplugin
#

TEMPLATE = lib
TARGET = $$qtLibraryTarget(calengriditemstyleplugin)

CONFIG += plugin hb

INCLUDEPATH += .\
		   ./inc 

DEPENDPATH += . \
		  ./inc \
		  ./src \
		  ./data

symbian:{
TARGET.UID3 = 0x2001FE72
TARGET.EPOCALLOWDLLDATA = 1
TARGET.CAPABILITY = CAP_GENERAL_DLL
}

SOURCES += calengriditemstyleplugin.cpp 

HEADERS += calengriditemstyleplugin.h 

RESOURCES += calengriditemstyleplugin.qrc


