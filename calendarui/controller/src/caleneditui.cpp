/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Controller class for editing related UI behavior
 *
*/


#include <eikenv.h>
#include <e32math.h>
#include <StringLoader.h>
#include <utf.h>
#include <aknnotewrappers.h>
#include <avkon.hrh>
#include <CAgnEntryUi.h>
#include <calentryview.h>
#include <calentry.h>
#include <calrrule.h>
#include <caluser.h>
#include <calenagendautils.h>
#include <calendateutils.h>
#include <CalenInterimUtils2.h>
#include <CalenEditorsPlugin.h>
#include <calenglobaldata.h>
#include <Calendar.rsg>
#include <calencommonui.rsg>
#include <calencommands.hrh>            // Calendar commands
#include <calencontext.h>
#include <caleninstanceid.h>            // TCalenInstanceId
#include <calentoolbar.h>
#include <calenactionuiutils.h>
#include <aknViewAppUi.h>           // CAknViewAppUi
#include <aknview.h>
#include <featmgr.h>

#include "calendarui_debug.h"
#include "caleneditui.h"
#include "calencontroller.h"
#include "calendar.hrh"
#include "calensend.h"
#include "CalenUid.h"
#include "calenservicesimpl.h"
#include "calenviewmanager.h"

// ----------------------------------------------------------------------------
// CCalenEditUi::NewL
// First stage construction
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
CCalenEditUi* CCalenEditUi::NewL( CCalenController& aController )
    {
    TRACE_ENTRY_POINT;

    CCalenEditUi* self = new(ELeave) CCalenEditUi( aController );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    TRACE_EXIT_POINT;
    return self;
    }

