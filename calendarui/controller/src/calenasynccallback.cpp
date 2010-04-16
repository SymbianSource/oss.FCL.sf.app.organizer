/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Calendar async callback implementation
 *
*/

 
 // INCLUDE FILES
 
#include "calenasynccallback.h"
#include "calendarui_debug.h" 


// ---------------------------------------------------------------------------
// CalenAsyncCallBack::CalenAsyncCallBack
// C++ constructor
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
CalenAsyncCallBack::CalenAsyncCallBack(TCallBack& aCallBack, CActive::TPriority  aPriority,
                                         CCalenController& aController)
    :CAsyncCallBack(aCallBack, aPriority),
    iController(aController)
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;    
    }

// ---------------------------------------------------------------------------
// CalenAsyncCallBack::NewL
// First phase construction
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//  
CalenAsyncCallBack* CalenAsyncCallBack::NewL(TCallBack& aCallBack, CActive::TPriority  aPriority,
                             CCalenController& aController)
    {
    TRACE_ENTRY_POINT;
  
    CalenAsyncCallBack* self = new( ELeave ) CalenAsyncCallBack( aCallBack, aPriority , aController );

    TRACE_EXIT_POINT;
    return self;
    }
// ---------------------------------------------------------------------------
// CalenAsyncCallBack::~CalenAsyncCallBack
// C++ Destructor
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//	
CalenAsyncCallBack::~CalenAsyncCallBack()
	{
	TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
	}

// ---------------------------------------------------------------------------
// CalenAsyncCallBack::BaseConstructL
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CalenAsyncCallBack::BaseConstructL()
	{
	TRACE_ENTRY_POINT;

    TRACE_EXIT_POINT;
	}
	
// ---------------------------------------------------------------------------
// CalenAsyncCallBack::RunL
// Handle Completion
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//	
void CalenAsyncCallBack::RunL()
	{
	TRACE_ENTRY_POINT;
	
	if(iCallBack.CallBack())
	    {
	    // callback only if any of the command handlers 
	    // request for continuing the executing command
	    CallBack();
	    }
        
	TRACE_EXIT_POINT;
	}


