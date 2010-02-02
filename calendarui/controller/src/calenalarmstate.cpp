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
* Description: Calendar alarm state machine  
*
*/

// includes
#include "calenalarmstate.h"
#include "calendarui_debug.h"           // Debug macros
#include "calencontroller.h"
#include "calenstatemachine.h"
#include "calennotifier.h"
#include <eikon.hrh>

// ----------------------------------------------------------------------------
// CCalenAlarmState::NewLC
// First stage construction
// ----------------------------------------------------------------------------
CCalenAlarmState* CCalenAlarmState::NewLC( CCalenController& aController,
                                                                    RHashSet<TCalenNotification>&  aOutstandingNotifications )
    {
    TRACE_ENTRY_POINT;

    CCalenAlarmState* self = new ( ELeave ) CCalenAlarmState( aController,
                                                                     aOutstandingNotifications );
    CleanupStack::PushL( self );
    self->ConstructL();

    TRACE_EXIT_POINT;
    return self;
    }

// ----------------------------------------------------------------------------
// CCalenAlarmState::ConstructL
// Second stage construction
// ----------------------------------------------------------------------------
void CCalenAlarmState::ConstructL()
    {
    TRACE_ENTRY_POINT;
    BaseConstructL();
    
    TRACE_EXIT_POINT;
    }
    
// ----------------------------------------------------------------------------
// CCalenIdleState::CCalenIdleState
// C++ Constructor
// ----------------------------------------------------------------------------
CCalenAlarmState::CCalenAlarmState( CCalenController& aController,
                                                         RHashSet<TCalenNotification>&  aOutstandingNotifications )
    : CCalenState( aController, aOutstandingNotifications )
    {
    TRACE_ENTRY_POINT;
    
    TRACE_EXIT_POINT;
    }
    
// ----------------------------------------------------------------------------
// CCalenAlarmState::CCalenAlarmState
// Destructor
// ----------------------------------------------------------------------------    
CCalenAlarmState::~CCalenAlarmState()
    {
    TRACE_ENTRY_POINT;
    
    TRACE_EXIT_POINT;
    }
    
// ----------------------------------------------------------------------------
// CCalenAlarmState::HandleCommandL
// From CCalenState
// ----------------------------------------------------------------------------    
TBool CCalenAlarmState::HandleCommandL( const TCalenCommand& /*aCommand*/,
                                        CCalenStateMachine& /*aStateMachine*/ )
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    return EFalse;
    }
    
// ----------------------------------------------------------------------------
// CCalenAlarmState::HandleNotificationL
// From CCalenState
// ----------------------------------------------------------------------------        
void CCalenAlarmState::HandleNotificationL(const TCalenNotification& aNotification,
                                              CCalenStateMachine& aStateMachine )
    {
    TRACE_ENTRY_POINT;

    switch( aNotification )
        {
        case ECalenNotifyAlarmStopped:
        case ECalenNotifyStopAlarm:
        	{
        	SetCurrentState( aStateMachine, CCalenStateMachine::ECalenViewingState );
        	iOutstandingNotifications.InsertL(aNotification);
            ActivateCurrentStateL(aStateMachine);
        	}
	        break;
        
        case ECalenNotifyAlarmSnoozed:
        	{
        	SetCurrentState( aStateMachine, CCalenStateMachine::ECalenIdleState );
        	iOutstandingNotifications.InsertL(aNotification);
            ActivateCurrentStateL(aStateMachine);
       		}
       		break;
			
		case ECalenNotifyEntryClosed:
        	{
        	SetCurrentState( aStateMachine, iPreviousState );
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
// CCalenAlarmState::HandleStateActivationL
// Behavior when state is activated.
// ----------------------------------------------------------------------------
void CCalenAlarmState::HandleStateActivationL(CCalenStateMachine& /*aStateMachine*/)
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    }
    
 // end of file
    
