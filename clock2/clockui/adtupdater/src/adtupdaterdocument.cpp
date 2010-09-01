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
* Description:   This is the source file for the CAdtUpdaterDocument class.
*
*/

// System includes

// User includes
#include "adtupdaterdocument.h"
#include "adtupdaterappui.h"
#include "clock_debug.h"

// ---------------------------------------------------------
// CAdtUpdaterDocument::NewL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
CAdtUpdaterDocument* CAdtUpdaterDocument::NewL( CEikApplication& aApp )
    {
    __PRINTS( "CAdtUpdaterDocument::NewL - Entry" );
    
    CAdtUpdaterDocument* selfObj = new ( ELeave ) CAdtUpdaterDocument( aApp );
    CleanupStack::PushL( selfObj );
    
    selfObj->ConstructL();
    
    CleanupStack::Pop( selfObj );
    
    __PRINTS( "CAdtUpdaterDocument::NewL - Exit" );
    
    return selfObj;
    }

// ---------------------------------------------------------
// CAdtUpdaterDocument::~CAdtUpdaterDocument
// rest of the details are commented in the header
// ---------------------------------------------------------
//
CAdtUpdaterDocument::~CAdtUpdaterDocument()
    {
    __PRINTS( "CAdtUpdaterDocument::~CAdtUpdaterDocument - Entry" );
    
    // No implementation yet.
    
    __PRINTS( "CAdtUpdaterDocument::~CAdtUpdaterDocument - Exit" );
    }

// ---------------------------------------------------------
// CAdtUpdaterDocument::CAdtUpdaterDocument
// rest of the details are commented in the header
// ---------------------------------------------------------
//
CAdtUpdaterDocument::CAdtUpdaterDocument( CEikApplication& aApp ) : CAknDocument( aApp )
    {
    __PRINTS( "CAdtUpdaterDocument::CAdtUpdaterDocument - Entry" );
    
    // No implementation yet.
    
    __PRINTS( "CAdtUpdaterDocument::CAdtUpdaterDocument - Exit" );
    }

// ---------------------------------------------------------
// CAdtUpdaterDocument::ConstructL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CAdtUpdaterDocument::ConstructL()
    {
    __PRINTS( "CAdtUpdaterDocument::ConstructL - Entry" );
    
    // No implementation yet.
    
    __PRINTS( "CAdtUpdaterDocument::ConstructL - Exit" );
    }

// ---------------------------------------------------------
// CAdtUpdaterDocument::CreateAppUiL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
CEikAppUi* CAdtUpdaterDocument::CreateAppUiL()
    {
    __PRINTS( "CAdtUpdaterDocument::CreateAppUiL - Entry" );
    
    __PRINTS( "CAdtUpdaterDocument::CreateAppUiL - Exit" );
    
    return new( ELeave ) CAdtUpdaterAppUi;
    }

// End of file
