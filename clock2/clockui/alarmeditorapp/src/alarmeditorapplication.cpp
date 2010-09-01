/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   This is the source file for the CAlarmEditorApplication class.
*
*/

// System includes

// User includes
#include "alarmeditorapplication.h"
#include "alarmeditordocument.h"
#include "alarmeditorapp.h"

// ---------------------------------------------------------
// CAlarmEditorApplication::CreateDocumentL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
CApaDocument* CAlarmEditorApplication::CreateDocumentL()
	{
	// Create the document object
	return CAlarmEditorDocument::NewL( *this );
	}

// ---------------------------------------------------------
// CAlarmEditorApplication::CreateDocumentL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
TUid CAlarmEditorApplication::AppDllUid() const
	{
	return KAlarmEditorAppUid;
	}

// End of file
