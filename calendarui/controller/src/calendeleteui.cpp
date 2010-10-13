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
* Description:  Handles deletion
*
*/


// INCLUDES
#include <Calendar.rsg>
#include <calendateutils.h>
#include <calenagendautils.h>
#include <aknnotewrappers.h>
#include <AknWaitDialog.h>
#include <calcommon.h>
#include <calentryview.h>
#include <calinstance.h>
#include <calinstanceview.h>
#include <caltime.h>
#include <centralrepository.h>
#include <eikenv.h>
#include <StringLoader.h>
#include <sysutil.h>
#include <calenglobaldata.h>
#include <calrrule.h>
#include <calsession.h>
#include <calencommands.hrh>            // Calendar commands
#include <calencontext.h>
#include <caleninstanceid.h>            // TCalenInstanceId
#include <calenactionuiutils.h>
#include <calcalendarinfo.h>
#include <calentoolbar.h>
#include <akntoolbar.h>
#include <CalenAttachmentModel.h>

#include "calendarui_debug.h"           // Debug
#include "calendeleteui.h"
#include "calencontroller.h"
#include "CleanupResetAndDestroy.h"
#include "CalenInterimUtils2.h"
#include "CalendarPrivateCRKeys.h"      // For CalendarInternalCRKeys.h
#include "calenmultipledbmanager.h"

// Local constants
const TInt KEntriesToDelete = 1;

// ----------------------------------------------------------------------------
// CCalenDeleteUi::NewL
// Two phased constructor
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
CCalenDeleteUi* CCalenDeleteUi::NewL( CCalenController& aController )
    {
    TRACE_ENTRY_POINT;

    CCalenDeleteUi* self = new( ELeave ) CCalenDeleteUi( aController );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    TRACE_EXIT_POINT;
    return self;
    }

