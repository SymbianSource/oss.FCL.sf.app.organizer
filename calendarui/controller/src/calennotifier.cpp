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
* Description:  Calendar notifier
*
*/



#include <aknappui.h>                 // iavkonappui macro
#include <bacntf.h>                   // cenvironmentchangenotifier
#include <coemain.h>                  // eactiveprioritylogona
#include <centralrepository.h>        // crepository
#include <ErrorUI.h>                  // cerrorui
#ifndef SYMBIAN_ENABLE_SPLIT_HEADERS // new 
#include <asshdalarm.h>
#else // new
#include <asshdalarm.h>
#include <ASShdAlarmCal.h>
#endif // new
#include <e32property.h>
#include <calfilechangenotification.h>
#include <calenecomwatcher.h>
#include <calenglobaldata.h>
#include <calenconstants.h>
#include <calencontext.h>
#include <calsession.h>
#include <calcalendarinfo.h>
#include <calenmulticaluids.hrh>

#include "calendarui_debug.h"
#include "calennotifier.h"            // CCalenNotifier
#include "CalendarPrivateCRKeys.h"    // Central Repository keys
#include "calensetting.h"
#include "calenstatemachine.h"
#include "calencontroller.h"

const TInt KHashLength = 64;
const TInt KBuffLength = 24;

// ----------------------------------------------------------------------------
// CCalenNotifier::CCalenNotifier
// C++ default constructor.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
CCalenNotifier::CCalenNotifier( CCalenController& aController )
    : iController( aController )
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenNotifier::~CCalenNotifier
// Destructor.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
CCalenNotifier::~CCalenNotifier()
    {
    TRACE_ENTRY_POINT;

    // Stop database change notifications.
    delete iDbChangeNotifier;

    // Stop ECom change notifications
    delete iEComWatcher;

    // Release the global data
    if( iGlobalData )
        {
        iGlobalData->Release();
        }

    if( iSetting )
        {
        iSetting->Release();
        }

    // Stop environment change notifications
    if( iEnvChangeNotifier )
        {
        iEnvChangeNotifier->Cancel();
        delete iEnvChangeNotifier;
        }

    // Stop settings change notifications
    delete iCenRepChangeNotifier;
    delete iRepository;

    // Reset the handler array.
    // Before we reset , close hashset for each handler
    for(TInt i = 0 ; i < iHandlers.Count() ; i++)
        {
        iHandlers[i].iHashSet.Close();
        }
    
    iHandlers.Reset();
    iBroadcastQueue.Reset();

	// stop listening for calendar file change notifications
	iGlobalData->CalSessionL().StopFileChangeNotification();
	
	iAsyncCallback->Cancel();
	delete iAsyncCallback;
	
	delete iFilnameDeleted;
	
	TRACE_EXIT_POINT;
	}

// ----------------------------------------------------------------------------
// CCalenNotifier::ConstructL
// Symbian 2nd phase of construction.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenNotifier::ConstructL()
    {
    TRACE_ENTRY_POINT;
    
    // Get the global data
    iGlobalData = CCalenGlobalData::InstanceL();
    
    // Get the setting singleton. We update it when settings change.
    iSetting = CCalenSetting::InstanceL();
    
    // Register for system environment changes
    TCallBack envCallback( EnvChangeCallbackL, this );
    iEnvChangeNotifier =
        CEnvironmentChangeNotifier::NewL( EActivePriorityLogonA, envCallback );
    iEnvChangeNotifier->Start();
    
    // Register for changes to Calendar settings from the Central Repository
    iRepository = CRepository::NewL( KCRUidCalendar );
    iCenRepChangeNotifier = CCenRepNotifyHandler::NewL( *this, *iRepository );
    iCenRepChangeNotifier->StartListeningL();
    
    // Register for changes to our database session
    iDbChangeNotifier = CCalenDbChangeNotifier::NewL( *iGlobalData );
    iDbChangeNotifier->RegisterObserverL( *this );
  
    // Register for changes to the ECom registry
    iEComWatcher = CCalenEComWatcher::NewL( *this );
     
	iIgnoreFirstLocaleChange = ETrue;

	// start listening for calendar file change notifications
	iGlobalData->CalSessionL().StartFileChangeNotificationL(*this);
	
	TCallBack callback(CCalenNotifier::AsyncRemoveCalendarL,this);
	iAsyncCallback = new(ELeave) CAsyncCallBack(callback,CActive::EPriorityStandard);
	
	iFilnameDeleted = NULL;

	TRACE_EXIT_POINT;
	}

