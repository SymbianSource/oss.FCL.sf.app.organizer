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
#include "CalenLunarInfoProvider.h"
#include "CalenLunarInfo.h"
#include "CalenSolarTerms.h"
#include <calendarconverter.h>
#include "CalenKoreanLunarPlugin.h"
#include "KoreanCalConv.h"

// CONSTANTS
const TInt KDaysInJanuary = 31; // total day count of January
const TInt KDaysInMarch = 31; // total day count of January
const TInt KDaysInDecemberAfterWinterSolstice = 10; // day count of December starting from winter solstice (21.12)
const TInt K105thDayAfterWinterSolstice = 105;

// ======== MEMBER FUNCTIONS ========


// ---------------------------------------------------------------------------
// TCalenLunarInfo::TLunarFestival()
// ---------------------------------------------------------------------------
//
static TCalenLunarInfo::TLunarFestival KoreanLunarFestival(
    const TKoreanDate& aDate,
    const TKoreanDate& aNextDate,
    const TTime& aDay )
    {
    TRACE_ENTRY_POINT;
 
    TCalenLunarInfo::TLunarFestival index = TCalenLunarInfo::ENoFestival;
    
    if (!aDate.iLeapMonth)
        {
        if (aDate.iMonth == 4 && aDate.iDay == 8)
            {
            index = TCalenLunarInfo::EFestivalBuddhaBirthDay;
            }
        else if (aDate.iMonth == 8 && aDate.iDay == 15)
            {
            index = TCalenLunarInfo::EFestivalHarvest;
            }
        else if(aDate.iMonth == 1 && aDate.iDay == 15)
            {            
            // full moon day
            index = TCalenLunarInfo::EFestivalFullMoonDay;            
            }
        else if(aDate.iMonth == 3 && aDate.iDay == 3)
            {            
            // 3rd of March festival
            index = TCalenLunarInfo::EFestival3rdMarchDay;
            }
        else if(aDate.iMonth == 5 && aDate.iDay == 5)
            {            
            // dano festival
            index = TCalenLunarInfo::EFestivalDano;
            }
        else if(aDate.iMonth == 6 && aDate.iDay == 15)
            {            
            // ?? festival
            index = TCalenLunarInfo::EFestivalUnknown_1;
            }                
        else if(aDate.iMonth == 7 && aDate.iDay == 7)
            {            
            // 7th of July festival
            index = TCalenLunarInfo::EFestival7thJulyDay;
            }
        else if(aDate.iMonth == 7 && aDate.iDay == 15)
            {            
            // ?? festival
            index = TCalenLunarInfo::EFestivalUnknown_2;
            }        
        else if(aDate.iMonth == 9 && aDate.iDay == 9)
            {            
            // ?? festival
            index = TCalenLunarInfo::EFestivalUnknown_3;
            }
        else
        	{
            TDateTime date = aDay.DateTime();
            TMonth month = date.Month();
            TInt day = date.Day();
            TInt year = date.Year();
            
            if( month == EApril )
            	{
            	// Check if it's 105th hay after Dong Shi (winter solstice)
            	TInt daysInFebruary;
            	if(year%4 == 0)
            		{
            		// leap year
            		daysInFebruary = 29;
            		}
            	else
            		{
            		daysInFebruary = 28;
            		}
            	
            	// we need to count dates beginning from winter soltice (21.22) till end of march
            	TInt daysBeforeApril = 
            	    KDaysInDecemberAfterWinterSolstice + KDaysInJanuary + daysInFebruary + KDaysInMarch;
            	
            	if( daysBeforeApril + day == K105thDayAfterWinterSolstice )
            		{
            		index = TCalenLunarInfo::EFestival105thDayAfterWS;
            		}
            	}
        	}   
        }
    if (!aNextDate.iLeapMonth &&
        aDate.iMonth == 1 && aDate.iDay == 1)
        {
        index = TCalenLunarInfo::EFestivalNewYearDay;
        }

    TRACE_EXIT_POINT;
    return index;
    }