// ----------------------------------------------------------------------------
// CCalenDeleteUi::CCalenDeleteUi
// ?implementation_description
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
CCalenDeleteUi::CCalenDeleteUi( CCalenController& aController )
    : iEikEnv( CEikonEnv::Static() ), iController( aController )
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenDeleteUi::~CCalenDeleteUi
// Destructor
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
CCalenDeleteUi::~CCalenDeleteUi()
    {
    TRACE_ENTRY_POINT;

    if( iWaitDialog )
        {
        delete iWaitDialog;
        iWaitDialog = NULL;
        }

    if( iGlobalData )
        {
        iGlobalData->Release();
        }

    if( iDelAllRange )
        delete iDelAllRange;

    iDeleteColIds.Reset();
    
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenDeleteUi::ConstructL
// Second phase of construction
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenDeleteUi::ConstructL()
    {
    TRACE_ENTRY_POINT;

    iGlobalData = CCalenGlobalData::InstanceL();

    // Both the entry view and instance views are needed
    // by the deleteUi commands, there queue the construction of both
    RArray<TInt> colArray;
    iController.GetActiveCollectionidsL(colArray);
    
    CCalInstanceView* instanceView = iGlobalData->InstanceViewL(colArray);
    colArray.Reset();
    if( !instanceView )
        {
        iController.RegisterForNotificationsL( this, ECalenNotifyEntryInstanceViewCreated );
        }
    iController.RegisterForNotificationsL( this, ECalenNotifyCancelDelete );
    iMoreEntriesToDelete = EFalse;
    iDisplayQuery = EFalse;
    iEntriesToDelete = KEntriesToDelete;
    iDelAllRange = NULL;
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenDeleteUi::HandleECalenNotifyViewCreatedL
// Handles ECalenNotifyViewCreated.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenDeleteUi::HandleECalenNotifyViewCreatedL()
    {
    TRACE_ENTRY_POINT;
    RArray<TInt> colArray;
    iController.GetActiveCollectionidsL(colArray);
    
    if( iGlobalData->InstanceViewL(colArray) )
        {
        // Handle the outstanding command
        HandleCommandL( iStoredCommand );

        // Cancel the notify as the entry view is now
        // constructed.
        iController.CancelNotifications( this );
        }
    colArray.Reset();
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenDeleteUi::HandleNotification
// Handles notifications.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenDeleteUi::HandleNotification(const TCalenNotification aNotification )
    {
    TRACE_ENTRY_POINT;

    if ( aNotification == ECalenNotifyEntryInstanceViewCreated )
        {
        PIM_TRAPD_HANDLE( HandleECalenNotifyViewCreatedL() );
        }
    if( aNotification == ECalenNotifyCancelDelete)
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

// ----------------------------------------------------------------------------
// CCalenDeleteUi::HandleCommandL
// Handles action ui commands
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
TBool CCalenDeleteUi::HandleCommandL( const TCalenCommand& aCommand )
    {
    TRACE_ENTRY_POINT;
    TBool continueCommand(EFalse);
    
    RArray<TInt> colArray;
    iController.GetActiveCollectionidsL(colArray);
    
    if( colArray.Count() && !( iGlobalData->InstanceViewL(colArray) ) )
        {
        iStoredCommand = aCommand;
        }
    else
        {
        switch( aCommand.Command() )
            {
            case ECalenDeleteCurrentEntry:
                DeleteCurrentEntryL(); // Entry & instance
                break;

            case ECalenDeleteSeries:
                DeleteThisOrAllL( CalCommon::EThisAndAll );
                break;
                
            case ECalenDeleteCurrentOccurrence:
                DeleteThisOrAllL( CalCommon::EThisOnly );
                break;
                
            case ECalenDeleteEntryWithoutQuery:
                continueCommand = DeleteEntryWithoutQueryL();
                break;

            case ECalenDeleteAllEntries:
                HandleDeleteAllEntriesL();
                break;

            case ECalenDeleteEntriesBeforeDate:
                DeleteEntriesBeforeDateL(); // EntryView & instance
                break;

            default:
                // Controller decided this class was the place to handle this
                // command but it wasn't in our list; something has gone wrong.
                //ASSERT( EFalse );
                break;
            }
        }
    colArray.Reset();
    TRACE_EXIT_POINT;
    return continueCommand;
    }

// ----------------------------------------------------------------------------
// CCalenDeleteUi::CalenCommandHandlerExtensionL
// Dummy implementation.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
TAny* CCalenDeleteUi::CalenCommandHandlerExtensionL( TUid /*aExtensionUid*/ )
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    return NULL;
    }

// ----------------------------------------------------------------------------
// CCalenDeleteUi::DeleteThisOrAllL
// Deletes series repeating entry
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenDeleteUi::DeleteThisOrAllL( CalCommon::TRecurrenceRange aRepeatType )
    {
    TRACE_ENTRY_POINT;
    
    TBool isDeleted( EFalse );
    
    RArray<TInt> colIdArray;
    colIdArray.AppendL(iGlobalData->Context().InstanceId().iColId);
    
    if( iGlobalData->Context().InstanceId().iEntryLocalUid )
        {
        CCalInstance* instance = 
            CalenActionUiUtils::FindPossibleInstanceL( 
                                           iGlobalData->Context().InstanceId(),
                                           *iGlobalData->InstanceViewL(colIdArray) );
            if( instance )
            {
            CleanupStack::PushL( instance );
            isDeleted = DeleteSingleInstanceL( instance, aRepeatType );
            
            if( isDeleted )
                {
                CleanupStack::Pop( instance );
                }
            else
                {
                CleanupStack::PopAndDestroy( instance );
                }
            }
        }
    colIdArray.Reset();
    
    iController.BroadcastNotification( isDeleted? ECalenNotifyEntryDeleted :
                                                               ECalenNotifyDeleteFailed );
    
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenDeleteUi::DeleteEntryWithoutQueryL()
// Deletes the current entry
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
TBool CCalenDeleteUi::DeleteEntryWithoutQueryL()
    {
    TRACE_ENTRY_POINT;
	TBool continueCommand(EFalse);
    
	// get the context
	MCalenContext& context = iGlobalData->Context();
	
	// get the multliple context ids count
	iMutlipleContextIdsCount = context.MutlipleContextIdsCount();
	
	ASSERT( iMutlipleContextIdsCount );

	    if(!iMoreEntriesToDelete)
		    {
		    iDisplayQuery = ShowMultipleEntriesDeleteQueryL(iMutlipleContextIdsCount);
		    }
		
		if(iDisplayQuery)
			{
			if(!iMoreEntriesToDelete)	
			    {
			    DisplayWaitDialogL();
			    }
			// get the multiple context instance ids
			RArray<TCalenInstanceId>& multipleContextIds = context.GetMutlipleContextIds();
			
			if(iMutlipleContextIdsCount <= iEntriesToDelete )
			    {
			    iMoreEntriesToDelete = EFalse; 
			    iEntriesToDelete = iMutlipleContextIdsCount;
			    }
			else
			    {
			    iMoreEntriesToDelete = ETrue;
	            // set the continue command flag if more entries are there to delete
			    continueCommand = ETrue;
			    }
			    
			TInt index(0);
			while(index<iEntriesToDelete)
			    {
			    // get the local uid of the entry through multiple context list
			    TCalLocalUid entryLocalUid = multipleContextIds[0].iEntryLocalUid;			    
			    if(entryLocalUid)
			        {
			        DeleteEntryL(entryLocalUid, multipleContextIds[0].iColId);
			        }
			    // remove mutliple context based on the instanceid
			    context.RemoveMultipleContextId(multipleContextIds[0]);
			    index++;
			   }

			if(!iMoreEntriesToDelete)
			    {
			    MarkedEntriesDeletedL();    
			    }
			MCalenToolbar* toolbarImpl = iController.Services().ToolbarOrNull(); 
            if (toolbarImpl)
                {
                CAknToolbar& toolbar = toolbarImpl->Toolbar();
    
                // dim clear and clear all toolbar buttons
                toolbar.SetItemDimmed(ECalenNewMeeting, EFalse, ETrue);
                }
			}
        else
            {
            context.ResetMultipleContextIds();
            // notify delete failed
            iController.BroadcastNotification(ECalenNotifyDeleteFailed);    
            }
	
    TRACE_EXIT_POINT;
    return continueCommand;
    }

// ----------------------------------------------------------------------------
// CCalenDeleteUi::DeleteCurrentEntryL
// Deletes the current entry
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenDeleteUi::DeleteCurrentEntryL()
    {
    TRACE_ENTRY_POINT;

    TBool deleted( EFalse );
    TCalenNotification notification = ECalenNotifyDeleteFailed;
    
    // Make sure we're focused on an entry.
    if( iGlobalData->Context().InstanceId().iEntryLocalUid )
        {
        //If todo, use the LUid.
        //Todos returns the current time if start or end time has not been saved.
        if( CCalEntry::ETodo == iGlobalData->Context().InstanceId().iType )
            {
            CCalEntry* entry = iGlobalData->EntryViewL(iGlobalData->Context().InstanceId().iColId)->FetchL(
                                    iGlobalData->Context().InstanceId().iEntryLocalUid );

            if( entry )
                {
                CleanupStack::PushL( entry );
                deleted = DeleteEntryL( iGlobalData->EntryViewL(iGlobalData->Context().InstanceId().iColId), entry );

                if( deleted )
                    {
                    CleanupStack::Pop( entry );
                    notification = ECalenNotifyEntryDeleted;
                    }
                else
                    {
                    CleanupStack::PopAndDestroy( entry );
                    }
                }
            }
        else // Not todo
            {
            RArray<TInt> colIdArray;
            colIdArray.AppendL(iGlobalData->Context().InstanceId().iColId);
                
            CCalInstance* instance = CalenActionUiUtils::FindPossibleInstanceL( 
                                                            iGlobalData->Context().InstanceId(),
                                                            *iGlobalData->InstanceViewL(colIdArray) );
            // if we have instance we will do delete other wise just return
            if( instance )
                {
                // Note: ownership handling of instance is dirty in this case, 
                // because DeleteSingleInstanceLtakes ownership, if it's deletes 
                // instance (property of CalInterimApi), otherwise not.
                CleanupStack::PushL( instance );
                deleted = DeleteSingleInstanceL( instance );

                if( deleted )
                    {
                    CleanupStack::Pop( instance );
                    notification = ECalenNotifyEntryDeleted;
                    }
                else
                    {
                    CleanupStack::PopAndDestroy( instance );
                    }
                }
            colIdArray.Reset();
            }
        }
    else
        {
         TBool doDelete( ETrue );
         
         doDelete = CalenActionUiUtils::ShowDeleteConfirmationQueryL( 
                                                                   iGlobalData->Context().InstanceId().iType == CCalEntry::ETodo ?
                                                                   CalenActionUiUtils::EDeleteToDo :
                                                                   CalenActionUiUtils::EDeleteEntry );
		if ( doDelete )
			{
			notification = ECalenNotifyEntryDeleted;	
			}
        }
        
    iController.BroadcastNotification( notification );

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenDeleteUi::DeleteAllEntriesL
// Deletes all entries
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenDeleteUi::DeleteAllEntriesL()
    {
    TRACE_ENTRY_POINT;

    ASSERT( !iIsDeleting );

    const TInt buttonId = CalenActionUiUtils::ShowDeleteConfirmationQueryL( 
                                           CalenActionUiUtils::EDeleteAll );

    if( buttonId )
        {
        HandleDeleteMultipleEventsL( TCalTime::MinTime(), TCalTime::MaxTime(),
                                 R_QTN_CALE_CONF_ALL_NOTES_DELETED );
        }
    else
        {
        // notify delete failed
        iController.BroadcastNotification(ECalenNotifyDeleteFailed); 
        }

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenDeleteUi::DeleteEntriesBeforeDateL
// Deletes all entries before a set date.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenDeleteUi::DeleteEntriesBeforeDateL()
    {
    TRACE_ENTRY_POINT;

    ASSERT( !iIsDeleting );
    TTime date;
    date.HomeTime();
    TTime today = date;

    TBool execute( EFalse );
    TBool exit( ETrue );
    do{
        exit = ETrue;
        execute = EFalse;

        TInt buttonId = CalenActionUiUtils::DateQueryL(date, R_CALEN_DEL_BEFORE_DATE_PROMPT);

        if( buttonId == EAknSoftkeyOk || buttonId == EEikBidOk )
            {
            execute = ETrue;
            if( today < date )
                {
                CAknNoteDialog* dialog = new( ELeave ) CAknNoteDialog( 
                                                            CAknNoteDialog::EConfirmationTone,
                                                            CAknNoteDialog::ELongTimeout );
                dialog->ExecuteLD( R_CALEN_DELETEERROR_NOTE );
                execute = EFalse;
                exit = EFalse;
                }
            }
        }while( !exit );

    // Do delete only if inputted day is after beginning of range
    if( execute && date > TCalTime::MinTime() )
        {
        // Two pass delete:
        // 1. pass
        // To prevent destroying entries starting and ending midnight
        // subtract one microsecond and do delete on that range.
        date -= TTimeIntervalMicroSeconds32( 1 );
        date = Max( date, TCalTime::MinTime() );

        HandleDeleteMultipleEventsL( TCalTime::MinTime(),
                                                date,
                                               R_QTN_CALE_CONF_PAST_NOTE_DELETED );
        }
    else
        {
        iController.BroadcastNotification( ECalenNotifyDeleteFailed );
        }
    
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenDeleteUi::HandleDeleteMultipleEventsL
// Handles multiple delete events
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenDeleteUi::HandleDeleteMultipleEventsL( const TTime& aFirstDay,
                                                                     const TTime& aLastDay,
                                                                     TInt /*aConfNoteId */)
    {
    TRACE_ENTRY_POINT;

    ASSERT( !iWaitDialog );
    ASSERT( !iIsDeleting );
    //iConfirmationNoteId = aConfNoteId;

    iWaitDialog = new( ELeave ) CAknWaitDialog( REINTERPRET_CAST( CEikDialog**, 
                                                                  &iWaitDialog ) );
    iWaitDialog->ExecuteLD( R_CALEN_DELETE_WAIT_NOTE );

    DeleteDayRangeL( aFirstDay, aLastDay );

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenDeleteUi::DeleteDayRangeL
// Deletes all entries in a given range.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenDeleteUi::DeleteDayRangeL( const TTime& aStart,
                                                      const TTime& aEnd )
    {
    TRACE_ENTRY_POINT;

    iStartTime = aStart;
    iEndTime = aEnd;

    TCalTime start, end;
    start.SetTimeLocalL( iStartTime );
    end.SetTimeLocalL( iEndTime );

    if( iDelAllRange )
        {
        delete iDelAllRange;
        iDelAllRange = NULL;
        }
    
    iDelAllRange = new(ELeave) CalCommon::TCalTimeRange( start, end );
    iDeleteColIds.Reset();

    iIsDeleting = ETrue;   

    iController.GetActiveCollectionidsL(iDeleteColIds);
    
    //remember the calenders, delete entries in each calendar one by one by calling DeleteL(...) after completed()
    iNumberOfCalendars = iDeleteColIds.Count();
    iToShowDeleteNote = 0;
    iGlobalData->EntryViewL(iDeleteColIds[iToShowDeleteNote])->DeleteL( *iDelAllRange, CalCommon::EIncludeAll, *this );
        
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenDeleteUi::Completed
// Completed callback
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenDeleteUi::Completed( TInt aFirstPassError )
    {
    TRACE_ENTRY_POINT;

    PIM_TRAPD_HANDLE( DoCompletedL( aFirstPassError ) );

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenDeleteUi::DoCompletedL
// Handles delete callback
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenDeleteUi::DoCompletedL( TInt aFirstPassError )
    {
    TRACE_ENTRY_POINT;
    iToShowDeleteNote++;
    if(iNumberOfCalendars == iToShowDeleteNote)
        {
	    if( aFirstPassError == KErrNone )
	        {
	        // 2: Second pass, delete notes that end 00:00 of next day of iEndTime
	        // We didn't delete them in first pass
	        TTime nextMidnight = CalenDateUtils::BeginningOfDay( iEndTime + TTimeIntervalDays( 1 ) );
	        nextMidnight = Min( nextMidnight, TCalTime::MaxTime() );
	        PIM_TRAPD_HANDLE( DeleteEntriesEndingAtMidnightL( nextMidnight ) );
	        }
	        
		// 3. End deleting, close wait dialog, and show confirmation or error note
		iIsDeleting = EFalse;
		iToShowDeleteNote = 0;
		// dismiss the waitdialog
		if(iWaitDialog)
			{
			TRAP_IGNORE(iWaitDialog->ProcessFinishedL());
			}

		if( aFirstPassError == KErrNone )
			{
			// Show confirmation note
//			HBufC* buf = StringLoader::LoadLC( iConfirmationNoteId, iEikEnv );
//			CAknConfirmationNote* dialog = new( ELeave ) CAknConfirmationNote();
//			dialog->ExecuteLD(*buf);
//			CleanupStack::PopAndDestroy( buf );
			}
		else
			{
			// Show error note
			if(iEikEnv)
			   {
			   iEikEnv->ResolveError( aFirstPassError );
			   }
			}
		iDeleteColIds.Reset();
		delete iDelAllRange;
		iDelAllRange = NULL;
		
	    iController.BroadcastNotification( ECalenNotifyMultipleEntriesDeleted );
        }
    else
        {
        //delete entries in next calendar...
        iGlobalData->EntryViewL(iDeleteColIds[iToShowDeleteNote])->DeleteL( *iDelAllRange, CalCommon::EIncludeAll, *this );
        }

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenDeleteUi::NotifyProgress
// Delete progress notification
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
TBool CCalenDeleteUi::NotifyProgress()
    {
    TRACE_ENTRY_POINT;
    // Tell framework that we are not intrested in Progress notifications.
    TRACE_EXIT_POINT;
    return EFalse;
    }

// ----------------------------------------------------------------------------
// CCalenDeleteUi::Progress
// Delete progress notification
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenDeleteUi::Progress( TInt /*aPercentageCompleted*/ )
    {
    TRACE_ENTRY_POINT;
    // do nothing, we are not intrested in Progress notifications
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenDeleteUi::DeleteEntriesEndingAtMidnightL
// Deletes entries ending at midnight on the given day
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenDeleteUi::DeleteEntriesEndingAtMidnightL( TTime aMidnight )
    {
    TRACE_ENTRY_POINT;

    TCalTime start, end;
    start.SetTimeLocalL( aMidnight - TTimeIntervalMinutes(1) );
    end.SetTimeLocalL( aMidnight );
    CalCommon::TCalTimeRange midnightRange( start, end );
    
    RArray<TInt> colIdArray;
    iController.GetActiveCollectionidsL(colIdArray);
    
    // 1: Find instances falling on midnight moment
    RPointerArray< CCalInstance > instances;
    CleanupResetAndDestroyPushL( instances );
    iGlobalData->InstanceViewL(colIdArray)->FindInstanceL( instances,
                                                 CalCommon::EIncludeAll, 
                                                 midnightRange );
    
    colIdArray.Reset();

    // 2. loop through them and delete those entries that end at midnight
    for( TInt i=0; i < instances.Count(); ++i )
        {
        CCalInstance* item = instances[i];
        RArray<TInt> colIdArray;
        colIdArray.AppendL(item->InstanceIdL().iCollectionId);
        
        // Checking that if entry ends at midnight, is quite clumsy, but here goes:
        // EndsAtStartOfDay takes only CCalInstance, but here we mimic EndsAtStartOfDay
        // for CCalEntry type.

        // First check that if _instance_ ends at midnight, but starts earlier
        if( CalenAgendaUtils::EndsAtStartOfDayL( item, aMidnight ) )
            {
            // Second, check that _entry's_ endtime is exactly the midnight
            // This prevents us from destroying repeating entries, that has one
            // instance falling on given midnight.
            if( item->Entry().EndTimeL().TimeLocalL() == aMidnight )
                {
                iGlobalData->InstanceViewL(colIdArray)->DeleteL( item, CalCommon::EThisOnly );
                // Ownership was transferred to DeleteL.
                // Put null to array to prevent double-deletion
                instances[i] = NULL;                
                }
            }
        colIdArray.Reset();
        }
    CleanupStack::PopAndDestroy( &instances );

    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenDeleteUi::DeleteSingleInstanceL
// Delete the given instance. Ask the user whether to delete the series or the instance.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CCalenDeleteUi::DeleteSingleInstanceL( CCalInstance* aInstance )
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    return DoDeleteSingleInstanceL( aInstance, EFalse, CalCommon::EThisAndAll );
    }

// -----------------------------------------------------------------------------
// CCalenDeleteUi::DeleteSingleInstanceL
// Delete the given instance. Delete the entry range given by aRepeatType.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CCalenDeleteUi::DeleteSingleInstanceL( CCalInstance* aInstance, 
                                                            CalCommon::TRecurrenceRange aRepeatType )
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    return DoDeleteSingleInstanceL( aInstance, ETrue, aRepeatType );
    }

// -----------------------------------------------------------------------------
// CCalenDeleteUi::DoDeleteSingleInstanceL
// Performs the deletion of the instance. If aHasRepeatType is EFalse, the user
// is prompted to delete either the instance or the entire series. In this case,
// aRepeatType is ignored. If aHasRepeatType is ETrue, aRepeatType determines
// whether to delete the instance or the entire series.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CCalenDeleteUi::DoDeleteSingleInstanceL( CCalInstance* aInstance,
                                                           TBool aHasRepeatType,
                                                           CalCommon::TRecurrenceRange aRepeatType )
    {
    TRACE_ENTRY_POINT;

    CCalEntry& entry = aInstance->Entry();
    const CCalEntry::TType entryType = entry.EntryTypeL(); 
    RArray<TInt> colIdArray;
    colIdArray.AppendL(aInstance->InstanceIdL().iCollectionId);
    

    TCalRRule rrule;

    TBool repeating = entry.GetRRuleL( rrule );

    if( !repeating )
        {
        // Even though there is no RRule, the entry might
        // have a list of rdates.
        RArray< TCalTime > rDateArray;
        CleanupClosePushL( rDateArray );
        entry.GetRDatesL( rDateArray );
        repeating = ( rDateArray.Count() > 0 );
        CleanupStack::PopAndDestroy(); // rDateArray
        }

    const TBool child = entry.RecurrenceIdL().TimeUtcL() != Time::NullTTime();

    if( !aHasRepeatType )
        {
        aRepeatType = CalCommon::EThisAndAll;
        }

    TBool doDelete( ETrue );

    if( !aHasRepeatType && ( child || repeating ) && ( entryType != CCalEntry::EAnniv ) )
        {
        doDelete = CalenActionUiUtils::ShowRepeatTypeQueryL( aRepeatType,
                                                           CalenActionUiUtils::EDelete );
        }
    else
        {
        doDelete = CalenActionUiUtils::ShowDeleteConfirmationQueryL( 
                                                                   entryType == CCalEntry::ETodo ?
                                                                   CalenActionUiUtils::EDeleteToDo :
                                                                   CalenActionUiUtils::EDeleteEntry );
        }
        
    if( doDelete )
        {
        //Before deleteing the entry reset the attachment model
        if(iController.Services().GetAttachmentData()->NumberOfItems())
            {
            iController.Services().GetAttachmentData()->Reset();
            }
        
        if( !TryDeleteWithMrUtilsL( aInstance, aRepeatType ) )
            {
            if( !child || aRepeatType == CalCommon::EThisOnly )
                {
                iGlobalData->InstanceViewL(colIdArray)->DeleteL( aInstance, aRepeatType );
                }
            else if( aRepeatType == CalCommon::EThisAndAll )
                {
                // Unfortunately we can't pass the existing child instance through to the
                // InstanceView DeleteL function because even if we pass in EThisAndAll, it
                // only ever deletes the exception. We'll have to fetch the parent then
                // delete it via the entry view.
                RPointerArray<CCalEntry> entries;
                CleanupResetAndDestroyPushL( entries );
                iGlobalData->EntryViewL(aInstance->InstanceIdL().iCollectionId)->FetchL( aInstance->Entry().UidL(), entries );
                iGlobalData->EntryViewL(aInstance->InstanceIdL().iCollectionId)->DeleteL( *entries[0] );
                CleanupStack::PopAndDestroy( &entries );
                if( aInstance )
                    {
                    delete aInstance;
                    aInstance = NULL;
                    }
                }
            else
                {
                User::Leave( KErrNotSupported );
                }
            }
        }

    colIdArray.Reset();
    TRACE_EXIT_POINT;
    return doDelete;
    }

// -----------------------------------------------------------------------------
// CCalenDeleteUi::DeleteEntryL
// Deletes an entry from the database
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CCalenDeleteUi::DeleteEntryL( CCalEntryView* aEntryView, CCalEntry* aEntry )
    {
    TRACE_ENTRY_POINT;
    const CCalEntry::TType entryType = aEntry->EntryTypeL();

    TBool doDelete = CalenActionUiUtils::ShowDeleteConfirmationQueryL( 
                                                                    entryType == CCalEntry::ETodo ?
                                                                    CalenActionUiUtils::EDeleteToDo :
                                                                    CalenActionUiUtils::EDeleteEntry );
    if( doDelete )
        {
        //Before deleteing the attachment, reset the attachment model
        if(iController.Services().GetAttachmentData()->NumberOfItems())
            {
            iController.Services().GetAttachmentData()->Reset();
            }
        aEntryView->DeleteL( *aEntry );

        if( aEntry )
            {
            delete aEntry;
            aEntry = NULL;
            }
        }

    TRACE_EXIT_POINT;
    return doDelete;
    }

// -----------------------------------------------------------------------------
// CCalenDeleteUi::TryDeleteWithMrUtilsL
// Attempt to delete the instance using the Meeting Request utilities,
// if MR viewers is enabled.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CCalenDeleteUi::TryDeleteWithMrUtilsL( CCalInstance* aInstance, 
                                                              CalCommon::TRecurrenceRange aRepeatType )
    {
    TRACE_ENTRY_POINT;
    
    TBool doDelete = ETrue;

	if( iGlobalData->InterimUtilsL().MRViewersEnabledL() && 
        iGlobalData->InterimUtilsL().IsMeetingRequestL(aInstance->Entry()))

        {
        CMRMailboxUtils::TMailboxInfo info;
        if( iGlobalData->AttemptToRetrieveDefaultMailboxL( info ) )
            {
            if(aRepeatType == CalCommon::EThisAndAll )
                {
                iGlobalData->MeetingRequestUtilsL().DeleteWithUiL( aInstance->Entry(),
                                                                                      info.iEntryId );
                if( aInstance )
                    {
                    delete aInstance;
                    aInstance = NULL;
                    }
                }
            else if( aRepeatType == CalCommon::EThisOnly )
                {
                iGlobalData->MeetingRequestUtilsL().DeleteWithUiL( aInstance, info.iEntryId );
                }
            else
                {
                User::Leave( KErrNotSupported );
                }
            }
        else
            {
            doDelete = EFalse;
            }
        }
    else
        {
        doDelete = EFalse;
        }

    TRACE_EXIT_POINT;
    return doDelete;
    }

// -----------------------------------------------------------------------------
// CCalenDeleteUi::ShowMultipleEntriesDeleteQueryL
// For displaying multiple entries deletion confirmation query
// -----------------------------------------------------------------------------
//
TInt CCalenDeleteUi::ShowMultipleEntriesDeleteQueryL(TInt aCount)
	{
    TRACE_ENTRY_POINT;

    CAknQueryDialog *dialog = CAknQueryDialog::NewL( );
    CleanupStack::PushL( dialog );
    TInt resID;
    HBufC* prompt;
    if( aCount > 1 )
        {
        resID = R_CALEN_QTN_TODO_QUEST_DELETE_MARKED_NOTES;
        prompt = StringLoader::LoadLC( resID, aCount );
        }
    else if( aCount ==  1 )
        {
        resID = R_CALEN_QTN_TODO_QUEST_DELETE_MARKED_NOTE;
        prompt = StringLoader::LoadLC( resID );
        }
    else
        {
        CleanupStack::PopAndDestroy( dialog );
        TRACE_EXIT_POINT;
        return 0;   //return 0 for other invalid aCount value ( < 0 )
        }
    dialog->SetPromptL( *prompt );
    CleanupStack::PopAndDestroy( prompt );

    CleanupStack::Pop( dialog );

    TRACE_EXIT_POINT;
    return dialog->ExecuteLD( R_CALEN_ERASEQUERY_NOTE );

	}

// -----------------------------------------------------------------------------
// CCalenDeleteUi::DialogDismissedL
// From MProgressDialogCallback
// Callback method
// called when a dialog is dismissed.
// -----------------------------------------------------------------------------
//
void CCalenDeleteUi::DialogDismissedL( const TInt /*aButtonId*/ )
    {
    TRACE_ENTRY_POINT;
    // dismiss the wait dialog
    if(iWaitDialog)
        {
        iWaitDialog->ProcessFinishedL();
        }
    
    // no more entries to delete
    iMoreEntriesToDelete = EFalse;
    iDisplayQuery = EFalse;
    
    // issue notification cancel delete
    iController.BroadcastNotification(ECalenNotifyCancelDelete);
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenDeleteUi::DeleteEntryL
// Delete entry using entry local uid
// -----------------------------------------------------------------------------
//
void CCalenDeleteUi::DeleteEntryL(TCalLocalUid& aEntryLocalUid, TInt aColId)
    {
    TRACE_ENTRY_POINT;
    
    // fetch the entry
    CCalEntry* entry = iGlobalData->EntryViewL(aColId)->FetchL(aEntryLocalUid);
    if( entry )
        {
        CleanupStack::PushL( entry );
        iGlobalData->EntryViewL(aColId)->DeleteL( *entry );
        CleanupStack::Pop( entry );
        delete entry;
        entry = NULL;
        }   
    
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenDeleteUi::DisplayWaitDialogL
// Display wait dialog
// -----------------------------------------------------------------------------
//
void CCalenDeleteUi::DisplayWaitDialogL()
    {
    TRACE_ENTRY_POINT;
    
    delete iWaitDialog;
    iWaitDialog = NULL;
    iWaitDialog = new( ELeave )CAknWaitDialog( REINTERPRET_CAST( CEikDialog**, &iWaitDialog ), ETrue );
    iWaitDialog->ExecuteLD( R_TODO_VIEW_DELETE_WAIT_NOTE );
    iWaitDialog->SetCallback(this);
    
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenDeleteUi::MarkedEntriesDeletedL
// Dismiss wait dialog and show information note
// -----------------------------------------------------------------------------
//
void CCalenDeleteUi::MarkedEntriesDeletedL()
    {
    TRACE_ENTRY_POINT;
    
    // dismiss the waitdialog
    if(iWaitDialog)
        {
        iWaitDialog->ProcessFinishedL();
        }
    
    // Show confirmation note
    //HBufC* buf = StringLoader::LoadLC( R_QTN_CALE_CONF_ALL_NOTES_DELETED, iEikEnv );
    //CAknConfirmationNote* dialog = new( ELeave ) CAknConfirmationNote();
    //dialog->ExecuteLD(*buf);
    //CleanupStack::PopAndDestroy( buf );

    // notify marked entries deleted
    iController.BroadcastNotification( ECalenNotifyMarkedEntryDeleted );
    
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenDeleteUi::HandleDeleteAllEntriesL
// Handles launching of the delete all entries list query
// -----------------------------------------------------------------------------
//
void CCalenDeleteUi::HandleDeleteAllEntriesL()
    {
    TRACE_ENTRY_POINT;
    RPointerArray<CCalCalendarInfo> calendarInfoList;
    CleanupClosePushL(calendarInfoList);
	iController.Services().GetAllCalendarInfoL(calendarInfoList);
    TInt visibleCalendarsCount(0);
    for(TInt index=0;index<calendarInfoList.Count();index++)
        {
        if(calendarInfoList[index]->Enabled())
            {
            visibleCalendarsCount++;
            }
        if(visibleCalendarsCount>1)
            {
            break;
            }
        }
    CleanupStack::PopAndDestroy();
    
    TInt headingTextResourceId(0);
    if(visibleCalendarsCount==1)
        {
        headingTextResourceId = R_CALE_SINGLE_CALENDAR_DELETE_ENTRIES;
        }
    else
        {
        headingTextResourceId = R_CALE_MULTI_CALENDAR_DELETE_ENTRIES;
        }

    TInt selectedIndex(0);
    CAknListQueryDialog* deleteEntriesListDialog = new(ELeave) CAknListQueryDialog(&selectedIndex);
    deleteEntriesListDialog->PrepareLC(R_DELETE_ENTRIES_LIST_QUERY);
    HBufC* buf = StringLoader::LoadLC( headingTextResourceId, iEikEnv );
    deleteEntriesListDialog->QueryHeading()->SetTextL(buf->Des());
    CleanupStack::PopAndDestroy( buf );
    
    if(deleteEntriesListDialog->RunLD())
        {
        if(selectedIndex)
            {
            DeleteAllEntriesL();
            }
        else
            {
            DeleteEntriesBeforeDateL();
            }
        }
    else
        {
        iController.BroadcastNotification(ECalenNotifyDeleteFailed);
        }
    
    TRACE_EXIT_POINT
    }
	
// End of File
