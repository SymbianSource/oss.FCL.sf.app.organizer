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
* Description:   Calendar state machine
*
*/



// includes
#include "calenbackgroundstate.h"
#include "calendarui_debug.h"           // Debug macros
#include "calencontroller.h"
#include "calenstatemachine.h"
#include "calennotifier.h"

// ----------------------------------------------------------------------------
// CCalenBackgroundState::NewLC
// First stage construction
// ----------------------------------------------------------------------------
CCalenBackgroundState* CCalenBackgroundState::NewLC( CCalenController& aController,
                                                            RHashSet<TCalenNotification>&  aOutstandingNotifications )
    {
    TRACE_ENTRY_POINT;

    CCalenBackgroundState* self = new ( ELeave ) CCalenBackgroundState( aController,aOutstandingNotifications );
    CleanupStack::PushL( self );
    self->ConstructL();

    TRACE_EXIT_POINT;
    return self;
    }

// ----------------------------------------------------------------------------
// CCalenBackgroundState::ConstructL
// Second stage construction
// ----------------------------------------------------------------------------
void CCalenBackgroundState::ConstructL()
    {
    TRACE_ENTRY_POINT;
    BaseConstructL();
    
    TRACE_EXIT_POINT;
    }
    
// ----------------------------------------------------------------------------
// CCalenBackgroundState::CCalenBackgroundState
// C++ Constructor
// ----------------------------------------------------------------------------
CCalenBackgroundState::CCalenBackgroundState( CCalenController& aController, 
                                                                RHashSet<TCalenNotification>&  aOutstandingNotifications )
    : CCalenState( aController, aOutstandingNotifications )
    {
    TRACE_ENTRY_POINT;
    
    TRACE_EXIT_POINT;
    }
    
// ----------------------------------------------------------------------------
// CCalenBackgroundState::CCalenBackgroundState
// Destructor
// ----------------------------------------------------------------------------    
CCalenBackgroundState::~CCalenBackgroundState()
    {
    TRACE_ENTRY_POINT;
    
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenBackgroundState::HandleCommandL
// From CCalenState
// ----------------------------------------------------------------------------    
TBool CCalenBackgroundState::HandleCommandL( const TCalenCommand& aCommand,
                                           CCalenStateMachine& aStateMachine )
    {
    TRACE_ENTRY_POINT;  
    
    //When application is pushed background
    //1,when launched from active idle ,( AI screen or missed alarm softnotification/small indicator),
    //  multiple entires -> Day view
    //2,when launched from active idle ( AI screen or missed alarm softnotification/small indicator),
    //  single entry -> Event view
    
    TInt cmd = aCommand.Command();
    MCalenCommandHandler* handler = iController.GetCommandHandlerL( cmd );
    
    ASSERT( handler ); // FIXME: error ui
    
    TBool cmdUsed = EFalse;

    switch( cmd )
        {
        case ECalenDayView:
        case ECalenTodoView:
        case ECalenMonthView:
        case ECalenWeekView:
            SetCurrentState(aStateMachine,CCalenStateMachine::ECalenIdleState);
            cmdUsed = ETrue;
            break;
        case ECalenEventView:
            SetCurrentState(aStateMachine,CCalenStateMachine::ECalenViewingState);
            cmdUsed = ETrue;
            break;
        case ECalenEventViewFromAlarm:
        case ECalenEventViewFromAlarmStopOnly:
            SetCurrentState(aStateMachine,CCalenStateMachine::ECalenAlarmState);
            cmdUsed = ETrue;
            break;
        case ECalenFasterAppExit:
        	if(iPreviousState == CCalenStateMachine::ECalenMapState)
				{
				// Issue map launch cancel notification
				iController.BroadcastNotification(ECalenNotifyCancelMapLaunch);
		        SetCurrentState( aStateMachine, CCalenStateMachine::ECalenIdleState );
		        ActivateCurrentStateL(aStateMachine);
		        cmdUsed = ETrue;
		        break;
		   		}
        	else if(iPreviousState == CCalenStateMachine::ECalenDeletingState)
        	    {
                // Issue ECalenNotifyDeleteFailed notification to cancel the delete
                iController.BroadcastNotification(ECalenNotifyDeleteFailed);
                SetCurrentState( aStateMachine, CCalenStateMachine::ECalenIdleState );
                ActivateCurrentStateL(aStateMachine);
                cmdUsed = ETrue;                 
        	    }
        	else if( iPreviousState == CCalenStateMachine::ECalenSettingsState
        	       || iPreviousState == CCalenStateMachine::ECalenEditingState
        	       || iPreviousState == CCalenStateMachine::ECalenSendingState 
        	       || iPreviousState == CCalenStateMachine::ECalenIdleState 
                   || iPreviousState == CCalenStateMachine::ECalenViewingState
                   || iPreviousState == CCalenStateMachine::ECalenAttachmentState)
        	    {
                SetCurrentState( aStateMachine, CCalenStateMachine::ECalenIdleState );
                ActivateCurrentStateL(aStateMachine);
                cmdUsed = ETrue;                 
        	    }
        default:
            break;
        }
    if(cmdUsed)
    RequestCallbackL( handler, aCommand );
    
    TRACE_EXIT_POINT;
    return cmdUsed;
    }

// ----------------------------------------------------------------------------
// CCalenBackgroundState::HandleNotificationL
// From CCalenState
// ----------------------------------------------------------------------------        
void CCalenBackgroundState::HandleNotificationL(const TCalenNotification& aNotification,
                                                 CCalenStateMachine& aStateMachine )
    {
    TRACE_ENTRY_POINT;
    
    switch( aNotification )
        {
        case ECalenNotifyAppForegrounded:
        	{
        	if( iPreviousState == CCalenStateMachine::ECalenSendingState )
        	    {
        	    SetCurrentState( aStateMachine, CCalenStateMachine::ECalenIdleState );
        	    }
        	else
        	    {
                SetCurrentState( aStateMachine, iPreviousState );
        	    }
        	// We set iOutstandingNotifications for two reasons.
        	// 1. The new state i.e. the state we are moving back can have notification info.
        	// 2. When we move to newstate we broadcast all notification from iOutstandingNotifications
        	//     and inform the registered notifiers.
        	iOutstandingNotifications.InsertL(aNotification);
            ActivateCurrentStateL(aStateMachine);
        	}
            break;
        case ECalenNotifyAppBackgrounded:
            // may receive more background notifications, just ignore
            break;
        default:
            // default is defer all other notifications when app in background
            // iOutstandingNotifications.InsertL(aNotification);
            iController.Notifier().BroadcastApprovedNotification( aNotification );
            break;
        }
    
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenBackgroundState::HandleStateActivationL
// Behavior when state is activated.
// ----------------------------------------------------------------------------
void CCalenBackgroundState::HandleStateActivationL(CCalenStateMachine& aStateMachine)
    {
    TRACE_ENTRY_POINT;
    if(iOutstandingNotifications.Find(ECalenNotifyAppBackgrounded))
        {
        CCalenState::HandleStateActivationL(aStateMachine);
        }
    
    TRACE_EXIT_POINT;
    }

 // end of file
