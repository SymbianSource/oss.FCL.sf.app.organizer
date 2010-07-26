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
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "calenlunarlocalizedinfoTraces.h"
#endif


// -----------------------------------------------------------------------------
// CCalenLunarLocalizedInfo::NewL
// -----------------------------------------------------------------------------
//
EXPORT_C CCalenLunarLocalizedInfo* CCalenLunarLocalizedInfo::NewL()
    {
    OstTraceFunctionEntry0( CCALENLUNARLOCALIZEDINFO_NEWL_ENTRY );
    CCalenLunarLocalizedInfo* self = new (ELeave) CCalenLunarLocalizedInfo;
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    
    OstTraceFunctionExit0( CCALENLUNARLOCALIZEDINFO_NEWL_EXIT );
    return self;
    }
// -----------------------------------------------------------------------------
// CCalenLunarLocalizedInfo::~CCalenLunarLocalizedInfo
// -----------------------------------------------------------------------------
//
EXPORT_C CCalenLunarLocalizedInfo::~CCalenLunarLocalizedInfo()
    {
    OstTraceFunctionEntry0( CCALENLUNARLOCALIZEDINFO_CCALENLUNARLOCALIZEDINFO_ENTRY );
    OstTraceFunctionExit0( CCALENLUNARLOCALIZEDINFO_CCALENLUNARLOCALIZEDINFO_EXIT );
    }

// -----------------------------------------------------------------------------
// CCalenLunarLocalizedInfo::CCalenLunarLocalizedInfo
// -----------------------------------------------------------------------------
//
CCalenLunarLocalizedInfo::CCalenLunarLocalizedInfo()
    {
    OstTraceFunctionEntry0( DUP1_CCALENLUNARLOCALIZEDINFO_CCALENLUNARLOCALIZEDINFO_ENTRY );
    iFestival.Set(KNullDesC);
    iSolarTerm.Set(KNullDesC);
    iAnimalYear.Set(KNullDesC);
    iLunarYear = KNullDesC;
    iLunarMonthAndDay = KNullDesC;
    iFullLunarDate = KNullDesC;
    iGregorianDate = KNullDesC; 
    OstTraceFunctionExit0( DUP1_CCALENLUNARLOCALIZEDINFO_CCALENLUNARLOCALIZEDINFO_EXIT );
    }

// -----------------------------------------------------------------------------
// CCalenLunarLocalizedInfo::ConstructL
// -----------------------------------------------------------------------------
//
void CCalenLunarLocalizedInfo::ConstructL()
    {
    OstTraceFunctionEntry0( CCALENLUNARLOCALIZEDINFO_CONSTRUCTL_ENTRY );
    OstTraceFunctionExit0( CCALENLUNARLOCALIZEDINFO_CONSTRUCTL_EXIT );
    }

// -----------------------------------------------------------------------------
// CCalenLunarLocalizedInfo::GetField
// -----------------------------------------------------------------------------
//
EXPORT_C const TDesC& CCalenLunarLocalizedInfo::GetField( TField aField )
    {
    OstTraceFunctionEntry0( CCALENLUNARLOCALIZEDINFO_GETFIELD_ENTRY );
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
            OstTraceFunctionExit0( CCALENLUNARLOCALIZEDINFO_GETFIELD_EXIT );
            return KNullDesC;
        }
    }

