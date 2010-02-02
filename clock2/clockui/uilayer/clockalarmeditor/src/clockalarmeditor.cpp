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
* Description:   This is the source file for the CClockAlarmEditor class.
*
*/

// System includes
#include <e32cmn.h>
#include <avkon.rsg>
#include <clock.rsg>
#include <akntitle.h>
#include <eikspane.h>
#include <aknpopupsettingpage.h>
#include <StringLoader.h>
#include <wakeupalarm.h>
#include <bautils.h>
#include <clockalarmeditor.h>

// User includes
#include "clockalarmeditorimpl.h"
#include "clock_debug.h"
#include "clock.hrh"

// ---------------------------------------------------------
// CClockAlarmEditor::NewL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
EXPORT_C CClockAlarmEditor* CClockAlarmEditor::NewL( TAlarmId aExistingAlarmId )
    {
	__PRINTS( "CClockAlarmEditor::NewL - Entry" );

    CClockAlarmEditor* self = new( ELeave ) CClockAlarmEditor();
    
    CleanupStack::PushL( self );
    self->ConstructL( aExistingAlarmId );
    CleanupStack::Pop( self );
	
	__PRINTS( "CClockAlarmEditor::NewL - Exit" );
    
    return self;
    }

// ---------------------------------------------------------
// CClockAlarmEditor::~CClockAlarmEditor
// rest of the details are commented in the header
// ---------------------------------------------------------
//
EXPORT_C CClockAlarmEditor::~CClockAlarmEditor()
    {
	__PRINTS( "CClockAlarmEditor::~CClockAlarmEditor - Entry" );
	
	// No implementation yet.
	
    __PRINTS( "CClockAlarmEditor::~CClockAlarmEditor - Exit" );
    }

// ---------------------------------------------------------
// CClockAlarmEditor::LaunchL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
EXPORT_C TInt CClockAlarmEditor::LaunchL()
    {
    __PRINTS( "CClockAlarmEditor::LaunchL - Entry" );
    
	// Launch the editor.
    TInt returnVal( iClockAlarmEditorImpl->ExecuteLD() );
	
    if( KErrNone != returnVal &&
        EClockAlarmExit != returnVal &&
        EClockAlarmDelete != returnVal &&
        EClockAlarmDiscardChanges != returnVal ) 
        {
        __PRINTS( "CClockAlarmEditor::LaunchL - Exit" );
        
        // Return the value of the new alarm id.
        return iAlarmId;
        }
    
    __PRINTS( "CClockAlarmEditor::LaunchL - Exit" );
    
    return returnVal;
    }

// ---------------------------------------------------------
// CClockAlarmEditor::ConstructL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClockAlarmEditor::ConstructL( TAlarmId aExistingAlarmId )
    {
	__PRINTS( "CClockAlarmEditor::ConstructL - Entry" );
	
	iAlarmId = aExistingAlarmId;
	
	// Construct the alarm editor implementation.
	iClockAlarmEditorImpl = CClockAlarmEditorImpl::NewL( iAlarmId );
		
	__PRINTS( "CClockAlarmEditor::ConstructL - Exit" );
    }

// ---------------------------------------------------------
// CClockAlarmEditor::CClockAlarmEditor
// rest of the details are commented in the header
// ---------------------------------------------------------
//
CClockAlarmEditor::CClockAlarmEditor()
    {
	__PRINTS( "CClockAlarmEditor::CClockAlarmEditor - Entry" );
	
    // No implementation yet.
	
	__PRINTS( "CClockAlarmEditor::CClockAlarmEditor - Exit" );
    }

// End of file
