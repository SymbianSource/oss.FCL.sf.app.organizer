/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Constant value definition.
*
*/



#ifndef _NOTESSEARCHPLUGIN_H
#define _NOTESSEARCHPLUGIN_H




_LIT(KNotesAlreadyActive,   "Notes Plugin");
_LIT(ECoePanicWrongResourceFormat,"Notes Plugin Worng resource");
  
enum TContactSearchPlugInPanics
	{
	ENotesSearchPlugInWrongConstructionParameters = 1,
	EPanicAlreadyActive,
	ENotesSearchPlugInNoOperation,
	EPanicWrongResource
	};

#endif // _NOTESSEARCHPLUGIN_H


