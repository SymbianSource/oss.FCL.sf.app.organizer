/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies).
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
#include "caleneventtypefield.h"
#include "CalenDefaultEditors.hrh"
#include "CalenDescription.h"
#include "calennotedatautil.h"

// debug
#include "calendarui_debug.h"

// -----------------------------------------------------------------------------
// CCalenEventTypeField::NewL
// First phase constructor
// -----------------------------------------------------------------------------
//
CCalenEventTypeField* CCalenEventTypeField::NewL( CCalenUnifiedEditor& aUnifiedEditor )
    {
    TRACE_ENTRY_POINT;

    CCalenEventTypeField* self =
        new( ELeave ) CCalenEventTypeField( aUnifiedEditor );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    TRACE_EXIT_POINT;
    return self;
    }

// -----------------------------------------------------------------------------
// CCalenEventTypeField::CCalenEventTypeField
// C++ constructor
// -----------------------------------------------------------------------------
//
CCalenEventTypeField::CCalenEventTypeField( CCalenUnifiedEditor& aUnifiedEditor )
    : iUnifiedEditor( aUnifiedEditor )
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenEventTypeField::ConstructL
// second phase constructor
// -----------------------------------------------------------------------------
//
void CCalenEventTypeField::ConstructL()
    {
    TRACE_ENTRY_POINT;

    // read resource for confideitiality popup list
    iEventTypeArrayText = iUnifiedEditor.ControlEnv()->ReadDesCArrayResourceL(
            R_CALEN_EDITOR_EVENT_TYPE_POPUP_ITEMS);

    // create textarray
    iEventTypeArray = CAknQueryValueTextArray::NewL();
    iEventTypeTextValues = CAknQueryValueText::NewL();
    iEventTypeArray->SetArray( *iEventTypeArrayText );
    iEventTypeTextValues->SetArrayL( iEventTypeArray );

    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenEventTypeField::~CCalenEventTypeField
// Destructor
// -----------------------------------------------------------------------------
//
CCalenEventTypeField::~CCalenEventTypeField()
    {
    TRACE_ENTRY_POINT;

    delete iEventTypeArrayText;
    delete iEventTypeArray;
    delete iEventTypeTextValues;

    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenEventTypeField::HandleControlStateChangedL
// Handle Event type change
// -----------------------------------------------------------------------------
//
void CCalenEventTypeField::HandleControlStateChangeL( TInt aControlId )
    {
    TRACE_ENTRY_POINT;

    switch ( aControlId )
        {
        case ECalenEditorEventType:
            {
            NotifyEventTypeChangeL();
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
// CCalenEventTypeField::NotifyEventTypeChangeL
// This function is used when the top item of each form
// ---------------------------------------------------------
//
void CCalenEventTypeField::NotifyEventTypeChangeL()
    {
    TRACE_ENTRY_POINT;

    TInt eventType = iEventTypeTextValues->CurrentValueIndex();
    
    switch(eventType)
        {
        case EMeeting:
            iUnifiedEditor.EditorFields().OnEventTypeChangedL( CCalEntry::EAppt );
            break;
        case EBirthDay:
            iUnifiedEditor.EditorFields().OnEventTypeChangedL( CCalEntry::EAnniv );
            break;
        case ETodo:
            iUnifiedEditor.EditorFields().OnEventTypeChangedL( CCalEntry::ETodo );
            break;
        default:
            break;
        }
    
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenEventTypeField::ReadDataFromFormL
// Read data from form
// -----------------------------------------------------------------------------
//
void CCalenEventTypeField::ReadDataFromFormL( TBool /*aContinueOnError*/ )
    {
    TRACE_ENTRY_POINT;
    // Nothing to do
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenEventTypeField::SetDataToEditorL
// Update form data
// -----------------------------------------------------------------------------
//
void CCalenEventTypeField::SetDataToEditorL()
    {
    TRACE_ENTRY_POINT;
    
    CCalEntry::TType entryType = iUnifiedEditor.GetEntryType();
    
    switch( entryType )
        {
        case CCalEntry::EEvent:
        	iEventTypeTextValues->SetCurrentValueIndex( EMeeting );
            break;
        case CCalEntry::EAppt:
            iEventTypeTextValues->SetCurrentValueIndex( EMeeting );
            break;
        case CCalEntry::EAnniv: 
            iEventTypeTextValues->SetCurrentValueIndex( EBirthDay );
            break;
        case CCalEntry::ETodo:
            iEventTypeTextValues->SetCurrentValueIndex( ETodo );
            break;
        default:
            break;
        }
   
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenEventTypeField::InitEventTypeFieldLayoutL
// Updates fields just before the form is shown.
// -----------------------------------------------------------------------------
//
void CCalenEventTypeField::InitEventTypeFieldLayoutL()
    {
    TRACE_ENTRY_POINT;

    CAknPopupField* pops =
        static_cast<CAknPopupField*>( iUnifiedEditor.Control( ECalenEditorEventType ) );
    pops->SetQueryValueL( iEventTypeTextValues );
    
    SetDataToEditorL();

    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenEventTypeField::PrepareForFocusTransitionL
// Prepare for focus change
// -----------------------------------------------------------------------------
//
void CCalenEventTypeField::PrepareForFocusTransitionL( TInt /*aFocusedId*/ )
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    }

// End of File
