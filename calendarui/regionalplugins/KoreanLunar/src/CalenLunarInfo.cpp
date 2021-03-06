/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies).
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
*  Description : Class looking after alarm fields for forms.
*
*/

//debug
#include "calendarui_debug.h"

//user includes
#include "CalenLunarInfo.h"

// ======== MEMBER FUNCTIONS ========


// ---------------------------------------------------------------------------
// TCalenLunarInfo::HasLunarFestival()
// ---------------------------------------------------------------------------
//
TBool TCalenLunarInfo::HasLunarFestival()
    {
    TRACE_ENTRY_POINT;

    TRACE_EXIT_POINT;
    return iLunarFestival >= 0;
    }

// ---------------------------------------------------------------------------
// TCalenLunarInfo::HasSolarFestival()
// ---------------------------------------------------------------------------
//
TBool TCalenLunarInfo::HasSolarFestival()
    {
    TRACE_ENTRY_POINT;

    TRACE_EXIT_POINT;
    return iSolarFestival >= 0;
    }

// ---------------------------------------------------------------------------
// TCalenLunarInfo::HasSolarTerm()
// ---------------------------------------------------------------------------
//
TBool TCalenLunarInfo::HasSolarTerm()
    {
    TRACE_ENTRY_POINT;

    TRACE_EXIT_POINT;
    return iSolarTerm >= 0;
    }

// End of file

