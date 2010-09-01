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
* Description:   This is the source file for the CAdtUpdaterApplication class.
*
*/

// System includes

// User includes
#include "adtupdaterapplication.h"
#include "adtupdaterdocument.h"
#include "adtupdater.h"
#include "clock_debug.h"

// ---------------------------------------------------------
// CAdtUpdaterApplication::CreateDocumentL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
CApaDocument* CAdtUpdaterApplication::CreateDocumentL()
	{
	__PRINTS( "CAdtUpdaterApplication::CreateDocumentL - Entry" );
	
	__PRINTS( "CAdtUpdaterApplication::CreateDocumentL - Exit" );	    
	    
	// Create the document object
	return CAdtUpdaterDocument::NewL( *this );
	}

// ---------------------------------------------------------
// CAdtUpdaterApplication::CreateDocumentL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
TUid CAdtUpdaterApplication::AppDllUid() const
	{
	__PRINTS( "CAdtUpdaterApplication::AppDllUid - Entry" );
	
	__PRINTS( "CAdtUpdaterApplication::AppDllUid - Exit" );
	
	return KAdtUpdaterAppUid;
	}

// End of file
