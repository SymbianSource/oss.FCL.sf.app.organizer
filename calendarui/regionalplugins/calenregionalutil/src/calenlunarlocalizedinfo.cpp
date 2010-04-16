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
#include "CalenLunarLocalizedInfo.h"

// -----------------------------------------------------------------------------
// CCalenLunarLocalizedInfo::NewL
// -----------------------------------------------------------------------------
//
EXPORT_C CCalenLunarLocalizedInfo* CCalenLunarLocalizedInfo::NewL()
    {
    TRACE_ENTRY_POINT;
    
    CCalenLunarLocalizedInfo* self = new (ELeave) CCalenLunarLocalizedInfo;
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    
    TRACE_EXIT_POINT;
    return self;
    }
// -----------------------------------------------------------------------------
// CCalenLunarLocalizedInfo::~CCalenLunarLocalizedInfo
// -----------------------------------------------------------------------------
//
EXPORT_C CCalenLunarLocalizedInfo::~CCalenLunarLocalizedInfo()
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenLunarLocalizedInfo::CCalenLunarLocalizedInfo
// -----------------------------------------------------------------------------
//
CCalenLunarLocalizedInfo::CCalenLunarLocalizedInfo()
    {
    TRACE_ENTRY_POINT;
    
    iFestival.Set(KNullDesC);
    iSolarTerm.Set(KNullDesC);
    iAnimalYear.Set(KNullDesC);
    iLunarYear = KNullDesC;
    iLunarMonthAndDay = KNullDesC;
    iFullLunarDate = KNullDesC;
    iGregorianDate = KNullDesC; 
    
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenLunarLocalizedInfo::ConstructL
// -----------------------------------------------------------------------------
//
void CCalenLunarLocalizedInfo::ConstructL()
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenLunarLocalizedInfo::GetField
// -----------------------------------------------------------------------------
//
EXPORT_C const TDesC& CCalenLunarLocalizedInfo::GetField( TField aField )
    {
    TRACE_ENTRY_POINT;
    
    switch ( aField )
        {
        case EFestival:           return Festival();
        case ESolarTerm:          return SolarTerm();            
        case EFullLunarDate:      return FullLunarDate();            
        case ELunarYear:          return LunarYear();            
        case ELunarMonthAndDay:   return LunarMonthAndDay();
        case EAnimalYear:         return AnimalYear();
        case EGregorianDate:      return GregorianDate();
        default:
            ASSERT( EFalse );
            return KNullDesC;
        }
    
    TRACE_EXIT_POINT;
    }

