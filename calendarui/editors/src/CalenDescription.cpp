/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  To create and handle, entry description field 
*                of Unifiededitor.
*/

// system includes
#include <aknlistquerydialog.h>
#include <aknnotewrappers.h>
#include <AknQueryDialog.h>
#include <calentry.h>
#include <eikmenup.h>
#include <NpdApi.h>
#include <StringLoader.h>
#include <CalenDefaultEditorsData.rsg>

// user includes
#include "CalenDescription.h"
#include "CalenDescriptionField.h"
#include "calenunifiededitor.h"
#include "CalenDefaultEditors.hrh"

// debug
#include "calendarui_debug.h"

// ============================ MEMBER FUNCTIONS ===============================

// ---------------------------------------------------------------------------
// CCalenDescription::CCalenDescription
// C++ constructor can NOT contain any code, that might leave.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
CCalenDescription::CCalenDescription( CCalenUnifiedEditor& aEditor )
    : iEditor( aEditor )
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------------------------
// CCalenDescription::ConstructL
// Leaving construction.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CCalenDescription::ConstructL()
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------------------------
// CCalenDescription::NewL
// Two-phased constructor.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
CCalenDescription* CCalenDescription::NewL( CCalenUnifiedEditor& aEditor )
    {
    TRACE_ENTRY_POINT;

    CCalenDescription* self = new( ELeave ) CCalenDescription( aEditor );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    TRACE_EXIT_POINT;
    return self;
    }

// -----------------------------------------------------------------------------
// CCalenDescription::~CCalenDescription
// Destructor.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CCalenDescription::~CCalenDescription()
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------------------------
// CCalenDescription::CreateFieldL
// Create a description field.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
SEikControlInfo CCalenDescription::CreateFieldL()
    {
    TRACE_ENTRY_POINT;

    SEikControlInfo control = { NULL, 0, 0 };
    control.iControl = new ( ELeave ) CCalenDescriptionField( *this );
    control.iTrailerTextId = 0;
    control.iFlags = 0;

    TRACE_EXIT_POINT;
    return control;
    }

