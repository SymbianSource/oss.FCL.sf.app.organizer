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
* Description:   Calendar state machine
*
*/



// includes
#include "calensendingstate.h"
#include "calendarui_debug.h"           // Debug macros
#include "calencontroller.h"
#include "calenstatemachine.h"
#include "calennotifier.h"

// ----------------------------------------------------------------------------
// CCalenSendingState::NewLC
// First stage construction
// ----------------------------------------------------------------------------
CCalenSendingState* CCalenSendingState::NewLC( CCalenController& aController,
                                                    RHashSet<TCalenNotification>&  aOutstandingNotifications )
    {
    TRACE_ENTRY_POINT;

    CCalenSendingState* self = new ( ELeave ) CCalenSendingState( aController,aOutstandingNotifications );
    CleanupStack::PushL( self );
    self->ConstructL();

    TRACE_EXIT_POINT;
    return self;
    }

// ----------------------------------------------------------------------------
// CCalenSendingState::ConstructL
// Second stage construction
// ----------------------------------------------------------------------------
void CCalenSendingState::ConstructL()
    {
    TRACE_ENTRY_POINT;
    BaseConstructL();
    
    TRACE_EXIT_POINT;
    }
    
// ----------------------------------------------------------------------------
// CCalenSendingState::CCalenSendingState
// C++ Constructor
// ----------------------------------------------------------------------------
CCalenSendingState::CCalenSendingState( CCalenController& aController,
                                                    RHashSet<TCalenNotification>&  aOutstandingNotifications )
    : CCalenState( aController, aOutstandingNotifications )
    {
    TRACE_ENTRY_POINT;
    
    TRACE_EXIT_POINT;
    }
    
// ----------------------------------------------------------------------------
// CCalenSendingState::CCalenSendingState
// Destructor
// ----------------------------------------------------------------------------    
CCalenSendingState::~CCalenSendingState()
    {
    TRACE_ENTRY_POINT;
    
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenSendingState::HandleCommandL
// From CCalenState
// ----------------------------------------------------------------------------    
TBool CCalenSendingState::HandleCommandL( const TCalenCommand& aCommand,
                                         CCalenStateMachine& aStateMachine )
    {
    TRACE_ENTRY_POINT;
    TInt cmd = aCommand.Command();
    MCalenCommandHandler* handler = iController.GetCommandHandlerL( cmd );
    
    ASSERT( handler ); // FIXME: error ui
    
    TBool cmdUsed = EFalse;
    
    if(ECalenFasterAppExit == cmd)
        {
        SetCurrentState( aStateMachine, CCalenStateMachine::ECalenIdleState );
        ActivateCurrentStateL(aStateMachine);
        RequestCallbackL( handler, aCommand );
        cmdUsed = ETrue;
        }
    else if(ECalenStartActiveStep == cmd)
        {
        RequestCallbackL( handler, aCommand );
        cmdUsed = ETrue;
        }    

    TRACE_EXIT_POINT;
    return cmdUsed;
    }

// ----------------------------------------------------------------------------
// CCalenSendingState::HandleNotificationL
// From CCalenState
// ----------------------------------------------------------------------------        
void CCalenSendingState::HandleNotificationL(const TCalenNotification& aNotification,
                                              CCalenStateMachine& aStateMachine )
    {
    TRACE_ENTRY_POINT;

    switch( aNotification )
        {
        case ECalenNotifyEntrySendCancel:
        case ECalenNotifyDialogClosed:  // issued when send method query dialog is cancelled
        	{
        	// FIXME Should have a ECalenNotifyEntrySent?
            SetCurrentState( aStateMachine, iPreviousState);
            iOutstandingNotifications.InsertL(aNotification);
        	ActivateCurrentStateL(aStateMachine);
        	}
            break;
        case ECalenNotifyEntrySent:
        	{
            SetCurrentState( aStateMachine, iPreviousState);
            iOutstandingNotifications.InsertL(aNotification);
        	ActivateCurrentStateL(aStateMachine);
        	}
        default:
            CCalenState::HandleNotificationL( aNotification, aStateMachine );
            break;
        }

    TRACE_EXIT_POINT;
    }


 // end of file
