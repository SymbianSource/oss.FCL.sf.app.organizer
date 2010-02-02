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
* Description:  
 *
*/


//debug
#include "calendarui_debug.h"

// INCLUDE FILES
#include "AIAgendaPluginEngine.h"
#include "AIAgendaPluginEngineImpl.h"

#include "PluginDataChangeObserver.h"
#include "EventExpirationTimer.h"

#include <calsession.h>
#include <calentry.h>
#include <calinstance.h>
#include <calrrule.h>
#include <calnotification.h>

#include <coemain.h>
#include <bacntf.h>

// =============================================================================
// =============================================================================


// ============================ MEMBER FUNCTIONS ===============================
// (CAIAgendaPluginEngineImpl)

// ---------------------------------------------------------
// CAIAgendaPluginEngineImpl::ConstructL
// Symbian 2nd phase constructor can leave.
// ---------------------------------------------------------
//
void CAIAgendaPluginEngineImpl::ConstructL(void)
{
    TRACE_ENTRY_POINT;
    
    iSession.ConnectL();

    if( iObserverType == ECalendarObserver || iObserverType == ECalendarAndTodoObserver )
    {
        TCallBack environmentChangeCallBack( EnvironmentChangeCallBack, this );

        // Create a notifier for system time and locale changes and midnight crossover
        iEnvironmentChangeNotifier = CEnvironmentChangeNotifier::NewL( EActivePriorityLogonA,
                                                                       environmentChangeCallBack );
        iEnvironmentChangeNotifier->Start();

        iEventChangeObserver = CPropertyObserver::NewL( *this, KCalPubSubCategory, ECalPubSubEventNotification, ETrue);
    }

    if( iObserverType == EToDoObserver || iObserverType == ECalendarAndTodoObserver )
    {
        iTodoChangeObserver = CPropertyObserver::NewL( *this, KCalPubSubCategory, ECalPubSubTodoNotification, ETrue );
    }

    TCallBack expirationCallBack( EventExpiredCallBack, this );
    iExpirationTimer = CEventExpirationTimer::NewL( expirationCallBack );

    TCallBack refreshCallBack( RefreshTimerCallBack, this );
    iRefreshTimer = CEventExpirationTimer::NewL( refreshCallBack );

    StateMachine();
    
    iCalendarSession = CCalSession::NewL();
    iCalendarSession->StartFileChangeNotificationL(*this);
    
    TRACE_EXIT_POINT;
}

// ---------------------------------------------------------
// CAIAgendaPluginEngineImpl::NewL
// Two-phased constructor.
// Create instance of concrete ECOM interface implementation
// ---------------------------------------------------------
//
CAIAgendaPluginEngineImpl* CAIAgendaPluginEngineImpl::NewL(MPluginDataChangeObserver& aDataChangeObserver, TObserverType aObserverType)
{
    TRACE_ENTRY_POINT;
    
    CAIAgendaPluginEngineImpl* self = new( ELeave )CAIAgendaPluginEngineImpl( aDataChangeObserver, aObserverType );
    CleanupStack::PushL( self );

    self->ConstructL();

    CleanupStack::Pop( self );
    
    TRACE_EXIT_POINT;
    return self;
}

// ---------------------------------------------------------
// CAIAgendaPluginEngineImpl::CAIAgendaPluginEngineImpl
// ?implementation_description
// (other items were commented in a header).
// ---------------------------------------------------------
//
CAIAgendaPluginEngineImpl::CAIAgendaPluginEngineImpl(MPluginDataChangeObserver& aDataChangeObserver, TObserverType aObserverType)
 : iDataChangeObserver( aDataChangeObserver ),
   iObserverType( aObserverType ),
   iDBState( EDBOffline ),
   iNeedsUpdate( EFalse ),
   iHadEvents( EFalse )
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------
// CAIAgendaPluginEngineImpl::~CAIAgendaPluginEngineImpl
// ?implementation_description
// (other items were commented in a header).
// ---------------------------------------------------------
//
CAIAgendaPluginEngineImpl::~CAIAgendaPluginEngineImpl(void)
{
    TRACE_ENTRY_POINT;
    
    iDBState = EDBOffline;
    
	if(iCalendarSession)
		{
		iCalendarSession->StopFileChangeNotification();
		delete iCalendarSession;
		iCalendarSession = NULL;
		}
    
    delete iExpirationTimer;
    delete iRefreshTimer;
    delete iEventChangeObserver;
    delete iTodoChangeObserver;

    if( iEnvironmentChangeNotifier )
    {
        iEnvironmentChangeNotifier->Cancel();
        delete iEnvironmentChangeNotifier;
    }

    delete iCalendarEngine;  // Close() called in destructor

    iInstanceArray.ResetAndDestroy();
    iInstanceArray.Close();

    iSession.Close();
    
    TRACE_EXIT_POINT;
}

