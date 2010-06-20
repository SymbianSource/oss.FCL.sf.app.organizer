/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Notifies observers of external changes to the calendar database
*
*/



//debug
#include "calendarui_debug.h"

// INCLUDE FILES
#include "CleanupResetAndDestroy.h"
#include "calendbchangenotifier.h"    // CCalenDbChangeNotifier
#include "calenglobaldata.h"            // Calendar global data
#include <calsession.h>                 // CalSession
#include <missedalarm.h>
#include <missedalarmstore.h>
#include <missedalarmstorecrkeys.h>
#include <calcalendarinfo.h>

// -----------------------------------------------------------------------------
// KTimerResolution limits the number of notifications sent to registered
// MCalenDBChangeObserver instances.  Notifications may come from 
// MCalChangeCallBack2 at a very high rate which could impact performance, 
// for example by causing constant view refreshes.
// CCalenDbChangeNotifier notifies observers when KTimerResolution has elapsed
// since the last notification was received from MCalChangeCallBack2
// -----------------------------------------------------------------------------
const TInt KTimerResolution = 1000000;  // 1 Second

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CCalenDbChangeNotifier::CCalenDbChangeNotifier
// C++ default constructor can NOT contain any code, that might leave.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
CCalenDbChangeNotifier::CCalenDbChangeNotifier( CCalSession& aSession ) 
    : CActive( EPriorityNormal ),
      iSession( aSession )
    {
    TRACE_ENTRY_POINT;

    iRestartTimer = EFalse;

    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenDbChangeNotifier::NewL
// Two-phased constructor.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
CCalenDbChangeNotifier* CCalenDbChangeNotifier::NewL( CCalSession& aSession )
    {
    TRACE_ENTRY_POINT;

    CCalenDbChangeNotifier* self = new( ELeave ) CCalenDbChangeNotifier( aSession );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    TRACE_EXIT_POINT;
    return self;
    }

// -----------------------------------------------------------------------------
// CCalenDbChangeNotifier::ConstructL
// Symbian 2nd phase constructor can leave.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
void CCalenDbChangeNotifier::ConstructL()
    {
    TRACE_ENTRY_POINT;

    // We want to receive notifications for the full agenda date range
    TCalTime start, end;
    start.SetTimeUtcL( TCalTime::MinTime() );
    end.SetTimeUtcL( TCalTime::MaxTime() );
    CalCommon::TCalTimeRange range( start, end ); 

    // Create a notification filter
    iCalChangeFilter = CCalChangeNotificationFilter::NewL(
                                                        MCalChangeCallBack2::EChangeEntryAll, 
                                                        ETrue, 
                                                        range );

    // Enable database change notifications on current global data session
    iSession.StartChangeNotification( *this, *iCalChangeFilter );

    // Create a timer to limit the number of notifications broadcast
    iNotificationTimer.CreateLocal();

    // Active object, add to active scheduler
    CActiveScheduler::Add( this );

    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenDbChangeNotifier::~CCalenDbChangeNotifier
// Destructor
// (other items were commented in a header).
// -----------------------------------------------------------------------------
CCalenDbChangeNotifier::~CCalenDbChangeNotifier()
    {
    TRACE_ENTRY_POINT;

    Cancel();
    // Reset the observer array.  Array contents are NOT owned by this class
    iDBObservers.Reset();

    iNotificationTimer.Close();

    // Disable database change notifications on current global data session   
    PIM_TRAPD_HANDLE( iSession.StopChangeNotification() );

    // Destroy the notification filter
    delete iCalChangeFilter;
    
    TRACE_EXIT_POINT;    
    }

// -----------------------------------------------------------------------------
// CCalenDbChangeNotifier::LastDBModificationTime
// Returns the time of the last database change notification.  This may not be
// the time of the last notification sent to MCalenDBChangeObservers.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
TTime CCalenDbChangeNotifier::LastDBModificationTime() const
    {
    TRACE_ENTRY_POINT;

    TRACE_EXIT_POINT;
    return iLastDbChangeNotification;
    }

// -----------------------------------------------------------------------------
// CCalenDbChangeNotifier::CalChangeNotification
// Called when a change to the agenda database occurs from a different session
// to the one we are currently using.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
void CCalenDbChangeNotifier::CalChangeNotification( RArray<TCalChangeEntry>& aChangeItems )
    {
    TRACE_ENTRY_POINT;

    TRAP_IGNORE(HandleMissedAlarmsL(aChangeItems));
    // Always update the last notification time, even if we don't notify 
    // our observers
    iLastDbChangeNotification.UniversalTime();
    if( !IsActive() )
        {
        iNotificationTimer.After( iStatus, KTimerResolution );
        SetActive();
        }
    else
        {
        iRestartTimer = ETrue;
        iNotificationTimer.Cancel();
        }

    TRACE_EXIT_POINT;
    }
void CCalenDbChangeNotifier::HandleMissedAlarmsL(const RArray<TCalChangeEntry>& aChangeItems)
    {
    TRACE_ENTRY_POINT
    CRepository* missedAlarmStoreRepository = CRepository::NewL(
            KCRUidMissedAlarmStore);
    // Create missed alarm store
    CMissedAlarmStore* missedAlarmStore = CMissedAlarmStore::NewL(
            *missedAlarmStoreRepository);
    CleanupStack::PushL(missedAlarmStore);
    RPointerArray<CMissedAlarm> missedAlarmStorelist;
    CleanupResetAndDestroyPushL(missedAlarmStorelist);
    missedAlarmStore->GetL(missedAlarmStorelist);
    CCalCalendarInfo* calendarInfo = iSession.CalendarInfoL();
    CleanupStack::PushL(calendarInfo);
    CCalenDbChangeNotifier::TCalLuidFilename calLuidFilename;
    calLuidFilename.iFilename = calendarInfo->FileNameL();
    if (missedAlarmStorelist.Count())
        {
        for (TInt idx = 0; idx < aChangeItems.Count(); idx++)
            {
            if (aChangeItems[idx].iChangeType == EChangeDelete)
                {
                calLuidFilename.iLuid = aChangeItems[idx].iEntryId;
                TInt index = missedAlarmStorelist.Find(
                        calLuidFilename,CCalenDbChangeNotifier::DoFindEntryByLuid);
                if(index != KErrNotFound)
                    {
                    CMissedAlarm* missedAlarm = missedAlarmStorelist[index];
                    missedAlarmStore->RemoveL(*missedAlarm);
                    }
                }
            }
        }
    CleanupStack::PopAndDestroy(calendarInfo);
    CleanupStack::PopAndDestroy(&missedAlarmStorelist);
    CleanupStack::PopAndDestroy(missedAlarmStore);
    TRACE_EXIT_POINT    
    }
TBool CCalenDbChangeNotifier::DoFindEntryByLuid(
                const TCalLuidFilename* aLuidFilename,const CMissedAlarm& aMissedAlarm)
    {
    TRACE_ENTRY_POINT
    TRACE_EXIT_POINT
    return (aLuidFilename->iLuid == aMissedAlarm.iLuid 
            && !aLuidFilename->iFilename.CompareF(aMissedAlarm.iCalFileName));
    }

// -----------------------------------------------------------------------------
// CCalenDbChangeNotifier::RegisterObserverL
// Adds the passed observer to the observer array.  All observers in the array 
// will be notified of changes to the agenda database.
// (other items were commented in a header).
// -----------------------------------------------------------------------------    
 void CCalenDbChangeNotifier::RegisterObserverL( MCalenDBChangeObserver& aDBObserver )
    {
    TRACE_ENTRY_POINT;

    iDBObservers.Append( &aDBObserver );

    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenDbChangeNotifier::DeRegisterObserverL
// Removes the passed observer to the observer array.  All observers in the array 
// will be notified of changes to the agenda database.
// -----------------------------------------------------------------------------       
void CCalenDbChangeNotifier::DeRegisterObserverL( MCalenDBChangeObserver& aDBObserver )
    {
    TRACE_ENTRY_POINT;

    for( TInt x = 0; x < iDBObservers.Count(); ++x )
        {
        if( iDBObservers[x] == &aDBObserver )
            {
            iDBObservers.Remove( x );
            return;
            }
        }
    User::Leave( KErrNotFound );

    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------       
// CCalenDbChangeNotifier::RunL
// From CActive::RunL
// Called when notification timer expires
// (other items were commented in a header).
// -----------------------------------------------------------------------------       
void CCalenDbChangeNotifier::RunL()
    {
    TRACE_ENTRY_POINT;

    switch( iStatus.Int() )
        {
        case KErrCancel:
            {
            // The normal reason for the timer being cancelled is another
            // database change.  Restart the timer.
            if( iRestartTimer )
                {
                iRestartTimer = EFalse;
                iNotificationTimer.After( iStatus, KTimerResolution );
                SetActive();
                }        
            }
        break;

        case KErrNone:
            {
            //Timer completion, notify observers
            for( TInt x = 0; x < iDBObservers.Count(); ++x )
                {
                iDBObservers[x]->HandleDBChangeL();
                }
            }
        break;

        default:
            {
            User::Leave( KErrArgument );
            }
        break;
        }

    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------       
// CCalenDbChangeNotifier::RunError
// From CActive::RunError
// (other items were commented in a header).
// -----------------------------------------------------------------------------
TInt CCalenDbChangeNotifier::RunError( TInt aError )
    {
    TRACE_ENTRY_POINT;

    //RunL leaving means that the view could not be refreshed.
    //Theres not much we can do except be ready for the next database event.
    iRestartTimer = EFalse;

    TRACE_EXIT_POINT;
    return aError;
    }

// -----------------------------------------------------------------------------       
// CCalenDbChangeNotifier::DoCancel
// From CActive::DoCancel
// (other items were commented in a header).
// -----------------------------------------------------------------------------
void CCalenDbChangeNotifier::DoCancel()
    {
    TRACE_ENTRY_POINT;

    // Stop the notification timer
    iRestartTimer = EFalse;
    iNotificationTimer.Cancel();

    TRACE_EXIT_POINT;
    }

// End of File
