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
#include "calenidlestate.h"
#include "calendarui_debug.h"           // Debug macros
#include "calencontroller.h"
#include "calenstatemachine.h"
#include "calennotifier.h"

// ----------------------------------------------------------------------------
// CCalenIdleState::NewLC
// First stage construction
// ----------------------------------------------------------------------------
CCalenIdleState* CCalenIdleState::NewLC( CCalenController& aController, 
                    RHashSet<TCalenNotification>&  aOutstandingNotifications  )
    {
    TRACE_ENTRY_POINT;

    CCalenIdleState* self = new ( ELeave ) CCalenIdleState( aController, 
                                                    aOutstandingNotifications );
    CleanupStack::PushL( self );
    self->ConstructL();

    TRACE_EXIT_POINT;
    return self;
    }

// ----------------------------------------------------------------------------
// CCalenIdleState::ConstructL
// Second stage construction
// ----------------------------------------------------------------------------
void CCalenIdleState::ConstructL()
    {
    TRACE_ENTRY_POINT;
    BaseConstructL();
    
    TRACE_EXIT_POINT;
    }
    
// ----------------------------------------------------------------------------
// CCalenIdleState::CCalenIdleState
// C++ Constructor
// ----------------------------------------------------------------------------
CCalenIdleState::CCalenIdleState( CCalenController& aController,
                    RHashSet<TCalenNotification>&  aOutstandingNotifications )
    : CCalenState( aController, aOutstandingNotifications )
    {
    TRACE_ENTRY_POINT;
    
    TRACE_EXIT_POINT;
    }
    