// ---------------------------------------------------------
// CAIAgendaPluginEngineImpl::StateMachine
// ?implementation_description
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CAIAgendaPluginEngineImpl::StateMachine(void)
{
    TRACE_ENTRY_POINT;
    
    if( iObserverType == ECalendarObserver || iObserverType == ECalendarAndTodoObserver )
    {
        iExpirationTimer->Cancel();
    }
    iNeedsUpdate = EFalse;

    switch( iDBState )
    {
        case EDBOffline:
        {
            iDBState = EDBInitializing;
            iSession.Initialize( *this );
        }
        break;

        case EDBInitialized:
        {
            OpenDataBase();
            iDBState = EDBOpening;  // EDBInitialized -> EDBOpening
        }
        break;

        case EDBInitializing:
        case EDBOpening:
        {
            // needs update 
            // -> set the fetch state to normal and wait...
            iFetchState = EFetchNormal;
        }
        break;

        case EDBOpen:
        {
            if( iFetchState == EFetchNormal )
            {
                GetTodayData();
            }
            else
            {
                GetFutureData();
            }
            iFetchState = EFetchNormal;
            iDBState = EDBReading;  // EDBOpen -> EDBReading
        }
        break;

        case EDBReading:
        {
            // needs update
            // -> set the flag
            iNeedsUpdate = ETrue;  // the only place where we set this to ETrue!!!

            // then set the fetch state to normal and wait...
            iFetchState = EFetchNormal;
        }
        break;

        default:
            // fall through...
            break;
    }
    
    TRACE_EXIT_POINT;
}

// ---------------------------------------------------------
// CAIAgendaPluginEngineImpl::OpenDataBase
// ?implementation_description
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CAIAgendaPluginEngineImpl::DoOpenDataBaseL(void)
{
    TRACE_ENTRY_POINT;
    
    iCalendarEngine = CCalenEngine::NewL();  // TRAPPED!
    iCalendarEngine->OpenDatabaseL( *this );  // TRAPPED!
    
    TRACE_EXIT_POINT;
}

void CAIAgendaPluginEngineImpl::OpenDataBase(void)
    {
    TRACE_ENTRY_POINT;
    
    TRAPD(err, DoOpenDataBaseL());
    if(err != KErrNone)
        {
        CloseCalendarEngine();
        }
    
    TRACE_EXIT_POINT;    
    }
    
// ---------------------------------------------------------
// CAIAgendaPluginEngineImpl::OpenDatabaseCompleted
// ?implementation_description
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CAIAgendaPluginEngineImpl::OpenDatabaseCompleted(void)
{
    TRACE_ENTRY_POINT;
    
    iDBState = EDBOpen;  // EDBOpening -> EDBOpen
    StateMachine();
    
    TRACE_EXIT_POINT;
}

void CAIAgendaPluginEngineImpl::GetTodayData(void)
   {
    TRACE_ENTRY_POINT;
    
    TRAPD(err, DoGetTodayDataL());
    if(err != KErrNone)
        {
        CloseCalendarEngine();
        }
    
    TRACE_EXIT_POINT;    
    }
// ---------------------------------------------------------
// CAIAgendaPluginEngineImpl::GetTodayData
// ?implementation_description
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CAIAgendaPluginEngineImpl::DoGetTodayDataL(void)
{
    TRACE_ENTRY_POINT;
    
    TTime today;
    today.HomeTime();

    iInstanceArray.ResetAndDestroy();

    switch( iObserverType )
    {
        case ECalendarObserver:
            iCalendarEngine->GetEntriesForDayL( *this, today, iInstanceArray );  // TRAPPED!
            break;

        case EToDoObserver:
            iCalendarEngine->GetTodosL( *this, iInstanceArray );  // TRAPPED!
            break;

        case ECalendarAndTodoObserver:
            iCalendarEngine->GetCalendarDataL( *this, iInstanceArray, today, 8/* total number of days to search */ );  // TRAPPED
            break;

        default:
            ASSERT( EFalse );
    }

    TRACE_EXIT_POINT;
}

