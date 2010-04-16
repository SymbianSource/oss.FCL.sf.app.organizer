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
* Description:  Calendar state machine
*
*/


// includes
#include "calendarui_debug.h"           // Debug macros
#include "calenstate.h"                 // Calendar States
#include "calencontroller.h"
#include "calenstatemachine.h"
#include "calennotifier.h"
#include "calenasynccallback.h"

// ----------------------------------------------------------------------------
// CCalenState::CCalenState
// C++ constructor
// ----------------------------------------------------------------------------
CCalenState::CCalenState( CCalenController& aController,RHashSet<TCalenNotification>&  aOutstandingNotifications )
    : iController( aController ),
      iOutstandingNotifications( aOutstandingNotifications ),
      iPreviousState( CCalenStateMachine::ECalenIdleState )
    {
    TRACE_ENTRY_POINT;

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenState::ConstructL
// second phase construction
// ----------------------------------------------------------------------------
void CCalenState::BaseConstructL()
    {
    TRACE_ENTRY_POINT;
    
    TCalenCommand command;
    MCalenCommandHandler* handler( NULL );
    iCallBackPackage = new( ELeave ) CalenCallbackPackage( this, command, handler );
    
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenState::~CCalenState
// Destructor
// ----------------------------------------------------------------------------
CCalenState::~CCalenState()
    {
    TRACE_ENTRY_POINT;
    
    delete iCallBackPackage;
    iCallBackPackage = NULL;
    delete iCmdCallback;
    
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenState::RequestCallback
// Request a callback
// ----------------------------------------------------------------------------
void CCalenState::RequestCallbackL( MCalenCommandHandler* aCommandHandler, const TCalenCommand& aCommand )
    {
    TRACE_ENTRY_POINT;

    iCallBackPackage->SetCommandHandler(this, aCommand, aCommandHandler);
     
    if( !iCmdCallback )
        {
        TCallBack callback( CommandCallback, iCallBackPackage );
        iCmdCallback = CalenAsyncCallBack::NewL( callback, CActive::EPriorityHigh , iController );
       }

    TCallBack callback( CommandCallback, iCallBackPackage );
    
    iCmdCallback->Cancel();
    iCmdCallback->Set( callback );
    iCmdCallback->CallBack();
    
    TRACE_EXIT_POINT;
    }



// ----------------------------------------------------------------------------
// CCalenState::CommandCallback
// Static callback function
// ----------------------------------------------------------------------------
TInt CCalenState::CommandCallback( TAny* aCommandStruct )
    {
    TRACE_ENTRY_POINT;
    TBool continueCommand(EFalse);
    
    CalenCallbackPackage* package = static_cast<CalenCallbackPackage*>( aCommandStruct );
    continueCommand = package->HandleCallBack();
    
    TRACE_EXIT_POINT;
    return continueCommand;
    }

// ----------------------------------------------------------------------------
// CCalenState::HandleNotificationL
// Default behaviour for notification handling is to go back to previous state.
// Concrete states would be expected to check the notification is related
// to the command issued.
// ----------------------------------------------------------------------------
void CCalenState::HandleNotificationL(const TCalenNotification& aNotification,
                                      CCalenStateMachine& aStateMachine )
    {
    TRACE_ENTRY_POINT;
    
    switch( aNotification )
        {
        case ECalenNotifyAppBackgrounded:
        	{
        	CCalenStateMachine::TCalenStateIndex cachedState = aStateMachine.GetCurrentState();
			aStateMachine.SetCurrentState(CCalenStateMachine::ECalenBackgroundState);
			aStateMachine.SetCurrentPreviousState(cachedState);
			iOutstandingNotifications.InsertL(aNotification);
			aStateMachine.ActivateCurrentStateL();
        	    
        	}
            break;
        default:
            iController.Notifier().BroadcastApprovedNotification( aNotification );
            break;
        }
    
   
    TRACE_EXIT_POINT;

    }


// ----------------------------------------------------------------------------
// CCalenState::PreviousState
// C++ constructor
// ----------------------------------------------------------------------------
CCalenStateMachine::TCalenStateIndex CCalenState::PreviousState()
    {
    TRACE_ENTRY_POINT;
    
    TRACE_EXIT_POINT;
    return iPreviousState;
    }

// ----------------------------------------------------------------------------
// CCalenState::SetPreviousState
// Sets previous state.
// ----------------------------------------------------------------------------
void CCalenState::SetPreviousState(const CCalenStateMachine::TCalenStateIndex& aPreviousState )
    {
    TRACE_ENTRY_POINT;
    
    iPreviousState = aPreviousState;
    
    TRACE_EXIT_POINT;
    }
    
// ----------------------------------------------------------------------------
// CCalenState::CommandExecuting
// From CCalenState
// ----------------------------------------------------------------------------    
void CCalenState::CommandExecuting()
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    }  

// ----------------------------------------------------------------------------
// CCalenState::CalenCallbackPackage::CalenCallbackPackage
// C++ constructor
// ----------------------------------------------------------------------------
CCalenState::CalenCallbackPackage::CalenCallbackPackage( CCalenState* aSelf,
                        TCalenCommand aCommand,  MCalenCommandHandler* aCommandHandler)
    : iSelf( aSelf ), iCommand( aCommand ), iCommandHandler( aCommandHandler )                   
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenState::CalenCallbackPackage::HandleCallBack
// Handles a callback
// ----------------------------------------------------------------------------
TBool CCalenState::CalenCallbackPackage::HandleCallBack()
    {
    TRACE_ENTRY_POINT;
    TBool continueCommand(EFalse);

    iSelf->CommandExecuting();
    PIM_TRAPD_HANDLE ( continueCommand = iCommandHandler->HandleCommandL( iCommand ) );
    
    TRACE_EXIT_POINT;
    return continueCommand;
    }

// ----------------------------------------------------------------------------
// CCalenState::CalenCallbackPackage::SetCommandHandler
// Sets the command handler and command
// ----------------------------------------------------------------------------
void CCalenState::CalenCallbackPackage::SetCommandHandler(CCalenState* aSelf,
                        TCalenCommand aCommand,  MCalenCommandHandler* aCommandHandler)
    {
    TRACE_ENTRY_POINT;
        
    iSelf = aSelf;
    iCommand = aCommand;
    iCommandHandler = aCommandHandler; 

    TRACE_EXIT_POINT;    
    }
    
// ----------------------------------------------------------------------------
// CCalenState::HandleStateActivationL
// Default behavior when state is activated. Check and issue any outstanding
// notifications
// ----------------------------------------------------------------------------
void CCalenState::HandleStateActivationL(CCalenStateMachine& /*aStateMachine*/)
	{
    TRACE_ENTRY_POINT;
    
    // Can only issue one of the following Notifications
    // ECalenNotifySettingsChanged or 
    // ECalenNotifySystemLocaleChanged or
    // ECalenNotifySettingsClosed
    
    if(iOutstandingNotifications.Find(ECalenNotifySettingsChanged))
        {
        iOutstandingNotifications.Remove(ECalenNotifySystemLocaleChanged);
        iOutstandingNotifications.Remove(ECalenNotifySettingsClosed);
        }
    else if(iOutstandingNotifications.Find(ECalenNotifySystemLocaleChanged))
        {
        iOutstandingNotifications.Remove(ECalenNotifySettingsClosed);
        }
    
    THashSetIter<TCalenNotification> iterator(iOutstandingNotifications);
    const TCalenNotification* notification = iterator.Next();
    while(notification)
        {
        iController.Notifier().BroadcastApprovedNotification( *notification );
        notification = iterator.Next();
        }
    
    iOutstandingNotifications.Close();
    
    TRACE_EXIT_POINT;
	}

// ----------------------------------------------------------------------------
// CCalenState::SetCurrentState
// Sets the current state
// ----------------------------------------------------------------------------
void CCalenState::SetCurrentState(CCalenStateMachine& aStateMachine,
								  const CCalenStateMachine::TCalenStateIndex& aState )	
	{
    TRACE_ENTRY_POINT;
    
    aStateMachine.SetCurrentState(aState);
    
    TRACE_EXIT_POINT;	
	}

// ----------------------------------------------------------------------------
// CCalenState::GetCurrentState
// Gets the current state
// ----------------------------------------------------------------------------
CCalenStateMachine::TCalenStateIndex CCalenState::GetCurrentState(
											CCalenStateMachine& aStateMachine)
	{
	TRACE_ENTRY_POINT;
	TRACE_EXIT_POINT;
	
	return aStateMachine.GetCurrentState();
	}

// ----------------------------------------------------------------------------
// CCalenState::SetPreviousState
// Sets the previous state through state machine
// ----------------------------------------------------------------------------
void CCalenState::SetCurrentPreviousState(CCalenStateMachine& aStateMachine, 
								const CCalenStateMachine::TCalenStateIndex& aState)
	{
	TRACE_ENTRY_POINT;
	
	aStateMachine.SetCurrentPreviousState(aState);
	
	TRACE_EXIT_POINT;
	}

// ----------------------------------------------------------------------------
// CCalenState::ActivateCurrentStateL
// Activate current state
// ----------------------------------------------------------------------------
void CCalenState::ActivateCurrentStateL(CCalenStateMachine& aStateMachine)
	{
    TRACE_ENTRY_POINT;
	
    aStateMachine.ActivateCurrentStateL();
    	
    TRACE_EXIT_POINT;	
	}
// ----------------------------------------------------------------------------
// CCalenState::CancelPreviousCmd
// Cancel the previous command
// ----------------------------------------------------------------------------
void CCalenState::CancelPreviousCmd(CCalenStateMachine& aStateMachine)
	{
	TRACE_ENTRY_POINT;
	
	aStateMachine.CancelPreviousCmd();
	
	TRACE_EXIT_POINT;
	}

// ----------------------------------------------------------------------------
// CCalenState::CancelExecutingCmd
// Cancels executing command
// ----------------------------------------------------------------------------
void CCalenState::CancelExecutingCmd()
	{
    TRACE_ENTRY_POINT;
	
	if(iCmdCallback)
		{
		iCmdCallback->Cancel();
		}
    	
    TRACE_EXIT_POINT;	
	}	
// End of file
