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
* Description:  This is the source file for the CClkUiAlarmModel class.
*
*/

// System includes
#include <asclisession.h>
#include <almconst.h>
#include <wakeupalarm.h>

// User includes
#include "clkuimdlbase.h"
#include "clkuialarmmodel.h"
#include "clkuialarmlistener.h"

// Constants

// Literals

// ---------------------------------------------------------
// CClkUiAlarmModel::NewL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
EXPORT_C CClkUiAlarmModel* CClkUiAlarmModel::NewL( MClkModelObserver* aObserver,
                                                   TInt aListenerPriority )
    {
    CClkUiAlarmModel *self = new( ELeave ) CClkUiAlarmModel;
    
    CleanupStack::PushL( self );
    self->ConstructL( aObserver, aListenerPriority );
    CleanupStack::Pop( self );
    
    return self;
    }

// ---------------------------------------------------------
// CClkUiAlarmModel::~CClkUiAlarmModel
// rest of the details are commented in the header
// ---------------------------------------------------------
//
EXPORT_C CClkUiAlarmModel::~CClkUiAlarmModel()
    {
    // Stop the listener first and close the session with alarm server.
    Stop();
    iAlarmSrvSes.Close();
    }

// ---------------------------------------------------------
// CClkUiAlarmModel::ProcessListenerL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClkUiAlarmModel::ProcessListenerL( TInt aStatus )
    {
    if( ( KErrNone != aStatus ) &&
        ( KErrCancel != aStatus ) )
        {
        NotifyL( aStatus );
        }
    else
        {
        NotifyL( KErrNone );
        }
    }

// ---------------------------------------------------------
// CClkUiAlarmModel::GetClkAlarmIds
// rest of the details are commented in the header
// ---------------------------------------------------------
//
EXPORT_C void CClkUiAlarmModel::GetClkAlarmIds( RArray< TAlarmId >& aAlarmIds )
    {
    // Return the alarm ids of category clock.
    TRAP_IGNORE( iAlarmSrvSes.GetAlarmIdListForCategoryL( KAlarmClockOne, aAlarmIds ) );
    }

// ---------------------------------------------------------
// CClkUiAlarmModel::DeleteClockAlarm
// rest of the details are commented in the header
// ---------------------------------------------------------
//
EXPORT_C TInt CClkUiAlarmModel::DeleteClockAlarm( TAlarmId aAlarmId )
    {
    // Pass the request to the server.
    TInt errorVal = iAlarmSrvSes.AlarmDelete( aAlarmId );
    
    return errorVal;
    }

// ---------------------------------------------------------
// CClkUiAlarmModel::ClockAlarmEnable
// rest of the details are commented in the header
// ---------------------------------------------------------
//
EXPORT_C TInt CClkUiAlarmModel::ClockAlarmEnable( TAlarmId aAlarmId, TAlarmStatus aStatus )
    {
    TInt returnVal( KErrNone );
    
    // If the alarm is enabled, we first disable it and then enable it.
    if( EAlarmStatusEnabled == aStatus )
        {
        returnVal = iAlarmSrvSes.SetAlarmStatus( aAlarmId, EAlarmStatusDisabled );
        
        if( KErrNone == returnVal )
            {
            returnVal = iAlarmSrvSes.SetAlarmStatus( aAlarmId, aStatus );
            }
        }
    // If the request is to disable the alarm, we just do the same.
    else
        {
        returnVal = iAlarmSrvSes.SetAlarmStatus( aAlarmId, aStatus );
        }
    
    return returnVal;
    }

// ---------------------------------------------------------
// CClkUiAlarmModel::ClockAlarmStatus
// rest of the details are commented in the header
// ---------------------------------------------------------
//
EXPORT_C TAlarmStatus CClkUiAlarmModel::ClockAlarmStatus( TAlarmId aAlarmId ) const
    {
    TAlarmStatus alarmStatus;
    
    // Pass the request to the server.
    iAlarmSrvSes.GetAlarmStatus( aAlarmId, alarmStatus );
    
    return alarmStatus;
    }

// ---------------------------------------------------------
// CClkUiAlarmModel::ClockAlarmType
// rest of the details are commented in the header
// ---------------------------------------------------------
//
EXPORT_C TInt CClkUiAlarmModel::ClockAlarmType( TAlarmCategory& aCategory, TAlarmId aAlarmId ) const
    {
    return ( iAlarmSrvSes.GetAlarmCategory( aAlarmId, aCategory ) );
    }

// ---------------------------------------------------------
// CClkUiAlarmModel::ClockAlarmSet
// rest of the details are commented in the header
// ---------------------------------------------------------
//
EXPORT_C TInt CClkUiAlarmModel::ClockAlarmSet( const SClkAlarmInfo& aAlarmInfo )
    {
    TASShdAlarm newAlarm;
    
    // Build the alarm properties from the info provided.
    newAlarm.Category()           = KAlarmClockOne;
    newAlarm.Message()            = aAlarmInfo.iMessage;
    newAlarm.OriginalExpiryTime() = aAlarmInfo.iAlarmTime;
    newAlarm.RepeatDefinition()   = aAlarmInfo.iRepeat;
    newAlarm.SoundName()          = aAlarmInfo.iSound;
    newAlarm.NextDueTime()        = aAlarmInfo.iAlarmTime;
    
	// SSM related change.
#ifdef SYMBIAN_SYSTEM_STATE_MANAGEMENT  
    newAlarm.SetWakeup( ETrue );
#else
    newAlarm.ClientFlags().Set( KWakeupAlarmFlagIndex );
#endif
    
    TInt returnVal = iAlarmSrvSes.AlarmAdd( newAlarm );
    
    if( KErrNone == returnVal )
        {
        iAlarmObject = newAlarm;
        iAlarmId = newAlarm.Id();
        }
    
    return returnVal;
    }

