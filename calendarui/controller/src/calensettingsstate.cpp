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
#include "calensettingsstate.h"
#include "calendarui_debug.h"           // Debug macros
#include "calencontroller.h"
#include "calenstatemachine.h"
#include "calennotifier.h"
#include "calenviewmanager.h"

// ----------------------------------------------------------------------------
// CCalenSettingsState::NewLC
// First stage construction
// ----------------------------------------------------------------------------
CCalenSettingsState* CCalenSettingsState::NewLC( CCalenController& aController,
                    RHashSet<TCalenNotification>& aOutstandingNotifications )
    {
    TRACE_ENTRY_POINT;

    CCalenSettingsState* self = new ( ELeave ) CCalenSettingsState( aController,
                                                    aOutstandingNotifications );
    CleanupStack::PushL( self );
    self->ConstructL();

    TRACE_EXIT_POINT;
    return self;
    }

// ----------------------------------------------------------------------------
// CCalenSettingsState::ConstructL
// Second stage construction
// ----------------------------------------------------------------------------
void CCalenSettingsState::ConstructL()
    {
    TRACE_ENTRY_POINT;
    BaseConstructL();
    
    TRACE_EXIT_POINT;
    }
    
// ----------------------------------------------------------------------------
// CCalenSettingsState::CCalenSettingsState
// C++ Constructor
// ----------------------------------------------------------------------------
CCalenSettingsState::CCalenSettingsState( CCalenController& aController,
                    RHashSet<TCalenNotification>& aOutstandingNotifications )
    : CCalenState( aController, aOutstandingNotifications )
    {
    TRACE_ENTRY_POINT;
    
    TRACE_EXIT_POINT;
    }
    
// ----------------------------------------------------------------------------
// CCalenSettingsState::CCalenSettingsState
// Destructor
// ----------------------------------------------------------------------------    
CCalenSettingsState::~CCalenSettingsState()
    {
    TRACE_ENTRY_POINT;
    
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenSettingsState::HandleCommandL
// From CCalenState
// ----------------------------------------------------------------------------    
TBool CCalenSettingsState::HandleCommandL( const TCalenCommand& aCommand,
                                        CCalenStateMachine& aStateMachine  )
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
            SetCurrentState( aStateMachine, CCalenStateMachine::ECalenIdleState );
            ActivateCurrentStateL(aStateMachine);
            cmdUsed = ETrue;
            }
            break;
        case ECalenMonthView:
        case ECalenDayView:
        case ECalenLandscapeDayView:
        case ECalenStartActiveStep:
            {
            // set previous state to idle
            CCalenStateMachine::TCalenStateIndex cachedState = CCalenStateMachine::ECalenIdleState;
            SetCurrentState( aStateMachine, CCalenStateMachine::ECalenPopulationState );
            SetCurrentPreviousState( aStateMachine, cachedState );
            ActivateCurrentStateL(aStateMachine);               
            cmdUsed = ETrue;
            }
            break;
        default:
            break;
        }
    if(cmdUsed)
        {
        RequestCallbackL( handler, aCommand );
        }

    TRACE_EXIT_POINT;
    return cmdUsed;
    }

// ----------------------------------------------------------------------------
// CCalenSettingsState::HandleNotificationL
// From CCalenState
// ----------------------------------------------------------------------------        
void CCalenSettingsState::HandleNotificationL(const TCalenNotification& aNotification,
                                               CCalenStateMachine& aStateMachine )
    {
    TRACE_ENTRY_POINT;
    
    switch( aNotification )
        {
        case ECalenNotifyPluginEnabledDisabled:
            {
			 CCalenState::HandleNotificationL( aNotification, aStateMachine );
            }
            break;
        case ECalenNotifySettingsClosed:
            {
            // Settings closed, go back to previous state.
            SetCurrentState( aStateMachine, iPreviousState );
            iOutstandingNotifications.InsertL(aNotification);
            ActivateCurrentStateL(aStateMachine);
            }
            break;
        case ECalenNotifySystemLocaleChanged:
            {
            CCalenState::HandleNotificationL( aNotification, aStateMachine );
            }
            break;
        default:
            // default is defer all other notifications when we are in setting state
            iOutstandingNotifications.InsertL(aNotification);
            break;
        }
    
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenSettingsState::HandleStateActivationL
// Behavior when state is activated.
// ----------------------------------------------------------------------------
void CCalenSettingsState::HandleStateActivationL()
    {
    TRACE_ENTRY_POINT;
    
    if( iOutstandingNotifications.FindL(ECalenNotifySettingsCRepKeyChanged) )
        {
        iController.Notifier().BroadcastApprovedNotification( ECalenNotifySettingsCRepKeyChanged );
        }
    
    TRACE_EXIT_POINT;
    }

 // end of file
