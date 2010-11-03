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
* Description:  TCalenWeekHour has data by one hour.
 *
*/


//debug
#include "calendarui_debug.h"

//  INCLUDES
#include "calenweekhour.h"

// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code, that
// might leave.
//
TCalenWeekHour::TCalenWeekHour()
    : iDataType(ENoData)
    {
    TRACE_ENTRY_POINT; 
    
    iIconIndex = 0;
    
    for(TInt i=0; i < KEventsInHour; ++i)
        {
        iEvent[i] = ENoEvent;
        } 
    
    TRACE_EXIT_POINT; 
    }

// ---------------------------------------------------------
// TCalenWeekHour::DataType
// Flag of No data or Timed or Non-timed note
// (other items were commented in a header).
// ---------------------------------------------------------
//
TCalenWeekHour::TDataType TCalenWeekHour::DataType() const
    {
    TRACE_ENTRY_POINT; 
    
    TRACE_EXIT_POINT; 
    return iDataType;
    }

// ---------------------------------------------------------
// TCalenWeekHour::Event
// Return event type
// (other items were commented in a header).
// ---------------------------------------------------------
//
TUint32 TCalenWeekHour::Event
(TInt aMinutes) const   // 0 : 0-9minutes 1: 10-19minutes...,
    {
    TRACE_ENTRY_POINT; 
    
    
    TRACE_EXIT_POINT; 
    return (TUint32) iEvent[aMinutes];
    }

// ---------------------------------------------------------
// TCalenWeekHour::SetEvent
// Set a event type
// (other items were commented in a header).
// ---------------------------------------------------------
//
void TCalenWeekHour::SetEvent
(TInt aMinutes,     // 0 : 0-9minutes 1: 10-19minutes...,
        TUint32 aType) // ENoEvent | EPublicEvent | etc...
    {
    TRACE_ENTRY_POINT; 
    
    iEvent[aMinutes] = (TUint32)aType;
    iDataType = EHasBar;
    
    TRACE_EXIT_POINT; 
    }

// ---------------------------------------------------------
// TCalenWeekHour::IconIndex
// Return a Icon index
// (other items were commented in a header).
// ---------------------------------------------------------
//
TInt TCalenWeekHour::IconIndex() const
    {
    TRACE_ENTRY_POINT; 
    
    __ASSERT_ALWAYS(iDataType == EHasIcon, User::Invariant());
    
    TRACE_EXIT_POINT; 
    return iIconIndex;
    }

// ---------------------------------------------------------
// TCalenWeekHour::SetIconIndex
// Set a Icon index
// (other items were commented in a header).
// ---------------------------------------------------------
//
void TCalenWeekHour::SetIconIndex
(TInt aIconIndex)   // Icon index
    {
    TRACE_ENTRY_POINT; 
    
    iIconIndex = aIconIndex;
    iDataType = EHasIcon;
    
    TRACE_EXIT_POINT; 
    }

// End of File
