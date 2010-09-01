/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Calendar Application Document
*
*/



// Debug
#include "calendarui_debug.h"           // Calendar debug macros

// Include files
#include "CalenDoc.h"                     // CCalenDocument
#include "CalenAppUi.h"                 // CCalenAppUi
#include "calencontroller.h"           // CCalenController

// ----------------------------------------------------------------------------
// CCalenDocument::NewL
// First phase constructor
// Creates a new CCalenDocument instance
// ----------------------------------------------------------------------------
//
CCalenDocument* CCalenDocument::NewL( CEikApplication& aApp )
    {
    TRACE_ENTRY_POINT;
    
    CCalenDocument* self = new( ELeave )CCalenDocument( aApp );

    TRACE_EXIT_POINT;
    return self;
    }

// ----------------------------------------------------------------------------
// CCalenDocument::~CCalenDocument
// Destructor
// ----------------------------------------------------------------------------
//
CCalenDocument::~CCalenDocument()
    {
    TRACE_ENTRY_POINT;
    
    if ( iController )
        {
        iController->ReleaseCustomisations();
        iController->Release();
        }

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenDocument::CCalenDocument
// C++ default constructor
// ----------------------------------------------------------------------------
//
CCalenDocument::CCalenDocument( CEikApplication& aApp )
    : CAknDocument( aApp )
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenDocument::CreateAppUiL
// Creates a new CCalenAppUi instance
// ----------------------------------------------------------------------------
//
CEikAppUi* CCalenDocument::CreateAppUiL(void)
    {
    TRACE_ENTRY_POINT;
    
    TRACE_EXIT_POINT;
    return new( ELeave )CCalenAppUi;
    }

// ----------------------------------------------------------------------------
// CCalenDocument::SetController
// Takes ownership of the controller
// ----------------------------------------------------------------------------
//
void CCalenDocument::SetController( CCalenController* aController )
    {
    TRACE_ENTRY_POINT;
    
    iController = aController;
    
    TRACE_EXIT_POINT;
    };

//  End of File
