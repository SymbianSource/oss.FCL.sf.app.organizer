/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: 
*
*/


#include "KoreanLunarDateUtil.h"
// debug
#include "calendarui_debug.h"

const TInt KMaxLunarYear = 60;

/**
 * Public methods for constructing a CKoreanLunarDateUtil object.
 */
CKoreanLunarDateUtil* CKoreanLunarDateUtil::NewL(MCalenServices* aServices)
    {
    TRACE_ENTRY_POINT;
    CKoreanLunarDateUtil* self = CKoreanLunarDateUtil::NewLC(aServices);
    CleanupStack::Pop( self );
    
    TRACE_EXIT_POINT;
    return self;
    }

CKoreanLunarDateUtil* CKoreanLunarDateUtil::NewLC(MCalenServices* aServices)
    {
    TRACE_ENTRY_POINT;
    CKoreanLunarDateUtil* self = new (ELeave) CKoreanLunarDateUtil(aServices);
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }


CKoreanLunarDateUtil::CKoreanLunarDateUtil(MCalenServices* aServices):iServices(aServices)
	{
    TRACE_ENTRY_POINT;
	}


void CKoreanLunarDateUtil::ConstructL()
	{
    TRACE_ENTRY_POINT;
    iConverter = CKoreanCalConv::NewL();
	}

CKoreanLunarDateUtil::~CKoreanLunarDateUtil()
    {
    TRACE_ENTRY_POINT;
    delete iConverter;
    }

/**
 * Method for getting nearest gregorian date from
 * lunar day and month and gregorian reference.
 */
TDateTime CKoreanLunarDateUtil::GetNearestGregorianDateL( const TInt aMonth,
                                                          const TInt aDay,
                                                          const TBool aLeap,
                                                          const TDateTime& aReference ) const
	{
    TRACE_ENTRY_POINT;
    TKoreanDate refLunarDate;
    TKoreanDate lunarDate;
    TDateTime resDateTime;
    TInt err = KErrNone;

    iConverter->DateTimeToKoreanL( aReference, refLunarDate );
    /* Try to convert with the year and cycle we got with the reference */
    lunarDate.iCycle = refLunarDate.iCycle;
    lunarDate.iYear = refLunarDate.iYear;
    lunarDate.iMonth = aMonth;
    lunarDate.iDay = aDay;
    lunarDate.iLeapMonth = aLeap;
    TRAP( err, iConverter->KoreanToDateTimeL( lunarDate, resDateTime ) );
    if( err == KErrArgument )
        { /* Date is invalid for this year (day 30 on a month
             that has only 29 days on selected year or a leap month
             not in this year), try to find suitable */
        TKoreanDate lowerLimit;
        TKoreanDate upperLimit;
        iConverter->DateRange( lowerLimit, upperLimit );
        while( 1 )
            { /* Search by increasing year */
            if( lunarDate.iYear < KMaxLunarYear )
               {
                lunarDate.iYear++;
                }
            else
                {
                lunarDate.iYear = 1;
                lunarDate.iCycle++;
                }
            if( (lunarDate.iCycle == upperLimit.iCycle &&
                 lunarDate.iYear > upperLimit.iYear)  ||
                lunarDate.iCycle > upperLimit.iCycle )
                { /* Could not find, just break */
                break;
                }
            TRAP( err, iConverter->KoreanToDateTimeL( lunarDate, resDateTime ) );
            if( err == KErrNone )
                { /* Found suitable */
                break;
                }
            }
        if( err != KErrNone )
            { /* Suitable not found by increasing,
                 reset values and search by decreasing year */
            lunarDate.iCycle = refLunarDate.iCycle;
            lunarDate.iYear = refLunarDate.iYear;
            lunarDate.iMonth = aMonth;
            lunarDate.iDay = aDay;
            while( 1 )
                { /* Search by decreasing year */
                if( lunarDate.iYear > 1 )
                   {
                    lunarDate.iYear--;
                    }
                else
                    {
                    if( lunarDate.iCycle == 0 )
                        { /* Run out of cycles, break with error */
                        break;
                        }
                    lunarDate.iYear = KMaxLunarYear;
                    lunarDate.iCycle--;
                    }
                if( (lunarDate.iCycle == lowerLimit.iCycle &&
                     lunarDate.iYear < lowerLimit.iYear)  ||
                    lunarDate.iCycle < lowerLimit.iCycle )
                    { /* Could not find, just break */
                    break;
                    }
                TRAP( err, iConverter->KoreanToDateTimeL( lunarDate, resDateTime ) );
                if( err == KErrNone )
                    { /* Found suitable */
                    break;
                    }
                }
            }
        }
    /* If still not found, leave with error */
    User::LeaveIfError( err );
    
    TRACE_ENTRY_POINT;
    /* Otherwise return the result */
    return resDateTime;
	}

/**
* Method for getting lunar yearly repeats in gregorian dates.
*/
void CKoreanLunarDateUtil::GetLunarYearlyRepeatsL( RArray<TCalTime>& aRDates,
                                                   const TDateTime& aEntryDate,
                                                   const TBool aFloating ) const
    {
    TInt err = KErrNone;
    TCalTime repeatDate;
    TDateTime resDateTime;
    TDateTime lowerLimit;
    TKoreanDate upperKoreanLimit;
    TKoreanDate lunarStartDate;
    
    /* Get the upper limit for dates in Korean lunar format */
    iConverter->DateRange( lunarStartDate, upperKoreanLimit );
    
    /* Get the entry date in Korean lunar format */
    iConverter->DateTimeToKoreanL( aEntryDate, lunarStartDate );
    
    /* Get the lower limit for dates in Gregorian format */
    iConverter->DateRange( lowerLimit, resDateTime );
    
    /* Get the first occurrence of the event after
    the lower limit in Korean lunar format */
    iConverter->DateTimeToKoreanL(  GetNearestGregorianDateL( lunarStartDate.iMonth,
                                  lunarStartDate.iDay,
                                  lunarStartDate.iLeapMonth,
                                  lowerLimit ),
                                  lunarStartDate );
    
    while( 1 )
        {
        TRAP( err, iConverter->KoreanToDateTimeL( lunarStartDate, resDateTime ) );
        if( err == KErrNone )
            { /* Date exists in current year */
            if( aFloating )
                {
                repeatDate.SetTimeLocalFloatingL( resDateTime );
                }
            else
                {
                repeatDate.SetTimeLocalL( resDateTime );
                }
            aRDates.AppendL( repeatDate );
            }
        /* Increase to next lunar year */
        if( lunarStartDate.iYear < KMaxLunarYear )
            {
            lunarStartDate.iYear++;
            }
        else
            {
            lunarStartDate.iYear = 1;
            lunarStartDate.iCycle++;
            }
        if( (lunarStartDate.iCycle == upperKoreanLimit.iCycle &&
                lunarStartDate.iYear > upperKoreanLimit.iYear)  ||
                lunarStartDate.iCycle > upperKoreanLimit.iCycle )
            { /* Reached the limit, break */
            break;
            }
        }
    }
    
// End of file
