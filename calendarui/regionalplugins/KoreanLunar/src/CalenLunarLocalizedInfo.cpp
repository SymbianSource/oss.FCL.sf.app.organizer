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
#include "CalenLunarLocalizedInfo.h"

// ======== MEMBER FUNCTIONS ========


// ---------------------------------------------------------------------------
// CCalenLunarLocalizedInfo::NewL()
// ---------------------------------------------------------------------------
//
CCalenLunarLocalizedInfo* CCalenLunarLocalizedInfo::NewL()
    {
    TRACE_ENTRY_POINT;

    CCalenLunarLocalizedInfo* self = new (ELeave) CCalenLunarLocalizedInfo;
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);

    TRACE_EXIT_POINT;
    return self;
    }

// ---------------------------------------------------------------------------
// CCalenLunarLocalizedInfo::~CCalenLunarLocalizedInfo()
// ---------------------------------------------------------------------------
//
CCalenLunarLocalizedInfo::~CCalenLunarLocalizedInfo()
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------------------------
// CCalenLunarLocalizedInfo::CCalenLunarLocalizedInfo()
// ---------------------------------------------------------------------------
//
CCalenLunarLocalizedInfo::CCalenLunarLocalizedInfo()
    {
    TRACE_ENTRY_POINT;

    iLunarFestival.Set( KNullDesC );
    iSolarFestival.Set( KNullDesC );
    iSolarTerm.Set( KNullDesC );
    iLunarMonthAndDay = KNullDesC;
    iGregorianDate = KNullDesC;

    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------------------------
// CCalenLunarLocalizedInfo::ConstructL()
// ---------------------------------------------------------------------------
//
void CCalenLunarLocalizedInfo::ConstructL()
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------------------------
// CCalenLunarLocalizedInfo::GetField()
// ---------------------------------------------------------------------------
//
const TDesC& CCalenLunarLocalizedInfo::GetField( TField aField )
    {
    TRACE_ENTRY_POINT;

    switch ( aField )
        {
        case ELunarFestival:      return LunarFestival();
        case ESolarFestival:      return SolarFestival();
        case ESolarTerm:          return SolarTerm();
        case ELunarMonthAndDay:   return LunarMonthAndDay();
        case EGregorianDate:      return GregorianDate();
        default:
            ASSERT( EFalse );
            return KNullDesC;
        }

    TRACE_EXIT_POINT;
    }

// End of file