// ----------------------------------------------------------------------------
// CCalenEditUi::~CCalenEditUi
// Destructor
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
CCalenEditUi::~CCalenEditUi()
    {
    TRACE_ENTRY_POINT;

    delete iEditorsPlugin;
    delete iListItems;
    delete iListItemCommands;
    
    if( iWaitDialog )
        {
        delete iWaitDialog;
        }

    if( iGlobalData )
        {
        iGlobalData->Release();
        }

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenEditUi::CCalenEditUi
// Constructor
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
CCalenEditUi::CCalenEditUi( CCalenController& aController )
    : iController( aController )
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenEditUi::ConstructL
// Second stage construction
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenEditUi::ConstructL()
    {
    TRACE_ENTRY_POINT;

    iGlobalData = CCalenGlobalData::InstanceL();
    iEntriesToComplete = 1;
    iMoreEntriesToComplete = EFalse;
    isEditorActive = EFalse;
    
    iController.RegisterForNotificationsL( this, ECalenNotifyCancelStatusUpdation );

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenEditUi::HandleActionUiCommandL
// Handles action ui commands
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
TBool CCalenEditUi::HandleCommandL( const TCalenCommand& aCommand )
    {
    TRACE_ENTRY_POINT;

    TBool continueCommand(EFalse);
    TInt command = aCommand.Command();

    switch(command)
        {
        case ECalenNewMeeting:
        case ECalenNewDayNote:
        
            {
            TUid currentView = iController.ViewManager().CurrentView();
            if( KUidCalenTodoView == currentView )
                {
                // Open NewEntry as "To-Do", if editor is launched from To-Do view.
                MCalenContext& context = iController.Services().Context();
                // Set the date on the context to today.
                TTime homeTime;
                homeTime.HomeTime();
                TCalTime today;
                today.SetTimeLocalL( homeTime );
                context.SetFocusDateL( today, TVwsViewId( KUidCalendar, KUidCalenTodoView ) );
                EditNewEntryL( CCalEntry::ETodo );
                }
            else
                {
                EditNewEntryL();
                }
            }
            break;
        case ECalenNewTodo:
            EditNewEntryL( CCalEntry::ETodo );
            break;
        case ECalenNewAnniv:
            EditNewEntryL( CCalEntry::EAnniv );
            break;
        case ECalenNewMeetingRequest:
            {
            EditNewMeetingRequestL();
            }
            break;
        case ECalenEditCurrentEntry:
        case ECalenEditSeries:
        case ECalenEditOccurrence:
        case ECalenCompleteTodo:
        case ECalenRestoreTodo:
        case ECalenMarkEntryAsDone:
        case ECalenMarkEntryAsUnDone:
            {
            // These commands need the entry view
            // to be constructed before being called
            CCalEntryView* entryView = iGlobalData->EntryViewL();
            if( !entryView )
                {
                // Store the command and wait for notification
                WaitForEntryViewNotificationL( aCommand );
                }
            else
                {
                switch ( command )
                    {
                    case ECalenEditCurrentEntry:
                    	{
                        MCalenContext& context = iGlobalData->Context();
                        entryView = iGlobalData->EntryViewL(context.InstanceId().iColId);
                        CCalEntry* entry = entryView->FetchL(
                                                context.InstanceId().iEntryLocalUid );
                        CleanupStack::PushL( entry );
                        User::LeaveIfNull( entry );
                        EditEntryL( entry,
                                    context.InstanceId().iInstanceTime,
                                    MAgnEntryUi::EEditExistingEntry );

                        CleanupStack::PopAndDestroy( entry );
                     
                    	break;
                    	}
                    case ECalenCompleteTodo:
                        SetTodoCompleteL( ETrue );
                        break;
                    case ECalenRestoreTodo:
                        SetTodoCompleteL( EFalse );
                        break;
                    case ECalenMarkEntryAsDone:
                        continueCommand = SetMultipleTodoCompleteL(ETrue );
                        break;
                     case ECalenMarkEntryAsUnDone:
                         continueCommand = SetMultipleTodoCompleteL(EFalse );
                        break;
                    default:
                         {
                         // Should never reach this point.
                         ASSERT( EFalse );   
                         }
                    }
                }
            }
            break;
        case ECalenSend:
            {
            iGlobalData->CalenSendL().DisplaySendCascadeMenuL();

            if( iGlobalData->CalenSendL().CanSendL( ECalenSend ) )
                {
                SendEntryL();
                iController.BroadcastNotification( ECalenNotifyEntrySent );
                }
            // ECalenNotifyDialogClosed issued when Messaging Query is cancelled
            // by the user.
            else
                {
                iController.BroadcastNotification( ECalenNotifyEntrySendCancel );
                }
                
            }
            break;
        default:
            // Controller decided this class was the place to handle this
            // command
            break;
        }

    TRACE_EXIT_POINT;
    return continueCommand;
    }

// ----------------------------------------------------------------------------
// CCalenEditUi::CalenCommandHandlerExtensionL
// Dummy implementation.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
TAny* CCalenEditUi::CalenCommandHandlerExtensionL( TUid /*aExtensionUid*/ )
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    return NULL;
    }

// ----------------------------------------------------------------------------
// CCalenEditUi::SetTodoCompleteL
// Completes or restores a todo.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenEditUi::SetTodoCompleteL( TBool aComplete )
    {
    TRACE_ENTRY_POINT;

    MCalenContext& context = iGlobalData->Context();
    ASSERT( !( context.InstanceId() == ( TCalenInstanceId::NullInstanceId() ) ) );

    // HandleActionUiCommandL creates the entry view if it has
    // not already been created before SetTodoCompleteL is called.
    CCalEntryView* entryView = iGlobalData->EntryViewL(context.InstanceId().iColId);
    ASSERT( entryView );

    CCalEntry* entry = entryView->FetchL( context.InstanceId().iEntryLocalUid );

    CleanupStack::PushL( entry );

    if( entry )
        {
        CalenActionUiUtils::SetToDoCompleteStatusL( *entryView, entry, aComplete );
        //iContainer->NotifyEntryChangeL( *entry, entry->StartTimeL() );
        iController.BroadcastNotification( ECalenNotifyEntrySaved );
        }

    CleanupStack::PopAndDestroy( entry );

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenEditUi::SetTodoEntryCompleteL
// Completes or restores a todo.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenEditUi::SetTodoEntryCompleteL(TBool aComplete,TCalLocalUid aEntryUid,TCalCollectionId colId)
    {
    TRACE_ENTRY_POINT;

    // HandleActionUiCommandL creates the entry view if it has
    // not already been created before SetTodoCompleteL is called.
   CCalEntryView* entryView = iGlobalData->EntryViewL(colId);
    ASSERT( entryView );

    CCalEntry* entry = entryView->FetchL( aEntryUid );
    CleanupStack::PushL( entry );

    if( entry )
        {
        CalenActionUiUtils::SetToDoCompleteStatusL( *entryView, entry, aComplete );
        }

    CleanupStack::PopAndDestroy( entry );    
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenEditUi::EditNewEntryL
// Creates a new entry and opens it for editing
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenEditUi::EditNewEntryL( CCalEntry::TType aEntryType)
    {
    TRACE_ENTRY_POINT;

    // Create unique ID.
    HBufC8* guid = iGlobalData->InterimUtilsL().GlobalUidL();
    CleanupStack::PushL( guid );
    CCalEntry* entry = CCalEntry::NewL( aEntryType, guid, CCalEntry::EMethodNone, 0 );
    CleanupStack::Pop( guid );

    CleanupStack::PushL( entry );

    // Set initial start and end times from Calendar context
    TTime focusedTime = CalculateActiveTimeFromContextL();
    TCalTime initialCalTime;

    initialCalTime.SetTimeLocalL( focusedTime );

    entry->SetStartAndEndTimeL( initialCalTime, initialCalTime );
    
    if( entry->EntryTypeL()==CCalEntry::EAnniv )
        {
        TCalRRule rrule( TCalRRule::EYearly );
        TCalTime startDate;
        rrule.SetDtStart( initialCalTime );
        rrule.SetInterval( 1 ); // once a year
        entry->SetRRuleL( rrule );
        }
    
    MCalenContext& context = iGlobalData->Context();
    TCalCollectionId colId = context.InstanceId().iColId;
    TCalenInstanceId id = TCalenInstanceId::CreateL( *entry, initialCalTime );
    id.iColId = colId;
    
    context.SetInstanceIdL( id, context.ViewId() ); 
    
    TTime instanceTime = entry->StartTimeL().TimeLocalL();
    EditEntryL( entry, instanceTime, MAgnEntryUi::ECreateNewEntry );
    CleanupStack::PopAndDestroy( entry );

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenEditUi::EditNewMeetingRequestL
// Creates a new meeting request and opens it for editing
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenEditUi::EditNewMeetingRequestL()
    {
    TRACE_ENTRY_POINT;

    CMRMailboxUtils::TMailboxInfo defaultMailBox;

    if( !iGlobalData->AttemptToRetrieveDefaultMailboxL( defaultMailBox ) )
        {
        // User either doesn't have any mailboxes or didn't select a default mailbox.
        // Don't create the meeting request.
        TRACE_EXIT_POINT;
        return;
        }

    // If we got this far, defaultMailBox should be initialised.
    HBufC8* guid = iGlobalData->InterimUtilsL().GlobalUidL();

    CleanupStack::PushL( guid );
    CCalEntry* entry = CCalEntry::NewL( CCalEntry::EAppt, 
                                                       guid,
                                                       CCalEntry::EMethodRequest,
                                                       0 );
    CleanupStack::Pop( guid );
    CleanupStack::PushL( entry );

    // Set initial start and end times from Active Calendar Context
    TTime focusedTime = iGlobalData->Context().FocusDateAndTimeL().TimeLocalL();
    TCalTime initialCalTime;
    // No need to handle floating time check here, as meetings are
    // always fixed.
    initialCalTime.SetTimeLocalL( focusedTime );

    entry->SetStartAndEndTimeL( initialCalTime, initialCalTime );

    //Set the organizer from the selected mailbox
    CCalUser* organizer = CCalUser::NewL( defaultMailBox.iEmailAddress );
    entry->SetOrganizerL( organizer );

    TTime instanceTime = entry->StartTimeL().TimeLocalL();
    EditEntryL( entry, instanceTime, MAgnEntryUi::ECreateNewEntry );
    CleanupStack::PopAndDestroy( entry );

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenEditUi::EditEntryL
// Opens the entry aEntry for editing
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenEditUi::EditEntryL( CCalEntry* aEntry, 
                                         const TTime& aInstanceDate,
                                         MAgnEntryUi::TAgnEntryUiEditorMode aEditorMode )
    {
    TRACE_ENTRY_POINT;
    
    // Set the flag
    isEditorActive = ETrue;
    
    if( !iEditorsPlugin )
        {
        //Only created on first use
        LoadEditorsPluginL();
        if( !iEditorsPlugin )
            {
            // User was prompted to set a default mailbox and hit cancel
            // Entry will not be opened
            TRACE_EXIT_POINT;
            return;
            }
        }

    MAgnEntryUi::TAgnEntryUiInParams inParams( KUidCalendar,
                                               iGlobalData->CalSessionL(),
                                               aEditorMode );

    // Use floating time for non-timed entries 
    if( !CalenAgendaUtils::IsTimedEntryL(aEntry->EntryTypeL()) )
        {
        inParams.iInstanceDate.SetTimeLocalFloatingL( aInstanceDate );
        }
    else
        {
        inParams.iInstanceDate.SetTimeLocalL( aInstanceDate );
        }
    
    inParams.iMsgSession = NULL;

    MAgnEntryUi::TAgnEntryUiOutParams outParams;

    // Launch Entry UI
    RPointerArray<CCalEntry> entries;
    CleanupClosePushL( entries );
    entries.AppendL( aEntry );  // aEntry already in cleanup stack -> safe

    // Editors need a services implementation.
    MCalenServices* svc = CCalenServicesImpl::NewL();
    CleanupReleasePushL( *svc );
    inParams.iSpare = ( TInt )svc;
	
	
    // Stop the population of the current view
    iController.ViewManager().InterruptPopulationL();
    
    // hide the toolbar
    MCalenToolbar* toolbar = iController.Services().ToolbarOrNull();
    if(toolbar)
        {
        toolbar->SetToolbarVisibilityL(EFalse);  
        }
	
    
    // If a menubar is active,stop displaying it before launching editor
    TUid viewUid = iController.ViewManager().CurrentView();
    CEikMenuBar* menuBar = iController.AppUi().View(viewUid)->MenuBar();
    if(menuBar)
        {
        menuBar->StopDisplayingMenuBar();
        }
 
    // Launch the viewer / editor plugin
    iEditorsPlugin->ExecuteViewL( entries, inParams, outParams, *this );
    
    // Reset the flag
    isEditorActive = EFalse;
    
    // Unhide the toolbar when settings is closed
    TUid activeViewUid = iController.ViewManager().CurrentView();
    if(toolbar && (activeViewUid != KUidCalenMissedAlarmsView ) &&(activeViewUid != KUidCalenMissedEventView ) )
        {
        toolbar->SetToolbarVisibilityL(ETrue); 
        }
	
    CleanupStack::PopAndDestroy( svc );

    // Close the entries array
    CleanupStack::PopAndDestroy( &entries );

    switch( outParams.iAction )
        {
        case MAgnEntryUi::EMeetingSaved:
        case MAgnEntryUi::EInstanceRescheduled:
            {
            // Update context and issue notification before confirmation dialog
            // to avoid delay of updating title pane
            MCalenContext& context = iGlobalData->Context();
            TCalCollectionId colId = context.InstanceId().iColId;
            TCalenInstanceId id = TCalenInstanceId::CreateL( TCalLocalUid(
                    outParams.iSpare),
                    outParams.iNewInstanceDate.TimeLocalL(), EFalse );
            id.iColId = colId;
            id.iType = context.InstanceId().iType;
            if( aEditorMode != MAgnEntryUi::ECreateNewEntry )
                {
                id.iInstanceTime = context.InstanceId().iInstanceTime;
                }
            context.SetInstanceIdL( id, context.ViewId() );
            iController.BroadcastNotification( ECalenNotifyEntrySaved );

            if( aEditorMode == MAgnEntryUi::ECreateNewEntry )
                {
                // Show saved note for new entries
                TInt res;
                if( context.InstanceId().iType == CCalEntry::ETodo )
                    {
                    res = R_QTN_CALE_NOTE_TODO_SAVED;
                    }
                else
                    {
                    res = R_TEXT_CALENDAR_NOTE_SAVED;
                    }

                HBufC* prompt = StringLoader::LoadLC( res, CEikonEnv::Static() );
                CAknConfirmationNote* dialog = new( ELeave )CAknConfirmationNote();
                dialog->ExecuteLD( *prompt );
                CleanupStack::PopAndDestroy( prompt );
                }
            }
            break;

        case MAgnEntryUi::EMeetingDeleted:
            iController.BroadcastNotification( ECalenNotifyEntryDeleted );
            break;

        case MAgnEntryUi::EInstanceDeleted:
            iController.BroadcastNotification( ECalenNotifyInstanceDeleted );
            break;

        case MAgnEntryUi::ENoAction:
        default:
            iController.BroadcastNotification( ECalenNotifyEntrySaved );
            break;
        }


    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenEditUi::LoadEditorsPluginL
// Attempts to load the editors plugin into iEditorsPlugin. If failure occurs
// (for example, if the user is prompted to select a default mailbox and hits
// cancel), iEditorsPlugin is set to NULL.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenEditUi::LoadEditorsPluginL()
    {
    TRACE_ENTRY_POINT;

    delete iEditorsPlugin;
    iEditorsPlugin = NULL;

    if( iGlobalData->InterimUtilsL().MRViewersEnabledL() )
        {
        //Get the currently selected mailbox
        CMRMailboxUtils::TMailboxInfo defaultMailBox;

         if( iGlobalData->AttemptToRetrieveDefaultMailboxL( defaultMailBox ) )
             {
             //CAgnEntryUi resolves Meeting request viewer based on mtm uid
             TBuf8<KMaxUidName> mrMtm;
             CnvUtfConverter::ConvertFromUnicodeToUtf8( mrMtm, 
                                                                            defaultMailBox.iMtmUid.Name() );
             iEditorsPlugin = CAgnEntryUi::NewL( mrMtm );
             }
        }
    else
        {
        iEditorsPlugin = CCalenEditorsPlugin::NewL();
        }

    TRACE_EXIT_POINT;
    }


// ----------------------------------------------------------------------------
// CCalenEditUi::SendEntryL
// Sends the current entry.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenEditUi::SendEntryL()
    {
    TRACE_ENTRY_POINT;

    MCalenContext& context = iGlobalData->Context();
    ASSERT( !( context.InstanceId() == TCalenInstanceId::NullInstanceId() ) );

    //If todo, use the LUid.
    //Todos returns the current time if start or end time has not been saved.
    if( CCalEntry::ETodo == context.InstanceId().iType )
        {
        CCalEntry* entry = iGlobalData->EntryViewL(context.InstanceId().iColId)->FetchL(
                                                      context.InstanceId().iEntryLocalUid );
        if( entry )
            {
            CleanupStack::PushL( entry );
            iGlobalData->CalenSendL().SendAsVCalendarL( ECalenSend, *entry );
            CleanupStack::PopAndDestroy( entry );
            }
        }
    else // Not todo
        {
        RArray<TInt> colIdArray;
        CleanupClosePushL(colIdArray);
        iController.GetActiveCollectionidsL(colIdArray);
        
        CCalInstance* instance = CalenActionUiUtils::FindPossibleInstanceL( 
                                                                context.InstanceId(),
                                                               *iGlobalData->InstanceViewL(colIdArray) );
        CleanupStack::PushL( instance );

        if( instance )
            {
            CCalEntry& entry = instance->Entry();
            iGlobalData->CalenSendL().SendAsVCalendarL( ECalenSend, entry );
            }

        CleanupStack::PopAndDestroy( instance );
        CleanupStack::PopAndDestroy(&colIdArray);
        }

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenEditUi::ProcessCommandWithResultL
// This is pure virtual from MAgnEntryUiCallback. We don't use it.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
TInt CCalenEditUi::ProcessCommandWithResultL( TInt /*aCommandId*/ )
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    return KErrNone;
    }

// ----------------------------------------------------------------------------
// CCalenEditUi::ProcessCommandL
// This is pure virtual from MAgnEntryUiCallback. We don't use it.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenEditUi::ProcessCommandL( TInt /*aCommandId*/ )
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenEditUi::WaitForEntryViewNotificationL
// Register for view created notification
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenEditUi::WaitForEntryViewNotificationL( const TCalenCommand& aCommand )
    {
    TRACE_ENTRY_POINT;

    iStoredCommand = aCommand;

    // Only wait for an entry view if it doesn't already exist
    ASSERT( !( iGlobalData->EntryViewL() ) );

    // Register for notifications
    iController.RegisterForNotificationsL( this, ECalenNotifyEntryInstanceViewCreated );

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenEditUi::HandleECalenNotifyViewCreatedL
// Handle ECalenNotifyViewCreated
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenEditUi::HandleECalenNotifyViewCreatedL()
    {
    TRACE_ENTRY_POINT;

    if( iGlobalData->EntryViewL() )
        {
        // Handle the outstanding command
        HandleCommandL( iStoredCommand );

        // Cancel the notify as the entry view is now
        // constructed.
        iController.CancelNotifications( this );
        }

    TRACE_EXIT_POINT;
    }
// ----------------------------------------------------------------------------
// CCalenEditUi::HandleNotification
// Handle notifications
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenEditUi::HandleNotification(const TCalenNotification aNotification )
    {
    TRACE_ENTRY_POINT;

    if ( aNotification == ECalenNotifyEntryInstanceViewCreated )
        {
        PIM_TRAPD_HANDLE( HandleECalenNotifyViewCreatedL() );
        }
    if( aNotification == ECalenNotifyCancelStatusUpdation)
        {
        if(iMutlipleContextIdsCount)
            {
            // get the context
            MCalenContext& context = iGlobalData->Context();
            // reset the multiple contexts
            context.ResetMultipleContextIds();
            
            // dismiss the waitdialog
            if(iWaitDialog)
                {
                TRAP_IGNORE(iWaitDialog->ProcessFinishedL());
                }
            }
        }
    TRACE_EXIT_POINT;
    }

TBool CCalenEditUi::SetMultipleTodoCompleteL(TBool aComplete )
    {
    TRACE_ENTRY_POINT;
   TBool continueCommand(EFalse);
   
   // get the context
   MCalenContext& context = iGlobalData->Context();
   
   // get the multliple context ids count
   iMutlipleContextIdsCount = context.MutlipleContextIdsCount();
   
   ASSERT( iMutlipleContextIdsCount );

   if(!iMoreEntriesToComplete)   
       {
       DisplayWaitDialogL();
       }
   // get the multiple context instance ids
   RArray<TCalenInstanceId>& multipleContextIds = context.GetMutlipleContextIds();
   
   if(iMutlipleContextIdsCount <= iEntriesToComplete )
       {
       iMoreEntriesToComplete = EFalse; 
       iEntriesToComplete = iMutlipleContextIdsCount;
       }
   else
       {
       iMoreEntriesToComplete = ETrue;
       // set the continue command flag if more entries are there to delete
       continueCommand = ETrue;
       }
       
   TInt index(0);
   while(index<iEntriesToComplete)
       {
       // get the local uid of the entry through multiple context list
       TCalLocalUid entryLocalUid = multipleContextIds[0].iEntryLocalUid;
       TCalCollectionId colId = multipleContextIds[0].iColId;
       if(entryLocalUid)
           {
            SetTodoEntryCompleteL(aComplete, entryLocalUid,colId);
           }
       // remove mutliple context based on the instanceid
       context.RemoveMultipleContextId(multipleContextIds[0]);
       index++;
      }

   if(!iMoreEntriesToComplete)
       {
        MarkedEntriesCompletedL();
       }
 
   
   
   TRACE_EXIT_POINT;
   return continueCommand;    
   }
// -----------------------------------------------------------------------------
// CCalenEditUi::DisplayWaitDialogL
// Display wait dialog
// -----------------------------------------------------------------------------
//
void CCalenEditUi::DisplayWaitDialogL()
    {
    TRACE_ENTRY_POINT;
    
    delete iWaitDialog;
    iWaitDialog = NULL;
    iWaitDialog = new( ELeave )CAknWaitDialog( REINTERPRET_CAST( CEikDialog**, &iWaitDialog ), ETrue );
    iWaitDialog->ExecuteLD( R_TODO_UPDATING_STATUS_NOTE );
    iWaitDialog->SetCallback(this);
    
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenEditUi::MarkedEntriesDeletedL
// Dismiss wait dialog and show information note
// -----------------------------------------------------------------------------
//
void CCalenEditUi::MarkedEntriesCompletedL()
    {
    TRACE_ENTRY_POINT;
    
    // dismiss the waitdialog
    if(iWaitDialog)
        {
        iWaitDialog->ProcessFinishedL();
        }
    
    if(iMutlipleContextIdsCount)
        {
        // get the context
        MCalenContext& context = iGlobalData->Context();
        // reset the multiple contexts
        context.ResetMultipleContextIds();
        }
    // notify marked entries deleted
    iController.BroadcastNotification( ECalenNotifyMarkedEntryCompleted );
    
    TRACE_EXIT_POINT;
    }

void CCalenEditUi::DialogDismissedL( const TInt /*aButtonId*/ )
   {
   TRACE_ENTRY_POINT;
  
   // no more entries to delete
   iMoreEntriesToComplete = EFalse;      
  
   // issue notification cancel delete
  // iController.BroadcastNotification(ECalenNotifyCancelStatusUpdation);   
  
   TRACE_EXIT_POINT;
   }

// -----------------------------------------------------------------------------
// CCalenEditUi::CalculateActiveTimeFromContextL
// CCalculate active time from context
// -----------------------------------------------------------------------------
//
TTime CCalenEditUi::CalculateActiveTimeFromContextL()
    {
    TRACE_ENTRY_POINT;
    // The active time is calculated based on the following table   
    //                          Current View
    //            Month        Day         Week        ToDo
    
    // Meeting*    CFD (@8am)  CFD (@8am)  CFD @ FCT   Today (@8am)
    // Day Note    CFD         CFD         CFD         Today
    // Anniv*      CFD         CFD         CFD         Today
    // ToDo*       CFD         CFD         CFD         Today
    
    // CFD = Currently Focused Date, the one shown in Navigator toolbar
    // FCT = Focused Cell's Time, applicable only to Week View
    // (*) Alarm Default times are also set using the values
        
    TTime activeTime(Time::NullTTime()); // initialize with NULL time
    TTime& activeTimeRef = activeTime;
    MCalenContext &context = iGlobalData->Context();
    TUid currentView = iController.ViewManager().CurrentView();
    if((currentView == KUidCalenWeekView ) && 
            (FeatureManager::FeatureSupported(KFeatureIdFfCalScrollablePreview  )))
        {
        // use CFD
         activeTime = CalenDateUtils::BeginningOfDay( 
                 context.FocusDateAndTimeL().TimeLocalL()); // CFD
         
         // CFD @ FCT
         // focus on empty nontimed cell
         if( context.FocusDateAndTimeL().TimeUtcL() == Time::NullTTime() )
             {
             activeTime = CalenDateUtils::DefaultTime( activeTimeRef );
             }
         else if(context.InstanceId().iType != CCalEntry::EAppt) 
             {
             activeTime = CalenDateUtils::DefaultTime( activeTimeRef );
             }
         else
             {
             activeTime += CalenDateUtils::TimeOfDay( 
                     context.FocusDateAndTimeL().TimeLocalL().DateTime() );
             }
         }
    else
        {
        // use Today @ 8 am
        activeTime = CalenDateUtils::Today();
        activeTime = CalenDateUtils::DefaultTime(context.FocusDateAndTimeL().TimeLocalL()); // 8 am
        }
    
    TRACE_EXIT_POINT;
    return activeTime;
    }

// -----------------------------------------------------------------------------
// CCalenEditUi::IsEditorActive
// Tells framework whether editor is active or not
// -----------------------------------------------------------------------------
//
TBool CCalenEditUi::IsEditorActive()
    {
    return isEditorActive;
    }
// End of file
