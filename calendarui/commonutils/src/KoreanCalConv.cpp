/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implementation of View Utils
*
*/

#include "KoreanCalConv.h"

//debug
#include "calendarui_debug.h"

/* Leap month exceptions for fixing conversion result
   to match Korean lunar calendar:

  year cycle, year, Chinese leap month, Korean leap month
*/
typedef struct
{
    TInt cycle;
    TInt year;
    TInt chineseLeapMonth;
    TInt koreanLeapMonth;
} TLeapExceptions;

//CONSTANTS
const TLeapExceptions KLeapExceptions[] = { {78, 29,  4,  3},
                                            {78, 34,  6,  5},
                                            {79, 56,  2,  3} };

const TInt KLeapExceptionCnt = (sizeof( KLeapExceptions ) / sizeof( TLeapExceptions ));

/* Month length exceptions for fixing conversion result
   to match Korean lunar calendar. Corresponding months
   have 30 days in Korean lunar calendar and 29 days
   in Chinese lunar calendar. The next month always contains
   30 days in Chinese lunar calendar and 29 days in Korean
   lunar calendar:

  year cycle, year, korean month, flag indicating leap month, flag indicating next month is leap
*/
typedef struct
{
    TInt cycle;
    TInt year;
    TInt koreanMonth;
    TBool isLeapMonth;
    TBool nextIsLeapMonth;
} TMonthExceptions;

const TMonthExceptions KMonthExceptions[] = { {76, 43,  3, EFalse, EFalse },
                                              {76, 51,  5, EFalse, ETrue },
                                              {76, 51,  9, EFalse, EFalse },
                                              {76, 52, 12, EFalse, EFalse },
                                              {76, 55, 10, EFalse, EFalse },
                                              {76, 56,  7, EFalse, ETrue },
                                              {76, 56,  9, EFalse, EFalse },
                                              {76, 57,  9, EFalse, EFalse },
                                              {76, 60,  9, EFalse, EFalse },
                                              {77,  1,  1, EFalse, EFalse },
                                              {77,  2,  4, EFalse, ETrue },
                                              {77,  4,  9, EFalse, EFalse },
                                              {77,  5,  8, EFalse, EFalse },
                                              {77,  8,  3, EFalse, EFalse },
                                              {77, 11,  8, EFalse, EFalse },
                                              {77, 13,  5, EFalse, EFalse },
                                              {77, 19,  7, EFalse, EFalse },
                                              {77, 19,  9, EFalse, EFalse },
                                              {77, 20, 10, EFalse, EFalse },
                                              {77, 20, 12, EFalse, EFalse },
                                              {77, 26,  2, EFalse, EFalse },
                                              {77, 27,  1, EFalse, EFalse },
                                              {77, 27,  4, EFalse, EFalse },
                                              {77, 29,  6, EFalse, EFalse },
                                              {77, 30, 12, EFalse, EFalse },
                                              {77, 32,  1, EFalse, EFalse },
                                              {77, 34, 12, EFalse, EFalse },
                                              {77, 45,  3, EFalse, EFalse },
                                              {77, 47,  5, EFalse, EFalse },
                                              {77, 49, 11, EFalse, EFalse },
                                              {77, 50, 11, EFalse, EFalse },
                                              {77, 53,  9, EFalse, EFalse },
                                              {77, 55,  2, EFalse, EFalse },
                                              {77, 59,  9, EFalse, EFalse },
                                              {78,  4,  4, EFalse, EFalse },
                                              {78,  4, 12, EFalse, EFalse },
                                              {78,  6,  9, EFalse, EFalse },
                                              {78, 12,  6, EFalse, EFalse },
                                              {78, 12,  9, EFalse, EFalse },
                                              {78, 13, 12, EFalse, EFalse },
                                              {78, 15, 11, EFalse, EFalse },
                                              {78, 18,  3, EFalse, EFalse },
                                              {78, 22, 10, EFalse, EFalse },
                                              {78, 29,  4, EFalse, EFalse },
                                              {78, 29,  6, EFalse, EFalse },
                                              {78, 30,  4, EFalse, EFalse },
                                              {78, 36, 10, EFalse, EFalse },
                                              {78, 37,  1, EFalse, EFalse },
                                              {78, 40,  3, EFalse, EFalse },
                                              {78, 43,  8, EFalse, EFalse },
                                              {78, 43, 12, EFalse, EFalse },
                                              {78, 44, 12, EFalse, EFalse },
                                              {78, 46,  5, EFalse, EFalse },
                                              {78, 48,  1, EFalse, EFalse },
                                              {78, 52, 11, EFalse, EFalse },
                                              {78, 53, 10, EFalse, EFalse },
                                              {78, 57,  7, EFalse, EFalse },
                                              {78, 58,  1, EFalse, EFalse },
                                              {78, 59, 10, EFalse, EFalse },
                                              {79,  3,  4, EFalse, EFalse },
                                              {79,  5, 10, EFalse, EFalse },
                                              {79,  7,  1, EFalse, EFalse },
                                              {79,  8,  6, EFalse, EFalse },
                                              {79,  8,  9, EFalse, EFalse },
                                              {79,  9,  8, ETrue, EFalse },
                                              {79, 15,  5, EFalse, EFalse },
                                              {79, 16,  8, EFalse, EFalse },
                                              {79, 17,  8, EFalse, EFalse },
                                              {79, 17, 12, EFalse, EFalse },
                                              {79, 20,  3, EFalse, EFalse },
                                              {79, 25,  5, EFalse, EFalse },
                                              {79, 26,  1, EFalse, EFalse },
                                              {79, 27,  1, EFalse, EFalse },
                                              {79, 28,  2, EFalse, EFalse },
                                              {79, 31,  6, ETrue, EFalse },
                                              {79, 37,  9, EFalse, EFalse },
                                              {79, 38,  1, EFalse, EFalse },
                                              {79, 39,  6, EFalse, EFalse },
                                              {79, 45, 12, EFalse, EFalse },
                                              {79, 46,  7, EFalse, EFalse },
                                              {79, 51,  1, EFalse, EFalse },
                                              {79, 53,  7, EFalse, EFalse },
                                              {79, 54,  6, EFalse, EFalse },
                                              {79, 55,  7, EFalse, EFalse },
                                              {79, 55, 11, EFalse, EFalse },
                                              {79, 56,  3, EFalse, ETrue } };

