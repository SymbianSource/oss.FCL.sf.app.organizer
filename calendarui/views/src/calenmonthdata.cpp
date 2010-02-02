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
* Description:   For data of MonthView by one day.
 *
*/



//debug
#include "calendarui_debug.h"

//  INCLUDES
#include "calenmonthdata.h"


// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code, that
// might leave.
//
TCalenMonthData::TCalenMonthData()
    : iDay(TInt64(0)), iHasEvents(EFalse)
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    }

// C++ constructor can NOT contain any code, that
// might leave.
//
// -----------------------------------------------------------------------------
// ?classname::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TCalenMonthData::TCalenMonthData
(TTime aDay)
    : iDay(aDay), iHasEvents(EFalse)
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    }

// Destructor
// -----------------------------------------------------------------------------
// ?classname::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TCalenMonthData::~TCalenMonthData()
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------
// TCalenMonthData::Day
// return day
// (other items were commented in a header).
// ---------------------------------------------------------
//
TTime TCalenMonthData::Day()
    {
    TRACE_ENTRY_POINT;
    
    TRACE_EXIT_POINT;
    return iDay;
    }

// -----------------------------------------------------------------------------
// ?classname::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void TCalenMonthData::SetHasEvents(TBool aHasEvents)
    {
    TRACE_ENTRY_POINT;
    
    iHasEvents = aHasEvents;
    
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// ?classname::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool TCalenMonthData::HasEvents()
    {
    TRACE_ENTRY_POINT;
          
    TRACE_EXIT_POINT;
    return iHasEvents;
    }


// End of File