// ----------------------------------------------------------------------------
// CCalenIdleState::CCalenIdleState
// Destructor
// ----------------------------------------------------------------------------    
CCalenIdleState::~CCalenIdleState()
    {
    TRACE_ENTRY_POINT;
    
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenIdleState::HandleCommandL
// From CCalenState
// ----------------------------------------------------------------------------    
TBool CCalenIdleState::HandleCommandL( const TCalenCommand& aCommand,
                                       CCalenStateMachine& aStateMachine )
    {
    TRACE_ENTRY_POINT;
    
    TInt cmd = aCommand.Command();
    MCalenCommandHandler* handler = iController.GetCommandHandlerL( cmd );
    
    ASSERT( handler ); // FIXME: error ui
    
    TBool cmdUsed = EFalse;

    switch( cmd )
        {
        case ECalenMonthView:
        case ECalenWeekView:
        case ECalenDayView:
        case ECalenTodoView:
        case ECalenForwardsToDayView:
        case ECalenForwardsToWeekView:
        case ECalenNextView:
        case ECalenPrevView:
        case ECalenSwitchView:
        case ECalenHidePreview:
        case ECalenShowPreview:
        case ECalenGotoToday:
        case ECalenGotoDate:
        case ECalenNotifyFocusChange:
        case ECalenCompleteTodo:
        case ECalenRestoreTodo:
        case ECalenMissedAlarmsView:
        case ECalenCmdClear:
        case ECalenCmdClearAll:
        case ECalenCmdGotoCalendar:
        case ECalenMissedEventViewFromIdle:
        case ECalenMissedAlarmsViewFromIdle:    
        	 cmdUsed = ETrue;
             break;
        
        case ECalenStartActiveStep:
        	{
        	// set previous state to idle
            CCalenStateMachine::TCalenStateIndex cachedState = GetCurrentState(aStateMachine);
            SetCurrentState( aStateMachine, CCalenStateMachine::ECalenPopulationState );
           	SetCurrentPreviousState( aStateMachine, cachedState );
            ActivateCurrentStateL(aStateMachine);           	
           	cmdUsed = ETrue;
           	}
           	break;
        case ECalenEventView: 
        case ECalenMissedEventView: 
            {
        	// set previous state to idle
            CCalenStateMachine::TCalenStateIndex cachedState = GetCurrentState(aStateMachine);
            SetCurrentState( aStateMachine, CCalenStateMachine::ECalenViewingState);
            SetCurrentPreviousState( aStateMachine, cachedState );
            ActivateCurrentStateL(aStateMachine);            
            cmdUsed = ETrue;
        	}
        	break;
        case ECalenNewMeeting:
        case ECalenNewTodo:
        case ECalenNewAnniv:
        case ECalenNewDayNote:
        case ECalenNewReminder:
        case ECalenNewMeetingRequest:
        case ECalenEditCurrentEntry:
        case ECalenEditSeries:
        case ECalenEditOccurrence:
        case ECalenViewCurrentEntry:
            {
            // set previous state to idle
            CCalenStateMachine::TCalenStateIndex cachedState = GetCurrentState(aStateMachine);
            SetCurrentState( aStateMachine, CCalenStateMachine::ECalenEditingState);
            SetCurrentPreviousState( aStateMachine, cachedState );
            ActivateCurrentStateL(aStateMachine);            
            cmdUsed = ETrue;
            }
            break;
        case ECalenSend:
            {
            // set previous state to idle
            CCalenStateMachine::TCalenStateIndex cachedState = GetCurrentState(aStateMachine);
            SetCurrentState( aStateMachine, CCalenStateMachine::ECalenSendingState);
            SetCurrentPreviousState( aStateMachine, cachedState );
            ActivateCurrentStateL(aStateMachine);
            cmdUsed = ETrue;
            }
            break;
        
        case ECalenDeleteCurrentEntry:
        case ECalenDeleteEntryWithoutQuery:	
        case ECalenDeleteSeries:
        case ECalenDeleteCurrentOccurrence:
        case ECalenDeleteAllEntries:
        case ECalenDeleteEntriesBeforeDate:
        	{
            // set previous state to idle
            CCalenStateMachine::TCalenStateIndex cachedState = GetCurrentState(aStateMachine);
            SetCurrentState( aStateMachine, CCalenStateMachine::ECalenDeletingState);
            SetCurrentPreviousState( aStateMachine, cachedState );
            ActivateCurrentStateL(aStateMachine);
            cmdUsed = ETrue;
        	}
            break;
        
        case ECalenShowSettings:
        	{
        	// set previous state to idle
            CCalenStateMachine::TCalenStateIndex cachedState = GetCurrentState(aStateMachine);
            SetCurrentState( aStateMachine, CCalenStateMachine::ECalenSettingsState);
            SetCurrentPreviousState( aStateMachine, cachedState );
            ActivateCurrentStateL(aStateMachine);
            cmdUsed = ETrue;
        	}
            break;
        case ECalenFasterAppExit:
        	{
	        SetCurrentState( aStateMachine, CCalenStateMachine::ECalenIdleState );
	        ActivateCurrentStateL(aStateMachine);
	        cmdUsed = ETrue;
        	} 
        	break;       
        case ECalenGetLocation:
    	case ECalenShowLocation:
    	case ECalenGetLocationAndSave:
    		{
    		CCalenStateMachine::TCalenStateIndex cachedState = GetCurrentState(aStateMachine);
	        SetCurrentState( aStateMachine, CCalenStateMachine::ECalenMapState );
	        SetCurrentPreviousState( aStateMachine, cachedState );
	        ActivateCurrentStateL(aStateMachine);        
	        cmdUsed = ETrue;
    		}
			break;
		case ECalenEventViewFromAlarm:
        case ECalenEventViewFromAlarmStopOnly:	
			{
			CCalenStateMachine::TCalenStateIndex cachedState = GetCurrentState(aStateMachine);
            SetCurrentState( aStateMachine, CCalenStateMachine::ECalenAlarmState);
            SetCurrentPreviousState( aStateMachine, cachedState );
            ActivateCurrentStateL(aStateMachine);            
            cmdUsed = ETrue;
			}
			break;
    	case ECalenAddAttachment:
    	case ECalenRemoveAttachment:
    	case ECalenViewAttachmentList:    
    	    {
    	    CCalenStateMachine::TCalenStateIndex cachedState = GetCurrentState(aStateMachine);
    	    SetCurrentState( aStateMachine, CCalenStateMachine::ECalenAttachmentState );
            SetCurrentPreviousState( aStateMachine, cachedState );
            ActivateCurrentStateL(aStateMachine);        
            cmdUsed = ETrue;
    	    }
    	    break;
        default:
            // This a valid custom command as there is a command handler
            // do not modify the new start and remain in idle.
            // remain in idle
            // don't modify aNewState.
            break;
        }
        
    RequestCallbackL( handler, aCommand );

    TRACE_EXIT_POINT;
    return cmdUsed;
    }

// ----------------------------------------------------------------------------
// CCalenIdleState::HandleNotificationL
// From CCalenState
// ----------------------------------------------------------------------------        
void CCalenIdleState::HandleNotificationL(const TCalenNotification& aNotification,
                                          CCalenStateMachine& aStateMachine)
    {
    TRACE_ENTRY_POINT;
    
    CCalenState::HandleNotificationL( aNotification, aStateMachine );
    
    TRACE_EXIT_POINT;
    }

// end of file

