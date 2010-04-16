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
#include "calenhelpstate.h"
#include "calendarui_debug.h"           // Debug macros
#include "calencontroller.h"
#include "calenstatemachine.h"
#include "calennotifier.h"

// ----------------------------------------------------------------------------
// CCalenHelpState::NewLC
// First stage construction
// ----------------------------------------------------------------------------
CCalenHelpState* CCalenHelpState::NewLC( CCalenController& aController,
                                                RHashSet<TCalenNotification>&  aOutstandingNotifications )
    {
    TRACE_ENTRY_POINT;

    CCalenHelpState* self = new ( ELeave ) CCalenHelpState( aController, aOutstandingNotifications );
    CleanupStack::PushL( self );
    self->ConstructL();

    TRACE_EXIT_POINT;
    return self;
    }

// ----------------------------------------------------------------------------
// CCalenHelpState::ConstructL
// Second stage construction
// ----------------------------------------------------------------------------
void CCalenHelpState::ConstructL()
    {
    TRACE_ENTRY_POINT;
    BaseConstructL();
    
    TRACE_EXIT_POINT;
    }
    
// ----------------------------------------------------------------------------
// CCalenHelpState::CCalenHelpState
// C++ Constructor
// ----------------------------------------------------------------------------
CCalenHelpState::CCalenHelpState( CCalenController& aController,
                                             RHashSet<TCalenNotification>&  aOutstandingNotifications )
    : CCalenState( aController, aOutstandingNotifications )
    {
    TRACE_ENTRY_POINT;
    
    TRACE_EXIT_POINT;
    }
    
// ----------------------------------------------------------------------------
// CCalenHelpState::CCalenHelpState
// Destructor
// ----------------------------------------------------------------------------    
CCalenHelpState::~CCalenHelpState()
    {
    TRACE_ENTRY_POINT;
    
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenHelpState::HandleCommandL
// From CCalenState
// ----------------------------------------------------------------------------    
TBool CCalenHelpState::HandleCommandL( const TCalenCommand& aCommand,
                                   CCalenStateMachine& aStateMachine )
    {
    TRACE_ENTRY_POINT;
    TInt cmd = aCommand.Command();
    MCalenCommandHandler* handler = iController.GetCommandHandlerL( cmd );
    
    ASSERT( handler ); // FIXME: error ui
    
    TBool cmdUsed = EFalse;
    
    if( ECalenFasterAppExit == cmd )
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
// CCalenHelpState::HandleNotificationL
// From CCalenState
// ----------------------------------------------------------------------------        
void CCalenHelpState::HandleNotificationL(const TCalenNotification& /*aNotification*/,
                                          CCalenStateMachine& /*aStateMachine*/ )
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    }



 // end of file