void CAIAgendaPluginEngineImpl::GetEntriesCompleted(void)
    {
    TRACE_ENTRY_POINT;
    
    TRAPD(err, DoGetEntriesCompletedL());
    if(err != KErrNone)
        {
        CloseCalendarEngine();
        }
    
    TRACE_EXIT_POINT;    
    }
// ---------------------------------------------------------
// CAIAgendaPluginEngineImpl::GetEntriesCompleted
// ?implementation_description
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CAIAgendaPluginEngineImpl::DoGetEntriesCompletedL(void)
{
    TRACE_ENTRY_POINT;
    
    iDBState = EDBOpen;  // EDBReading -> EDBOpen

    if( !iNeedsUpdate )
    {
        RemoveExpiredEntries();

        if( HasEventsForTodayL() )
        {
            SetTimerForNextExpiringEventL();
            NotifyObserverAndCloseDB();
        }
        else
        {
            iFetchState = EFetchFuture;
            StateMachine();
        }
    }
    else
    {
        StateMachine();
    }
 
    TRACE_EXIT_POINT;
}

void CAIAgendaPluginEngineImpl::GetFutureData(void)
    {
    TRACE_ENTRY_POINT;
    
    TRAPD(err, DoGetFutureDataL());
    if(err != KErrNone)
        {
        CloseCalendarEngine();
        }
    
    TRACE_EXIT_POINT;    
    }
// ---------------------------------------------------------
// CAIAgendaPluginEngineImpl::GetFutureData
// ?implementation_description
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CAIAgendaPluginEngineImpl::DoGetFutureDataL(void)
{
    TRACE_ENTRY_POINT;
    
    TTime tomorrow;
    tomorrow.HomeTime();
    tomorrow += TTimeIntervalDays( 1 );
    const TInt daysTosearch( 7 );

    iCalendarEngine->GetEventForNextSevenDaysL( *this, tomorrow, daysTosearch, iInstanceArray );  // TRAPPED!
 
    TRACE_EXIT_POINT;
}

// ---------------------------------------------------------
// CAIAgendaPluginEngineImpl::GetEntriesCompleted
// ?implementation_description
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CAIAgendaPluginEngineImpl::GetFutureEventCompleted(void)
{
    TRACE_ENTRY_POINT;
    
    iDBState = EDBOpen;  // EDBReading -> EDBOpen

    if( !iNeedsUpdate )
    {
        NotifyObserverAndCloseDB();
    }
    else
    {
        StateMachine();
    }
    
    TRACE_EXIT_POINT;
}

// ---------------------------------------------------------
// CAIAgendaPluginEngineImpl::GetTodosCompleted
// ?implementation_description
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CAIAgendaPluginEngineImpl::GetTodosCompleted(void)
{
    TRACE_ENTRY_POINT;
    
    iDBState = EDBOpen;  // EDBReading -> EDBOpen

    if( !iNeedsUpdate )
    {
        NotifyObserverAndCloseDB();
    }
    else
    {
        StateMachine();
    }
    
    TRACE_EXIT_POINT;
}

void CAIAgendaPluginEngineImpl::GetCalendarDataCompleted(void)
    {
    TRACE_ENTRY_POINT;
    
    TRAPD(err, DoGetCalendarDataCompletedL());
    if(err != KErrNone)
        {
        CloseCalendarEngine();
        }
    
    TRACE_EXIT_POINT;    
    }
    
// ---------------------------------------------------------
// CAIAgendaPluginEngineImpl::GetCalendarDataCompleted
// ?implementation_description
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CAIAgendaPluginEngineImpl::DoGetCalendarDataCompletedL(void)
{
    TRACE_ENTRY_POINT;
    
    iDBState = EDBOpen;  // EDBReading -> EDBOpen

    if( !iNeedsUpdate )
    {
        RemoveExpiredEntries();
        SetTimerForNextExpiringEventL();
        NotifyObserverAndCloseDB();
    }
    else
    {
        StateMachine();
    }
    
    TRACE_EXIT_POINT;
}

