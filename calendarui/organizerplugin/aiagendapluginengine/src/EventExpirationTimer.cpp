/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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


//debug
#include "calendarui_debug.h"

// INCLUDE FILES
#include "EventExpirationTimer.h"


// ============================ MEMBER FUNCTIONS ===============================

// ---------------------------------------------------------
// CEventExpirationTimer::ConstructL
// Symbian 2nd phase constructor can leave.
// ---------------------------------------------------------
//
void CEventExpirationTimer::ConstructL(void)
{
    TRACE_ENTRY_POINT;
    
    CTimer::ConstructL();
    CActiveScheduler::Add( this );
    
    TRACE_EXIT_POINT;
}

// ---------------------------------------------------------
// CEventExpirationTimer::NewL
// Two-phased constructor.
// Create instance of concrete ECOM interface implementation
// ---------------------------------------------------------
//
CEventExpirationTimer* CEventExpirationTimer::NewL(TCallBack& aCallBack)
{
    TRACE_ENTRY_POINT;
    
    CEventExpirationTimer* self = new( ELeave )CEventExpirationTimer( aCallBack );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    
    TRACE_EXIT_POINT;
    return self;
}

// ---------------------------------------------------------
// CEventExpirationTimer::CEventExpirationTimer
// ?implementation_description
// (other items were commented in a header).
// ---------------------------------------------------------
//
CEventExpirationTimer::CEventExpirationTimer(TCallBack& aCallBack)
 : CTimer( CActive::EPriorityStandard ),
   iCallBack( aCallBack )
{
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
}

// ---------------------------------------------------------
// CEventExpirationTimer::~CEventExpirationTimer
// ?implementation_description
// (other items were commented in a header).
// ---------------------------------------------------------
//
CEventExpirationTimer::~CEventExpirationTimer(void)
{
    TRACE_ENTRY_POINT;
    
    Deque();
    
    TRACE_EXIT_POINT;
}

// ---------------------------------------------------------
// CEventExpirationTimer::RunL
// ?implementation_description
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CEventExpirationTimer::RunL(void)
{
    TRACE_ENTRY_POINT;
    
    iCallBack.CallBack();
    
    TRACE_EXIT_POINT;
}


// End of File
