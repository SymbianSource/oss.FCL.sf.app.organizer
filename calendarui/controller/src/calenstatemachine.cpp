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
#include "calendarui_debug.h"           // Debug macros

#include "calenstatemachine.h"
#include "calenstate.h"
#include "calenidlestate.h"
#include "calenpopulationstate.h"
#include "calendeletingstate.h"
#include "calenbackgroundstate.h"
#include "caleneditingstate.h"
#include "calenexitingstate.h"
#include "calenhelpstate.h"
#include "calenprintingstate.h"
#include "calensendingstate.h"
#include "calensettingsstate.h"
#include "calenviewingstate.h"


const TInt KHashLength = 64;
// ----------------------------------------------------------------------------
// CCalenStateMachine::NewL
// Two phased constructor.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
CCalenStateMachine* CCalenStateMachine::NewL( CCalenController& aController )
    {
    TRACE_ENTRY_POINT;

    CCalenStateMachine* self = new ( ELeave ) CCalenStateMachine( aController );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    TRACE_EXIT_POINT;
    return self;
    }

 // ----------------------------------------------------------------------------
// CCalenStateMachine::CCalenStateMachine
// Constructor.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//      
CCalenStateMachine::CCalenStateMachine( CCalenController& aController )
    : iController( aController ) , 
      iOutstandingNotifications(&::HashCalenNotificationFunction,&::HashCalenNotificationIdentityRelation)
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenStateMachine::ConstructL
// Second phase of construction.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//        
void CCalenStateMachine::ConstructL()
    {
    TRACE_ENTRY_POINT;

    CCalenState* state = NULL;

    iOutstandingNotifications.ReserveL(KHashLength);
    state = CCalenIdleState::NewLC( iController, iOutstandingNotifications );
    iStates.InsertL( state, ECalenIdleState );
    CleanupStack::Pop( state );
    
    state = CCalenPopulationState::NewLC( iController, iOutstandingNotifications);
    iStates.InsertL( state, ECalenPopulationState );
    CleanupStack::Pop( state );
    
    state = CCalenBackgroundState::NewLC( iController, iOutstandingNotifications );
    iStates.InsertL( state, ECalenBackgroundState );
    CleanupStack::Pop( state );
    
    state = CCalenViewingState::NewLC( iController, iOutstandingNotifications );
    iStates.InsertL( state, ECalenViewingState );
    CleanupStack::Pop( state );
    
    state = CCalenEditingState::NewLC( iController, iOutstandingNotifications );
    iStates.InsertL( state, ECalenEditingState );
    CleanupStack::Pop( state );
    
    state = CCalenDeletingState::NewLC( iController, iOutstandingNotifications );
    iStates.InsertL( state, ECalenDeletingState );
    CleanupStack::Pop( state );
    
    state = CCalenPrintingState::NewLC( iController, iOutstandingNotifications );
    iStates.InsertL( state, ECalenPrintingState );
    CleanupStack::Pop( state );
    
    state = CCalenSendingState::NewLC( iController, iOutstandingNotifications );
    iStates.InsertL( state, ECalenSendingState );
    CleanupStack::Pop( state );
    
    state = CCalenSettingsState::NewLC( iController, iOutstandingNotifications );
    iStates.InsertL( state, ECalenSettingsState );
    CleanupStack::Pop( state );
    
    state = CCalenHelpState::NewLC( iController, iOutstandingNotifications );
    iStates.InsertL( state, ECalenHelpState );
    CleanupStack::Pop( state );

    state = CCalenExitingState::NewLC( iController, iOutstandingNotifications );
    iStates.InsertL( state, ECalenExitingState );
    CleanupStack::Pop( state );

    ASSERT( iStates.Count() == KCalenLastState );

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenStateMachine::~CCalenStateMachine
// Destructor.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
CCalenStateMachine::~CCalenStateMachine()
    {
    TRACE_ENTRY_POINT;
    
    iOutstandingNotifications.Close();
    iStates.ResetAndDestroy();

    TRACE_EXIT_POINT;
    }     

// ----------------------------------------------------------------------------
// CCalenStateMachine::HandleNotification
// Notification handler interface.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenStateMachine::HandleNotification(const TCalenNotification aNotification )
    {
    TRACE_ENTRY_POINT;

	TRAP_IGNORE(iStates[iCurrentState]->HandleNotificationL( aNotification, *this)); 
   
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenStateMachine::CurrentState
// Returns the current state in which calendar is.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
CCalenStateMachine::TCalenStateIndex CCalenStateMachine::CurrentState()
{
	return iCurrentState;
}

// ----------------------------------------------------------------------------
// CCalenStateMachine::HandleCommandL
// Command handler interface.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
TBool CCalenStateMachine::HandleCommandL( const TCalenCommand& aCommand )
    {
    TRACE_ENTRY_POINT;  
    
    TBool cmdUsed = iStates[iCurrentState]->HandleCommandL( aCommand, *this);
    
    TRACE_EXIT_POINT;
    return cmdUsed;
    }

// ----------------------------------------------------------------------------
// CCalenStateMachine::CancelPreviousCmd
// Interface to cancel the previous command
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenStateMachine::CancelPreviousCmd()
	{
	TRACE_ENTRY_POINT;
	
	TCalenStateIndex previousState = iStates[iCurrentState]->PreviousState();
	iStates[iCurrentState]->CancelExecutingCmd();
	
	if(previousState != iCurrentState)
		{
		iCurrentState = previousState;
		}
	
	TRACE_EXIT_POINT;
	}

// ----------------------------------------------------------------------------
// CCalenStateMachine::GetCurrentState
// Interface to get the current state
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//	
CCalenStateMachine::TCalenStateIndex CCalenStateMachine::GetCurrentState()
	{
	TRACE_ENTRY_POINT;
	TRACE_EXIT_POINT;
	
	return iCurrentState;
	}

// ----------------------------------------------------------------------------
// CCalenStateMachine::SetCurrentState
// Interface to set the current state
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//		
void CCalenStateMachine::SetCurrentState(const TCalenStateIndex& aState)
	{
	TRACE_ENTRY_POINT;
	
	iCurrentState = aState;
	
	TRACE_EXIT_POINT;
	}

// ----------------------------------------------------------------------------
// CCalenStateMachine::SetPreviousState
// Interface to set previous state
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenStateMachine::SetCurrentPreviousState(const TCalenStateIndex& aState)
	{
	TRACE_ENTRY_POINT;
		
	iStates[iCurrentState]->SetPreviousState(aState);
	
	TRACE_EXIT_POINT;
	}

// ----------------------------------------------------------------------------
// CCalenStateMachine::ActivateCurrentStateL
// Interface to activate the current state
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//	
void CCalenStateMachine::ActivateCurrentStateL()
	{
	TRACE_ENTRY_POINT;
		
	iStates[iCurrentState]->HandleStateActivationL(*this);
		
	TRACE_EXIT_POINT;	
	}
// End of file
