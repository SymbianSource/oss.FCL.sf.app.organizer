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
# Description: Project definition file for notes application.
#

TEMPLATE = app
CONFIG += hb
TARGET = notes
INCLUDEPATH += \
		. \
		../notesappcontroller/inc \
		../../inc

DEPENDPATH += \
		. \
		./src \
		../../data

symbian: {
	TARGET.CAPABILITY = ALL -TCB
	TARGET.EPOCALLOWDLLDATA = 1
	TARGET.UID3 = 0x20029F80
	TARGET.EPOCHEAPSIZE = 0x200000 0x4000000
	
	LIBS += \
		-lnotesappcontroller

	notes.sources += \
		agendainterface.dll \
		notesviews.dll \
		notesmodel.dll \
		noteseditor.dll \
		notesviewmanager.dll \
		notesmodelhandler.dll \
		notesappcontroller.dll

	notes.path = /sys/bin
	DEPLOYMENT += notes
	
	SKINICON = qtg_large_notes
}

SOURCES += main.cpp

RESOURCES += notes.qrc

TRANSLATIONS += notes.ts

# End of file	--Don't remove this.