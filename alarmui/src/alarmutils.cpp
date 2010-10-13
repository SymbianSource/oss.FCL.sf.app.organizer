/*
* Copyright (c) 2007-2007 Nokia Corporation and/or its subsidiary(-ies).
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



// INCLUDE FILES
#include "alarmutils.h"

#include "AlmAlertVariant.hrh"
#include "pim_trace.h"

#include "AlmAlarmPlayer.h"
#include "AlmAlarmControl.h"
#include "AlmAlarmInfo.h"
#include "AlarmService.h"
#include "alarmremconeventshandler.h"
#ifdef RD_ALMALERT__SENSOR_SUPPORT
#include "alarmcontextfwsupport.h"
#endif // RD_ALMALERT__SENSOR_SUPPORT

#include <w32std.h>
#include <almconst.h>
#include <eikenv.h>
#include <AknCapServer.h>
#include <e32property.h>
#include <centralrepository.h>
#include <MProfileEngine.h>
#include <MProfile.h>
#include <MProfileTones.h>
#include <ProfileEngineDomainConstants.h>
#include <sbdefs.h>
#include <coreapplicationuisdomainpskeys.h>
#include <clockdomaincrkeys.h>
#include <CalendarInternalCRKeys.h>
#include <wakeupalarm.h>
#include <calalarm.h> // KUidAgendaModelAlarmCategory - the alarm category id for calendar alarms
#include <AknUtils.h>
#include <hwrmpowerstatesdkpskeys.h>
#include <startupdomainpskeys.h>


#ifndef SYMBIAN_CALENDAR_V2
#include <agmalarm.h> // deprecated, use CalAlarm.h when SYMBIAN_CALENDAR_V2 flag is enabled
#endif // SYMBIAN_CALENDAR_V2
#include <calsession.h>
#include <calentryview.h>
#include <calenlauncher.h>  // for launching calendar entry view



// CONSTANTS AND MACROS
const TInt KAlmAlertMinSnooze( 100 );
const TInt KAlmAlertMaxSnooze( 104 );

const TInt KDefaultSnoozeTime( 5 ); // 5 minutes

const TUint KAlarmAutoHide( 60000000 );  // 60 s
const TUint KAlarmAutoHideCalendar( 30000000 );  // 30 s
const TUint KKeyBlockTime( 500000 );  // 0.5 s
const TUint KShutdownTime( 1500000 ); // 1.5 s
const TUint KAlarmDelayTime( 1000000 ); // 1.0 s
const TUint KInactivityResetInterval( 1000000 ); // 1.0 s

const TInt KMaxProfileVolume( 10 );
const TInt KVolumeRampPeriod( 3000000 );  // 3 seconds
_LIT( KRngMimeType, "application/vnd.nokia.ringing-tone" );


// ==========================================================
// ================= MEMBER FUNCTIONS =======================


// ---------------------------------------------------------
// 
// ---------------------------------------------------------
//
CAlarmUtils* CAlarmUtils::NewL( CAlmAlarmControl* aAlarmControl,
                                CAknAlarmService* aSupervisor )
    {
    TRACE_ENTRY_POINT;
    CAlarmUtils* self = new( ELeave )CAlarmUtils( aAlarmControl, aSupervisor );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    TRACE_EXIT_POINT;
    return self;
    }

// ---------------------------------------------------------
// 
// ---------------------------------------------------------
//
void CAlarmUtils::ConstructL()
    {
    TRACE_ENTRY_POINT;
    User::LeaveIfError( iApaSession.Connect() );

    iAlarmInfo = new( ELeave )CAlmAlarmInfo( this );

    // we can still work without profile engine
    PIM_TRAPD_ASSERT( iProfileEng = CreateProfileEngineL(); )

    iNotifierDialogController = ((CAknCapServer*)CEikonEnv::Static()->AppServer())->GlobalNoteControllerL();
    iNotifierDialogController->SetNoteObserver( iAlarmControl );

    // create timers
    iShutdownTimer   = CPeriodic::NewL( CActive::EPriorityStandard );
    iAutoSnoozeTimer = CPeriodic::NewL( CActive::EPriorityStandard );
    iKeyBlockTimer   = CPeriodic::NewL( CActive::EPriorityStandard );
    iAlarmDelayTimer = CPeriodic::NewL( CActive::EPriorityStandard );
    iInactivityResetTimer = CPeriodic::NewL( CActive::EPriorityStandard );
    // set async callback priority so that it finishes before any of the timers
    iAsyncCallBack = new( ELeave )CAsyncCallBack( CActive::EPriorityStandard +1 );

    //RD_ALMALERT__ACCESSORY_STOP removed as a fix for ABAA-74R757
    // accessory observer
    iRemConHandler = CAlarmRemConEventsHandler::NewL( *iAlarmControl );
    
    iCalendarAlarmViewer = EFalse;

    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------
// C++ default constructor
// ---------------------------------------------------------
//
CAlarmUtils::CAlarmUtils( CAlmAlarmControl* aAlarmControl, 
                          CAknAlarmService* aSupervisor )
    : iAlarmControl( aAlarmControl ),
      iSupervisor( aSupervisor ),
      iLightsBlinking( EFalse ),
      iKeyGuardActivated( ETrue )
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------
// Destructor
// ---------------------------------------------------------
//
CAlarmUtils::~CAlarmUtils()
    {
    TRACE_ENTRY_POINT;
    if( iNotifierDialogController )
        {
        iNotifierDialogController->SetNoteObserver( NULL );
        }

    if( iShutdownTimer )
        {
        iShutdownTimer->Cancel();
        delete iShutdownTimer;
        iShutdownTimer = NULL;
        }

    if( iAutoSnoozeTimer )
        {
        iAutoSnoozeTimer->Cancel();
        delete iAutoSnoozeTimer;
        iAutoSnoozeTimer = NULL;
        }

    if( iKeyBlockTimer )
        {
        iKeyBlockTimer->Cancel();
        delete iKeyBlockTimer;
        iKeyBlockTimer = NULL;
        }

    if( iAlarmDelayTimer )
        {
        iAlarmDelayTimer->Cancel();
        delete iAlarmDelayTimer;
        iAlarmDelayTimer = NULL;
        }

    if( iInactivityResetTimer )
        {
        iInactivityResetTimer->Cancel();
        delete iInactivityResetTimer;
        iInactivityResetTimer = NULL;
        }

    if( iAsyncCallBack )
        {
        iAsyncCallBack->Cancel();
        delete iAsyncCallBack;
        iAsyncCallBack = NULL;
        }

    delete iAlarmPlayer;
    iAlarmPlayer = NULL;

    delete iAlarmInfo;
    iAlarmInfo = NULL;

    delete iRemConHandler;
    iRemConHandler = NULL;

    #ifdef RD_ALMALERT__SENSOR_SUPPORT
    delete iCFSupport;
    iCFSupport = NULL;
    #endif // RD_ALMALERT__SENSOR_SUPPORT

    if( iProfileEng )
        {
        iProfileEng->Release();
        iProfileEng = NULL;
        }

    iApaSession.Close();
	
	if(iQueueManagerAsw)
		{
		delete iQueueManagerAsw;
		}
	if(iEntryAsyncWait)
		{
		delete iEntryAsyncWait;
		}
		
	if(iCalSession)
		{
		delete iCalSession;
		}

	if(iCalEntryView)
		{
		delete iCalEntryView;
		}

    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------
// Start the alarm player.
// ---------------------------------------------------------
//
void CAlarmUtils::PlayAlarmSound()
    {
    TRACE_ENTRY_POINT;
    StopAlarmSound();
    PIM_TRAPD_ASSERT( iAlarmPlayer = CAlmAlarmPlayer::NewL( this ); )
    SetBackLight( ETrue );
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------
// Stop the alarm player.
// ---------------------------------------------------------
//
void CAlarmUtils::StopAlarmSound()
    {
    TRACE_ENTRY_POINT;
    #if defined( RD_ALMALERT__SENSOR_SUPPORT )
    // notify the result through the context framework
    // only set the context if the player was really active
    if( iAlarmPlayer && iCFSupport )
        {
        PIM_TRAPD_ASSERT( iCFSupport->PublishAlarmResultL( EResultAlarmSilenced ); )
        }
    #endif // RD_ALMALERT__SENSOR_SUPPORT
    delete iAlarmPlayer;
    iAlarmPlayer = NULL;
    SetBackLight( EFalse );
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------
// Backlight on/off
// ---------------------------------------------------------
//
void CAlarmUtils::SetBackLight(TBool aActive)
    {
    TRACE_ENTRY_POINT;
	TInt status;
	// reading the value before setting. This also ensures if the P&S key is defined or not. 
	TInt ret = RProperty::Get( KPSUidCoreApplicationUIs, 
									KLightsAlarmLightActive, 
									status); 

	if(ret == KErrNotFound )
		{	
		return ;
		}

    if( iLightsBlinking != aActive )
        {
        iLightsBlinking = aActive;
        PIM_ASSERT( RProperty::Set( KPSUidCoreApplicationUIs, 
                                    KLightsAlarmLightActive, 
                                    aActive ? ELightsBlinking : ELightsNotBlinking ); )
        }
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------
// Key guard on/off
// ---------------------------------------------------------
//
void CAlarmUtils::SetKeyGuard(TBool aActive)
    {
    TRACE_ENTRY_POINT;
    if( iKeyGuardActivated != aActive )
        {
        iKeyGuardActivated = aActive;
        PIM_ASSERT( RProperty::Set( KPSUidCoreApplicationUIs, 
                                    KCoreAppUIsDisableKeyguard, 
                                    aActive ? ECoreAppUIsEnableKeyguard : ECoreAppUIsDisableKeyguard ); )
        }
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------
// 
// ---------------------------------------------------------
//
TBool CAlarmUtils::IsClockAlarm() const
    {
    TRACE_ENTRY_POINT;
    const TBool retVal( iAlarmData.iAlarm.Category() == KAlarmClockOne );
    TRACE_EXIT_POINT;
    return retVal;
    }

// ---------------------------------------------------------
// 
// ---------------------------------------------------------
//
TBool CAlarmUtils::IsCalendarAlarm() const
    {
    TRACE_ENTRY_POINT;
    const TBool retVal( iAlarmData.iAlarm.Category() == KUidAgendaModelAlarmCategory );
    TRACE_EXIT_POINT;
    return retVal;
    }

// ---------------------------------------------------------
// Check the active profile ringing type.
// ---------------------------------------------------------
//
TBool CAlarmUtils::IsRingingTypeSilent() const
    {
    TRACE_ENTRY_POINT;
    TBool isSilent( EFalse );

    if( iProfileEng )
        {
        MProfile* profile = NULL;
        PIM_TRAPD_ASSERT( profile = iProfileEng->ActiveProfileL(); )

        if( profile )
            {
            if( profile->ProfileTones().ToneSettings().iRingingType == EProfileRingingTypeSilent ||
                profile->ProfileTones().ToneSettings().iRingingType == EProfileRingingTypeBeepOnce )
                {
                isSilent = ETrue;  // Ringing Type = Silent or BeepOnce
                }
            profile->Release();
            }
        }
    TRACE_EXIT_POINT;
    return isSilent;
    }

// ---------------------------------------------------------
// Check if the used alarm tone is an "off" tone.
// ---------------------------------------------------------
//
TBool CAlarmUtils::IsOffTone() const
    {
    TRACE_ENTRY_POINT;
    TBool isOffTone( EFalse );

    if( iAlarmData.iAlarmTone == KNullDesC )
        {
        isOffTone = ETrue;
        }
    else
        {
        // alarm tone file path w/o drive name
        TPtrC alarmToneFilePath = 
            iAlarmData.iAlarmTone.Mid( 
                TParsePtrC( iAlarmData.iAlarmTone ).Drive().Length() );

        if( alarmToneFilePath == KProfileNoSoundPath )
            {
            isOffTone = ETrue;
            }
        }
    TRACE_EXIT_POINT;
    return isOffTone;
    }

// -----------------------------------------------------------------------------
// Read alarm sound filenames from central repository.
// -----------------------------------------------------------------------------
//
void CAlarmUtils::GetAlarmSoundFilenames()
    {
    TRACE_ENTRY_POINT;
    CRepository* repository = NULL;

    if( IsClockAlarm() )
        {
        PIM_TRAPD_ASSERT( repository = CRepository::NewL( KCRUidClockApp ); )

        if( repository )
            {
            PIM_ASSERT( repository->Get( KClockAppSoundFile, iAlarmData.iAlarmTone ); )
            PIM_ASSERT( repository->Get( KClockAppDefaultSoundFile, iAlarmData.iDefaultAlarmTone ); )
            }
        }
    else
        {
        PIM_TRAPD_ASSERT( repository = CRepository::NewL( KCRUidCalendar ); )

        if( repository )
            {
            PIM_ASSERT( repository->Get( KCalendarSoundFile, iAlarmData.iAlarmTone ); )
            PIM_ASSERT( repository->Get( KCalendarDefaultSoundFile, iAlarmData.iDefaultAlarmTone ); )
            }
        }
    delete repository;
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// Check if the alarm sound file is a KRngMimeType file.
// -----------------------------------------------------------------------------
//
TBool CAlarmUtils::IsRngTone()
    {
    TRACE_ENTRY_POINT;
    TBool isRngTone( EFalse );

    RFs& session = iAlarmControl->ControlEnv()->FsSession();
    // Check that the file exist.  If not, use the default file.
    TUint attributes;
    TInt error = session.Att( iAlarmData.iAlarmTone, attributes );

    if( error )
        {
        iAlarmData.iAlarmTone = iAlarmData.iDefaultAlarmTone;
        }

    TUid dummyUid( KNullUid );
    TDataType dataType;

    error = iApaSession.AppForDocument( iAlarmData.iAlarmTone, dummyUid, dataType );
    ASSERT( !error );

    if( !error && dataType.Des().CompareF( KRngMimeType ) == 0 )
        {
        isRngTone = ETrue;
        }
    TRACE_EXIT_POINT;
    return isRngTone;
    }

// -----------------------------------------------------------------------------
// Read active profile settings from the profile engine.
// -----------------------------------------------------------------------------
//
void CAlarmUtils::GetProfileSettings()
    {
    TRACE_ENTRY_POINT;
    // set default/fallback values
    iAlarmData.iRingType = EProfileRingingTypeRinging; // not used with clock alarm
    iAlarmData.iRepeatValue = KMdaAudioToneRepeatForever; // KMdaRepeatForever
    iAlarmData.iVolume = KMaxProfileVolume; // clock alarms always ramp to maximum volume
    iAlarmData.iVolumeRampTime = iAlarmData.iVolume * KVolumeRampPeriod; // volume ramp only for clock alarms

    if( !IsClockAlarm() )
        {
        if( iProfileEng )
            {
            MProfile* profile = NULL;
            PIM_TRAPD_ASSERT( profile = iProfileEng->ActiveProfileL(); )

            if( profile )
                {
                const TProfileToneSettings& setting = profile->ProfileTones().ToneSettings();
                iAlarmData.iRingType = setting.iRingingType;

                // calendar alarms: if profile is "silent" -> set volume to zero
                iAlarmData.iVolume = (iAlarmData.iRingType == EProfileRingingTypeSilent ? 0 : setting.iRingingVolume);

                profile->Release();
                }
            }

        if( iAlarmData.iRingType == EProfileRingingTypeRingingOnce )
            {
            iAlarmData.iRepeatValue = 1;
            }
        iAlarmData.iVolumeRampTime = 0;
        }
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
TInt CAlarmUtils::MaxProfileVolume() const
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    return KMaxProfileVolume;
    }

// -----------------------------------------------------------------------------
// Access to iAlarmData structure.
// -----------------------------------------------------------------------------
//
CAlarmUtils::TAlarmData& CAlarmUtils::AlarmData()
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    return iAlarmData;
    }

// -----------------------------------------------------------------------------
// Update the iAlarmData structure.
// NOTE! Only to be called from CAlmAlarmControl::ShowAlarm.
// -----------------------------------------------------------------------------
//
void CAlarmUtils::InitialiseAlarmDataL()
    {
    TRACE_ENTRY_POINT;
    // set alarm type first!
    iAlarmData.iAlarmType = IsClockAlarm() ? EAlarmTypeClock : 
                                IsCalendarAlarm() ? EAlarmTypeCalendar : 
                                    EAlarmTypeOther;
    GetSnoozeTimeSetting();
    GetProfileSettings();
    GetAlarmSoundFilenames();

    // get calendar entry data and store the local uid and instance time
    GetCalendarEntryInfoL();
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// Update the iAlarmData structure.
// NOTE! Only to be called from CAlmAlarmControl::ShowAlarm.
// -----------------------------------------------------------------------------
//
void CAlarmUtils::UninitialiseAlarmData()
    {
    TRACE_ENTRY_POINT;

	SetCalendarViewStatus(ECalViewCreateNone);

	if(iEntryAsyncWait)
		{
		delete iEntryAsyncWait;
		iEntryAsyncWait =  NULL;
		}
	
	if(iQueueManagerAsw)
		{
		delete iQueueManagerAsw;
		iQueueManagerAsw =  NULL;
		}

	if(iCalEntryView)
		{
		delete iCalEntryView;
		iCalEntryView= NULL;
		}
	
	if(iCalSession)
		{
		delete iCalSession;
		iCalSession =  NULL;
		}

    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------
// Check the snooze count
// ---------------------------------------------------------
//
TBool CAlarmUtils::CanSnooze() const
    {
    TRACE_ENTRY_POINT;
    const TInt snoozeCount( iAlarmData.iAlarm.ClientData1() );
    // can't snooze if snoozeCount equals KAlmAlertMinSnooze
    const TBool retVal = !(snoozeCount == KAlmAlertMinSnooze);
    TRACE_EXIT_POINT;
    return retVal;
    }

// ---------------------------------------------------------
// Read the user snooze time setting for the active alarm.
// ---------------------------------------------------------
//
void CAlarmUtils::GetSnoozeTimeSetting()
    {
    TRACE_ENTRY_POINT;
    // use the default value if cenrep reading fails
    iAlarmData.iSnoozeTime = KDefaultSnoozeTime;

    switch( iAlarmData.iAlarmType )
        {
        case EAlarmTypeClock:
            {
            CRepository* repository = NULL;
            PIM_TRAPD_ASSERT( repository = CRepository::NewL( KCRUidClockApp ); )

            if( repository )
                {
                repository->Get( KClockAppSnoozeTime, iAlarmData.iSnoozeTime );
                delete repository;
                }
            break;
            }

        case EAlarmTypeCalendar:
            {
            CRepository* repository = NULL;
            PIM_TRAPD_ASSERT( repository = CRepository::NewL( KCRUidCalendar ); )

            if( repository )
                {
                repository->Get( KCalendarSnoozeTime, iAlarmData.iSnoozeTime );
                delete repository;
                }
            break;
            }

        case EAlarmTypeOther:
            {
            iAlarmData.iSnoozeTime = KDefaultSnoozeTime;
            break;
            }

        default:
            ASSERT( EFalse );
        }

    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// Return ETrue if we are restoring data.
// -----------------------------------------------------------------------------
//
TBool CAlarmUtils::IsPhoneInRestoreMode() const
    {
    TRACE_ENTRY_POINT;
    using namespace conn;
    TBool backupOrRestore( EFalse );
    TInt keyVal( 0 );

    // Get the back-up restore key
    const TInt error = RProperty::Get( KUidSystemCategory, conn::KUidBackupRestoreKey, keyVal );
    ASSERT( !error );

    if( !error )
        {
        // cast keyVal to TBURPartType
        const TBURPartType partType = static_cast< TBURPartType >( keyVal & KBURPartTypeMask );

        if( keyVal != 0 )
            {
            switch(partType)
                {
                case EBURRestoreFull:
                case EBURRestorePartial:
                    backupOrRestore = ETrue;
                    break;

                //case EBURUnset:
                //case EBURNormal:
                //case EBURBackupFull:
                //case EBURBackupPartial:
                default:
                    backupOrRestore = EFalse;
                    break;
                }
            }
        }
    TRACE_EXIT_POINT;
    return backupOrRestore;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void CAlarmUtils::GetAlarmLabelL(HBufC*& aText)
    {
    TRACE_ENTRY_POINT;
    iAlarmInfo->GetAlarmLabelL( iAlarmData.iAlarm,
                                aText, 
                                iAlarmData.iAlarmType );
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void CAlarmUtils::GetWakeupLabelL(HBufC*& aLabel)
    {
    TRACE_ENTRY_POINT;
    iAlarmInfo->GetWakeupLabelL( aLabel );
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
CNotifierDialogController* CAlarmUtils::NotifierDialogController()
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    return iNotifierDialogController;
    }

// ---------------------------------------------------------
// Checks if the alarm server has more expired alarms ready 
// to be shown.
// ---------------------------------------------------------
//
TBool CAlarmUtils::CheckHasExpiredAlarmsWaiting()
    {
    TRACE_ENTRY_POINT;
    TBool retVal( EFalse );
    const TInt err = iAlarmServer.Connect();

    if( !err )
        {
        RArray<TAlarmId> array;
        PIM_TRAPD_ASSERT( iAlarmServer.GetAlarmIdListByStateL( EAlarmStateWaitingToNotify, array ); )
        retVal = array.Count();
        array.Close();
        }
    iAlarmServer.Close();
    ASSERT( !err );
    TRACE_EXIT_POINT;
    return retVal;
    }

// ---------------------------------------------------------
// Update the alarm snooze count
// ---------------------------------------------------------
//
void CAlarmUtils::UpdateAlarmSnoozeCount()
    {
    TRACE_ENTRY_POINT;
    TInt& count = iAlarmData.iAlarm.ClientData1();

    if( count >= KAlmAlertMinSnooze && 
        count <= KAlmAlertMaxSnooze )
        {
        count--;
        }
    else
        {
        count = KAlmAlertMaxSnooze;
        }

    const TInt err = iAlarmServer.Connect();

    if( !err )
        {
        iAlarmServer.SetClientData( iAlarmData.iAlarm );
        }
    iAlarmServer.Close();
    ASSERT( !err );
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------
// Stop the active alarm and reset the snooze count.
// ---------------------------------------------------------
//
void CAlarmUtils::DoStopAlarm()
    {
    TRACE_ENTRY_POINT;
    // reset the snooze count
    iAlarmData.iAlarm.ClientData1() = 0;
    const TInt err = iAlarmServer.Connect();

    if( !err )
        {
        iAlarmServer.SetClientData( iAlarmData.iAlarm );
        }
    iAlarmServer.Close();
    ASSERT( !err );

    // check if alarm server has more expired alarms waiting
    iHasExpiredAlarmsWaiting = CheckHasExpiredAlarmsWaiting();

    // stop the alarm
    iSupervisor->CmdAcknowledgeAlarm();

    #if defined( RD_ALMALERT__SENSOR_SUPPORT )
    // notify the result through the context framework
    if( iCFSupport )
        {
	     if(!iAlarmControl->IsStopFromContext() && IsCalendarAlarm())
	     	{
	     	PIM_TRAPD_ASSERT( iCFSupport->PublishAlarmResultL( EResultAlarmStoppedAndExit );)
            iAlarmControl->SetStopFromContext(EFalse);
	     	}
	     else
	     	{
	     	PIM_TRAPD_ASSERT( iCFSupport->PublishAlarmResultL( EResultAlarmStopped );)
	     	iAlarmControl->SetStopFromContext(EFalse);
	     	}
        }
    #endif // RD_ALMALERT__SENSOR_SUPPORT
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------
// 
// ---------------------------------------------------------
//
TBool CAlarmUtils::HasExpiredAlarmsWaiting() const
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    return iHasExpiredAlarmsWaiting;
    }

// ---------------------------------------------------------
// Snooze the alarm 
// ---------------------------------------------------------
//
void CAlarmUtils::DoSnooze()
    {
    TRACE_ENTRY_POINT;
    /**
    * The snooze time can be shorter than iSnoozeTime because Alarm Server 
    * doesn't use seconds to count the new alarm time.  I.e. if the snooze time 
    * is 1 minute and 'snooze' is pressed at 12:05:50 the new alarm time will be 
    * 12:06:00.  So the actual snooze time will be only 10 seconds.  To prevent 
    * this kind of problems snooze time will be incremented by 1 minute when we
    * are less than 30 seconds away from the next minute.  The increment is done 
    * only when iSnoozeTime is 1.
    */
    TInt snoozeIncrement( 0 );

    if( iAlarmData.iSnoozeTime == 1 )
        {
        TTime now;  now.HomeTime();
        snoozeIncrement = now.DateTime().Second() > 30 ? 1 : 0;
        }

    // check if alarm server has more expired alarms waiting
    iHasExpiredAlarmsWaiting = CheckHasExpiredAlarmsWaiting();

    PIM_TRAPD_ASSERT( iSupervisor->CmdTaskAwayFromAlarmL( iAlarmData.iSnoozeTime + snoozeIncrement ); )

    #if defined( RD_ALMALERT__SENSOR_SUPPORT )
    // notify the result through the context framework
    if( iCFSupport )
        {
        PIM_TRAPD_ASSERT( iCFSupport->PublishAlarmResultL( EResultAlarmSnoozed ); )
        }
    #endif // RD_ALMALERT__SENSOR_SUPPORT
    TRACE_EXIT_POINT;
    }


