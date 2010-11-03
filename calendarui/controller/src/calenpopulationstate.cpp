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
#include "calenpopulationstate.h"
#include "calendarui_debug.h"           // Debug macros
#include "calencontroller.h"
#include "calenstatemachine.h"
#include "calennotifier.h"

// ----------------------------------------------------------------------------
// CCalenPopulationState::NewLC
// First stage construction
// ----------------------------------------------------------------------------
CCalenPopulationState* CCalenPopulationState::NewLC( CCalenController& aController,
                                                    RHashSet<TCalenNotification>& aOutstandingNotifications )
    {
    TRACE_ENTRY_POINT;

    CCalenPopulationState* self = new( ELeave ) CCalenPopulationState( aController, aOutstandingNotifications );
    CleanupStack::PushL( self );
    self->ConstructL();

    TRACE_EXIT_POINT;
    return self;
    }

// ----------------------------------------------------------------------------
// CCalenPopulationState::ConstructL
// Second stage construction
// ----------------------------------------------------------------------------
void CCalenPopulationState::ConstructL()
    {
    TRACE_ENTRY_POINT;
    BaseConstructL();   
    
    TRACE_EXIT_POINT;
    }
    
// ----------------------------------------------------------------------------
// CCalenPopulationState::CCalenPopulationState
// C++ Constructor
// ----------------------------------------------------------------------------
CCalenPopulationState::CCalenPopulationState( CCalenController& aController,
                                                          RHashSet<TCalenNotification>&  aOutstandingNotifications )
    : CCalenState( aController, aOutstandingNotifications )
    {
    TRACE_ENTRY_POINT;
    
    TRACE_EXIT_POINT;
    }
    
// ----------------------------------------------------------------------------
// CCalenPopulationState::CCalenPopulationState
// Destructor
// ----------------------------------------------------------------------------    
CCalenPopulationState::~CCalenPopulationState()
    {
    TRACE_ENTRY_POINT;
    
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenPopulationState::HandleCommandL
// From CCalenState
// ----------------------------------------------------------------------------    
TBool CCalenPopulationState::HandleCommandL( const TCalenCommand& aCommand,
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
// CCalenPopulationState::HandleNotificationL
// From CCalenState
// ----------------------------------------------------------------------------        
void CCalenPopulationState::HandleNotificationL(const TCalenNotification& aNotification,
                                                  CCalenStateMachine& aStateMachine )
    {
    TRACE_ENTRY_POINT;
    
    switch ( aNotification )
        {
        case ECalenNotifyViewPopulationComplete:
            {
            SetCurrentState( aStateMachine, iPreviousState );
            // don't set us as the previous state for the state we're moving back to.
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
