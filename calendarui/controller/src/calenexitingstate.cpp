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
#include "calenexitingstate.h"
#include "calendarui_debug.h"           // Debug macros
#include "calencontroller.h"
#include "calenstatemachine.h"
#include "calennotifier.h"

// ----------------------------------------------------------------------------
// CCalenExitingState::NewLC
// First stage construction
// ----------------------------------------------------------------------------
CCalenExitingState* CCalenExitingState::NewLC( CCalenController& aController,
                                                       RHashSet<TCalenNotification>&  aOutstandingNotifications )
    {
    TRACE_ENTRY_POINT;

    CCalenExitingState* self = new ( ELeave ) CCalenExitingState( aController,aOutstandingNotifications);
    CleanupStack::PushL( self );
    self->ConstructL();

    TRACE_EXIT_POINT;
    return self;
    }

// ----------------------------------------------------------------------------
// CCalenExitingState::ConstructL
// Second stage construction
// ----------------------------------------------------------------------------
void CCalenExitingState::ConstructL()
    {
    TRACE_ENTRY_POINT;
    BaseConstructL();
    
    TRACE_EXIT_POINT;
    }
    
// ----------------------------------------------------------------------------
// CCalenExitingState::CCalenExitingState
// C++ Constructor
// ----------------------------------------------------------------------------
CCalenExitingState::CCalenExitingState( CCalenController& aController,
                                                    RHashSet<TCalenNotification>&  aOutstandingNotifications )
    : CCalenState( aController, aOutstandingNotifications )
    {
    TRACE_ENTRY_POINT;
    
    TRACE_EXIT_POINT;
    }
    
// ----------------------------------------------------------------------------
// CCalenExitingState::CCalenExitingState
// Destructor
// ----------------------------------------------------------------------------    
CCalenExitingState::~CCalenExitingState()
    {
    TRACE_ENTRY_POINT;
    
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenExitingState::HandleCommandL
// From CCalenState
// ----------------------------------------------------------------------------    
TBool CCalenExitingState::HandleCommandL( const TCalenCommand& /*aCommand*/,
                                        CCalenStateMachine& /*aStateMachine*/  )
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    return EFalse;
    }

// ----------------------------------------------------------------------------
// CCalenExitingState::HandleNotificationL
// From CCalenState
// ----------------------------------------------------------------------------        
void CCalenExitingState::HandleNotificationL(const TCalenNotification& /*aNotification*/,
                                              CCalenStateMachine& /*aStateMachine*/ )
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    }  


 // end of file