// ---------------------------------------------------------
// Checks if this alarm can be shown in "alarm" or in 
// "charging" mode.
// ---------------------------------------------------------
//
TBool CAlarmUtils::IsWakeupAlarm() const
    {
    TRACE_ENTRY_POINT;    
    // has wakeup flag?
	// SSM specific changes.
#ifdef SYMBIAN_SYSTEM_STATE_MANAGEMENT
    const TBool retVal( iAlarmData.iAlarm.IsWakeup() );
#else
    const TBool retVal( iAlarmData.iAlarm.ClientFlags().IsSet( KWakeupAlarmFlagIndex ) );
#endif
    TRACE_EXIT_POINT;
    return retVal;
    }

// ---------------------------------------------------------
// 
// ---------------------------------------------------------
//
TBool CAlarmUtils::IsDeviceInAlarmState() const
    {
    TRACE_ENTRY_POINT;    
    const TBool retVal( iCurrentDeviceState == ESwStateAlarm || 
                        iCurrentDeviceState == ESwStateCharging );    
    TRACE_EXIT_POINT;
    return retVal;
    }

// ---------------------------------------------------------
// 
// ---------------------------------------------------------
//
TBool CAlarmUtils::IsDeviceInNormalState() const
    {
    TRACE_ENTRY_POINT;    
    const TBool retVal( iCurrentDeviceState == ESwStateNormalRfOn  || 
                        iCurrentDeviceState == ESwStateNormalRfOff || 
                        iCurrentDeviceState == ESwStateNormalBTSap );    
    TRACE_EXIT_POINT;
    return retVal;
    }