// ----------------------------------------------------------------------------
// CCalenNotifier::RegisterForNotificationsL
// Adds the passed handler to the handler array.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenNotifier::RegisterForNotificationsL( MCalenNotificationHandler* aHandler, 
                                                TCalenNotification aNotification)
    {
    TRACE_ENTRY_POINT;

    TNotificationHandler handler;
    handler.iHandler = aHandler;
    
    //Prepare hash
    handler.iHashSet.ReserveL(KHashLength);
    
    if(ECalenNotifyAll == aNotification)
        {
        //ECalenNotifyAll indicates that all notifications to be registered
        //iterate through TCalenNotification enum and add the notifications to iHashSet
         
        for(TCalenNotification notificationIter = TCalenNotification(ECalenNotifyAll + 1); 
                                notificationIter < ECalenNotifyLast; 
                                notificationIter = TCalenNotification(notificationIter + 1))
            {
            handler.iHashSet.InsertL(notificationIter);
            }
        }
    else
        {
        //Add only single notification.
        handler.iHashSet.InsertL(aNotification);
        }
    
    iHandlers.Append( handler );
    
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenNotifier::RegisterForNotificationsL
// Adds the passed handler to the handler array.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenNotifier::RegisterForNotificationsL( MCalenNotificationHandler* aHandler, 
                                                           RArray<TCalenNotification>& aNotifications  )
    {
    TRACE_ENTRY_POINT;

    TNotificationHandler handler;
    handler.iHandler = aHandler;
    
    //Prepare hash
    handler.iHashSet.ReserveL(KHashLength);
    
    if(aNotifications.Find(ECalenNotifyAll) != KErrNotFound)
        {
        //ECalenNotifyAll indicates that all notifications to be registered
        //If ECalenNotifyAll is found in aNotifications along with other notifications
        //iterate through TCalenNotification enum and add the notifications to iHashSet
        for(TCalenNotification notificationIter = TCalenNotification(ECalenNotifyAll + 1); 
                                    notificationIter < ECalenNotifyLast; 
                                    notificationIter = TCalenNotification(notificationIter + 1))
            {
            handler.iHashSet.InsertL(notificationIter);
            }
        }
    else
        {
        //Add notifocations from Array(aNotifications) into iHashSet member 
        for(TInt i = 0 ; i < aNotifications.Count() ; i++)
            {
            handler.iHashSet.InsertL(aNotifications[i]);
            }
        }
    
    iHandlers.Append( handler );

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenNotifier::CancelNotifications
// Removes the passed handler from the handler array.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenNotifier::CancelNotifications( MCalenNotificationHandler* aHandler )
    {
    TRACE_ENTRY_POINT;

    for( TInt x = 0; x < iHandlers.Count(); ++x )
        {
        if( iHandlers[x].iHandler == aHandler )
            {
            // Mark the notification for deletion by
            // settings the handler to NULL. Actual deletion
            // will take place in DoBroadcast
            iHandlers[x].iHashSet.Close();
            iHandlers[x].iHandler = NULL;
            TRACE_EXIT_POINT;
            return;
            }
        }

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
//  CCalenNotifier::EnvChangeCallbackL
//  CEnvironmentChangeNotifier callback.  Calendar is only interested in:
//  EChangesLocale              - System locale changed
//  EChangesMidnightCrossover   - System time passed midnight
//  EChangesSystemTime          - System time changed
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
TInt CCalenNotifier::EnvChangeCallbackL( TAny* aThisPtr )
    {
    TRACE_ENTRY_POINT;

  /*  CCalenNotifier* thisPtr = static_cast<CCalenNotifier*>( aThisPtr );

    if( thisPtr->iEnvChangeNotifier->Change() & EChangesMidnightCrossover )
        {
        thisPtr->BroadcastNotification( ECalenNotifySystemTimeChanged );
        }

    if( thisPtr->iEnvChangeNotifier->Change() & EChangesLocale )
        {
        thisPtr->BroadcastNotification( ECalenNotifySystemLocaleChanged );
        }

    if( thisPtr->iEnvChangeNotifier->Change() & EChangesSystemTime ) 
        {
        thisPtr->BroadcastNotification( ECalenNotifySystemTimeChanged );
        }*/
        
    TRACE_EXIT_POINT;
    // Return value for functions used as TCallBack objects should be EFalse
    // unless the function is intended to be called again from a timer.
   // return EFalse;
    return static_cast<CCalenNotifier*>(aThisPtr)->DoEnvChange();
    }

// ----------------------------------------------------------------------------
//  CCalenNotifier::DoEnvChange
//  EnvChangeCallbackL calls this function
// ----------------------------------------------------------------------------
//
TInt CCalenNotifier::DoEnvChange()
	{
	TRACE_ENTRY_POINT;
	
	if( ((iEnvChangeNotifier->Change() & EChangesMidnightCrossover)
		|| (iEnvChangeNotifier->Change() & EChangesSystemTime))
		&& !iIgnoreFirstLocaleChange )
        {
        BroadcastNotification( ECalenNotifySystemTimeChanged );
        }
	else if( (iEnvChangeNotifier->Change() & EChangesLocale)
			&& !iIgnoreFirstLocaleChange )	
        {
        BroadcastNotification( ECalenNotifySystemLocaleChanged );
        }
     else
     	{
     	iIgnoreFirstLocaleChange = EFalse;
     	}   

    TRACE_EXIT_POINT; 
    return EFalse ;
	}
// ----------------------------------------------------------------------------
// CCalenNotifier::HandleNotifyGeneric
// From MCenRepNotifyHandlerCallback
// Generic notification that one of our central repository keys has changed
// If any keys change we broadcast a settings changed notification
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenNotifier::HandleNotifyGeneric( TUint32 /*aId*/ )
    {
    TRACE_ENTRY_POINT;

    PIM_TRAPD_HANDLE( iSetting->LoadL() );
    BroadcastNotification( ECalenNotifySettingsChanged );

    // Use another trap to make sure we start listening again, regardless
    // of whether the previous function left or not.
    PIM_TRAPD_HANDLE( iCenRepChangeNotifier->StartListeningL() );

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenNotifier::HandleNotifyError
// Cenrep watcher error callback
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenNotifier::HandleNotifyError( TUint32 /*aId*/,
                                                    TInt /*aError*/,
                                                    CCenRepNotifyHandler* /*aHandler*/ )
    {
    TRACE_ENTRY_POINT;

    PIM_TRAPD_HANDLE( iCenRepChangeNotifier->StartListeningL() );

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenNotifier::HandleDBChangeL
// From MCalenDBChangeObserver
// Notification that an external CCalSession has modified the database we are
// using in some way.  This notification is limited to a maximum of one per
// second.  This is to avoid multiple notifications when performing large sync
// operations
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenNotifier::HandleDBChangeL()
    {
    TRACE_ENTRY_POINT;

    BroadcastNotification( ECalenNotifyExternalDatabaseChanged );

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenNotifier::ContextChanged
// From MCalenContextChangeObserver. Called when the context changes.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenNotifier::ContextChanged()
    {
    TRACE_ENTRY_POINT;

    BroadcastNotification( ECalenNotifyContextChanged );

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenNotifier::EComChanged
// From MCalenEComChangeObserver. Called when the ECom registry changes 
// (install/uninstall).
// ----------------------------------------------------------------------------
//
void CCalenNotifier::EComChanged()
    {
    TRACE_ENTRY_POINT;
    
    if(!iController.IsFasterAppFlagEnabled())
        {
        BroadcastNotification( ECalenNotifyEComRegistryChanged );
        }
    
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenNotifier::DeferSettingsNotifications
// After calling this function, any settings changed notifications
// will not be broadcast until after ResumeSettingsNotifications
// has been called.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenNotifier::DeferSettingsNotifications()
    {
    TRACE_ENTRY_POINT;

    iIsSettingsBroadcastDeferred = ETrue;

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenNotifier::ResumeSettingsNotifications
// Resumes settings notifications after they have been paused
// with DeferSettingsNotifications.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenNotifier::ResumeSettingsNotifications()
    {
    TRACE_ENTRY_POINT;

    iIsSettingsBroadcastDeferred = EFalse;

    if( iSettingsNeedsBroadcast )
        {
        iSettingsNeedsBroadcast = EFalse;
        BroadcastNotification( ECalenNotifySettingsChanged );
        }

    if( iLocaleNeedsBroadcast )
        {
        iLocaleNeedsBroadcast = EFalse;
        BroadcastNotification( ECalenNotifySystemLocaleChanged );
        }

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenNotifier::BroadcastNotification
// Issues a notification to all registered handlers
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenNotifier::BroadcastNotification( TCalenNotification aNotification )
    {
    TRACE_ENTRY_POINT;

    // Someone has told us to broadcast, or one of our notifiers completed.
    // We run it past the state machine and that may or may not call the
    // function to really do the broadcast.
    iController.StateMachine().HandleNotification( aNotification );
    
    TRACE_EXIT_POINT;
    }
    
// ----------------------------------------------------------------------------
// CCalenNotifier::BroadcastApprovedNotification
// Issues a notification to all registered handlers
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenNotifier::BroadcastApprovedNotification( TCalenNotification aNotification )
    {
    TRACE_ENTRY_POINT;
    /*if ( aNotification == ECalenNotifySettingsChanged
            &&  iIsSettingsBroadcastDeferred )
        {
        iSettingsNeedsBroadcast = ETrue;
        }
    else if ( aNotification == ECalenNotifySystemLocaleChanged 
                && iIsSettingsBroadcastDeferred)
        {
        iLocaleNeedsBroadcast = ETrue;
        }
    else*/
        {
        iBroadcastQueue.Append( aNotification );

        if( !iBroadcastActive )
            {
            iBroadcastActive = ETrue;
            while( iBroadcastQueue.Count() )
                {
                TCalenNotification notification = iBroadcastQueue[0];
                DoBroadcast( notification );
                iBroadcastQueue.Remove( 0 );
                }
            iBroadcastActive = EFalse;
            }
        }

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenNotifier::DoBroadcast
// Issues a notification to all registered handlers
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenNotifier::DoBroadcast( TCalenNotification aNotification )
    {
    TRACE_ENTRY_POINT;

    for( TInt x = 0; x < iHandlers.Count(); ++x )
        {
        TNotificationHandler handler = iHandlers[x];
        if( handler.iHandler )
            {
             if( handler.iHashSet.Find(aNotification) )
                {
                handler.iHandler->HandleNotification( aNotification );
                }
            }
        else
            {
            // The handler has been marked for deletion
            iHandlers.Remove( x ); // remove the entry
            --x; // decrement the index.
            }
        }

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenNotifier::Progress
// From MCalProgressCallback. Intentionally empty.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenNotifier::Progress( TInt /*aPercentageCompleted*/ )
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenNotifier::NotifyProgress
// From MCalProgressCallback. Don't notify us about progress updates.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
TBool CCalenNotifier::NotifyProgress()
    {
    TRACE_ENTRY_POINT;

    BroadcastNotification( ECalenNotifyViewCreationStarted );

    TRACE_EXIT_POINT;
    return EFalse;
    }

// ----------------------------------------------------------------------------
// CCalenNotifier::Completed
// From MCalProgressCallback.
// Notifies observer of completion
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenNotifier::Completed( TInt aStatus )
    {
    TRACE_ENTRY_POINT;

    if( aStatus == KErrNone )
        {
        BroadcastNotification( ECalenNotifyEntryInstanceViewCreated );
        }
    else
        {
        BroadcastNotification( ECalenNotifyEntryInstanceViewCreationFailed );
        // The view creation has failed, hence the calendar
        // application needs to close gracefully
        // 1) Display error note.
        
        CErrorUI* errorUi;
        TRAPD(error,errorUi = CErrorUI::NewLC();
       if(error!=KErrNone)
       		{
    		// do avoid warning
    		}        
        errorUi->ShowGlobalErrorNoteL( aStatus );        
        CleanupStack::PopAndDestroy( errorUi );
        );

        // Exit application
        iAvkonAppUi->Exit();
        }

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenNotifier::SystemTimeChangedL
// Check if the system time changed since Calendar was last launched
// If the system time did change, we need to notify the user that alarms may
// have been missed.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
TInt CCalenNotifier::SystemTimeChangedL()
    {
    TRACE_ENTRY_POINT;
    
    TBool timeZoneChanged(EFalse);

    TPckgBuf<TMissedAlarmPubSubData> alarmPkgVarBuf;
    TInt errorVal = RProperty::Get( KAlarmServerPubSubCategory, 
                            KMissingAlarmPubSubKey, alarmPkgVarBuf);

    if(errorVal != KErrNone)
        {
        // Error in accessing the P&S key.
        // Alarm server defines this key when first time SystemTime Changes after bootup.
        // But Calendar may try to access this before it is defined by Alarm server.
        // So better not leaving based on errorVal
        return timeZoneChanged;
        }
    
    // read the latest timechange from agenda Server Time Stamp
    TTime timeOfChangeUtc = alarmPkgVarBuf().iTimeOfChangeUtc;
    //timeOfChangeUtc.RoundUpToNextMinute();
    iTimeOfChangeUtcReal = I64REAL(timeOfChangeUtc.Int64());

    // read the persistent time stamp from CalendarInternalCRKeys
    TReal previousTimeOfChange = 1.0;
    CRepository* repository = CRepository::NewL( KCRUidCalendar );
    CleanupStack::PushL( repository );
    errorVal = repository->Get( KCalendarPersistentTime, previousTimeOfChange );
    User::LeaveIfError( errorVal );    
    
    TInt tzChangedOrAlarmsMissed(0);
    // compare the times. If the time set in the PubSub key by the Alarm Server is 
    // greater than the last time we looked at it, we will show 1 of the 2 info notes 
    // to the user.
    if (iTimeOfChangeUtcReal != previousTimeOfChange)
        {
        // Agenda Server set this value to tell what has happened since
        // the time change
        tzChangedOrAlarmsMissed = alarmPkgVarBuf().iValue;
        }
    CleanupStack::PopAndDestroy( repository );
       
    TRACE_EXIT_POINT;
    return tzChangedOrAlarmsMissed;    
    }

// ----------------------------------------------------------------------------
// CCalenNotifier::UpdateSytemTimeChangeInfoL
// Update cenrep with latest system time change info
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenNotifier::UpdateSytemTimeChangeInfoL()
    {
    TRACE_ENTRY_POINT;
    
    CRepository* repository = CRepository::NewL( KCRUidCalendar );
    CleanupStack::PushL( repository );
        
    // Update the persistent time stamp to the time stamp 
    // indicated by the agenda server
    TInt errorVal = repository->Set( KCalendarPersistentTime, iTimeOfChangeUtcReal);
    User::LeaveIfError( errorVal );
    CleanupStack::PopAndDestroy( repository );
    
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenNotifier::TNotificationHandler()
// TNotificationHandler contructor
// ----------------------------------------------------------------------------
CCalenNotifier::TNotificationHandler::TNotificationHandler() : 
                iHashSet(&::HashCalenNotificationFunction,&::HashCalenNotificationIdentityRelation)
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenNotifier::CalendarInfoChangeNotificationL()
// Handle calendar file change notifications
// ----------------------------------------------------------------------------
void CCalenNotifier::CalendarInfoChangeNotificationL( 
        RPointerArray<CCalFileChangeInfo>& aCalendarInfoChangeEntries)
	{
	TRACE_ENTRY_POINT;

	// get the file change count
	TInt calenInfoChangeCount = aCalendarInfoChangeEntries.Count();

	for(TInt index = 0;index < calenInfoChangeCount;index++)
		{
		//get the context and set the calendar filename which triggered the
		// notification
		MCalenContext &context = iController.Services().Context();
		context.SetCalendarFileNameL(
				aCalendarInfoChangeEntries[index]->FileNameL());
		
		MCalFileChangeObserver::TChangeType changeType = 
					aCalendarInfoChangeEntries[index]->ChangeType();
		switch(changeType)
			{
			case MCalFileChangeObserver::ECalendarFileCreated:
			case MCalFileChangeObserver::ECalendarInfoCreated:
				{
				BroadcastNotification(ECalenNotifyCalendarInfoCreated);
				}
				break;
			case MCalFileChangeObserver::ECalendarFileDeleted:
				{
				BroadcastNotification(ECalenNotifyCalendarFileDeleted);
				}
				break;
			case MCalFileChangeObserver::ECalendarInfoUpdated:
			case MCalFileChangeObserver::ECalendarInfoDeleted:
				{
				TFileName calFileName = aCalendarInfoChangeEntries[index]->FileNameL();
                CCalSession& session = iGlobalData->CalSessionL( calFileName );
				
				CCalCalendarInfo* calendarInfo = session.CalendarInfoL();
                CleanupStack::PushL(calendarInfo);

                TBuf8<KBuffLength> keyBuff;
                keyBuff.AppendNum(EMarkAsDelete);

                TBool markAsdelete;
                TPckgC<TBool> pkgMarkAsDelete(markAsdelete);
                TRAPD(err,pkgMarkAsDelete.Set(calendarInfo->PropertyValueL(keyBuff)));
                markAsdelete = pkgMarkAsDelete();

                CleanupStack::PopAndDestroy(calendarInfo);

                if (err == KErrNone && markAsdelete)
                    {
                    //BroadcastNotification(ECalenNotifyCloseDialog);
                    BroadcastNotification(ECalenNotifyDeleteInstanceView);
                    BroadcastNotification(ECalenNotifyCalendarFileDeleted);
                    iFilnameDeleted = aCalendarInfoChangeEntries[index]->FileNameL().AllocL();
                    iAsyncCallback->CallBack();
                    }
                else
                    {
                    BroadcastNotification(ECalenNotifyCalendarInfoUpdated);
                    }
                }
				break;
			default:
				break;
			}
		}

	TRACE_EXIT_POINT;
	}

// ----------------------------------------------------------------------------
// CCalenNotifier::AsyncRemoveCalendarL(TAny* aThisPtr)
// ----------------------------------------------------------------------------
TInt CCalenNotifier::AsyncRemoveCalendarL(TAny* aThisPtr)
    {
    TRACE_ENTRY_POINT
    static_cast<CCalenNotifier*>(aThisPtr)->AsyncRemoveCalendarL();
    TRACE_EXIT_POINT
    return 0;
    }

// ----------------------------------------------------------------------------
// CCalenNotifier::AsyncRemoveCalendarL()
//
void CCalenNotifier::AsyncRemoveCalendarL()
    {
    TRACE_ENTRY_POINT
    iGlobalData->RemoveCalendarL(iFilnameDeleted->Des());
    delete iFilnameDeleted;
    iFilnameDeleted = NULL;
    TRACE_EXIT_POINT
    }
// End of file
