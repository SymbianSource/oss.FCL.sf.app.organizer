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
* Description:  CalenMonthData implementation.
*
*/


//debug
#include "calendarui_debug.h"

//  INCLUDES
#include "calenmonthdata.h"


// ================= MEMBER FUNCTIONS =======================

// ----------------------------------------------------------------------------
// CalenMonthData::CalenMonthData
// 
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
CalenMonthData::CalenMonthData()
    : mDay(QDateTime()), mHasEvents(false)
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CalenMonthData::CalenMonthData
// 
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
CalenMonthData::CalenMonthData
(QDateTime day)
    : mDay(day), mHasEvents(false)
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CalenMonthData::~CalenMonthData
// 
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
CalenMonthData::~CalenMonthData()
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------
// CalenMonthData::Day
// return day
// (other items were commented in a header).
// ---------------------------------------------------------
//
QDateTime CalenMonthData::Day()
    {
    TRACE_ENTRY_POINT;
    
    TRACE_EXIT_POINT;
    return mDay;
    }

// -----------------------------------------------------------------------------
// ?classname::SetHasEvents
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CalenMonthData::SetHasEvents(bool hasEvents)
    {
    TRACE_ENTRY_POINT;
    
    mHasEvents = hasEvents;
    
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// ?classname::HasEvents
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
bool CalenMonthData::HasEvents()
    {
    TRACE_ENTRY_POINT;
    
    TRACE_EXIT_POINT;
    return mHasEvents;
    }

void CalenMonthData::setActive(bool isActive)
    {
    mIsActive = isActive;
    }

bool CalenMonthData::isActive()
    {
    return mIsActive;
    }
// End of File