// ---------------------------------------------------------------------------
// TCalenLunarInfo::TSolarFestival()
// ---------------------------------------------------------------------------
//
static TCalenLunarInfo::TSolarFestival KoreanSolarFestival(
    const TTime& aDay )
    {
    TRACE_ENTRY_POINT;

    TCalenLunarInfo::TSolarFestival index = TCalenLunarInfo::ENoSolarFestival;

    TDateTime date = aDay.DateTime();
    TMonth month = date.Month();
    TInt day = date.Day();

    if( month == EJanuary && day == 0 )
        {
        index = TCalenLunarInfo::ESolarFestivalNewYearDay;
        }
    else if( month == EMarch && day == 0 )
        {
        index = TCalenLunarInfo::ESolarFestivalIndependenceMovement;
        }
    else if( month == EMay && day == 4 )
        {
        index = TCalenLunarInfo::ESolarFestivalChildrensDay;
        }
    else if( month == EJune && day == 5 )
        {
        index = TCalenLunarInfo::ESolarFestivalMemorialDay;
        }
    else if( month == EJuly && day == 16 )
        {
        index = TCalenLunarInfo::ESolarFestivalConstitutionDay;
        }
    else if( month == EAugust && day == 14 )
        {
        index = TCalenLunarInfo::ESolarFestivalLiberationDay;
        }
    else if( month == EOctober && day == 2 )
        {
        index = TCalenLunarInfo::ESolarFestivalFoundationDay;
        }
    else if( month == EDecember && day == 24 )
        {
        index = TCalenLunarInfo::ESolarFestivalChristmasDay;
        }   
    else if( month == EOctober && day == 8 )
        {
        index = TCalenLunarInfo::ESolarFestivalHangulDay;
        }  
    TRACE_EXIT_POINT;
    return index;
    }

// ---------------------------------------------------------------------------
// CCalenLunarInfoProvider::NewL()
// ---------------------------------------------------------------------------
//
CCalenLunarInfoProvider* CCalenLunarInfoProvider::NewL( RFs& aFs )
    {
    TRACE_ENTRY_POINT;

    CCalenLunarInfoProvider* self = new (ELeave) CCalenLunarInfoProvider();
    CleanupStack::PushL(self);
    self->ConstructL(aFs);
    CleanupStack::Pop(self);

    TRACE_EXIT_POINT;
    return self;
    }

// ---------------------------------------------------------------------------
// CCalenLunarInfoProvider::~CCalenLunarInfoProvider()
// ---------------------------------------------------------------------------
//
CCalenLunarInfoProvider::~CCalenLunarInfoProvider()
    {
    TRACE_ENTRY_POINT;

    delete iSolarTerms;
    delete iConverter;

    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------------------------
// CCalenLunarInfoProvider::CCalenLunarInfoProvider()
// ---------------------------------------------------------------------------
//
CCalenLunarInfoProvider::CCalenLunarInfoProvider()
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------------------------
// CCalenLunarInfoProvider::ConstructL()
// ---------------------------------------------------------------------------
//
void CCalenLunarInfoProvider::ConstructL( RFs& aFs )
    {
    TRACE_ENTRY_POINT;

    iConverter = CKoreanCalConv::NewL();
    iSolarTerms = CCalenSolarTerms::NewL( aFs );

    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------------------------
// CCalenLunarInfoProvider::GetLunarInfoL()
// ---------------------------------------------------------------------------
//
TCalenLunarInfo CCalenLunarInfoProvider::GetLunarInfoL( const TTime& aDay )
    {
    TRACE_ENTRY_POINT;

    TDateTime dayDt = aDay.DateTime();

    TTime nextDay = aDay + TTimeIntervalDays(1);
    TDateTime nextDayDt = nextDay.DateTime();

    TKoreanDate koreanDate;
    TKoreanDate nextKoreanDate;

    iConverter->DateTimeToKoreanL(dayDt, koreanDate);
    iConverter->DateTimeToKoreanL(nextDayDt, nextKoreanDate);


    TCalenLunarInfo info;
    info.iLunarDate = koreanDate;
    info.iGregorianDate = aDay;
    info.iLunarFestival = KoreanLunarFestival( koreanDate, nextKoreanDate, aDay);
    info.iSolarFestival = KoreanSolarFestival( aDay );
    info.iSolarTerm = iSolarTerms->CheckSolarTermDateL( dayDt );
    
    TRACE_EXIT_POINT;
    return info;
    }

// End of file
