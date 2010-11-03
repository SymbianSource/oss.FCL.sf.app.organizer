/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  ?Description
 *
*/

#ifndef __CALENDATEUTILS_H__
#define __CALENDATEUTILS_H__

#include <e32base.h>
#include <calcommon.h>


/**
 *  CalenDateUtils contains static utility functions useful
 *  when comparing and calculating with dates and times.
 *
 *  @lib Calendar.app
 *  @since 2.1
 */
NONSHARABLE_CLASS( CalenDateUtils )
    {
public:
    IMPORT_C static TBool OnSameDay( const TTime& aX, const TTime& aY );
    IMPORT_C static TBool OnSameMonth( const TTime& aX, const TTime& aY );
    IMPORT_C static TTime BeginningOfDay( const TTime& aStartTime );
    IMPORT_C static TTime DisplayTimeOnDay( const TTime& aStartTime, 
                                            const TTime& aDay );

    IMPORT_C static TBool TimeRangesIntersect( const TTime& aXStart,
                                               const TTime& aXEnd,
                                               const TTime& aYStart, 
                                               const TTime& aYEnd );

    /**
     * Is aTime between KCalenMaxYear/KCalenMaxMonth/KCalenMaxDay
     * and KCalenMinYear/KCalenMinMonth/KCalenMinDay.
     * Min/Max day is defined agenda server.
     * @param aTime aTime to be checked
     * @return EFalse : Out of range
     */
    IMPORT_C static TBool IsValidDay( const TTime& aTime );

    /**
     * Gets range from aStartDay @ 0:00 -> aEndDay @ 23:59
     * @param aStartDay Day for the range start.
     * @param aEndDay Day for the range end.
     * @param aRange The range is returned here.
     */
    IMPORT_C static void GetDayRangeL( const TTime& aStartDay,
                                       const TTime& aEndDay,
                                       CalCommon::TCalTimeRange& aRange );

    /**
     * Return ETrue if aTime is NULL
     * @param aTime time to be compared against NULL time
     */
    IMPORT_C static TBool IsNullTimeL( TCalTime& aTime );

    /**
     * Return Min or Max time if aTime goes out of bounds. 
     * Valid range is [CalenDateUtils::MinTime(), CalenDateUtils::MaxTime]
     * @param aTime time to be checked
     * @return aTime, if aTime in [CalenDateUtils::MinTime(), CalenDateUtils::MaxTime]
     *         CalenDateUtils::MinTime(), if aTime < CalenDateUtils::MinTime()
     *         CalenDateUtils::MaxTime(), if aTime > CalenDateUtils::MaxTime()
     */
    IMPORT_C static TTime LimitToValidTime( const TTime& aTime );

    /**
     * Return maximum allowed time. (31.
     */
    IMPORT_C static TTime MaxTime();

    /**
     * Return minimum allowed time.
     */
    IMPORT_C static TTime MinTime();

    /**
     * Return time of day as a minutes from midnight. Useful to get hours::minutes component of datetime, 
     * regardless of date
     */
    IMPORT_C static TTimeIntervalMinutes TimeOfDay( const TTime& aDateTime );

    /**
     * Round TTime to previous full hour, e.g. RoundToPreviousHour( 23.11.2006 9:31 ) = 23.11.2006 9:00
     */
    IMPORT_C static TTime RoundToPreviousHour( const TTime& aTime );

    /**
     * Round TTimeIntervalMinutes to previous full hour, e.g. RoundToPreviousHour( 130 min ) = 120 min
     */
    IMPORT_C static TTimeIntervalMinutes RoundToPreviousHour( const TTimeIntervalMinutes& aMinutes );
    

    /**
     * @return current time. 
     */ 
    IMPORT_C static TTime Now();
    
    /**
     * @return today with time component set to 00:00. 
     */ 
    IMPORT_C static TTime Today();

    /**
     * @return ETrue if given aTime is on today, EFalse otherwise
     */     
    IMPORT_C static TBool IsOnToday( const TTime& aTime );

    /* 
     * Given aDate = DD::MM::YY @ hh:mm:ss, it returns a TTime obj DD::MM::YY @ 08:00 am
     * @param: aDate, which has the DD::MM::YY
     */
    IMPORT_C static TTime DefaultTime( const TTime& aDate );
    
   
private:
    // Hiding constructor, because this is static utility class.
    //lint -e{1526} 
    CalenDateUtils();

    // Hiding assignment, because this is static utility class.
    //lint -e{1526} 
    CalenDateUtils& operator=( const CalenDateUtils& );

    };

#endif // __CALENDATEUTILS_H__


// End of File
