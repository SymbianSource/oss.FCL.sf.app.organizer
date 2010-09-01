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
#include "calenlunarinfoprovider.h"
#include "calenlunarinfo.h"
#include "calensolarterms.h"

#include <calendarconverter.h>


// ---------------------------------------------------------
// CCalenCalanderConverter::CalculateCelestialIndex
// Calculates a Heavenly stem's index and a Terrestrial branch's index
// ---------------------------------------------------------
//
static void CelestialIndex(const TChineseDate& aChineseDate,
                           TInt& aCelestialIndex, 
                           TInt& aTerrestrialIndex)
    {
    TRACE_ENTRY_POINT;
    
    aCelestialIndex = aChineseDate.iYear % 10;
    
    if (aCelestialIndex == 0)
        {
        aCelestialIndex = 9;
        }
    else
        {
        aCelestialIndex--;
        }

    aTerrestrialIndex = aChineseDate.iYear % 12;
    if (aTerrestrialIndex == 0)
        {
        aTerrestrialIndex = 11;
        }
    else
        {
        aTerrestrialIndex--;
        }
    
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenCalanderConverter::ChineseFestival
// -----------------------------------------------------------------------------
//
static TCalenLunarInfo::TFestival ChineseFestival(
    const TChineseDate& aChineseDate, 
    const TChineseDate& aNextDate)
    {
    TRACE_ENTRY_POINT;
    
    TCalenLunarInfo::TFestival index = TCalenLunarInfo::ENoFestival;

    if (!aChineseDate.iLeapMonth)
        {
        if (aChineseDate.iMonth == 1 && aChineseDate.iDay == 1)
            {
            index = TCalenLunarInfo::EFestivalSpring;
            }
        else if (aChineseDate.iMonth == 1 && aChineseDate.iDay == 15)
            {
            index = TCalenLunarInfo::EFestivalLantern;
            }
        else if (aChineseDate.iMonth == 5 && aChineseDate.iDay == 5)
            {
            index = TCalenLunarInfo::EFestivalDragonBoat;
            }
        else if (aChineseDate.iMonth == 7 && aChineseDate.iDay == 7)
            {
            index = TCalenLunarInfo::EFestivalSeventhNight;
            }
        else if (aChineseDate.iMonth == 7 && aChineseDate.iDay == 15)
            {
            index = TCalenLunarInfo::EFestivalGhost;
            }
        else if (aChineseDate.iMonth == 8 && aChineseDate.iDay == 15)
            {
            index = TCalenLunarInfo::EFestivalMidAutumn;
            }
        else if (aChineseDate.iMonth == 9 && aChineseDate.iDay == 9)
            {
            index = TCalenLunarInfo::EFestivalDoubleNinth;
            }
        else if (aChineseDate.iMonth == 12 && aChineseDate.iDay == 8)
            {
            index = TCalenLunarInfo::EFestivalTwelfthMonth;
            }
        else
            {
            // nothing
            }
        }
    if (!aNextDate.iLeapMonth &&
        aNextDate.iMonth == 1 && aNextDate.iDay == 1)
        {
        index = TCalenLunarInfo::EFestivalNewYearEve;
        }
    
    TRACE_EXIT_POINT;
    return index;
    }

// -----------------------------------------------------------------------------
// CCalenLunarInfoProvider::NewL
// -----------------------------------------------------------------------------
//
EXPORT_C CCalenLunarInfoProvider* CCalenLunarInfoProvider::NewL(RFs& aFs)
    {
    TRACE_ENTRY_POINT;
    
    CCalenLunarInfoProvider* self = new (ELeave) CCalenLunarInfoProvider();
    CleanupStack::PushL(self);
    self->ConstructL(aFs);
    CleanupStack::Pop(self);
    
    TRACE_EXIT_POINT;
    return self;
    }

// -----------------------------------------------------------------------------
// CCalenLunarInfoProvider::~CCalenLunarInfoProvider
// -----------------------------------------------------------------------------
//
EXPORT_C CCalenLunarInfoProvider::~CCalenLunarInfoProvider()
    {
    TRACE_ENTRY_POINT;
    
    delete iSolarTerms; 
    delete iConverter;
    
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenLunarInfoProvider::CCalenLunarInfoProvider
// -----------------------------------------------------------------------------
//
CCalenLunarInfoProvider::CCalenLunarInfoProvider()
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenLunarInfoProvider::ConstructL
// -----------------------------------------------------------------------------
//
void CCalenLunarInfoProvider::ConstructL(RFs& aFs)
    {
    TRACE_ENTRY_POINT;

    iConverter = CChineseCalendarConverter::NewL();
    iSolarTerms = CCalenSolarTerms::NewL(aFs);
    
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenLunarInfoProvider::GetLunarInfoL
// -----------------------------------------------------------------------------
//
EXPORT_C TCalenLunarInfo CCalenLunarInfoProvider::GetLunarInfoL( const TTime& aDay ) 
    {
    TRACE_ENTRY_POINT;
       
    TDateTime dayDt = aDay.DateTime();

    TTime nextDay = aDay + TTimeIntervalDays(1);
    TDateTime nextDayDt = nextDay.DateTime();

    TChineseDate chineseDate;
    TChineseDate nextChineseDate;
    
    iConverter->DateTimeToChineseL(dayDt, chineseDate);
    iConverter->DateTimeToChineseL(nextDayDt, nextChineseDate);
    
    TCalenLunarInfo info;
    info.iLunarDate = chineseDate;
    info.iGregorianDate = aDay;
    info.iFestival = ChineseFestival( chineseDate, nextChineseDate );
    info.iSolarTerm = iSolarTerms->CheckSolarTermDateL( dayDt );
    CelestialIndex( chineseDate, info.iHeavenlyStem, info.iTerrestialBranch );
    
    TRACE_EXIT_POINT;
    return info;
    }