// ---------------------------------------------------------
// CAIAgendaPluginEngineImpl::HandleError
// ?implementation_description
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CAIAgendaPluginEngineImpl::HandleError(TInt aError)
{
    TRACE_ENTRY_POINT;
    
    // ignore engine cancellation if we're already offline...
    if( aError != KErrCancel && iDBState != EDBOffline )
    {
        // reset the state machine (no notification to the client)
        CloseCalendarEngine();
        // Just to make sure the UI gets updated after an error
        iExpirationTimer->Cancel();
        iRefreshTimer->Cancel();
        iRefreshCounter = 0;
        const TTimeIntervalMicroSeconds32 oneMinute( 60 * 1000000 );
        iRefreshTimer->After( oneMinute );  // refresh after one minute
    }
    
    TRACE_EXIT_POINT;
}

// ---------------------------------------------------------
// CAIAgendaPluginEngineImpl::NotifyObserverAndCloseDB
// ?implementation_description
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CAIAgendaPluginEngineImpl::NotifyObserverAndCloseDB(void)
{
    TRACE_ENTRY_POINT;
    
    iDataChangeObserver.PluginDataChanged( iInstanceArray, iHadEvents );
    CloseCalendarEngine();
    
    TRACE_EXIT_POINT;
}

// ---------------------------------------------------------
// CAIAgendaPluginEngineImpl::CloseCalendarEngine
// ?implementation_description
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CAIAgendaPluginEngineImpl::CloseCalendarEngine(void)
{
    TRACE_ENTRY_POINT;
    
    iDBState = EDBOffline;  // EDBBOpen -> EDBOffline

    // We can't keep the entries w/o an open session!
    iInstanceArray.ResetAndDestroy();

    delete iCalendarEngine;
    iCalendarEngine = NULL;

    iSession.Uninitialize();
    
    TRACE_EXIT_POINT;
}

void CAIAgendaPluginEngineImpl::RemoveExpiredEntries(void)
    {
    TRACE_ENTRY_POINT;
    
    TRAPD(err, DoRemoveExpiredEntriesL());
    if(err != KErrNone)
        {
        CloseCalendarEngine();
        }
    
    TRACE_EXIT_POINT;    
    }

// ---------------------------------------------------------
// CAIAgendaPluginEngineImpl::RemoveExpiredEntries
// ?implementation_description
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CAIAgendaPluginEngineImpl::DoRemoveExpiredEntriesL(void)
{
    TRACE_ENTRY_POINT;
    
    const TTimeIntervalMinutes thirtyMinutes( 30 );
    const TTimeIntervalMinutes oneMinute( 1 );
    TTime now;
    now.HomeTime();
    iHadEvents = EFalse;

    for( TInt i(0), count(iInstanceArray.Count()); i<count; i++ )
    {
        // for each timed entry (appointment)
        if( iInstanceArray[i]->Entry().EntryTypeL() == CCalEntry::EAppt )
        {
            TBool expired( EFalse );

            if( iObserverType == ECalendarAndTodoObserver )
            {
                // end time already passed?
                if( now >= iInstanceArray[i]->EndTimeL().TimeLocalL() +oneMinute )
                {
                    expired = ETrue;
                }
            }
            else
            {
                const TTime startTime = iInstanceArray[i]->StartTimeL().TimeLocalL();

                TTimeIntervalMinutes currentEventAge( 0 );
                now.MinutesFrom( startTime, currentEventAge );

                TTimeIntervalMinutes diffToNextItem( 0 );
                DiffToNextEventL( i, diffToNextItem );

                // remove if start time more than 30 minutes ago
                // OR
                // start time passed AND next event starting in 30 minutes
                if( currentEventAge >= thirtyMinutes ||
                    ( currentEventAge >= oneMinute && ( diffToNextItem >= oneMinute && diffToNextItem <= thirtyMinutes ) ) )
                // NOTE! if you change the comparison values above remember to check the SetTimerForNextExpiringEventL
                // implementation to prevent timer expiration loop!

                {
                    expired = ETrue;
                }
            }

            if( expired )
            {
                delete iInstanceArray[i];
                iInstanceArray.Remove( i );
                count--;
                i--;
                iHadEvents = ETrue;
            }
        }
    }
    iInstanceArray.Compress();

    TRACE_EXIT_POINT;
}

// ---------------------------------------------------------
// CAIAgendaPluginEngineImpl::HasEventsForTodayL
// ?implementation_description
// (other items were commented in a header).
// ---------------------------------------------------------
//
TBool CAIAgendaPluginEngineImpl::HasEventsForTodayL(void)
{
    TRACE_ENTRY_POINT;
    
    for( TInt i(0), count(iInstanceArray.Count()); i<count; i++ )
    {
        if( iInstanceArray[i]->Entry().EntryTypeL() == CCalEntry::EAppt )
        {
            TRACE_EXIT_POINT;
            return ETrue;
        }
    }
    TRACE_EXIT_POINT;
    return EFalse;
}

