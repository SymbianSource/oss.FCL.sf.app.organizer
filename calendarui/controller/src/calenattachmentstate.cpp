/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:
*
*/

// includes
#include "calenattachmentstate.h"
#include "calendarui_debug.h"
#include "calencontroller.h"
#include "calenstatemachine.h"
#include "calennotifier.h"

// ----------------------------------------------------------------------------
// CCalenAttachmentState::NewLC
// First stage construction
// ----------------------------------------------------------------------------
CCalenAttachmentState* CCalenAttachmentState::NewLC( CCalenController& aController,
                         RHashSet<TCalenNotification>& aOutstandingNotifications )
    {
    TRACE_ENTRY_POINT;

    CCalenAttachmentState* self = new ( ELeave ) CCalenAttachmentState( aController,
                                                          aOutstandingNotifications );
    CleanupStack::PushL( self );
    self->ConstructL();

    TRACE_EXIT_POINT;
    return self;
    }

// ----------------------------------------------------------------------------
// CCalenAttachmentState::ConstructL
// Second stage construction
// ----------------------------------------------------------------------------
void CCalenAttachmentState::ConstructL()
    {
    TRACE_ENTRY_POINT;
    BaseConstructL();
    
    TRACE_EXIT_POINT;
    }
    
// ----------------------------------------------------------------------------
// CCalenAttachmentState::CCalenAttachmentState
// C++ Constructor
// ----------------------------------------------------------------------------
CCalenAttachmentState::CCalenAttachmentState( CCalenController& aController, 
            RHashSet<TCalenNotification>& aOutstandingNotifications )
    : CCalenState( aController, aOutstandingNotifications )
    {
    TRACE_ENTRY_POINT;
    
    TRACE_EXIT_POINT;
    }
    
// ----------------------------------------------------------------------------
// CCalenAttachmentState::CCalenAttachmentState
// Destructor
// ----------------------------------------------------------------------------    
CCalenAttachmentState::~CCalenAttachmentState()
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenAttachmentState::HandleCommandL
// From CCalenState
// ----------------------------------------------------------------------------    
TBool CCalenAttachmentState::HandleCommandL( const TCalenCommand& aCommand,
                                           CCalenStateMachine& aStateMachine )
    {
    TRACE_ENTRY_POINT; 
    
    TInt cmd = aCommand.Command();
    MCalenCommandHandler* handler = iController.GetCommandHandlerL(cmd);
	    
	ASSERT(handler);
     
    TBool cmdUsed = EFalse;
    switch(cmd)
        {
        case ECalenFasterAppExit:
            {
            SetCurrentState( aStateMachine, CCalenStateMachine::ECalenIdleState );
            ActivateCurrentStateL(aStateMachine);
            cmdUsed = ETrue;
            }
            break;
        case ECalenMissedEventViewFromIdle:
            {
            SetCurrentState( aStateMachine, CCalenStateMachine::ECalenIdleState );
            ActivateCurrentStateL(aStateMachine);
            cmdUsed = ETrue;
            }
            break;    
        case ECalenAddAttachmentFromViewer:
            {
            cmdUsed = ETrue;
            }
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
// CCalenAttachmentState::HandleNotificationL
// From CCalenState
// ----------------------------------------------------------------------------        
void CCalenAttachmentState::HandleNotificationL(const TCalenNotification& aNotification,
                                                 CCalenStateMachine& aStateMachine )
    {
    TRACE_ENTRY_POINT;
    
    switch( aNotification )
        {
        case ECalenNotifyDialogClosed:
        case ECalenNotifyAttachmentAdded:
        case ECalenNotifyAttachmentRemoved:    
        case ECalenNotifyAttachmentViewerClosed:    
        	{
        	SetCurrentState( aStateMachine, iPreviousState );
        	// We set iOutstandingNotifications for two reasons.
        	// 1. The new state i.e. the state we are moving back can have notification info.
        	// 2. When we move to newstate we broadcast all notification from iOutstandingNotifications
        	//     and inform the registered notifiers.
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
// CCalenAttachmentState::HandleStateActivationL
// Behavior when state is activated.
// ----------------------------------------------------------------------------
void CCalenAttachmentState::HandleStateActivationL(CCalenStateMachine& /*aStateMachine*/)
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    }

 // end of file