const TInt KMonthExceptionCnt = (sizeof( KMonthExceptions ) / sizeof( TMonthExceptions ));

// -----------------------------------------------------------------------------
// CKoreanCalConv::NewL
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C CKoreanCalConv* CKoreanCalConv::NewL()
    {
    TRACE_ENTRY_POINT;
    CKoreanCalConv* self = CKoreanCalConv::NewLC();
    CleanupStack::Pop( self );

    TRACE_EXIT_POINT;
    return self;
    }

// -----------------------------------------------------------------------------
// CKoreanCalConv::NewLC
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C CKoreanCalConv* CKoreanCalConv::NewLC()
    {
    TRACE_ENTRY_POINT;
    CKoreanCalConv* self = new (ELeave) CKoreanCalConv();
    CleanupStack::PushL( self );
    self->ConstructL();
    
    TRACE_EXIT_POINT;
    return self;
    }

// -----------------------------------------------------------------------------
// CKoreanCalConv::CKoreanCalConv
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CKoreanCalConv::CKoreanCalConv()
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CKoreanCalConv::ConstructL
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CKoreanCalConv::ConstructL()
    {
    TRACE_ENTRY_POINT;
    chineseConv = CChineseCalendarConverter::NewL();
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CKoreanCalConv::~CKoreanCalConv
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CKoreanCalConv::~CKoreanCalConv()
    {
    TRACE_ENTRY_POINT;
    delete chineseConv;
    TRACE_EXIT_POINT;
    }


// -----------------------------------------------------------------------------
// CKoreanCalConv::DateTimeToKoreanL
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CKoreanCalConv::DateTimeToKoreanL( const TDateTime& aDateTime, TKoreanDate& aKoreanDate )
    {
    TRACE_ENTRY_POINT;
    TInt i;
    
    /* Use the chinese converter for initial result */
    chineseConv->DateTimeToChineseL( aDateTime, aKoreanDate );
    
    /* Check if leap month fix is needed */
    for( i = 0 ; i < KLeapExceptionCnt ; i++ )
        {
        if( KLeapExceptions[i].cycle == aKoreanDate.iCycle &&
        KLeapExceptions[i].year == aKoreanDate.iYear )
            { /* Year contains an exception */
            if( KLeapExceptions[i].koreanLeapMonth < KLeapExceptions[i].chineseLeapMonth )
                {
                if( aKoreanDate.iLeapMonth )
                    { /* Resulted month is leap month in Chinese but not in Korean,
                    fix by unsetting the leap month flag */
                    aKoreanDate.iLeapMonth = EFalse;
                    }
                else
                    {
                    if( aKoreanDate.iMonth == (KLeapExceptions[i].koreanLeapMonth + 1) )
                        { /* Resulted month is actually a leap month on Korean Calendar,
                        fix by setting the leap month flag and decreasing month by one */
                        aKoreanDate.iLeapMonth = ETrue;
                        aKoreanDate.iMonth -= 1;
                        }
                    
                    /* See if month needs an offset fix */
                    if( aKoreanDate.iMonth > KLeapExceptions[i].koreanLeapMonth &&
                                    aKoreanDate.iMonth <= KLeapExceptions[i].chineseLeapMonth )
                        { /* Between the leap months, fix by decreasing the month by one */
                        aKoreanDate.iMonth -= 1;
                        }
                    }
                }
            else if( KLeapExceptions[i].koreanLeapMonth > KLeapExceptions[i].chineseLeapMonth )
                {
                if( aKoreanDate.iLeapMonth )
                    { /* Resulted month is leap month in Chinese but not in Korean,
                    fix by unsetting the leap month flag and increase the month by one */
                    aKoreanDate.iLeapMonth = EFalse;
                    aKoreanDate.iMonth += 1;
                    }
                else
                    {
                    if( aKoreanDate.iMonth == KLeapExceptions[i].koreanLeapMonth )
                        { /* Resulted month is actually a leap month on Korean Calendar,
                        fix by setting the leap month flag */
                        aKoreanDate.iLeapMonth = ETrue;
                        }
                    
                    /* See if month needs an offset fix */
                    if( aKoreanDate.iMonth > KLeapExceptions[i].chineseLeapMonth &&
                                aKoreanDate.iMonth < KLeapExceptions[i].koreanLeapMonth )
                        { /* Between the leap months, fix by increasing the month by one */
                        aKoreanDate.iMonth += 1;
                        }
                    }
                }
            break;
            }
        }
    
    /* Check if month length fix is needed */
    for( i = 0 ; i < KMonthExceptionCnt ; i++ )
        {
        if( (KMonthExceptions[i].cycle == aKoreanDate.iCycle &&
                    KMonthExceptions[i].year == aKoreanDate.iYear &&
                    KMonthExceptions[i].koreanMonth != 12 &&
                    aKoreanDate.iMonth != 1) ||
                    (KMonthExceptions[i].koreanMonth == 12 && aKoreanDate.iMonth == 1 &&
                    (KMonthExceptions[i].year == (aKoreanDate.iYear - 1) &&
                    KMonthExceptions[i].cycle == aKoreanDate.iCycle) ||
                    (KMonthExceptions[i].year == 60 && aKoreanDate.iYear == 1 && 
                    KMonthExceptions[i].cycle == (aKoreanDate.iCycle - 1))) )
            { /* Year may contain an exception */
            if( (KMonthExceptions[i].koreanMonth == (aKoreanDate.iMonth - 1) && 
                        !(KMonthExceptions[i].nextIsLeapMonth || aKoreanDate.iLeapMonth)) ||
                        (KMonthExceptions[i].koreanMonth == 12 && aKoreanDate.iMonth == 1) ||
                        (KMonthExceptions[i].koreanMonth == aKoreanDate.iMonth &&
                        KMonthExceptions[i].nextIsLeapMonth && aKoreanDate.iLeapMonth) )
                { /* Month contains an exception, decrease the day by one */
                if( aKoreanDate.iDay == 1 )
                    {
                    aKoreanDate.iDay = 30;
                    if( aKoreanDate.iMonth == 1 )
                        {
                        aKoreanDate.iMonth = 12;
                        if( aKoreanDate.iYear == 1 )
                            {
                            aKoreanDate.iYear = 60;
                            aKoreanDate.iCycle -= 1;
                            }
                        else
                            {
                            aKoreanDate.iYear -= 1;
                            }
                        }
                    else
                        {
                        if( !aKoreanDate.iLeapMonth )
                            { /* Leap months don't need month decreasing */
                            aKoreanDate.iMonth -= 1;
                            }
                        if( KMonthExceptions[i].isLeapMonth )
                            {
                            aKoreanDate.iLeapMonth = ETrue;
                            }
                        else
                            {
                            aKoreanDate.iLeapMonth = EFalse;
                            }
                        }
                    }
                else
                    {
                    aKoreanDate.iDay -= 1;
                    }
                break;
                }
            }
        }
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CKoreanCalConv::KoreanToDateTimeL
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CKoreanCalConv::KoreanToDateTimeL( TKoreanDate& aKoreanDate, TDateTime& aDateTime )
    {
    TRACE_ENTRY_POINT;
    TInt i;
    TKoreanDate fixDate = aKoreanDate;
    
    /* Check if month length fix is needed */
    for( i = 0 ; i < KMonthExceptionCnt ; i++ )
        {
        if( (KMonthExceptions[i].cycle == fixDate.iCycle &&
                KMonthExceptions[i].year == fixDate.iYear &&
                fixDate.iMonth != 1) ||
                (fixDate.iMonth == 1 &&
                (KMonthExceptions[i].year == (fixDate.iYear - 1) &&
                KMonthExceptions[i].cycle == fixDate.iCycle) ||
                (KMonthExceptions[i].year == 60 && fixDate.iYear == 1 && 
                KMonthExceptions[i].cycle == (fixDate.iCycle - 1))) )
            { /* Year may contain an exception */
            if( (KMonthExceptions[i].koreanMonth == fixDate.iMonth &&
            KMonthExceptions[i].isLeapMonth == fixDate.iLeapMonth) )
                { /* Month contains an exception */
                if( fixDate.iDay == 30 )
                    { /* Fix the last day to be the first day of next month */
                    fixDate.iDay = 1;
                    if( fixDate.iMonth == 12 )
                        {
                        fixDate.iMonth = 1;
                        if( fixDate.iYear == 60 )
                            {
                            fixDate.iYear = 1;
                            fixDate.iCycle += 1;
                            }
                        else
                            {
                            fixDate.iYear += 1;
                            }
                        }
                    else
                        {
                        if( KMonthExceptions[i].nextIsLeapMonth )
                            { /* Leap months don't need increasing,
                            just set the flag */
                            fixDate.iLeapMonth = ETrue;
                            }
                        else
                            {
                            fixDate.iLeapMonth = EFalse;
                            fixDate.iMonth += 1;
                            }
                        }
                    }
                break;
                }
            else if( (KMonthExceptions[i].koreanMonth == (fixDate.iMonth - 1) && 
                    !(KMonthExceptions[i].nextIsLeapMonth || fixDate.iLeapMonth)) ||
                    (KMonthExceptions[i].koreanMonth == 12 && fixDate.iMonth == 1) ||
                    (KMonthExceptions[i].koreanMonth == fixDate.iMonth &&
                    KMonthExceptions[i].nextIsLeapMonth && fixDate.iLeapMonth) )
                { /* It is the month following the exception, increase the day by one */
                if( fixDate.iDay == 30 )
                    { /* Invalid day, Korean Lunar calendar has always only 29 days
                    after the exception month */
                    User::Leave( KErrArgument );
                    }
                else
                    {
                    fixDate.iDay += 1;
                    }
                break;
                }
            }
        }
    
    /* Check if leap month fix is needed */
    for( i = 0 ; i < KLeapExceptionCnt ; i++ )
        {
        if( KLeapExceptions[i].cycle == fixDate.iCycle &&
                    KLeapExceptions[i].year == fixDate.iYear )
            { /* Year contains an exception */
            if( KLeapExceptions[i].koreanLeapMonth != fixDate.iMonth &&
            fixDate.iLeapMonth )
                { /* This year does not have the requested leap month in 
                Korean lunar calendar, leave with error */
                User::Leave( KErrArgument );
                }
            
            if( fixDate.iMonth == KLeapExceptions[i].koreanLeapMonth &&
            fixDate.iLeapMonth )
                { /* Leap month in Korean lunar calendar, but not in Chinese
                lunar calendar, fix by clearing the leap indicator*/
                fixDate.iLeapMonth = EFalse;
                if( KLeapExceptions[i].koreanLeapMonth < KLeapExceptions[i].chineseLeapMonth )
                { /* Also month needs to be increased */
                fixDate.iMonth += 1;
                }
                }
            else if( fixDate.iMonth > KLeapExceptions[i].koreanLeapMonth &&
            fixDate.iMonth < KLeapExceptions[i].chineseLeapMonth )
                { /* Month between the leap month */
                if( KLeapExceptions[i].koreanLeapMonth < KLeapExceptions[i].chineseLeapMonth )
                    {
                    fixDate.iMonth += 1;
                    }
                else
                    {
                    fixDate.iMonth -= 1;
                    }
                }
            else if( fixDate.iMonth == (KLeapExceptions[i].chineseLeapMonth + 1) &&
                            !fixDate.iLeapMonth )
                { /* Leap month in Chinese lunar calendar, fix by decreasing
                the monrth by one and setting the leap indicator */
                fixDate.iMonth -= 1;
                fixDate.iLeapMonth = ETrue;
                }
            chineseConv->ChineseToDateTimeL( fixDate, aDateTime );
            
            TRACE_EXIT_POINT;
            return;
            }
        }
    /* Use the chinese converter for result */
    chineseConv->ChineseToDateTimeL( fixDate, aDateTime );
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CKoreanCalConv::DateRange
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CKoreanCalConv::DateRange( TDateTime& aLower, TDateTime& aUpper )
    {
    TRACE_ENTRY_POINT;
    aLower.SetYear( 1900 );
    aLower.SetMonth( EJanuary );
    aLower.SetDay( 1 );
    aUpper.SetYear( 2099 );
    aUpper.SetMonth( EDecember );
    aUpper.SetDay( 31 );
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CKoreanCalConv::DateRange
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CKoreanCalConv::DateRange( TKoreanDate& aLower, TKoreanDate& aUpper )
    {
    TRACE_ENTRY_POINT;
    aLower.iCycle = 76;
    aLower.iYear = 36;
    aLower.iMonth = 12;
    aLower.iDay = 1;
    aLower.iLeapMonth = EFalse;
    aUpper.iCycle = 79;
    aUpper.iYear = 56;
    aUpper.iMonth = 11;
    aUpper.iDay = 20;
    aUpper.iLeapMonth = EFalse;
    TRACE_EXIT_POINT;
    }

// End of file