// ---------------------------------------------------------
// CAIAgendaPluginEngineImpl::SetTimerForNextExpiringEventL
// ?implementation_description
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CAIAgendaPluginEngineImpl::SetTimerForNextExpiringEventL(void)
{
    TRACE_ENTRY_POINT;
    
    const TTimeIntervalMinutes oneMinute( 1 );
    const TTimeIntervalMinutes thirtyMinutes( 30 );

    if( iObserverType == ECalendarAndTodoObserver )
    {
        TTime now;  now.HomeTime();
        TTime tomorrow( 0 );
        tomorrow += now.DaysFrom( tomorrow );
        tomorrow += TTimeIntervalDays( 1 );

        /**
        * Refresh is needed whenever an appointment passes its end time or becomes over 30 minutes old:
        * Loop through all EAppt items and find the nearest refresh/expiration time.
        **/
        TTime expiryTime( tomorrow ); // initial value (won't cause timer to be set)

        for( TInt i(0), count(iInstanceArray.Count()); i<count; i++ )
        {
            if( iInstanceArray[i]->Entry().EntryTypeL() == CCalEntry::EAppt )
            {
                const TTime endTime( iInstanceArray[i]->EndTimeL().TimeLocalL() +oneMinute );
                TTime ongoingTime( iInstanceArray[i]->StartTimeL().TimeLocalL() +thirtyMinutes +oneMinute );

                // using already expired timer would cause a nasty refresh loop
                if( ongoingTime < now  )
                {
                    ongoingTime = endTime;
                }
                // select the nearest time
                expiryTime = Min( expiryTime, Min( endTime, ongoingTime ) );
            }
        }

        // timer is set only for times between "now" and end of the day
        if( expiryTime > now && expiryTime < tomorrow  )
        {
            iExpirationTimer->At( expiryTime );
        }
    }
    else
    {
        for( TInt i(0), count(iInstanceArray.Count()); i<count; i++ )
        {
            if( iInstanceArray[i]->Entry().EntryTypeL() == CCalEntry::EAppt )
            {
                const TTime startTime = iInstanceArray[i]->StartTimeL().TimeLocalL();

                TTimeIntervalMinutes diffToNextItem( 0 );
                DiffToNextEventL( i, diffToNextItem );

                // if next event is starting from 1 to 30 minutes after the current one
                if( diffToNextItem >= oneMinute && diffToNextItem <= thirtyMinutes )
                {
                    iExpirationTimer->At( startTime + oneMinute );
                }
                else
                {
                    iExpirationTimer->At( startTime  + thirtyMinutes );
                }
                break;  // only one timer... 
            }
        }
    }
    
    TRACE_EXIT_POINT;
}

// ---------------------------------------------------------
// CAIAgendaPluginEngineImpl::DiffToNextEventL
// ?implementation_description
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CAIAgendaPluginEngineImpl::DiffToNextEventL(const TInt aIndex, TTimeIntervalMinutes& aDifference)
{
    TRACE_ENTRY_POINT;
    
    const TInt count( iInstanceArray.Count() );

    if( aIndex+1 < count )
    {
        TInt index( aIndex );
        const TTime startTime = iInstanceArray[ index ]->StartTimeL().TimeLocalL();

        // find next item with different start time (if exist)
        while( ++index < count )
        {
            if( iInstanceArray[ index ]->Entry().EntryTypeL() == CCalEntry::EAppt )
            {
                const TTime nextStartTime = iInstanceArray[ index ]->StartTimeL().TimeLocalL();

                if( nextStartTime != startTime )
                {
                    startTime.MinutesFrom( nextStartTime, aDifference );
                    aDifference = Abs( aDifference.Int() );
                    break;
                }
            }
        }
    }
    
    TRACE_EXIT_POINT;
}

// ---------------------------------------------------------
// CAIAgendaPluginEngineImpl::EventExpiredCallBack
// ?implementation_description
// (other items were commented in a header).
// ---------------------------------------------------------
//
TInt CAIAgendaPluginEngineImpl::EventExpiredCallBack(TAny* aPtr)
{
    TRACE_ENTRY_POINT;
    
    CAIAgendaPluginEngineImpl* thisPtr = static_cast<CAIAgendaPluginEngineImpl*>( aPtr );
    thisPtr->StateMachine();
    
    TRACE_EXIT_POINT;
    return 0; // ignored
}

