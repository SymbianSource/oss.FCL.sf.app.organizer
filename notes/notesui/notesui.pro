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
# Description: This is the project specification file the notesui.
#

TEMPLATE = subdirs

SUBDIRS += \
		notesmodel \
		notesviews \
		notesviewmanager \
		notesmodelhandler \
		notesappcontroller \
		notesapplication
CONFIG += ordered

# End of file	--Don't remove this.
