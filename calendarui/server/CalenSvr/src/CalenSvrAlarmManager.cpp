/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Provides methods for queueing and other funtionalities for alarms. 
 *
*/



//debug
#include "calendarui_debug.h"

#include "CalenSvrAlarmManager.h"
#include <calenglobaldata.h>

#include "CalendarPrivateCRKeys.h"  // includes CalendarInternalCRKeys.h

#include <asshdalarm.h>
#include <bacntf.h>
#include <centralrepository.h>
#include <tz.h>
#include <calenconstants.h>

const TUid KUidAgendaAlarmCategory = { 0x101F4A70 };


// ---------------------------------------------------------------------------
// ?description_if_needed
// ---------------------------------------------------------------------------
//
CCalenSvrAlarmManager* CCalenSvrAlarmManager::NewL()
    {
    TRACE_ENTRY_POINT;

    CCalenSvrAlarmManager* self = new( ELeave )CCalenSvrAlarmManager();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);

    TRACE_EXIT_POINT;
    return self;
    }

// ---------------------------------------------------------------------------
// ?description_if_needed
// ---------------------------------------------------------------------------
//
CCalenSvrAlarmManager::CCalenSvrAlarmManager()
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------------------------
// ?description_if_needed
// ---------------------------------------------------------------------------
//
void CCalenSvrAlarmManager::ConstructL()
    {
    TRACE_ENTRY_POINT;

    // there is no visible need to keep the session open...
    // would it break something if we open it only when needed...?
    User::LeaveIfError( iAlarmServer.Connect() );

    // store the current time zone 
    // so we can check if it was changed after system time change
    iTimeZone = CurrentTimeZoneIdL();

    // register locale change listener. This listens for system time
    // change events. When system time is changed, skipped alarms are checked
    // and alarms are requeued. 
    TCallBack callback( LocaleChangeCallback, this );
    iLocaleChangeNotifier = CEnvironmentChangeNotifier::NewL( CActive::EPriorityStandard, callback );
    iLocaleChangeNotifier->Start();

    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------------------------
// ?description_if_needed
// ---------------------------------------------------------------------------
//
TUint CCalenSvrAlarmManager::CurrentTimeZoneIdL() const
    {
    TRACE_ENTRY_POINT;

    RTz timeZoneSrv;
    User::LeaveIfError( timeZoneSrv.Connect() );
    CleanupClosePushL( timeZoneSrv );

    CTzId* tzID = timeZoneSrv.GetTimeZoneIdL();
    TUint id = tzID->TimeZoneNumericID();
    delete tzID;

    CleanupStack::PopAndDestroy(); // close timeZoneSrv

    TRACE_EXIT_POINT;
    return id;
    }

// ---------------------------------------------------------------------------
// ?description_if_needed
// ---------------------------------------------------------------------------
//
CCalenSvrAlarmManager::~CCalenSvrAlarmManager()
    {
    TRACE_ENTRY_POINT;

    delete iLocaleChangeNotifier;

    iAlarmServer.Close();
    
    if ( iGlobalData )
        {
        iGlobalData->Release();
        }

    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------------------------
// ?description_if_needed
// ---------------------------------------------------------------------------
//
TInt CCalenSvrAlarmManager::LocaleChangeCallback(TAny* aThisPtr)
    {
    TRACE_ENTRY_POINT;
    TInt retVal = static_cast<CCalenSvrAlarmManager*>( aThisPtr )->HandleLocaleChange();
    TRACE_EXIT_POINT;
    return retVal;
    }

// ---------------------------------------------------------------------------
// ?description_if_needed
// ---------------------------------------------------------------------------
//
TInt CCalenSvrAlarmManager::HandleLocaleChange()
    {
    TRACE_ENTRY_POINT;
    if( iLocaleChangeNotifier->Change() & EChangesSystemTime )
        {
        PIM_TRAPD_HANDLE( CheckSkippedAlarmsL(); )
        PIM_TRAPD_HANDLE( ReQueueAlarmsL(); )
        }
    TRACE_EXIT_POINT;
    return 0;
    }

// ---------------------------------------------------------------------------
// ?description_if_needed
// ---------------------------------------------------------------------------
//
void CCalenSvrAlarmManager::CheckSkippedAlarmsL()
    {
    TRACE_ENTRY_POINT;

    // remove alarms from alarm server
    RArray<TAlarmId> skippedAlarms;
    CleanupClosePushL( skippedAlarms );
    GetSkippedAgendaAlarmsL( skippedAlarms );

    // fetch the current time zone to compare it with the old one
    const TUint newTimeZone = CurrentTimeZoneIdL();
    const TBool tzChanged( newTimeZone != iTimeZone );
    iTimeZone = newTimeZone;

    if( skippedAlarms.Count() )
        {
        for(TInt i(0); i < skippedAlarms.Count(); i++)
            {
            iAlarmServer.AlarmDelete( skippedAlarms[i] );
            }
        SetSystemTimeChangedFlagL( KCalenLostAlarms );
        }
    else
        {
        // there wasn't any skipped alarms, but time zone was changed
        if( tzChanged )
            {
            // value KCalenTimeZoneChanged in KCalendarLostAlarm indicates a time zone change
            SetSystemTimeChangedFlagL( KCalenTimeZoneChanged );
            }
        }

    CleanupStack::PopAndDestroy( &skippedAlarms ); // close skippedAlarms

    TRACE_EXIT_POINT;
    }
    
// ---------------------------------------------------------------------------
// Open an entry view to re-queue calendar alarms.
// ---------------------------------------------------------------------------
//
void CCalenSvrAlarmManager::ReQueueAlarmsL()
    {
    TRACE_ENTRY_POINT;
    if( !iOpeningEntryView )
        {
        // leave only after the iOpeningEntryView has been set back to EFalse!
        iOpeningEntryView = ETrue;
        
        
        // An entry view is created to make sure that the calendar 
        // alarms get queued. The entry view creation within the 
        // global data is done asyncronously. 
        TRAPD( error,
                    // Is the global data already constructed?
                    CCalenGlobalData* existingGD = CCalenGlobalData::Instance();
                    CCalenGlobalData* gData = NULL;
                    if ( !existingGD )
                        {
                        // If not create the global data, but using 
                        // this class as the observer.
                        // Context change notifier is NULL.
                        iGlobalData = CCalenGlobalData::NewL( *this, NULL );
                        gData = iGlobalData;
                        }
                    else 
                        {
                        gData = existingGD;
                        CleanupReleasePushL( *existingGD );
                        }
                          
                    // Create the session
                    gData->CalSessionL();
                        
                    // Get the entryView, if this is null it the entry view
                    // will be created asyncronously.
                    gData->EntryViewL();
                    
                    if ( existingGD )
                        {
                        // If the global data already existed, it can be
                        // released here else it will be released upon the
                        // callback being completed.
                        CleanupStack::PopAndDestroy(); // existingGD
                        }
            );

            iOpeningEntryView = EFalse;
            User::LeaveIfError( error );
        }
        
    TRACE_EXIT_POINT;
    }
    
// -----------------------------------------------------------------------------
// CCalenSvrAlarmManager::Completed
// Called when the construction of the entry view
// has completed.
// -----------------------------------------------------------------------------
//    
void CCalenSvrAlarmManager::Completed(TInt /*aError*/)
    {
    // In this instance, interest is in the
    // entry view being created. Therefore
    // the global data can be released.
    iGlobalData->Release();
    iGlobalData = NULL;
    }

// -----------------------------------------------------------------------------
// CCalenSvrAlarmManager::NotifyProgress()
// Returns if progress notifications are
// required. As there is no interest in
// the progress returns EFalse.
// -----------------------------------------------------------------------------
//
TBool CCalenSvrAlarmManager::NotifyProgress()
    {
    TRACE_ENTRY_POINT;
    
    return EFalse;
     
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenSvrAlarmManager::Progress
// Intentionally empty as there is no interest in the
// progress of the entry view creation
// -----------------------------------------------------------------------------
//
void CCalenSvrAlarmManager::Progress(TInt /*aPercentageCompleted*/)
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------------------------
// ?description_if_needed
// ---------------------------------------------------------------------------
//
void CCalenSvrAlarmManager::SetSystemTimeChangedFlagL(TInt aFlag)
    {
    TRACE_ENTRY_POINT;

    CRepository* repository = CRepository::NewL( KCRUidCalendar );
    CleanupStack::PushL( repository );

    User::LeaveIfError( repository->Set( KCalendarLostAlarm, aFlag ) );

    CleanupStack::PopAndDestroy(); // repository

    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------------------------
// ?description_if_needed
// ---------------------------------------------------------------------------
//
void CCalenSvrAlarmManager::GetSkippedAgendaAlarmsL(RArray<TAlarmId>& aSkippedAlarms) const
    {
    TRACE_ENTRY_POINT;

    RArray<TAlarmId> alarmIds;
    CleanupClosePushL( alarmIds );

    TRAPD( err, 
           iAlarmServer.GetAlarmIdListForCategoryL( KUidAgendaAlarmCategory, alarmIds ); )

    if( err == KErrNone )
        {
        TTime now;
        now.HomeTime();
        // ignoring seconds and microseconds.
        TDateTime tmp( now.DateTime() );
        tmp.SetSecond( 0 );
        tmp.SetMicroSecond( 0 );
        now = tmp;

        for(TInt i(0); i < alarmIds.Count(); i++)
            {
            TASShdAlarm alarm;
            iAlarmServer.GetAlarmDetails( alarmIds[i], alarm );

            if( alarm.IsFloating() && // fixed alarms won't expire due to time or time zone changes
                alarm.State() != EAlarmStateNotified &&
                alarm.NextDueTime() < now )
                {
                aSkippedAlarms.Append( alarmIds[i] );
                }
            }
        }

    CleanupStack::PopAndDestroy( &alarmIds ); // close alarmIds array

    TRACE_EXIT_POINT;
    }


// End of File
