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
* Description:  Calendar state machine
*
*/


// includes
#include "calenviewingstate.h"
#include "calendarui_debug.h"           // Debug macros
#include "calencontroller.h"
#include "calenstatemachine.h"
#include "calennotifier.h"

// ----------------------------------------------------------------------------
// CCalenViewingState::NewLC
// First stage construction
// ----------------------------------------------------------------------------
CCalenViewingState* CCalenViewingState::NewLC( CCalenController& aController,
                                                   RHashSet<TCalenNotification>& aOutstandingNotifications )
    {
    TRACE_ENTRY_POINT;

    CCalenViewingState* self = new ( ELeave ) CCalenViewingState( aController,aOutstandingNotifications );
    CleanupStack::PushL( self );
    self->ConstructL();

    TRACE_EXIT_POINT;
    return self;
    }

// ----------------------------------------------------------------------------
// CCalenViewingState::ConstructL
// Second stage construction
// ----------------------------------------------------------------------------
void CCalenViewingState::ConstructL()
    {
    TRACE_ENTRY_POINT;
    BaseConstructL();
    
    TRACE_EXIT_POINT;
    }
    
// ----------------------------------------------------------------------------
// CCalenIdleState::CCalenIdleState
// C++ Constructor
// ----------------------------------------------------------------------------
CCalenViewingState::CCalenViewingState( CCalenController& aController,
                                                    RHashSet<TCalenNotification>& aOutstandingNotifications )
    : CCalenState( aController, aOutstandingNotifications )
    {
    TRACE_ENTRY_POINT;
    
    TRACE_EXIT_POINT;
    }
    
// ----------------------------------------------------------------------------
// CCalenViewingState::CCalenViewingState
// Destructor
// ----------------------------------------------------------------------------    
CCalenViewingState::~CCalenViewingState()
    {
    TRACE_ENTRY_POINT;
    
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenViewingState::HandleCommandL
// From CCalenState
// ----------------------------------------------------------------------------    
TBool CCalenViewingState::HandleCommandL( const TCalenCommand& aCommand,
                                        CCalenStateMachine& aStateMachine )
    {
    TRACE_ENTRY_POINT;
    
    TInt cmd = aCommand.Command();
    MCalenCommandHandler* handler = iController.GetCommandHandlerL( cmd );
    
    ASSERT( handler ); // FIXME: error ui
    
    TBool cmdUsed = EFalse;

    switch( cmd )
        {
        case ECalenEditCurrentEntry:
	    case ECalenEditOccurrence:
	    case ECalenEditSeries:
        case ECalenRestoreTodo:
        case ECalenCompleteTodo:
      		{
    		// set the previous state as viewing state
    		CCalenStateMachine::TCalenStateIndex cachedState = GetCurrentState(aStateMachine);
            SetCurrentState( aStateMachine, CCalenStateMachine::ECalenEditingState);
            SetCurrentPreviousState( aStateMachine, cachedState );
            ActivateCurrentStateL(aStateMachine);
            cmdUsed = ETrue;
    		}
    		break;	    
		case ECalenSend:
			{
			// set the previous state as viewing state
    		CCalenStateMachine::TCalenStateIndex cachedState = GetCurrentState(aStateMachine);
            SetCurrentState( aStateMachine, CCalenStateMachine::ECalenSendingState);
            SetCurrentPreviousState( aStateMachine, cachedState );
            ActivateCurrentStateL(aStateMachine);
            cmdUsed = ETrue;
			}
			break;	    		
        case ECalenDeleteCurrentEntry:
        case ECalenDeleteSeries:
        case ECalenDeleteCurrentOccurrence:
    		{    
			// set the previous state as viewing state
            CCalenStateMachine::TCalenStateIndex cachedState = GetCurrentState(aStateMachine);
            SetCurrentState( aStateMachine, CCalenStateMachine::ECalenDeletingState);
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
	        break;	
    		}
        case ECalenViewAttachmentList:
        case ECalenAddAttachmentFromViewer:    
            {
            CCalenStateMachine::TCalenStateIndex cachedState = GetCurrentState(aStateMachine);
            SetCurrentState( aStateMachine, CCalenStateMachine::ECalenAttachmentState );
            SetCurrentPreviousState( aStateMachine, cachedState );
            ActivateCurrentStateL(aStateMachine);        
            cmdUsed = ETrue;
            }
            break;    		
        }
    RequestCallbackL( handler, aCommand );

    TRACE_EXIT_POINT;
    return cmdUsed;
    }

// ----------------------------------------------------------------------------
// CCalenViewingState::HandleNotificationL
// From CCalenState
// ----------------------------------------------------------------------------        
void CCalenViewingState::HandleNotificationL(const TCalenNotification& aNotification,
                                              CCalenStateMachine& aStateMachine )
    {
    TRACE_ENTRY_POINT;

    switch( aNotification )
        {
        case ECalenNotifyEntryClosed:
        	{
        	SetCurrentState( aStateMachine, iPreviousState );
            iOutstandingNotifications.InsertL(aNotification);
            ActivateCurrentStateL(aStateMachine);
        	}
	        break;
        
        case ECalenNotifyEntryDeleted:
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
// CCalenViewingState::HandleStateActivationL
// Behavior when state is activated.
// ----------------------------------------------------------------------------
void CCalenViewingState::HandleStateActivationL(CCalenStateMachine& aStateMachine)
    {
    TRACE_ENTRY_POINT;
    
    if( iOutstandingNotifications.Find(ECalenNotifyEntryDeleted) )
    	{
    	SetCurrentState( aStateMachine, iPreviousState );
        ActivateCurrentStateL(aStateMachine);
    	}
    else 
    	{
    	CCalenState::HandleStateActivationL(aStateMachine);
    	}
    TRACE_EXIT_POINT;
    }    
    
 // end of file
