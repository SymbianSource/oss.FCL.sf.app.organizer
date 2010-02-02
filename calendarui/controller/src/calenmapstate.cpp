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
* Description:   Map state for Calendar
*
*/



// includes
#include "calenmapstate.h"
#include "calendarui_debug.h"           // Debug macros
#include "calencontroller.h"
#include "calenstatemachine.h"
#include "calennotifier.h"

// ----------------------------------------------------------------------------
// CCalenMapState::NewLC
// First stage construction
// ----------------------------------------------------------------------------
CCalenMapState* CCalenMapState::NewLC( CCalenController& aController,
                                       RHashSet<TCalenNotification>& aOutstandingNotifications )
    {
    TRACE_ENTRY_POINT;

    CCalenMapState* self = new ( ELeave ) CCalenMapState( aController,
                                                          aOutstandingNotifications );
    CleanupStack::PushL( self );
    self->ConstructL();

    TRACE_EXIT_POINT;
    return self;
    }

// ----------------------------------------------------------------------------
// CCalenMapState::ConstructL
// Second stage construction
// ----------------------------------------------------------------------------
void CCalenMapState::ConstructL()
    {
    TRACE_ENTRY_POINT;
    BaseConstructL();
    
    TRACE_EXIT_POINT;
    }
    
// ----------------------------------------------------------------------------
// CCalenMapState::CCalenMapState
// C++ Constructor
// ----------------------------------------------------------------------------
CCalenMapState::CCalenMapState( CCalenController& aController, RHashSet<TCalenNotification>& aOutstandingNotifications )
    : CCalenState( aController, aOutstandingNotifications )
    {
    TRACE_ENTRY_POINT;
    
    TRACE_EXIT_POINT;
    }
    
// ----------------------------------------------------------------------------
// CCalenMapState::CCalenMapState
// Destructor
// ----------------------------------------------------------------------------    
CCalenMapState::~CCalenMapState()
    {
    TRACE_ENTRY_POINT;
    
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenMapState::HandleCommandL
// From CCalenState
// ----------------------------------------------------------------------------    
TBool CCalenMapState::HandleCommandL( const TCalenCommand& aCommand,
                                           CCalenStateMachine& aStateMachine )
    {
    TRACE_ENTRY_POINT; 
    
    TInt cmd = aCommand.Command();
    MCalenCommandHandler* handler = iController.GetCommandHandlerL( cmd );
	    
	ASSERT( handler );
     
    TBool cmdUsed = EFalse;
    switch( cmd )
    {
    case ECalenFasterAppExit:
		{
		// Issue map launch cancel notification
		iController.BroadcastNotification(ECalenNotifyCancelMapLaunch);
        SetCurrentState( aStateMachine, CCalenStateMachine::ECalenIdleState );
        ActivateCurrentStateL(aStateMachine);
        cmdUsed = ETrue;
        break;
   		}
    case ECalenMissedEventViewFromIdle:
        {
        // Issue map launch cancel notification
        iController.BroadcastNotification(ECalenNotifyCancelMapLaunch);
        SetCurrentState( aStateMachine, CCalenStateMachine::ECalenIdleState );
        ActivateCurrentStateL(aStateMachine);
        cmdUsed = ETrue;
        break;
        }
        
    case ECalenEventViewFromAlarm:
    case ECalenEventViewFromAlarmStopOnly:
        {
        // Issue map launch cancel notification
        iController.BroadcastNotification(ECalenNotifyCancelMapLaunch);
        SetCurrentState( aStateMachine, CCalenStateMachine::ECalenIdleState );
        ActivateCurrentStateL(aStateMachine);
        cmdUsed = ETrue;
        break;
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
// CCalenMapState::HandleNotificationL
// From CCalenState
// ----------------------------------------------------------------------------        
void CCalenMapState::HandleNotificationL(const TCalenNotification& aNotification,
                                                 CCalenStateMachine& aStateMachine )
    {
    TRACE_ENTRY_POINT;
    
    switch( aNotification )
        {
        case ECalenNotifyMapClosed:
        case ECalenNotifyAppForegrounded:
        	{
        	SetCurrentState( aStateMachine, iPreviousState );
        	// We set iOutstandingNotifications for two reasons.
        	// 1. The new state i.e. the state we are moving back can have notification info.
        	// 2. When we move to newstate we broadcast all notification from iOutstandingNotifications
        	//     and inform the registered notifiers.
        	iOutstandingNotifications.InsertL(aNotification);
            ActivateCurrentStateL(aStateMachine);
        	}
            break;
        default:
            CCalenState::HandleNotificationL( aNotification, aStateMachine );
            break;
        }
    
    TRACE_EXIT_POINT;
    }


// ----------------------------------------------------------------------------
// CCalenMapState::HandleStateActivationL
// Behavior when state is activated.
// ----------------------------------------------------------------------------
void CCalenMapState::HandleStateActivationL(CCalenStateMachine& /*aStateMachine*/)
    {
    TRACE_ENTRY_POINT;
    
    TRACE_EXIT_POINT;
    }

 // end of file