// ---------------------------------------------------------
// CAIAgendaPluginEngineImpl::RefreshTimerCallBack
// ?implementation_description
// (other items were commented in a header).
// ---------------------------------------------------------
//
TInt CAIAgendaPluginEngineImpl::RefreshTimerCallBack(TAny* aPtr)
{
    TRACE_ENTRY_POINT;
    
    CAIAgendaPluginEngineImpl* thisPtr = static_cast<CAIAgendaPluginEngineImpl*>( aPtr );
    const TUint count( thisPtr->iRefreshCounter );
    thisPtr->iRefreshCounter = 0;

    if( !count || count > 1 )
    {
        thisPtr->StateMachine();
    }
    
    TRACE_EXIT_POINT;
    return 0; // ignored
}

// ---------------------------------------------------------
// CAIAgendaPluginEngineImpl::HandlePropertyChange
// ?implementation_description
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CAIAgendaPluginEngineImpl::HandlePropertyChange(const TUid& /*aCategory*/, const TUint& /*aKey*/, const TInt& /*aValue*/)
{
    TRACE_ENTRY_POINT;
    
    SafeRefresh();
    
    TRACE_EXIT_POINT;
}

// ---------------------------------------------------------
// CAIAgendaPluginEngineImpl::CalendarInfoChangeNotificationL
// ?implementation_description
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CAIAgendaPluginEngineImpl::CalendarInfoChangeNotificationL(
        RPointerArray<CCalFileChangeInfo>& aCalendarInfoChangeEntries)
    {
    TRACE_ENTRY_POINT
    TInt count = aCalendarInfoChangeEntries.Count();
    //if(count)
        {
        SafeRefresh();
        }
    TRACE_EXIT_POINT
    }
// ---------------------------------------------------------
// CAIAgendaPluginEngineImpl::SafeRefresh
// ?implementation_description
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CAIAgendaPluginEngineImpl::SafeRefresh(void)
{
    TRACE_ENTRY_POINT;
    
    const TTimeIntervalMicroSeconds32 waitBeforeRefresh( 1500000 ); // 1.5 seconds

    // Let the first refresh call through...
    // iRefreshCounter is used to prevent double refresh and will reset in RefreshTimerCallBack.
    if( !iRefreshCounter )
    {
        StateMachine();
    }
    iRefreshCounter++;

    // Restart the timer for every request to prevent excessive refresh.
    // There will be only a single refresh call (RefreshTimerCallBack) when the timer expires.
    iRefreshTimer->Cancel();
    iRefreshTimer->After( waitBeforeRefresh );
    
    TRACE_EXIT_POINT;
}

// ---------------------------------------------------------
// CAIAgendaPluginEngineImpl::CalendarServerInitialized
// ?implementation_description
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CAIAgendaPluginEngineImpl::CalendarServerInitialized(void)
{
    TRACE_ENTRY_POINT;
    
    iDBState = EDBInitialized;  // EDBInitializing -> EDBInitialized
    StateMachine();
    
    TRACE_EXIT_POINT;
}

// ---------------------------------------------------------
// CAIAgendaPluginEngineImpl::EnvironmentChangeCallBack
// ?implementation_description
// (other items were commented in a header).
// ---------------------------------------------------------
//
TInt CAIAgendaPluginEngineImpl::EnvironmentChangeCallBack(TAny* aPtr)
{
    TRACE_ENTRY_POINT;
    
    CAIAgendaPluginEngineImpl* thisPtr = static_cast<CAIAgendaPluginEngineImpl*>( aPtr );
    const TInt changes( thisPtr->iEnvironmentChangeNotifier->Change() );

    // IF  iObserverType is ECalendarObserver or ECalendarAndTodoObserver
    //     AND system time/day has changed
    // OR
    // a locale change
    if( ( (thisPtr->iObserverType == ECalendarObserver || 
           thisPtr->iObserverType == ECalendarAndTodoObserver)
          && 
          (changes & EChangesMidnightCrossover ||
           changes & EChangesSystemTime) )
        ||
        changes & EChangesLocale )
        {
            thisPtr->SafeRefresh();
        }
    
    TRACE_EXIT_POINT;
    return EFalse;
}

// End of File