// ---------------------------------------------------------
// Check if the phone start-up is ready
// ---------------------------------------------------------
//
TBool CAlarmUtils::IsPhoneStartupReady() const
    {
    TRACE_ENTRY_POINT;
    const TBool retVal( IsDeviceInAlarmState() ||
                        IsDeviceInNormalState() );
    TRACE_EXIT_POINT;
    return retVal;
    }

// ---------------------------------------------------------
// Set the iCurrentDeviceState
// ---------------------------------------------------------
//
void CAlarmUtils::SetDeviceState(TPSGlobalSystemState aDeviceState)
    {
    TRACE_ENTRY_POINT;
    iCurrentDeviceState = aDeviceState;
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------
// 
// ---------------------------------------------------------
//
void CAlarmUtils::DeviceShutdown()
    {
    TRACE_ENTRY_POINT;
    
    // charging state added for the err EMDN-835CW2.
    TInt chargingState;
    RProperty::Get( KPSUidHWRMPowerState, KHWRMChargingStatus , chargingState );
    
    if( IsDeviceInAlarmState() && ( chargingState == EChargingStatusNotConnected ) )
        {       
        iShutdownTimer->Cancel();
        if( StarterConnect() )
            {
            iStarter.Shutdown();
            iStarter.Close();
            }    
        }
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------
// 
// ---------------------------------------------------------
//
void CAlarmUtils::DeviceStartup()
    {
    TRACE_ENTRY_POINT;    
    if( StarterConnect() )
        {
        PIM_ASSERT( iStarter.SetState( RStarterSession::ENormal ); )
        iStarter.Close();
        }  
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------
// Connect to the starter.
// ---------------------------------------------------------
TBool CAlarmUtils::StarterConnect()
    {
    TRACE_ENTRY_POINT;
    TInt err( KErrNone );    
    /**
    * Starter session's Connect() call can NOT be inside any 
    * function called during the class construction!
    * (NewL, ConstructL and all the functions called by these functions)
    * Calling Connect() during construction causes a deadlock!
    */
    err = iStarter.Connect();
    ASSERT( !err );    
    TRACE_EXIT_POINT;
    return( err == KErrNone );
    }

// ---------------------------------------------------------
// (re)start the shutdown timer.
// ---------------------------------------------------------
//
void CAlarmUtils::StartShutdownTimer(TUint aTimeout)
    {
    TRACE_ENTRY_POINT;
    // shutdown allowed only if the device is in alarm state
    if( IsDeviceInAlarmState() )
        {
        iShutdownTimer->Cancel();
        TCallBack shutdownCallback( ShutdownCallBack, this );
        iShutdownTimer->Start(
            TTimeIntervalMicroSeconds32( aTimeout ),
            TTimeIntervalMicroSeconds32( aTimeout ),
            shutdownCallback );
        }
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------
// Callback for the shutdown timer
// ---------------------------------------------------------
//
TInt CAlarmUtils::ShutdownCallBack(TAny* aPtr)
    {
    TRACE_ENTRY_POINT;
    static_cast<CAlarmUtils*>( aPtr )->DeviceShutdown();
    TRACE_EXIT_POINT;
    return 0;
    }

// ---------------------------------------------------------
// Cancel the shutdown timer
// ---------------------------------------------------------
//
void CAlarmUtils::CancelShutdown()
    {
    TRACE_ENTRY_POINT;
    iShutdownTimer->Cancel();
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------
// (re)start the auto-snooze timer.
// ---------------------------------------------------------
//
void CAlarmUtils::StartAutoSnoozeTimer()
    {
    TRACE_ENTRY_POINT;
    iAutoSnoozeTimer->Cancel();
    TCallBack autoSnoozeCallback( AutoSnoozeCallBack, this );
    TInt autoSnoozeTime;
	if(IsCalendarAlarm())
		{
		autoSnoozeTime = KAlarmAutoHideCalendar; // 30 Sec
		}
	else
		{
		autoSnoozeTime = KAlarmAutoHide; // 60 Sec
		}
	iAutoSnoozeTimer->Start(
        TTimeIntervalMicroSeconds32( autoSnoozeTime ),
        TTimeIntervalMicroSeconds32( autoSnoozeTime ),
        autoSnoozeCallback );
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------
// Callback for the auto-snooze timer.
// ---------------------------------------------------------
//
TInt CAlarmUtils::AutoSnoozeCallBack(TAny* aPtr)
    {
    TRACE_ENTRY_POINT;
    static_cast<CAlarmUtils*>( aPtr )->AutoSnooze();
    TRACE_EXIT_POINT;
    return 0;
    }

// ---------------------------------------------------------
// 
// ---------------------------------------------------------
//
void CAlarmUtils::AutoSnooze()
    {
    TRACE_ENTRY_POINT;
    iAutoSnoozeTimer->Cancel();
    iAlarmControl->DoAutoSnooze();
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------
// Cancel the snooze timer
// ---------------------------------------------------------
//
void CAlarmUtils::CancelAutoSnooze()
    {
    TRACE_ENTRY_POINT;
    iAutoSnoozeTimer->Cancel();
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------
// 
// ---------------------------------------------------------
//
void CAlarmUtils::StartAccessoryObserver()
    {
    TRACE_ENTRY_POINT;    
    PIM_TRAPD_ASSERT( iRemConHandler->StartL(); )    
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------
// 
// ---------------------------------------------------------
//
void CAlarmUtils::StopAccessoryObserver()
    {
    TRACE_ENTRY_POINT;    
    iRemConHandler->Stop();    
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------
// Publish new alarm context value and wait for any actions
// ---------------------------------------------------------
//
void CAlarmUtils::StartCFObserverL()
    {
    TRACE_ENTRY_POINT;
    #ifdef RD_ALMALERT__SENSOR_SUPPORT
    if( !iCFSupport )
        {
        /**
        * NOTE!
        * We shouldn't create the CCFClient before the 
        * CF server has been started by the starter. 
        */
        iCFSupport = CAlarmContextFwSupport::NewL( iAlarmControl );
        }
    iCFSupport->StartL( IsCalendarAlarm() ? EStateCalendarAlarm : 
                            IsClockAlarm() ? EStateClockAlarm : 
                                EStateOtherAlarm );
    #endif // RD_ALMALERT__SENSOR_SUPPORT
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------
// Stop observing context framework/sensors.
// ---------------------------------------------------------
//
void CAlarmUtils::StopCFObserverL()
    {
    TRACE_ENTRY_POINT;
    #ifdef RD_ALMALERT__SENSOR_SUPPORT
    if( iCFSupport )
        {
        iCFSupport->StopL();
        }
    #endif // RD_ALMALERT__SENSOR_SUPPORT
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------
// Try to snooze the active alarm.
// ---------------------------------------------------------
//
void CAlarmUtils::TryToSnoozeActiveAlarm()
    {
    TRACE_ENTRY_POINT;
    iAutoSnoozeTimer->Cancel();

    if( CanSnooze() )
        {
        UpdateAlarmSnoozeCount();
        DoSnooze();
        }
    else // max snooze count reached >> stop the alarm
        {
        DoStopAlarm();
        }
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------
// Ignore all key presses for [KKeyBlockTime] seconds.
// ---------------------------------------------------------
//
void CAlarmUtils::StartKeyBlocker()
    {
    TRACE_ENTRY_POINT;
    iKeyBlockTimer->Cancel();
    TCallBack keyBlockCallback( KeyBlockCallBack, iKeyBlockTimer );
    iKeyBlockTimer->Start(
        TTimeIntervalMicroSeconds32( KKeyBlockTime ),
        TTimeIntervalMicroSeconds32( KKeyBlockTime ),
        keyBlockCallback );
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------
// Callback function for the key block timer
// ---------------------------------------------------------
//
TInt CAlarmUtils::KeyBlockCallBack(TAny* aPtr)
    {
    TRACE_ENTRY_POINT;
    static_cast<CPeriodic*>( aPtr )->Cancel();
    TRACE_EXIT_POINT;
    return 0;
    }

// ---------------------------------------------------------
// 
// ---------------------------------------------------------
//
TBool CAlarmUtils::KeyBlockActive()
    {
    TRACE_ENTRY_POINT;
    const TBool retVal( iKeyBlockTimer->IsActive() );
    TRACE_EXIT_POINT;
    return retVal;
    }

// ---------------------------------------------------------
// 
// ---------------------------------------------------------
//
void CAlarmUtils::AsyncShowSnoozeInfoNote()
    {
    TRACE_ENTRY_POINT;
    iAsyncCallBack->Cancel();
    iAsyncCallBack->Set( TCallBack( SnoozeInfoCallBack, iAlarmControl ) );
    iAsyncCallBack->CallBack();
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------
// Callback function for the auto snooze timer
// ---------------------------------------------------------
//
TInt CAlarmUtils::SnoozeInfoCallBack(TAny* aPtr)
    {
    TRACE_ENTRY_POINT;
    PIM_TRAPD_ASSERT( static_cast<CAlmAlarmControl*>( aPtr )->ShowSnoozeInfoNoteL(); )
    TRACE_EXIT_POINT;
    return 0;
    }

// ---------------------------------------------------------
// 
// ---------------------------------------------------------
//
void CAlarmUtils::AsyncShowWakeupQuery()
    {
    TRACE_ENTRY_POINT;
    iAsyncCallBack->Cancel();
    iAsyncCallBack->Set( TCallBack( WakeupCallBack, this ) );
    iAsyncCallBack->CallBack();
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------
// 
// ---------------------------------------------------------
//
TInt CAlarmUtils::WakeupCallBack(TAny* aPtr)
    {
    TRACE_ENTRY_POINT;
    static_cast<CAlarmUtils*>( aPtr )->DoAskWakeup();
    TRACE_EXIT_POINT;
    return 0;
    }

// ---------------------------------------------------------
// Request wakeup dialog. Shutdown on error.
// ---------------------------------------------------------
//
void CAlarmUtils::DoAskWakeup()
    {
    TRACE_ENTRY_POINT;
    TRAPD( err, iAlarmControl->AskWakeupPhoneL(); )
    ASSERT( !err );

    if( err ) // shutdown if the query failed
        {
        StartShutdownTimer( KShutdownTime );
        }
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------
// 
// ---------------------------------------------------------
//
void CAlarmUtils::CancelAsynchRequest()
    {
    TRACE_ENTRY_POINT;
    iAsyncCallBack->Cancel();
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------
// Delay alarm for KAlarmDelayTime
// ---------------------------------------------------------
//
void CAlarmUtils::StartAlarmDelayTimer()
    {
    TRACE_ENTRY_POINT;
    iAlarmDelayTimer->Cancel();
    TCallBack delayTimerCallback( DelayTimerCallBack, this );
    iAlarmDelayTimer->Start(
        TTimeIntervalMicroSeconds32( KAlarmDelayTime ),
        TTimeIntervalMicroSeconds32( KAlarmDelayTime ),
        delayTimerCallback );
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------
// Callback function for the key block timer
// ---------------------------------------------------------
//
TInt CAlarmUtils::DelayTimerCallBack(TAny* aPtr)
    {
    TRACE_ENTRY_POINT;
    CAlarmUtils* alarmUtils = static_cast<CAlarmUtils*>( aPtr );
    alarmUtils->iAlarmDelayTimer->Cancel();
    alarmUtils->iAlarmControl->ShowAlarm();
    TRACE_EXIT_POINT;
    return 0;
    }

// ---------------------------------------------------------
// Reset system inactivity timers periodically.
// ---------------------------------------------------------
//
void CAlarmUtils::StartInactivityResetTimer()
    {
    TRACE_ENTRY_POINT;
    User::ResetInactivityTime();
    iInactivityResetTimer->Cancel();
    TCallBack inactivityResetCallback( InactivityResetTimerCallBack, this );
    iInactivityResetTimer->Start(
        TTimeIntervalMicroSeconds32( KInactivityResetInterval ),
        TTimeIntervalMicroSeconds32( KInactivityResetInterval ),
        inactivityResetCallback );
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------
// Callback function for the inactivity reset timer.
// ---------------------------------------------------------
//
TInt CAlarmUtils::InactivityResetTimerCallBack(TAny* /*aPtr*/)
    {
    TRACE_ENTRY_POINT;
    User::ResetInactivityTime();
    TRACE_EXIT_POINT;
    return 1;
    }

// ---------------------------------------------------------
// Reset system inactivity timers periodically.
// ---------------------------------------------------------
//
void CAlarmUtils::StopInactivityResetTimer()
    {
    TRACE_ENTRY_POINT;
    iInactivityResetTimer->Cancel();
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------
// 
// ---------------------------------------------------------
//
TBool CAlarmUtils::AlarmDelayTimerActive()
    {
    TRACE_ENTRY_POINT;
    const TBool retVal( iAlarmDelayTimer->IsActive() );
    TRACE_EXIT_POINT;
    return retVal;
    }

// ---------------------------------------------------------
// Fetch local uid and instance time for the alarmed calendar entry.
// ---------------------------------------------------------
//
void CAlarmUtils::GetCalendarEntryInfoL()
    {
    TRACE_ENTRY_POINT;
    iAlarmData.iInstanceTime = TCalTime();
    iAlarmData.iLocalUid = 0;

    if( !IsCalendarAlarm() )
        {
        TRACE_EXIT_POINT;
        return;
        }
    
    TInt err = KErrNone;
    
    //Retriving alarm data from alarm server.
    err = iAlarmServer.Connect();
    User::LeaveIfError( err );
    HBufC8* alarmData = NULL;
    err = iAlarmServer.GetAlarmData( iAlarmData.iAlarm.Id(), alarmData );
    CleanupStack::PushL( alarmData );
    iAlarmServer.Close();
    User::LeaveIfError( err );
    
    // get entry data
    CCalEntryId* entryId = CCalEntryId::NewL( alarmData );
    CleanupStack::Pop( alarmData ); // ownership transferred to CCalEntryId
    CleanupStack::PushL( entryId );

    // instance time
    iAlarmData.iInstanceTime = entryId->InstanceTimeL();

    // local UID
    iAlarmData.iLocalUid = entryId->LocalUidL();
	
	// Get the calendar db name.
	iAlarmData.iCalFileName = entryId->StoreFileNameL();

	// Creating session with Calendar server
	if(!iCalSession)
		{
		iCalSession = CCalSession::NewL();
		TRAP( err, iCalSession->OpenL( iAlarmData.iCalFileName ) );
		User::LeaveIfError( err );

		// active scheduler wait object to wait for Calendar view creation
		iEntryAsyncWait = new(ELeave) CActiveSchedulerWait();
		
		// active scheduler wait object to stop new view creation req. while one is ongoing.
		iQueueManagerAsw = new(ELeave) CActiveSchedulerWait();
		}

	if(!iCalEntryView && !(iEntryAsyncWait->IsStarted()) && iCalSession)
		{
		
		// Creating Calendar Entry View for retrieving event details.
	    iCalEntryView = CCalEntryView::NewL( *iCalSession, *this );
		SetCalendarViewStatus(ECalViewCreateReqPending);
		
		// Since View creation is aynshronous,the thread need to wait till view creation completed
		iEntryAsyncWait->Start();

		// If Async wait  queue manager is still in in waiting,Cancel it since view creation completed.
		if(iQueueManagerAsw->IsStarted())
			{
			iQueueManagerAsw->AsyncStop();
			}
		
		}
	else
		{	
		// Waiting for completion of current view creation request.
		if(iQueueManagerAsw && !iQueueManagerAsw->IsStarted())
			{
			iQueueManagerAsw->Start();
			}
		}
	// Leaves if view creation failed...
	User::LeaveIfError(CalendarViewStatus());
	
	CleanupStack::PopAndDestroy( entryId );
    
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// Fetch the alarmed entry from calendar database
// ----------------------------------------------------------------------------
//
void CAlarmUtils::FetchEntryL(CCalEntry*& aEntry)
    {
    TRACE_ENTRY_POINT;
	if(CalendarViewStatus()== ECalViewCreateCompleted&& NULL!=iCalEntryView)
		{
		aEntry = iCalEntryView->FetchL( iAlarmData.iLocalUid );
		User::LeaveIfNull(aEntry);
		}
	else
		{
		// leaving if Calender Entry view is not available.
		User::Leave(KErrNotFound);
		}

    if( aEntry )
        {
        // check if the entry still has an alarm
        // (prevent deleted snoozed alarms)
        //
        CCalAlarm* alarm = NULL;
        TRAPD( err, alarm = aEntry->AlarmL(); )

        // ignore and stop the alarm (handled in CAlmAlarmControl) if
        // the alarm was removed from the calendar entry
        if( !alarm || err != KErrNone )
            {
            delete aEntry;
            aEntry = NULL;
            }
        delete alarm;
        }

    if( !aEntry )
        {
        // calendar entry not found...stop the alarm
        User::Leave( KErrNotFound );  // trapped in CAlmAlarmControl
        }
    TRACE_EXIT_POINT;
    }


// ----------------------------------------------------------------------------
//From MCalProgressCallBack . Notifies the progress percentage of  completion of 
//calendar entry view creation
// ----------------------------------------------------------------------------
//

void CAlarmUtils::Progress(TInt /* aPercentageCompleted */)
{
    TRACE_ENTRY_POINT;
	// Do Nothing
	TRACE_EXIT_POINT;

}

// ----------------------------------------------------------------------------
//From MCalProgressCallBack . Called  on completion of calendar entry view creation
// ----------------------------------------------------------------------------
//
void CAlarmUtils::Completed(TInt aError)
	{
	TRACE_ENTRY_POINT;

	if(iEntryAsyncWait->IsStarted())
	{
		iEntryAsyncWait->AsyncStop();
	}

	if(iQueueManagerAsw->IsStarted())
	{
		iQueueManagerAsw->AsyncStop();
	}
		
	
	if(KErrNone == aError)
	{
		SetCalendarViewStatus(ECalViewCreateCompleted);
	}
	else
	{
		SetCalendarViewStatus(ECalViewCreateError);
	}

	TRACE_EXIT_POINT;
	}


// ----------------------------------------------------------------------------
//From MCalProgressCallBack . Called  on compltetion of calendar entry view creation
// ----------------------------------------------------------------------------
//

TBool CAlarmUtils::NotifyProgress()
	{
    TRACE_ENTRY_POINT;
	// Don nothing
	TRACE_EXIT_POINT;
	return EFalse;

	}


// ----------------------------------------------------------------------------
// Sets the Calendar view creation status.
//
void CAlarmUtils::SetCalendarViewStatus(TInt aStatus)
	{
	TRACE_ENTRY_POINT;

	iCalEntryViewStatus = aStatus ;
	
	TRACE_EXIT_POINT;
	}

// ----------------------------------------------------------------------------
//Gets the Calendar view creation status.
// ----------------------------------------------------------------------------
TInt CAlarmUtils::CalendarViewStatus()
	{
	TRACE_ENTRY_POINT;	
	TRACE_EXIT_POINT;
	return iCalEntryViewStatus;
	}

// ---------------------------------------------------------
// Launch calendar into alarm viewer mode.
// ---------------------------------------------------------
//
void CAlarmUtils::StartCalendarL()
    {
    TRACE_ENTRY_POINT;
    
    iCalendarAlarmViewer = ETrue;
    
    CalenLauncher::ViewEntryL( iAlarmData.iLocalUid,
                               iAlarmData.iInstanceTime, iAlarmData.iCalFileName,
                               CanSnooze() ? CalenLauncher::EAlarmViewer : 
                                             CalenLauncher::EAlarmViewerNoSnooze );
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------
// Check if the security lock is active.
// ---------------------------------------------------------
//
TBool CAlarmUtils::IsSecurityLockActive()
    {
    TRACE_ENTRY_POINT;
    TInt keyVal( 0 );
    TInt currentBootupQueriesStatus(0);
    TBool retVal( EFalse );

    PIM_ASSERT( RProperty::Get( KPSUidCoreApplicationUIs, KCoreAppUIsAutolockStatus, keyVal ); )
    PIM_ASSERT(RProperty::Get(KPSUidStartup,KPSStartupUiPhase,currentBootupQueriesStatus ); )
    
    if( (keyVal > EAutolockOff) || (currentBootupQueriesStatus != EStartupUiPhaseAllDone ) )
        {
        retVal = ETrue;
        }
    TRACE_EXIT_POINT;
    return retVal;
    }

// ---------------------------------------------------------
// Set the status for calendar alarm viewer.
// ---------------------------------------------------------
//    
void CAlarmUtils::SetCalendarAlarmViewer(TBool aCalendarAlarmViewer)
    {
    iCalendarAlarmViewer = aCalendarAlarmViewer ;
    }

// ---------------------------------------------------------
// Get the status whether calendar viewer is open or not.
// ---------------------------------------------------------
//
TBool CAlarmUtils::IsCalendarAlarmViewer()
    {
    return iCalendarAlarmViewer;
    }

// ---------------------------------------------------------
// Silence the notifying alarm 
// ---------------------------------------------------------
//
void CAlarmUtils::DoSilence()
    {
    TRACE_ENTRY_POINT;
    
    // silence only if snoozing is possible
    // this way user must hear the last "call"
    if( CanSnooze() )
        {
        delete iAlarmPlayer;
        iAlarmPlayer = NULL;
        SetBackLight( EFalse );
        
        if( IsCalendarAlarm() )
            {
    
            // calendar alarm needs extra handling due to having stop - silence
            //simulate right softkey pressing
            RWsSession wsSession=CCoeEnv::Static()->WsSession();
            TKeyEvent keyEvent;
            keyEvent.iCode = EKeyCBA2;  
            keyEvent.iScanCode = EStdKeyDevice1;
            keyEvent.iModifiers = 0;
            keyEvent.iRepeats = 0;
            wsSession.SimulateKeyEvent( keyEvent );
            wsSession.Flush();
            }
        // clockalarm
        else
            {
            StartAutoSnoozeTimer();
            }
    
        #if defined( RD_ALMALERT__SENSOR_SUPPORT )
        // notify the result through the context framework
        if( iCFSupport )
            {
            PIM_TRAPD_ASSERT( iCFSupport->PublishAlarmResultL( EResultAlarmSilenced ); )
            }
        #endif // RD_ALMALERT__SENSOR_SUPPORT
        }
    TRACE_EXIT_POINT;     
    }

// ---------------------------------------------------------
// Check if the alarm is duplicate calendar alarm
// Rest of the details in header file
// ---------------------------------------------------------
//
TBool CAlarmUtils::CheckForDuplicateAlarm()
    {
	TRACE_ENTRY_POINT;
    TBool ret = EFalse;
    if(IsCalendarAlarm())
        {
        TTime currAlarmOrigExpTime = iAlarmData.iAlarm.OriginalExpiryTime();

        if(currAlarmOrigExpTime == iPrevAlarmOriginalExpiryTime && iPrevLocalUid == iAlarmData.iLocalUid 
			&& !iPrevCalFileName.CompareF(iAlarmData.iCalFileName) )
            {    
            TInt& count = iAlarmData.iAlarm.ClientData1();
            if( (count >= KAlmAlertMinSnooze && 
                count <= KAlmAlertMaxSnooze ))
                {
                ret = ETrue;
                }
            }
        }
	TRACE_EXIT_POINT;
    return ret;
    }


// ---------------------------------------------------------
// Store details of current alarm for future comparison when next calendar alarm comes.
// Rest of the details in header file
// ---------------------------------------------------------
//
void CAlarmUtils::StoreCurrentCalendarAlarmData()
	{
	TRACE_ENTRY_POINT;
	iPrevLocalUid					= iAlarmData.iLocalUid;  
    iPrevCalFileName 				= iAlarmData.iCalFileName;
	iPrevAlarmOriginalExpiryTime 	= iAlarmData.iAlarm.OriginalExpiryTime();
	TRACE_EXIT_POINT;
	}
	
// End of File
