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
* Description:   Calendar Lunar Plugin 
*
*/



#include "calendarui_debug.h"
#include "calenlunarinfo.h"

// -----------------------------------------------------------------------------
// TCalenLunarInfo::HasFestival
// -----------------------------------------------------------------------------
//
TBool TCalenLunarInfo::HasFestival()
    {
    TRACE_ENTRY_POINT;
    
    TRACE_EXIT_POINT;
    return iFestival >= 0;
    }

// -----------------------------------------------------------------------------
// TCalenLunarInfo::HasSolarTerm
// -----------------------------------------------------------------------------
//
TBool TCalenLunarInfo::HasSolarTerm()
    {
    TRACE_ENTRY_POINT;
    
    TRACE_EXIT_POINT;
    return iSolarTerm >= 0;
    }

// -----------------------------------------------------------------------------
// TCalenLunarInfo::AnimalYear
// -----------------------------------------------------------------------------
//

EXPORT_C TCalenLunarInfo::TAnimalYear TCalenLunarInfo::AnimalYear()
    {
    TRACE_ENTRY_POINT;
    
    ASSERT( EYearOfRat < EYearOfPig );
    ASSERT( EYearOfRat <= iTerrestialBranch );
    ASSERT( iTerrestialBranch <= EYearOfPig );
    
    // Ordering is same for animal years and terrestial branches
    TRACE_EXIT_POINT;
    return static_cast<TCalenLunarInfo::TAnimalYear>( iTerrestialBranch );
    }
