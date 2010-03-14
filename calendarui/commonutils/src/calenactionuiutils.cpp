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
* Description:  Implementation of UI utils
*
*/


#include <calencommonui.rsg>
#include <Calendar.rsg>
#include <calenagendautils.h>
#include <aknlists.h>
#include <aknnotewrappers.h>
#include <AknQueryDialog.h>
#include <AknUtils.h>
#include <calalarm.h>
#include <calentry.h>
#include <calentryview.h>
#include <ErrorUI.h>
#include <StringLoader.h>
#include <sysutil.h>
#include <calentry.h>
#include <calinstance.h>
#include <calinstanceview.h>
#include <calencommands.hrh>            // Calendar commands
#include <calendateutils.h>
#include <caleninstanceid.h>            // TCalenInstanceId
#include <calenactionuiutils.h>

#include "calendarui_debug.h"           // Debug
#include "calendar.hrh"
#include "calenicons.h"
#include "calenglobaldata.h"
#include "CalenInterimUtils2.h"
#include "CleanupResetAndDestroy.h"

// Constants
const TInt KArrayGranularity = 5;

// ----------------------------------------------------------------------------
// CalenActionUiUtils::ShowDeleteConfirmationQueryL
// Shows a delete confirmation query.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
EXPORT_C TInt CalenActionUiUtils::ShowDeleteConfirmationQueryL( 
        const TDeleteConfirmationType aType, 
        const TInt aCount )
    {
    TRACE_ENTRY_POINT;

    CAknQueryDialog* dialog = CAknQueryDialog::NewL();
    CleanupStack::PushL( dialog );

    TInt resID( 0 );

    switch( aType )
        {
        case EDeleteEntry:
            resID = R_CALEN_COMMONUI_QTN_CALE_QUEST_DELETE_EVENT;
            break;
        case EDeleteToDo:
            resID = R_CALEN_COMMONUI_QTN_TODO_QUEST_DELETE_NOTE;
            break;
        case EDeleteToDos:
            resID = R_CALEN_COMMONUI_QTN_TODO_QUEST_DELETE_NOTES;
            break;
        case EDeleteAll:
            resID = R_CALEN_COMMONUI_QTN_CALE_QUEST_DELETE_ALL_NOTES;
            break;
        default:
            User::Leave( KErrArgument );
            break;
        }

    HBufC* prompt;

    // if >1 event to be deleted, load multi-deletion confirmation string
    if( aCount > 1 )
        {
        prompt = StringLoader::LoadLC( resID, aCount );
        }
    // otherwise load common deletion confirmation string
    else
        {
        prompt = StringLoader::LoadLC( resID );
        }

    dialog->SetPromptL( *prompt );
    CleanupStack::PopAndDestroy( prompt );

    CleanupStack::Pop( dialog );

    TRACE_EXIT_POINT;
    return dialog->ExecuteLD( R_CALEN_COMMONUI_ERASEQUERY_NOTE );
    }

// ----------------------------------------------------------------------------
// CalenActionUiUtils::ShowRepeatTypeQueryL
// Shows a query asking the user to choose the repeat type.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
EXPORT_C TBool CalenActionUiUtils::ShowRepeatTypeQueryL( 
        CalCommon::TRecurrenceRange& aAnswer, 
        const TRepeatQueryType aType )
    {
    TRACE_ENTRY_POINT;

    CAknSinglePopupMenuStyleListBox* list;
    list = new( ELeave ) CAknSinglePopupMenuStyleListBox;
    CleanupStack::PushL( list );
    
    CAknPopupList* popupList;
    popupList = CAknPopupList::NewL( list, R_AVKON_SOFTKEYS_OK_CANCEL );
    CleanupStack::PushL( popupList );

    list->ConstructL( popupList, CEikListBox::ELeftDownInViewRect );
    list->CreateScrollBarFrameL(ETrue);
    list->ScrollBarFrame()->SetScrollBarVisibilityL( CEikScrollBarFrame::EOff,
                                                                  CEikScrollBarFrame::EAuto );
    CEikonEnv* eikEnv = CEikonEnv::Static();

    CDesCArrayFlat* items = eikEnv->ReadDesCArrayResourceL( aType == EEdit ?
                                R_CALEN_COMMONUI_REPEATNOTE_EDIT_CHOICELIST :
                                R_CALEN_COMMONUI_REPEATNOTE_CHOICELIST );
    CTextListBoxModel* model = list->Model();
    model->SetItemTextArray( items );

    TInt resID( 0 );

    switch( aType )
        {
       case ESave:
           resID = R_CALEN_COMMONUI_QTN_CALE_LQ_SAVE_CHANG_REPEATED;
           break;
        case EDelete:
            resID = R_CALEN_COMMONUI_QTN_CALE_LQ_DEL_REPEATED_NOTE;
            break;
        case EEdit:
            resID = R_CALEN_COMMONUI_QTN_CALE_LQ_EDIT_RECURRING;
            break;
        default:
            User::Leave( KErrArgument );
            break;
        }

    HBufC* title = StringLoader::LoadLC( resID, eikEnv );
    popupList->SetTitleL( *title );
    CleanupStack::PopAndDestroy( title );

    TBool ret( ETrue );

    if( popupList->ExecuteLD() )
        {
        const TInt selected( list->CurrentItemIndex() );

        switch( selected )
            {
            case ECalenRepeatThis:
                aAnswer = CalCommon::EThisOnly;
                break;
            case ECalenRepeatThisFuture:
                aAnswer = CalCommon::EThisAndFuture;
                break;
            case ECalenRepeatThisPast:
                aAnswer = CalCommon::EThisAndPrior;
                break;
            case ECalenRepeatAll:
                aAnswer = CalCommon::EThisAndAll;
                break;
            default:
                ASSERT( EFalse );
                break;
            }
        }
    else
        {
        ret = EFalse;
        }

    CleanupStack::Pop( popupList );
    CleanupStack::PopAndDestroy( list );

    TRACE_EXIT_POINT;
    return ret;
    }