// ---------------------------------------------------------------------------
// CCalenDescription::InitDescritpionFieldLayoutL
// Updates fields just before the form is shown.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CCalenDescription::InitDescritpionFieldLayoutL()
    {
    TRACE_ENTRY_POINT;

    if ( EntryHasDescription() )
        {
        ShowFieldL();
        }

    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------------------------
// CCalenDescription::PostLayoutDynInitL
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CCalenDescription::PostLayoutDynInitL()
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------------------------
// CCalenDescription::DynInitMenuPaneL
// Called just before the options menu is shown.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CCalenDescription::DynInitMenuPaneL( TInt aResourceId, 
                                         CEikMenuPane* aMenuPane )
    {
    TRACE_ENTRY_POINT;
    
    // Handle unified editor menu pane
    if( aResourceId == R_CALEN_UNIFIED_EDITOR_MENUPANE )
        {
        aMenuPane->AddMenuItemsL( R_CALEN_DESCRIPTION_MENU_PANE );

        if( EntryHasDescription() )
            {
            // Entry already has Description, delete Add description menu item
            aMenuPane->DeleteMenuItem( ECalenCmdAddDescription );
            if ( !IsFieldFocused() )
                {
                // Description field not focused,
                // delete Show/Remove descripion menu item.
                aMenuPane->DeleteMenuItem( ECalenCmdShowDescription );
                aMenuPane->DeleteMenuItem( ECalenCmdRemoveDescription );
                }
            }
        else 
            {
            // Entry don't have description,
            // delete Show/Remove descripion menu item.
            aMenuPane->DeleteMenuItem( ECalenCmdShowDescription );
            aMenuPane->DeleteMenuItem( ECalenCmdRemoveDescription );
            }
        }

    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------------------------
// CCalenDescription::ProcessCommandL
// Process commands from the user.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TBool CCalenDescription::ProcessCommandL( TInt aCommandId )
    {
    TRACE_ENTRY_POINT;

    TBool processed = EFalse;
    switch( aCommandId )
        {
        case ECalenCmdAddDescription:
            {
            processed = HandleAddDescriptionL();
            break;
            }
        case ECalenCmdShowDescription:
            {
            ShowL();
            processed = ETrue;
            break;
            }
        case ECalenCmdRemoveDescription:
            {
            RemoveL();
            processed = ETrue;
            break;
            }
         default:
            break;            
        }

    TRACE_EXIT_POINT;
    return processed;
    }

// ---------------------------------------------------------------------------
// CCalenDescription::AddNewL
// Add a new description.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CCalenDescription::AddNewL()
    {
    TRACE_ENTRY_POINT;

    __ASSERT_DEBUG( !EntryHasDescription(), User::Invariant() );
    
    TInt status = KErrNone;
    HBufC* desc = ExecTextEditorL( status, KNullDesC );
    HandleAddL( status, desc );

    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------------------------
// CCalenDescription::AddMemoL
// Add a memo.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CCalenDescription::AddMemoL()
    {
    TRACE_ENTRY_POINT;

    __ASSERT_DEBUG( !EntryHasDescription(), User::Invariant() );

    HBufC* desc = CNotepadApi::FetchMemoL();
    HandleAddL( KErrNone, desc ); // ownership of description transferred

    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------------------------
// CCalenDescription::ShowL
// Show the description.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CCalenDescription::ShowL()
    {
    TRACE_ENTRY_POINT;

    __ASSERT_DEBUG( EntryHasDescription(), User::Invariant() );
    __ASSERT_DEBUG( IsFieldVisible(), User::Invariant() );
    __ASSERT_DEBUG( IsFieldFocused(), User::Invariant() );
  
    TInt status = KErrNone;
    
    // Get description text to 'oldDesc'
    const TDesC& oldDesc = iEditor.Edited().Description();

      
    // Open Description in edit mode
    HBufC* desc = ExecTextEditorL( status, oldDesc );

    HandleUpdateL( status, desc ); //ownership of description transferred

    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------------------------
// CCalenDescription::EditL
// Edit the description.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CCalenDescription::EditL()
    {
    TRACE_ENTRY_POINT;

    __ASSERT_DEBUG( EntryHasDescription(), User::Invariant() );
    __ASSERT_DEBUG( IsFieldVisible(), User::Invariant() );
    __ASSERT_DEBUG( IsFieldFocused(), User::Invariant() );

    TInt status = KErrNone;
    
    // Get description text to 'oldDesc'
    const TDesC& oldDesc = iEditor.Edited().Description();

    // Edit Descripton 'oldDesc', in notepad viewer
    HBufC* desc = ExecTextEditorL( status, oldDesc );
    HandleUpdateL( status, desc ); //ownership of description transferred

    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------------------------
// CCalenDescription::RemoveL
// Remove the description.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CCalenDescription::RemoveL()
    {
    TRACE_ENTRY_POINT;

    __ASSERT_DEBUG( EntryHasDescription(), User::Invariant() );
    __ASSERT_DEBUG( IsFieldVisible(), User::Invariant() );
    __ASSERT_DEBUG( IsFieldFocused(), User::Invariant() );

    // Ask remove description query.
    CAknQueryDialog* dlg = CAknQueryDialog::NewL();
    if( dlg->ExecuteLD(R_CALEN_REMOVE_DESCRIPTION_CONFIRMATION_QUERY) )
        {
        // remove description
        DoRemoveL();
        }

    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------------------------
// CCalenDescription::EntryHasDescription
// Returns ETrue if the entry has a desciption, EFalse otherwise.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TBool CCalenDescription::EntryHasDescription() const
    {
    TRACE_ENTRY_POINT;

    TRACE_EXIT_POINT;
    return iEditor.Edited().Description().Length() > 0;
    }

// ---------------------------------------------------------------------------
// CCalenDescription::IsEditedL
// Returns ETrue if the description has been edited, EFalse otherwise.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TBool CCalenDescription::IsEditedL() const
    {
    TRACE_ENTRY_POINT;

    TRACE_EXIT_POINT;
    return iEditor.EditorDataHandler().IsDescriptionEdited();
    }

// ---------------------------------------------------------------------------
// CCalenDescription::ShowFieldL
// Show the description field.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CCalenDescription::ShowFieldL()
    {
    TRACE_ENTRY_POINT;

    Field()->SetTextL( &( iEditor.Edited().Description() ) );

    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------------------------
// CCalenDescription::HideFieldL
// Hide the description field.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CCalenDescription::HideFieldL()
    {
    TRACE_ENTRY_POINT;

    if( iEditor.ControlOrNull( ECalenEditorDescription ) != NULL)
        {
        iEditor.DeleteLine( ECalenEditorDescription );
        }

    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------------------------
// CCalenDescription::Field
// Returns the description field.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
CCalenDescriptionField* CCalenDescription::Field()
    {
    TRACE_ENTRY_POINT;

    TRACE_EXIT_POINT;
    return static_cast< CCalenDescriptionField* >( iEditor.Control( ECalenEditorDescription ) );
    }

// ---------------------------------------------------------------------------
// CCalenDescription::IsFieldVisible
// Returns ETrue if the description field is visible, EFalse otherwise.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TBool CCalenDescription::IsFieldVisible()
    {
    TRACE_ENTRY_POINT;

    TRACE_EXIT_POINT;
    return iEditor.ControlOrNull( ECalenEditorDescription ) != NULL;
    }

// ---------------------------------------------------------------------------
// CCalenDescription::IsFieldFocused
// Returns ETrue if the description field is focused, EFalse otherwise.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TBool CCalenDescription::IsFieldFocused()
    {
    TRACE_ENTRY_POINT;

    if( IsFieldVisible() )
        {
        TRACE_EXIT_POINT;
        return Field()->IsFocused();
        }
    else 
        {
        TRACE_EXIT_POINT;
        return EFalse;
        }
    }

// ---------------------------------------------------------------------------
// CCalenDescription::DoRemoveL
// Remove the description field.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CCalenDescription::DoRemoveL()
    {
    TRACE_ENTRY_POINT;

    iEditor.Edited().SetDescriptionL( KNullDesC );
    const TDesC& emptyString = _L("");
    iEditor.SetEditorTextL( ECalenEditorDescription, &emptyString );
    iEditor.UpdateFormL();

    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------------------------
// CCalenDescription::HandleAddL
// Handles a new description being added. Sets the new description to the entry.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CCalenDescription::HandleAddL( TInt aStatus, HBufC* aDescription )
    {
    TRACE_ENTRY_POINT;

    if ( aStatus == KErrNone && aDescription != NULL ) // entry was edited
        {
        CleanupStack::PushL( aDescription );
        iEditor.Edited().SetDescriptionL( *aDescription );
        CleanupStack::PopAndDestroy( aDescription );

        ShowFieldL();
        iEditor.UpdateFormL();
        iEditor.TryChangeFocusToL( ECalenEditorDescription );
        }
    else // error 
        {
        delete aDescription;
        }

    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------------------------
// CCalenDescription::HandleUpdateL
// Handle the description being updated.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CCalenDescription::HandleUpdateL( TInt aStatus, HBufC* aDescription )
    {
    TRACE_ENTRY_POINT;

    CleanupStack::PushL( aDescription );
    if ( aStatus == CNotepadApi::ENpdDataDeleted || 
         aStatus == CNotepadApi::ENpdDataErased ) // entry was erased or deleted 
        {
        DoRemoveL();
        if( aStatus == CNotepadApi::ENpdDataErased )
            {
            // Info note is only shown when erasing from Notepad
            HBufC* infoText = StringLoader::LoadLC( R_QTN_CALE_INOTE_DESC_DEL_MEMO );
            CAknInformationNote* note = new ( ELeave ) CAknInformationNote( ETrue );
            note->ExecuteLD( *infoText );
            CleanupStack::PopAndDestroy( infoText );
            }
        }
    else if ( aStatus == KErrNone && aDescription != NULL ) 
        {
        CleanupStack::Pop( aDescription );
        iEditor.Edited().SetDescriptionTransferOwnershipL( aDescription );

        Field()->SetTextL( &( iEditor.Edited().Description() ) );
        iEditor.UpdateFormL();
        // push NULL to cleanup stack to keep it balanced 
        aDescription = NULL; 
        CleanupStack::PushL( aDescription );
        }
    else // error
        {
        // unless some error happened, we should never get here
        // do nothing
        } 
    CleanupStack::PopAndDestroy( aDescription );

    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------------------------
// CCalenDescription::ExecTextEditorL
// Open the notepad editor to allow editing of the description.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
HBufC* CCalenDescription::ExecTextEditorL( TInt& aStatus, const TDesC& aDescription )
    {
    TRACE_ENTRY_POINT;

    HBufC* title = StringLoader::LoadLC( R_QTN_CALE_NOTE_TITLE );
    HBufC* delConf = StringLoader::LoadLC( R_QTN_CALE_Q_DEL_NOTEP_DESC );
    HBufC* desc = CNotepadApi::ExecTextEditorL( aStatus, aDescription,
                                                *title, *delConf );
    CleanupStack::PopAndDestroy( delConf );
    CleanupStack::PopAndDestroy( title );

    TRACE_EXIT_POINT;
    return desc;
    }

// ---------------------------------------------------------------------------
// CCalenDescription::ExecTextViewerL
// Open the notepad viewer to allow viewing of the description.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
HBufC* CCalenDescription::ExecTextViewerL( TInt& aStatus, const TDesC& aDescription )
    {
    TRACE_ENTRY_POINT;

    HBufC* title = StringLoader::LoadLC( R_QTN_CALE_NOTE_TITLE );
    HBufC* delConf = StringLoader::LoadLC( R_QTN_CALE_Q_DEL_NOTEP_DESC );
    HBufC* desc = CNotepadApi::ExecTextViewerL( aStatus, aDescription,
                                                *title, *delConf );
    CleanupStack::PopAndDestroy( delConf );
    CleanupStack::PopAndDestroy( title );

    TRACE_EXIT_POINT;
    return desc;
    }

// ----------------------------------------------------------------------------
// CCalenDescription::HandleAddDescriptionL
// Handles the command ECalenAddDescription
// ----------------------------------------------------------------------------
// 
TBool CCalenDescription::HandleAddDescriptionL()
    {
    TRACE_ENTRY_POINT;
    
    TBool processed = EFalse;
    TInt selectedIndex(0);
    CAknListQueryDialog* dlg = new (ELeave) CAknListQueryDialog(&selectedIndex);
    dlg->PrepareLC( R_DESCRIPTION_LIST_QUERY );
    
    if(dlg->RunLD())
        {
        switch(selectedIndex)
            {
            case 0:             //ECalenCmdAddDescriptionNew
                {
                AddNewL();
                processed = ETrue;
                break;
                }
            case 1:             //ECalenCmdAddDescriptionExisting
                {
                AddMemoL();
                processed = ETrue;
                break;
                }
            default:
                break;
            }
        }
	
	TRACE_EXIT_POINT;
	
    return processed;
    
    }
//  End of File
