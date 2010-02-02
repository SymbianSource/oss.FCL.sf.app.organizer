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
* Description:   An missed alarm handler class, CCalenSvrMissedAlarmManager.
 *                This class takes care of handling the UI and cenrep updation
 *                for missed alarms.
 *
*/



#include <avkon.rsg> 
#include <AknSmallIndicator.h>
#include <centralrepository.h>
#include <missedalarmstorecrkeys.h>
#include <CalendarInternalCRKeys.h>
#include <calensvrmissedalarmmanagerresource.rsg>

#include "calendarui_debug.h"
#include "calensvrmissedalarmmanager.h"
#include "calenmissedalarmconstants.h"

// -----------------------------------------------------------------------------
// CCalenSvrMissedAlarmManager::NewL
// First phase construction
// -----------------------------------------------------------------------------
//
CCalenSvrMissedAlarmManager* CCalenSvrMissedAlarmManager::NewL()
    {
    TRACE_ENTRY_POINT;
    CCalenSvrMissedAlarmManager* self = new( ELeave )CCalenSvrMissedAlarmManager();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    TRACE_EXIT_POINT;
    return self;
    }
    
// -----------------------------------------------------------------------------
// CCalenSvrMissedAlarmManager::CCalenSvrMissedAlarmManager
// Default constructor
// -----------------------------------------------------------------------------
//
CCalenSvrMissedAlarmManager::CCalenSvrMissedAlarmManager() :
		CActive(CActive::EPriorityLow),
    iNumOfMissedAlarms( KNoMissedAlarms ),
    iShowSoftNotification( EFalse )
    {
    TRACE_ENTRY_POINT;
    
    CActiveScheduler::Add(this);
    
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenSvrMissedAlarmManager::CCalenSvrMissedAlarmManager
// Second phase construction
// -----------------------------------------------------------------------------
//
void CCalenSvrMissedAlarmManager::ConstructL()
    {
    TRACE_ENTRY_POINT;
    
    iSoftNotifier = CAknSoftNotifier::NewL();

    iMissedAlarmStoreRepository = CRepository::NewL( KCRUidMissedAlarmStore );
    
    // Create missed alarm store
    iMissedAlarmStore = CMissedAlarmStore::NewL(*iMissedAlarmStoreRepository);
    
    iCenRepChangeNotifier = CCenRepNotifyHandler::NewL( *this, *iMissedAlarmStoreRepository );
    iCenRepChangeNotifier->StartListeningL();
   
    Start();
    
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenSvrMissedAlarmManager::~CCalenSvrMissedAlarmManager
// Destructor
// -----------------------------------------------------------------------------
//
CCalenSvrMissedAlarmManager::~CCalenSvrMissedAlarmManager()
    {
	TRACE_ENTRY_POINT;
	
	Cancel();
	
	delete iSoftNotifier;
    iSoftNotifier = NULL;
    
    if(iCenRepChangeNotifier)
        {
        iCenRepChangeNotifier->StopListening();
        delete iCenRepChangeNotifier;
        }
    
    delete iMissedAlarmStore;
    iMissedAlarmStore = NULL;
    
	TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenSvrMissedAlarmManager::Start
// Start the scheduler
// -----------------------------------------------------------------------------
//
void CCalenSvrMissedAlarmManager::Start()
	{
	TRACE_ENTRY_POINT;
	
	__ASSERT_ALWAYS(!IsActive(), User::Invariant());
	TRequestStatus* pStat = &iStatus;
    User::RequestComplete(pStat, KErrNone);
    SetActive();	
    
    TRACE_EXIT_POINT;
	}
	
// ----------------------------------------------------------------------------
// CCalenSvrMissedAlarmManager::HandleNotifyGeneric
// From MCenRepNotifyHandlerCallback
// Generic notification that one of our central repository keys has changed
// If any keys change we broadcast a settings changed notification
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenSvrMissedAlarmManager::HandleNotifyGeneric( TUint32 aCenrepKeyId )
    {
    TRACE_ENTRY_POINT;
    
    if(aCenrepKeyId <= KMissedAlarmsMaxValue)
        {
        TRAP_IGNORE(HandleMissedAlarmL());
        }
    
    TRACE_EXIT_POINT;
    } 

// -----------------------------------------------------------------------------
// CCalenSvrMissedAlarmManager::HandleMissedAlarmL
// Handles the change in missed alarm store.
// updates soft notification and indicator plugins
// -----------------------------------------------------------------------------
//
void CCalenSvrMissedAlarmManager::HandleMissedAlarmL()
    {
	TRACE_ENTRY_POINT;
	//If the value of supressnotification is 1 then supress the missed alarm soft notification
	//If the value of supressnotification is 0 then show the missed alarm soft notification
	CRepository* repository = CRepository::NewL( KCRUidCalendar );
    TBool supressnotification = EFalse;
    repository->Get( KCalendarSupressMissedAlarmSoftNotification, supressnotification );
    
    if( supressnotification )
        {
        TRACE_EXIT_POINT;
        return;
        }
	//Update with the count from the metadata
	GetCountOfMissedAlarmsL();
	
	// Update Soft Notifications 
	HandleMissedAlarmSoftNotificationL();
	
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenSvrMissedAlarmManager::DoCancel
// For cancel
// -----------------------------------------------------------------------------
//
void CCalenSvrMissedAlarmManager::DoCancel()
	{
	TRACE_ENTRY_POINT;
	TRACE_EXIT_POINT;
	}

// -----------------------------------------------------------------------------
// CCalenSvrMissedAlarmManager::RunError
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//	
TInt CCalenSvrMissedAlarmManager::RunError(TInt /*aError*/)
	{
	TRACE_ENTRY_POINT;
	TRACE_EXIT_POINT;

	return 0;	
	}

// -----------------------------------------------------------------------------
// CCalenSvrMissedAlarmManager::RunL
// Handle missed alarms on RunL
// -----------------------------------------------------------------------------
//	
void CCalenSvrMissedAlarmManager::RunL()
	{
	TRACE_ENTRY_POINT;

	HandleMissedAlarmL();

	TRACE_EXIT_POINT;	
	}

// -----------------------------------------------------------------------------
// CCalenSvrMissedAlarmManager::GetCountOfMissedAlarms
// Get count of missed alarms from cenrep
// -----------------------------------------------------------------------------
//
void CCalenSvrMissedAlarmManager::GetCountOfMissedAlarmsL()
    {
	TRACE_ENTRY_POINT;
	
	TUint32 newCount = 0;
	iMissedAlarmStore->CountL(newCount);
	
	if(TInt(newCount) <= KNoMissedAlarms)
	    {
		iNumOfMissedAlarms = KNoMissedAlarms;
		iShowSoftNotification = EFalse;	
	    }
	else
	    {
		if(iNumOfMissedAlarms > TInt(newCount))
		    {
		    iShowSoftNotification = EFalse;
		    }
		else
		    {
		    iShowSoftNotification = ETrue;
		    }
		
		iNumOfMissedAlarms = TInt(newCount);
	    }
	
	TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenSvrMissedAlarmManager::HandleMissedAlarmSoftNotificationL
// Update soft notification/status pane indicator
// -----------------------------------------------------------------------------
//
void CCalenSvrMissedAlarmManager::HandleMissedAlarmSoftNotificationL()
    {
	TRACE_ENTRY_POINT;	
	
	//Update indicator 
	HandleMissedAlarmSmallIndicatorL(
	        (iNumOfMissedAlarms > KNoMissedAlarms ? ETrue : EFalse));
	
	CAknSoftNotificationParameters* softNotificationParameters = 
	                        CreateNotificationParametersLC( aNotificationType );
	
	if(!iShowSoftNotification )
	    {
	    iSoftNotifier->CancelCustomSoftNotificationL( *softNotificationParameters );
	    }
	else
	    { 
	    if(iNumOfMissedAlarms > KNoMissedAlarms) //check for -ve
	        {
	        iSoftNotifier->SetCustomNotificationCountL(
	                                *softNotificationParameters, iNumOfMissedAlarms );	
	        }
	    }
	
	CleanupStack::PopAndDestroy( softNotificationParameters );		
	
	TRACE_EXIT_POINT;	
    }

// -----------------------------------------------------------------------------
// CCalenSvrMissedAlarmManager::HandleMissedAlarmCoverUISoftNotificationL
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCalenSvrMissedAlarmManager::HandleMissedAlarmCoverUISoftNotificationL()
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenSvrMissedAlarmManager::CreateNotificationParametersLC
// Initialise soft notification parameters. 
// -----------------------------------------------------------------------------
//
CAknSoftNotificationParameters* CCalenSvrMissedAlarmManager::
                                CreateNotificationParametersLC(
                       			const TAknSoftNotificationType aNotificationType  )
    {
	TRACE_ENTRY_POINT;
	
	// instantiate parameters
	CAknSoftNotificationParameters* softNotificationParameters = NULL;
	
	switch( aNotificationType )
	    {
        case ECustomSoftNotification: //Notification for missed alarm message
			{
            TUint32 newCount = 0;
            iMissedAlarmStore->CountL(newCount);
			
            if(newCount>1)
                {
                // if count is more than one,launch missed alarms view
                const TDesC8& viewActivationMsg = _L8("MAV");
                softNotificationParameters = CAknSoftNotificationParameters::NewL(
                        KMissedAlarmResourceFile,
                        R_MISSED_ALARM_SOFT_NOTIFICATION ,
                        KMissedAlarmNotificationPriority,
                        R_AVKON_SOFTKEYS_SHOW_EXIT,                             
                        CAknNoteDialog::ENoTone,
                        KMissedAlarmsViewId,
                        KCommandUid,
                        EAknSoftkeyShow,
                        viewActivationMsg);
                
                softNotificationParameters->SetGroupedTexts( R_MISSED_ALARM_UI_GROUPED );
                }
            else
                {
                // if count is one,launch missed event view
                const TDesC8& viewActivationMsg = _L8("MEV");
                softNotificationParameters = CAknSoftNotificationParameters::NewL(
                        KMissedAlarmResourceFile,
                        R_MISSED_ALARM_SOFT_NOTIFICATION ,
                        KMissedAlarmNotificationPriority,
                        R_AVKON_SOFTKEYS_SHOW_EXIT,                             
                        CAknNoteDialog::ENoTone,
                        KMissedEventViewId,
                        KCommandUid,
                        EAknSoftkeyShow,
                        viewActivationMsg);
                softNotificationParameters->SetGroupedTexts( R_MISSED_ALARM_UI_GROUPED );
                }
			break;
			}
         default:
			{
			// No other notification type is supported for now.
			User::Leave( KErrNotFound );
			break;
			}
	  }
  CleanupStack::PushL( softNotificationParameters );

  TRACE_EXIT_POINT;
  return softNotificationParameters;
}

// -----------------------------------------------------------------------------
// CCalenSvrMissedAlarmManager::HandleMissedAlarmSmallIndicatorL
// For Handling calendar's status pane indicator
// -----------------------------------------------------------------------------
//
void CCalenSvrMissedAlarmManager::HandleMissedAlarmSmallIndicatorL(TBool aArg)
    {
    TRACE_ENTRY_POINT;
	CAknSmallIndicator* indicator = CAknSmallIndicator::NewL(
	                            TUid::Uid( EAknIndicatorMissedCalendarAlarm ) );
	CleanupStack::PushL(indicator);
	indicator->SetIndicatorStateL( aArg );	
	CleanupStack::PopAndDestroy(); // indicator
    TRACE_EXIT_POINT;
    }   

// End of file
