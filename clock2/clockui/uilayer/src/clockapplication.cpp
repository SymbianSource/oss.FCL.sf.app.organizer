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
* Description:   This is the source file for the CClockApplication class.
*
*/

// System includes

// User includes
#include "clockapplication.h"
#include "clockdocument.h"
#include "clock.h"

// ---------------------------------------------------------
// CClockApplication::CreateDocumentL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
CApaDocument* CClockApplication::CreateDocumentL()
	{
	// Create the document object
	return CClockDocument::NewL( *this );
	}

// ---------------------------------------------------------
// CClockApplication::CreateDocumentL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
TUid CClockApplication::AppDllUid() const
	{
	return KClockAppUid;
	}

// End of file