// ----------------------------------------------------------------------------
// CalenActionUiUtils::DateQueryL
// Prompts the user for a time. Requires a title resource id, or pass 0
// for default title "Date:".
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
EXPORT_C TInt CalenActionUiUtils::DateQueryL( TTime& aDate, TInt aPromptId )
    {
    TRACE_ENTRY_POINT;

    if ( aPromptId == 0 )
        {
        aPromptId = R_CALEN_DATE_PROMPT;
        }

    CAknTimeQueryDialog* dlg = CAknTimeQueryDialog::NewL( aDate,
                                                          CAknQueryDialog::ENoTone );
    CleanupStack::PushL( dlg );

    HBufC* prompt = StringLoader::LoadLC( aPromptId, CEikonEnv::Static() );
    dlg->SetPromptL( *prompt );
    CleanupStack::PopAndDestroy( prompt );

    CleanupStack::Pop(); // dlg

    TRACE_EXIT_POINT;
    return dlg->ExecuteLD( R_CALEN_DAYQUERY_NOTE );
    }

// ----------------------------------------------------------------------------
// CalenActionUiUtils::SetToDoCompleteStatusL
// Mark to-do entry as completed or restore a completed to-do and save.
// This is not in the engine layer because it shows an error mesage on failure.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
EXPORT_C void CalenActionUiUtils::SetToDoCompleteStatusL( 
        CCalEntryView& aEntryView, 
        CCalEntry* aEntry, 
        const TBool aStatus )
    {
    TRACE_ENTRY_POINT;

    ASSERT( aEntry );
    TTime now;
    now.HomeTime();
    TCalTime caltime;
    caltime.SetTimeLocalL( now );

    // set as completed or restore
    aEntry->SetCompletedL( aStatus, caltime );

    // Save the to-do
    TInt error( 0 );
    CEikonEnv* eikenv = CEikonEnv::Static();

    // Check disk space of C drive.
    const TInt writesize( 512 ); // Write size

    if( SysUtil::FFSSpaceBelowCriticalLevelL( &( eikenv->FsSession() ), writesize ) )
        {
        CErrorUI* errorui = CErrorUI::NewLC();
        errorui->ShowGlobalErrorNoteL( KErrDiskFull );
        CleanupStack::PopAndDestroy();
        TRACE_EXIT_POINT;
        return;
        }

    RPointerArray<CCalEntry> list;
    CleanupClosePushL( list );

    TInt entrynum;

    // update entry
    list.Append( aEntry );
    TRAP( error, aEntryView.UpdateL( list, entrynum ) );

    if( error == KErrNotFound )
        {
        // entry was deleted...?
        list.Reset();
        list.Append( aEntry );
        TRAP( error, aEntryView.StoreL( list, entrynum ) );
        }
    CleanupStack::PopAndDestroy( &list );
    User::LeaveIfError( error );

    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenCommonUI::PrepareLbDataForNewNotesQueryL
// Prepares ListBox items for NewEntry query
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CalenActionUiUtils::PrepareLbDataForNewNotesQueryL( 
        CDesCArrayFlat*& aListItems, 
        CArrayFixFlat<TInt>*& aListItemCommands )
    {
    TRACE_ENTRY_POINT;

    if( ( aListItems == NULL ) && ( aListItemCommands == NULL) )
        {
        // List items for list query.
        aListItems = new( ELeave ) CDesCArrayFlat( KArrayGranularity );

        // This array is used to map user selection to correct service.
        aListItemCommands = new( ELeave ) CArrayFixFlat<TInt>( KArrayGranularity );

        // Add the item strings to listItems
        // Add the item commands to listItemCommands
        // Consider also meeting request item
        HBufC* temp = NULL;

        temp = CCoeEnv::Static()->AllocReadResourceLC( R_CALEN_NEW_MEETING_NOTE );
        aListItems->AppendL( *temp );
        CleanupStack::PopAndDestroy( temp );
        temp = NULL;
        aListItemCommands->AppendL( ECalenNewMeeting );
        
        // Check with interim utils if the Meeting request viewers are enabled.
        CCalenInterimUtils2* interimUtils = CCalenInterimUtils2::NewL();
        CleanupStack::PushL( interimUtils );
        if( interimUtils->MRViewersEnabledL() )
            {
            temp = CCoeEnv::Static()->AllocReadResourceLC( R_CALEN_NEW_MEETING_REQUEST_NOTE );
            aListItems->AppendL( *temp );
            CleanupStack::PopAndDestroy( temp );
            temp = NULL;
            aListItemCommands->AppendL( ECalenNewMeetingRequest );
            }
        CleanupStack::PopAndDestroy( interimUtils );
        
        temp = CCoeEnv::Static()->AllocReadResourceLC( R_CALEN_DAY_NOTE );
        aListItems->AppendL(*temp );
        CleanupStack::PopAndDestroy( temp );
        temp = NULL;
        aListItemCommands->AppendL( ECalenNewDayNote );

        temp = CCoeEnv::Static()->AllocReadResourceLC( R_CALEN_NOTE_ANNIVERSARY );
        aListItems->AppendL(*temp);
        CleanupStack::PopAndDestroy( temp );
        temp = NULL;
        aListItemCommands->AppendL( ECalenNewAnniv );

        temp = CCoeEnv::Static()->AllocReadResourceLC( R_CALEN_NOTE_TODO );
        aListItems->AppendL(*temp );
        CleanupStack::PopAndDestroy( temp );
        temp = NULL;
        aListItemCommands->AppendL( ECalenNewTodo );
        }

    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenCommonUI::ShowNewNoteQueryL
// Shows the list query for NewEntry
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CalenActionUiUtils::ShowNewNoteQueryL( 
        CDesCArrayFlat*& aListItems, 
        CArrayFixFlat<TInt>*& aListItemCommands )
    {
    TRACE_ENTRY_POINT;

    PrepareLbDataForNewNotesQueryL( aListItems, aListItemCommands );

    // Show list query.
    TInt choice = KErrCancel; //KErrNotFound;
    CAknListQueryDialog* dlg = new( ELeave ) CAknListQueryDialog( &choice );
    dlg->PrepareLC( R_CALENDAR_NEW_NOTE_QUERY ); // pushes dlg to CS
    dlg->SetItemTextArray( aListItems );
    dlg->SetOwnershipType( ELbmDoesNotOwnItemArray );

    // Set title
    CAknPopupHeadingPane* heading = dlg->Heading();
    HBufC* title = NULL;
    title = CCoeEnv::Static()->AllocReadResourceLC( 
                                            R_CALENDAR_NEW_NOTE_QUERY_TITLE );
    heading->SetTextL( *title );
    CleanupStack::PopAndDestroy( title );

    if( dlg->RunLD() )
        {
        // user made a choice
        choice = aListItemCommands->At( choice );
        }

    TRACE_EXIT_POINT;
    return choice;
    }

// -----------------------------------------------------------------------------
// CCalenCommonUI::FindPossibleInstanceL
// Finds an instance with the given instance id and instance view.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C CCalInstance* CalenActionUiUtils::FindPossibleInstanceL( 
        const TCalenInstanceId& aId,
        CCalInstanceView& aInstanceView )
    {
    TRACE_ENTRY_POINT;

    RPointerArray<CCalInstance> instances;
    CleanupResetAndDestroyPushL( instances );

    TCalTime moment;
    moment.SetTimeLocalL( aId.iInstanceTime );
    CalCommon::TCalTimeRange range( moment, moment );
    aInstanceView.FindInstanceL( instances, CalCommon::EIncludeAll, range );

    CCalInstance* result = NULL;

    // For instances finishing the next day (now possible with unified DateTime editor),
    // we have to do our best to match the instance time exactly - otherwise we could
    // match the LocalUid to the incorrect instance in a series.
    for ( TInt i=0; i < instances.Count() && !result; ++i )
        {
        if( (instances[i]->InstanceIdL().iCollectionId == aId.iColId) && (instances[i]->Entry().LocalUidL() == aId.iEntryLocalUid )) // comparing the collection id and Local Uid 
            {
            // Check the instance time matches.
            if( instances[i]->StartTimeL().TimeLocalL() == aId.iInstanceTime )
                {
                result = instances[i];
                instances.Remove(i);
                }
            }
        }

    if( !result )
        {
        // Couldn't match the instance time exactly - just use the instance
        // with the same LocalUid as the one we're looking for.
        for ( TInt i=0; i < instances.Count() && !result; ++i )
            {
            if( instances[i]->Entry().LocalUidL() == aId.iEntryLocalUid )
                {
                result = instances[i];
                instances.Remove(i);
                }
            }
        }

    CleanupStack::PopAndDestroy( &instances );

    TRACE_EXIT_POINT;
    return result;
    }

// End of file
