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
#include "calenprintingstate.h"
#include "calendarui_debug.h"           // Debug macros
#include "calencontroller.h"
#include "calenstatemachine.h"
#include "calennotifier.h"

// ----------------------------------------------------------------------------
// CCalenPrintingState::NewLC
// First stage construction
// ----------------------------------------------------------------------------
CCalenPrintingState* CCalenPrintingState::NewLC( CCalenController& aController,
                                                        RHashSet<TCalenNotification>& aOutstandingNotifications )
    {
    TRACE_ENTRY_POINT;

    CCalenPrintingState* self = new ( ELeave ) CCalenPrintingState( aController,aOutstandingNotifications );
    CleanupStack::PushL( self );
    self->ConstructL();

    TRACE_EXIT_POINT;
    return self;
    }

// ----------------------------------------------------------------------------
// CCalenPrintingState::ConstructL
// Second stage construction
// ----------------------------------------------------------------------------
void CCalenPrintingState::ConstructL()
    {
    TRACE_ENTRY_POINT;
    BaseConstructL();    
    
    TRACE_EXIT_POINT;
    }
    
// ----------------------------------------------------------------------------
// CCalenPrintingState::CCalenPrintingState
// C++ Constructor
// ----------------------------------------------------------------------------
CCalenPrintingState::CCalenPrintingState( CCalenController& aController,
                                                RHashSet<TCalenNotification>& aOutstandingNotifications )
    : CCalenState( aController, aOutstandingNotifications )
    {
    TRACE_ENTRY_POINT;
    
    TRACE_EXIT_POINT;
    }
    
// ----------------------------------------------------------------------------
// CCalenPrintingState::CCalenPrintingState
// Destructor
// ----------------------------------------------------------------------------    
CCalenPrintingState::~CCalenPrintingState()
    {
    TRACE_ENTRY_POINT;
    
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenPrintingState::HandleCommandL
// From CCalenState
// ----------------------------------------------------------------------------    
TBool CCalenPrintingState::HandleCommandL( const TCalenCommand& aCommand,
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
// CCalenPrintingState::HandleNotificationL
// From CCalenState
// ----------------------------------------------------------------------------        
void CCalenPrintingState::HandleNotificationL(const TCalenNotification& /*aNotification*/,
                                               CCalenStateMachine& /*aStateMachine*/ )
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    }

// end of file
