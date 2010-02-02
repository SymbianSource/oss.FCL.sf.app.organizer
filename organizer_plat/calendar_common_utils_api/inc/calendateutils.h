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
 * Description:   Utility class that serves to know beginning of the day, 
 *                maximum/minimum date allowed in calendar, 
 *	              day range, on same day, on same month etc.
 *
 */

#ifndef __CALENDATEUTILS_H__
#define __CALENDATEUTILS_H__

// System includes
#include <e32base.h>
#include <calcommon.h>

/**
 * @class CalenDateUtils 
 * @brief Contains static utility functions related to date and time
 */
NONSHARABLE_CLASS( CalenDateUtils )
    {
	public:
		
		/**
		 * @brief Checks if aY is on the same day as aX
		 *
		 * @param aX Time being compared with
		 * @param aY Time to compare with
		 * @return TBool ETrue if aX and aY are on the same day, EFalse otherwise
		 */
	    IMPORT_C static TBool OnSameDay( const TTime& aX, const TTime& aY );
		
		/**
		 * @brief Checks if aY is on the same month as aX
		 *
		 * @param aX Time being compared with
		 * @param aY Time to compare with
		 * @return TBool ETrue if aX and aY are on the same month, EFalse otherwise
		 */
	    IMPORT_C static TBool OnSameMonth( const TTime& aX, const TTime& aY );
		
		/**
		 * @brief Returns the beginning of the day corresponding to aStartTime i.e 12:00 am
		 *
		 * @param aStartTime Time representing the day for which the beginning is needed
		 * @return TTime The time corresponding to the beginning of the day
		 */
	    IMPORT_C static TTime BeginningOfDay( const TTime& aStartTime );
		
		/**
		 * @brief Checks if aStartTime is not on the same day as aDay, if so returns the beginning of the day
		 * else returns the same time
		 *
		 * @param aStartTime The time to be checked
		 * @param aDay The day to be used as reference
		 * @return TTime The time to display for the day
		 */
	    IMPORT_C static TTime DisplayTimeOnDay( const TTime& aStartTime, 
	                                            const TTime& aDay );

		/**
		 * @brief Checks if the time ranges aXStart->aXEnd intersects with aYStart->aYEnd
		 *
		 * @param aXStart Start time of first time range
		 * @param aXEnd End time of first time range
		 * @param aYStart Start time of second time range
		 * @param aYEnd End time of second time range
		 * @return TBool ETrue if the time ranges intersect, EFalse otherwise
		 */
	    IMPORT_C static TBool TimeRangesIntersect( const TTime& aXStart,
	                                               const TTime& aXEnd,
	                                               const TTime& aYStart, 
	                                               const TTime& aYEnd );

		/**
		 * @brief Checks if aTime is between KCalenMaxYear/KCalenMaxMonth/KCalenMaxDay
		 * and KCalenMinYear/KCalenMinMonth/KCalenMinDay.
		 * Min/Max day is defined by agenda server
		 *
		 * @param aTime aTime to be checked
		 * @return TBool ETrue if the time is valid, EFalse if out of range
		 */
		IMPORT_C static TBool IsValidDay( const TTime& aTime );

		/**
		 * @brief Gets the day range from aStartDay @ 0:00 -> aEndDay @ 23:59
		 *
		 * @param aStartDay Day for the range start
		 * @param aEndDay Day for the range end
		 * @param aRange The range returned
		 */
		IMPORT_C static void GetDayRangeL( const TTime& aStartDay,
										   const TTime& aEndDay,
										   CalCommon::TCalTimeRange& aRange );

		/**
		 * @brief Checks if aTime is NULL
		 *
		 * @param aTime Time to be compared against NULL time
		 * @return ETrue if aTime is NULL, EFalse otherwise
		 */
		IMPORT_C static TBool IsNullTimeL( TCalTime& aTime );

		/**
		 * @brief Returns Min or Max time if aTime goes out of bounds. 
		 * Valid range is [CalenDateUtils::MinTime(), CalenDateUtils::MaxTime]
		 *
		 * @param aTime Time to be checked
		 * @return TTime If aTime is inside [CalenDateUtils::MinTime(), CalenDateUtils::MaxTime]
		 *         CalenDateUtils::MinTime(), if aTime < CalenDateUtils::MinTime()
		 *         CalenDateUtils::MaxTime(), if aTime > CalenDateUtils::MaxTime()
		 */
		IMPORT_C static TTime LimitToValidTime( const TTime& aTime );

		/**
		 * @brief Returns the maximum allowed time
		 *
		 * @return TTime The maximum allowed time
		 */
		IMPORT_C static TTime MaxTime();

		/**
		 * @brief Returns the minimum allowed time
		 *
		 * @return TTime The minimum allowed time
		 */
		IMPORT_C static TTime MinTime();

		/**
		 * @brief Returns time of day in minutes, from midnight. Useful to get hours::minutes component of datetime, 
		 * regardless of date
		 *
		 * @param aDateTime The reference time
		 * @return TTimeIntervalMinutes Minutes elapsed since midnight
		 */
		IMPORT_C static TTimeIntervalMinutes TimeOfDay( const TTime& aDateTime );

		/**
		 * @brief Rounds off TTime to previous full hour
		 * E.g. RoundToPreviousHour( 23.11.2006 9:31 ) = 23.11.2006 9:00
		 *
		 * @param aTime The time to round off
		 * @return TTime The time rounded off to previous hour
		 */
		IMPORT_C static TTime RoundToPreviousHour( const TTime& aTime );

		/**
		 * @brief Rounds off TTimeIntervalMinutes to previous full hour
		 * E.g. RoundToPreviousHour( 130 min ) = 120 min
		 *
		 * @param  aMinutes The minute to round off
		 * @return TTimeIntervalMinutes The minutes rounded off to previous hour
		 */
		IMPORT_C static TTimeIntervalMinutes RoundToPreviousHour( const TTimeIntervalMinutes& aMinutes );
    
		/**
		 * @brief Returns the current time
		 *
		 * @return TTime Current time. 
		 */ 
		IMPORT_C static TTime Now();
    
		/**
		 * @brief Returns the current day with the time component set to 00:00
		 *
		 * @return TTime Today
		 */ 
		IMPORT_C static TTime Today();

		/**
		 * @brief Checks if aTime is on the current day
		 * 
		 * @param aTime The time to check for
		 * @return ETrue if aTime is on today, EFalse otherwise
		 */     
		IMPORT_C static TBool IsOnToday( const TTime& aTime );

		/** 
		 * @brief Given aDate = DD::MM::YY @ hh:mm:ss, it returns a TTime obj DD::MM::YY @ 08:00 am
		 * 
		 * @param aDate The date for reference
		 * @return TTime The default time for the specified date
		 */
		IMPORT_C static TTime DefaultTime( const TTime& aDate );
    
	private:
    
		/** 
		 * @brief Default C++ constructor. Hiding it since this is a utility class
		 */
		CalenDateUtils();

		/** 
		 * @brief Disallow copying since this is a utility class
		 */
		CalenDateUtils& operator=( const CalenDateUtils& );

    };

#endif // __CALENDATEUTILS_H__

// End of File
