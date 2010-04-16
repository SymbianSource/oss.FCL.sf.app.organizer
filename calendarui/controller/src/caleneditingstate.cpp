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
#include "caleneditingstate.h"
#include "calendarui_debug.h"           // Debug macros
#include "calencontroller.h"
#include "calenstatemachine.h"
#include "calennotifier.h"

// ----------------------------------------------------------------------------
// CCalenEditingState::NewLC
// First stage construction
// ----------------------------------------------------------------------------
CCalenEditingState* CCalenEditingState::NewLC( CCalenController& aController,
                                                            RHashSet<TCalenNotification>& aOutstandingNotifications )
    {
    TRACE_ENTRY_POINT;

    CCalenEditingState* self = new( ELeave ) CCalenEditingState( aController, aOutstandingNotifications );
    CleanupStack::PushL( self );
    self->ConstructL();

    TRACE_EXIT_POINT;
    return self;
    }

// ----------------------------------------------------------------------------
// CCalenEditingState::ConstructL
// Second stage construction
// ----------------------------------------------------------------------------
void CCalenEditingState::ConstructL()
    {
    TRACE_ENTRY_POINT;
    BaseConstructL();
    
    TRACE_EXIT_POINT;
    }
    
// ----------------------------------------------------------------------------
// CCalenEditingState::CCalenEditingState
// C++ Constructor
// ----------------------------------------------------------------------------
CCalenEditingState::CCalenEditingState( CCalenController& aController,
                                                        RHashSet<TCalenNotification>& aOutstandingNotifications )
    : CCalenState( aController, aOutstandingNotifications )
    {
    TRACE_ENTRY_POINT;

    TRACE_EXIT_POINT;
    }
    
// ----------------------------------------------------------------------------
// CCalenEditingState::CCalenEditingState
// Destructor
// ----------------------------------------------------------------------------    
CCalenEditingState::~CCalenEditingState()
    {
    TRACE_ENTRY_POINT;
    
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenEditingState::HandleCommandL
// From CCalenState
// ----------------------------------------------------------------------------    
TBool CCalenEditingState::HandleCommandL( const TCalenCommand& aCommand,
                              CCalenStateMachine& aStateMachine )
    {
    TRACE_ENTRY_POINT
    
    TInt cmd = aCommand.Command();
    MCalenCommandHandler* handler = iController.GetCommandHandlerL( cmd );
	    
	ASSERT( handler );
     
    TBool cmdUsed = EFalse;
    switch( cmd )
    {
    	case ECalenDeleteCurrentEntry:
    	case ECalenDeleteSeries:
    	case ECalenDeleteCurrentOccurrence:
    		{
    		CCalenStateMachine::TCalenStateIndex cachedState = GetCurrentState(aStateMachine);
	        SetCurrentState( aStateMachine, CCalenStateMachine::ECalenDeletingState);
	        SetCurrentPreviousState( aStateMachine, cachedState );
	        ActivateCurrentStateL(aStateMachine);
	        
	        cmdUsed = ETrue;
	        break;
    		}
    		
    	case ECalenSend: // For handling send in viewer
    		{
    		CCalenStateMachine::TCalenStateIndex cachedState = GetCurrentState(aStateMachine);
	        SetCurrentState( aStateMachine, CCalenStateMachine::ECalenSendingState );
	        SetCurrentPreviousState( aStateMachine, cachedState );
	        ActivateCurrentStateL(aStateMachine);        
	        
	        cmdUsed = ETrue;
	        break;	
    		}
    		
    	case ECalenFasterAppExit:
    		{
	        SetCurrentState( aStateMachine, CCalenStateMachine::ECalenIdleState );
	        ActivateCurrentStateL(aStateMachine);
	        cmdUsed = ETrue;
	        break;
        	}
        	
    	case ECalenGetLocation:
    	case ECalenShowLocation:
    		{       	
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
// CCalenEditingState::HandleNotificationL
// From CCalenState
// ----------------------------------------------------------------------------        
void CCalenEditingState::HandleNotificationL(const TCalenNotification& aNotification,
                                              CCalenStateMachine& aStateMachine )
    {
    TRACE_ENTRY_POINT;
    
    switch( aNotification )
        {
        case ECalenNotifyEntryClosed:  // issued when new entry dialog is cancelled
        case ECalenNotifyEntrySaved:
        case ECalenNotifyInstanceSaved:
        case ECalenNotifyDialogClosed:
        case ECalenNotifyEntryDeleted:
        case ECalenNotifyEditorClosedFromViewer:
            {
            SetCurrentState( aStateMachine, iPreviousState );
            // Let new state does the broadcast
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

// end of file