// ---------------------------------------------------------
// CClkUiAlarmModel::ClockAlarmInfo
// rest of the details are commented in the header
// ---------------------------------------------------------
//
EXPORT_C TInt CClkUiAlarmModel::ClockAlarmInfo( TAlarmId aAlarmId, SClkAlarmInfo& aAlarmInfo ) const
    {
    TASShdAlarm tempAlarm;
    
    if( !aAlarmId )
        {
        aAlarmInfo.iState = EAlarmStateInPreparation;
        aAlarmInfo.iStatus = EAlarmStatusDisabled;
        
        return KErrNone;
        }
    
    TInt returnVal = iAlarmSrvSes.GetAlarmDetails( aAlarmId, tempAlarm );
    
    if( KErrNone == returnVal )
        {
        aAlarmInfo.iAlarmTime = tempAlarm.NextDueTime();
        aAlarmInfo.iMessage = tempAlarm.Message();
        aAlarmInfo.iOrigExpiryTime = tempAlarm.OriginalExpiryTime();
        aAlarmInfo.iRepeat = tempAlarm.RepeatDefinition();
        aAlarmInfo.iSound = tempAlarm.SoundName();
        aAlarmInfo.iState = tempAlarm.State();
        aAlarmInfo.iStatus = tempAlarm.Status();
        iAlarmSrvSes.GetAlarmCategory( aAlarmId, aAlarmInfo.iCategory );
        }
    
    return returnVal;
    }

// ---------------------------------------------------------
// CClkUiAlarmModel::ClockAlarmInfo
// rest of the details are commented in the header
// ---------------------------------------------------------
//
EXPORT_C const TAlarmId& CClkUiAlarmModel::AlarmId()
    {
    return iAlarmId;
    }

// ---------------------------------------------------------
// CClkUiAlarmModel::SaveLatestAlarmId
// rest of the details are commented in the header
// ---------------------------------------------------------
//
EXPORT_C void CClkUiAlarmModel::SaveLatestAlarmId( TAlarmId aAlarmId )
    {
    iAlarmId = aAlarmId;
    }

// ---------------------------------------------------------
// CClkUiAlarmModel::ClockAlarmInfo
// rest of the details are commented in the header
// ---------------------------------------------------------
//
EXPORT_C TBool CClkUiAlarmModel::GetAlarmQueuedOrSnoozedId( TAlarmId& aAlarmId, TBool aUpdatedId )
    {
    TBool alarmQueued( EFalse );
    RArray< TAlarmId > alarmIds;

    TRAP_IGNORE( iAlarmSrvSes.GetAlarmIdListForCategoryL( KAlarmClockOne, alarmIds ) );

    TInt alarmCount( alarmIds.Count() );
    for( TInt alarmIndex( KZerothIndex ); alarmIndex < alarmCount; alarmIndex++ )
        {
        const TAlarmId alarmId = alarmIds[ alarmIndex ];
        TAlarmStatus alarmStatus;
        TASShdAlarm tempAlarm;

        // First get the alarm details.
        TInt errorVal = iAlarmSrvSes.GetAlarmDetails( alarmId, tempAlarm );
        // Then the status.
        errorVal = iAlarmSrvSes.GetAlarmStatus( alarmId, alarmStatus );

        // If the alarm status is enabled and the alarm is in either queued or snoozed state.
        // The alarm ids we have received will be sorted based on the expiry time.
        if( ( KErrNone == errorVal ) &&
            ( EAlarmStatusEnabled == tempAlarm.Status() ) &&
            ( EAlarmStateQueued == tempAlarm.State() || 
              EAlarmStateSnoozed == tempAlarm.State() ) )
            {
            // The queued alarm is found.
            aAlarmId = alarmId;
            alarmQueued = ETrue;

            if( aUpdatedId )
                {
                iAlarmId = alarmId;
                }
            break;
            }
        }

    alarmIds.Close();

    if( aUpdatedId && !alarmQueued )
        {
        iAlarmId = KZerothIndex;
        }

    return alarmQueued;
    }

// ---------------------------------------------------------
// CClkUiAlarmModel::ConstructL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClkUiAlarmModel::ConstructL( MClkModelObserver* aObserver,
                                   TInt aListenerPriority )
    {
    // First Set the observer.
    SetModelObserver( aObserver );
    
    // Connect to the alarm server.
    User::LeaveIfError( iAlarmSrvSes.Connect() );
    
    // Construct the listener and make it active.
    CClkUiMdlAlarmListener* alarmListener = new( ELeave ) CClkUiMdlAlarmListener( iAlarmSrvSes,
                                                                                  *this,
                                                                                  aListenerPriority );
    SetListenerActive( alarmListener );
    
    // Get the latest alarm and store it.
    TAlarmId latestAlarm;
    GetAlarmQueuedOrSnoozedId( latestAlarm, ETrue );    
    }

// End of file
