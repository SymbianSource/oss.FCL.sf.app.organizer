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
* Description:  ?Description
*
*/

// system includes
#include <AknPopupField.h>
#include <AknQueryValueText.h>
#include <CalenDefaultEditorsData.rsg>

// user includes
#include "calenpriorityfield.h"
#include "CalenDefaultEditors.hrh"
#include "CalenDescription.h"
#include "calennotedatautil.h"

// debug
#include "calendarui_debug.h"

// -----------------------------------------------------------------------------
// CCalenPriorityField::NewL
// First phase constructor
// -----------------------------------------------------------------------------
//
CCalenPriorityField* CCalenPriorityField::NewL( CCalenUnifiedEditor& aUnifiedEditor )
    {
    TRACE_ENTRY_POINT;

    CCalenPriorityField* self =
        new( ELeave ) CCalenPriorityField( aUnifiedEditor );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    TRACE_EXIT_POINT;
    return self;
    }

// -----------------------------------------------------------------------------
// CCalenPriorityField::CCalenPriorityField
// C++ constructor
// -----------------------------------------------------------------------------
//
CCalenPriorityField::CCalenPriorityField( CCalenUnifiedEditor& aUnifiedEditor )
    : iUnifiedEditor( aUnifiedEditor )
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenPriorityField::ConstructL
// second phase constructor
// -----------------------------------------------------------------------------
//
void CCalenPriorityField::ConstructL()
    {
    TRACE_ENTRY_POINT;

    // read resource for confideitiality popup list
    iPriorityArrayText = iUnifiedEditor.ControlEnv()->ReadDesCArrayResourceL(
            R_CALEN_EDITOR_PRIORITY_POPUP_ITEMS);

    // create textarray
    iPriorityArray = CAknQueryValueTextArray::NewL();
    iPriorityTextValues = CAknQueryValueText::NewL();
    iPriorityArray->SetArray( *iPriorityArrayText );
    iPriorityTextValues->SetArrayL( iPriorityArray );

    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenPriorityField::~CCalenPriorityField
// Destructor
// -----------------------------------------------------------------------------
//
CCalenPriorityField::~CCalenPriorityField()
    {
    TRACE_ENTRY_POINT;

    delete iPriorityArrayText;
    delete iPriorityArray;
    delete iPriorityTextValues;

    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenPriorityField::HandleControlStateChangedL
// Handle Event type change
// -----------------------------------------------------------------------------
//
void CCalenPriorityField::HandleControlStateChangeL( TInt aControlId )
    {
    TRACE_ENTRY_POINT;

    switch ( aControlId )
        {
        case ECalenEditorPriority:
            {
            NotifyPriorityChangeL();
            break;
            }
        default:
            {
            break;
            }
        }

    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------
// CCalenPriorityField::NotifyPriorityChangeL
// This function is used when the top item of each form
// ---------------------------------------------------------
//
void CCalenPriorityField::NotifyPriorityChangeL()
    {
    TRACE_ENTRY_POINT;

    // Nothing to do
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenPriorityField::ReadDataFromFormL
// Read data from form
// -----------------------------------------------------------------------------
//
void CCalenPriorityField::ReadDataFromFormL( TBool /*aContinueOnError*/ )
    {
    TRACE_ENTRY_POINT;
    
    TInt priority = iPriorityTextValues->CurrentValueIndex();
    iUnifiedEditor.Edited().SetPriorityL( static_cast<CCalenEntryUtil::TTodoPriority>( priority+1 ) );
    
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenPriorityField::SetDataToEditorL
// Update form data
// -----------------------------------------------------------------------------
//
void CCalenPriorityField::SetDataToEditorL()
    {
    TRACE_ENTRY_POINT;    

    // set priority item
    TInt priority = iUnifiedEditor.Edited().Priority();
    iPriorityTextValues->SetCurrentValueIndex( priority-1 );
    
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenPriorityField::InitPriorityFieldLayoutL
// Updates fields just before the form is shown.
// -----------------------------------------------------------------------------
//
void CCalenPriorityField::InitPriorityFieldLayoutL()
    {
    TRACE_ENTRY_POINT;

    CAknPopupField* pops =
        static_cast<CAknPopupField*>( iUnifiedEditor.Control( ECalenEditorPriority ) );
    pops->SetQueryValueL( iPriorityTextValues );
    
    // Set value to priority field in Editor
    SetDataToEditorL();

    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenPriorityField::PrepareForFocusTransitionL
// Prepare for focus change
// -----------------------------------------------------------------------------
//
void CCalenPriorityField::PrepareForFocusTransitionL( TInt aFocusedId )
    {
    TRACE_ENTRY_POINT;
    

    switch( aFocusedId )
        {
        case ECalenEditorPriority:
            {
            TBool continueOnError = EFalse;
            ReadDataFromFormL( continueOnError );
            break;
            }
        default: 
            break;
        }
    TRACE_EXIT_POINT;
    }

// End of File
