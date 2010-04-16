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
#include "calendeletingstate.h"
#include "calendarui_debug.h"           // Debug macros
#include "calencontroller.h"
#include "calenstatemachine.h"
#include "calennotifier.h"

// ----------------------------------------------------------------------------
// CCalenDeletingState::NewLC
// First stage construction
// ----------------------------------------------------------------------------
CCalenDeletingState* CCalenDeletingState::NewLC( CCalenController& aController,
                                                      RHashSet<TCalenNotification>&  aOutstandingNotifications )
    {
    TRACE_ENTRY_POINT;

    CCalenDeletingState* self = new( ELeave ) CCalenDeletingState( aController,aOutstandingNotifications );

    CleanupStack::PushL( self );
    self->ConstructL();

    TRACE_EXIT_POINT;
    return self;
    }

// ----------------------------------------------------------------------------
// CCalenDeletingState::ConstructL
// Second stage construction
// ----------------------------------------------------------------------------
void CCalenDeletingState::ConstructL()
    {
    TRACE_ENTRY_POINT;
    BaseConstructL();
  
    TRACE_EXIT_POINT;
    }
    
// ----------------------------------------------------------------------------
// CCalenDeletingState::CCalenDeletingState
// C++ Constructor
// ----------------------------------------------------------------------------
CCalenDeletingState::CCalenDeletingState( CCalenController& aController,
                                                    RHashSet<TCalenNotification>&  aOutstandingNotifications )
    : CCalenState( aController, aOutstandingNotifications )
    {
    TRACE_ENTRY_POINT;
    
    TRACE_EXIT_POINT;
    }
    
// ----------------------------------------------------------------------------
// CCalenDeletingState::CCalenDeletingState
// Destructor
// ----------------------------------------------------------------------------    
CCalenDeletingState::~CCalenDeletingState()
    {
    TRACE_ENTRY_POINT;
    
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenDeletingState::HandleCommandL
// From CCCalenState
// ----------------------------------------------------------------------------    
TBool CCalenDeletingState::HandleCommandL( const TCalenCommand& aCommand,
                                         CCalenStateMachine& aStateMachine )
    {
    TRACE_ENTRY_POINT;
    
    TInt cmd = aCommand.Command();
    MCalenCommandHandler* handler = iController.GetCommandHandlerL( cmd );
    
    ASSERT( handler ); // FIXME: error ui
    
    TBool cmdUsed = EFalse;
    
    if( cmd == ECalenDeleteEntryWithoutQuery )
        {
        RequestCallbackL( handler, aCommand );
        cmdUsed = ETrue;
        }
    else if( ECalenFasterAppExit == cmd )
    	{
        SetCurrentState( aStateMachine, CCalenStateMachine::ECalenIdleState );
        ActivateCurrentStateL(aStateMachine);
        RequestCallbackL( handler, aCommand );
        cmdUsed = ETrue;
    	}

    TRACE_EXIT_POINT;
    return cmdUsed;
    }

// ----------------------------------------------------------------------------
// CCalenDeletingState::HandleNotificationL
// From CCCalenState
// ----------------------------------------------------------------------------        
void CCalenDeletingState::HandleNotificationL(const TCalenNotification& aNotification,
                                              CCalenStateMachine& aStateMachine )
    {
    TRACE_ENTRY_POINT;
    
    switch( aNotification )
        {
        case ECalenNotifyEntryDeleted:  // issued when new entry dialog is cancelled
        case ECalenNotifyInstanceDeleted:
        case ECalenNotifyMultipleEntriesDeleted:
        case ECalenNotifyDeleteFailed:  // delete failed or cancelled
        case ECalenNotifyMarkedEntryDeleted:
            {
            SetCurrentState( aStateMachine, iPreviousState );
            // DONT broadcast these notifications until the previous state( idle or viewer ) is activated
            // which would broadcast in HandleStateActivationL()
            iOutstandingNotifications.InsertL(aNotification);
            ActivateCurrentStateL(aStateMachine);
            }
            break;
        case ECalenNotifyCancelDelete:
            {
            SetCurrentState( aStateMachine, iPreviousState);
            SetCurrentPreviousState( aStateMachine, GetCurrentState(aStateMachine) );

            iOutstandingNotifications.InsertL(aNotification);
            ActivateCurrentStateL(aStateMachine);
            
            CancelPreviousCmd(aStateMachine);
            }
            break;
        default:
            CCalenState::HandleNotificationL( aNotification, aStateMachine );
            break;
        }

    TRACE_EXIT_POINT;
    }
 
 // end of file
