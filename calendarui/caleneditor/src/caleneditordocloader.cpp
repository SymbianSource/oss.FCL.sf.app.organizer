/*
 * Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
 * All rights reserved.
 * This component and the accompanying materials are made available
 * under the terms of "Eclipse Public License v1.0"
 * which accompanies this distribution, and is available
 * at the URL "http://www.eclipse.org/legal/epl-v10.html".
 *
 * Initial Contributors:
 * Nokia Corporation - initial contribution.
 *
 * Contributors: 
 *
 * Description: Definition file for class CalenEditorDocLoader.
 *
 */

// User includes
#include "caleneditordocloader.h"

CalenEditorDocLoader::CalenEditorDocLoader()
{
}

CalenEditorDocLoader::~CalenEditorDocLoader()
{
	
}

QObject* CalenEditorDocLoader::createObject(const QString &type, const QString &name)
{
	return HbDocumentLoader::createObject(type, name);
}


// End of file	--Don't remove this.
