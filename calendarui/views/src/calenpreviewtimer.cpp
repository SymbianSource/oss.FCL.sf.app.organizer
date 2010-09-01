/*
* Copyright (c) 2002, 2003 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: Expiration timer for timed calendar events.
 *             Always set to expire by the next timed calendar event (for today).  
 *
*/



//debug
#include "calendarui_debug.h"

// INCLUDE FILES
#include "calenpreviewtimer.h"


// ============================ MEMBER FUNCTIONS ===============================

void CCalenPreviewTimer::ConstructL(void)
{
    TRACE_ENTRY_POINT;
    
    CTimer::ConstructL();
    CActiveScheduler::Add( this );
    
    TRACE_EXIT_POINT;
}


CCalenPreviewTimer* CCalenPreviewTimer::NewL(TCallBack& aCallBack)
{
    TRACE_ENTRY_POINT;
    
    CCalenPreviewTimer* self = new( ELeave )CCalenPreviewTimer( aCallBack );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    
    TRACE_EXIT_POINT;
    return self;
}


CCalenPreviewTimer::CCalenPreviewTimer(TCallBack& aCallBack)
 : CTimer( CActive::EPriorityStandard ),
   iCallBack( aCallBack )
{
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
}


CCalenPreviewTimer::~CCalenPreviewTimer(void)
{
    TRACE_ENTRY_POINT;
    
    Deque();
    
    TRACE_EXIT_POINT;
}

void CCalenPreviewTimer::RunL(void)
{
    TRACE_ENTRY_POINT;
    
    iCallBack.CallBack();
    
    TRACE_EXIT_POINT;
}

// End of File
