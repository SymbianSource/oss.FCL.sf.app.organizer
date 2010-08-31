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
#include <qglobal.h>					// Q_DECL_EXPORT macro
#ifdef  CALENDATEUTILS_DLL
#define CALENDATEUTIL_EXPORT Q_DECL_EXPORT
#else
#define CALENDATEUTIL_EXPORT Q_DECL_IMPORT
#endif

// forward declarations
class QDateTime;

/**
 *  CalenDateUtils contains static utility functions useful
 *  when comparing and calculating with dates and times.
 *
 *  @lib Calendar.app
 *  @since 2.1
 */

class CalenDateUtils
	{
public:
	static bool onSameDay(const QDateTime& x, const QDateTime& y)
		{
		Q_UNUSED(x);Q_UNUSED(y);
		return false;
		}
	static bool onSameMonth(const QDateTime& x, const QDateTime& y)
		{
		Q_UNUSED(x);Q_UNUSED(y);
		return false;
		}
	static QDateTime beginningOfDay(const QDateTime& startTime)
		{
		Q_UNUSED(startTime);
		return QDateTime();
		}
	static QDateTime displayTimeOnDay(const QDateTime& startTime,
			const QDateTime& day)
		{
		Q_UNUSED(startTime);Q_UNUSED(day);
		return QDateTime();
		}

	static bool timeRangesIntersect(const QDateTime& xStart,
			const QDateTime& xEnd, const QDateTime& yStart,
			const QDateTime& yEnd)
		{
		Q_UNUSED(xStart);Q_UNUSED(xEnd);Q_UNUSED(yStart);Q_UNUSED(yEnd);
		return false;
		}

	/**
	 * Is aTime between KCalenMaxYear/KCalenMaxMonth/KCalenMaxDay
	 * and KCalenMinYear/KCalenMinMonth/KCalenMinDay.
	 * Min/Max day is defined agenda server.
	 * @param aTime aTime to be checked
	 * @return EFalse : Out of range
	 */
	static bool isValidDay(const QDateTime& time)
		{
		Q_UNUSED(time);
		return false;
		}

	/**
	 * Return Min or Max time if aTime goes out of bounds. 
	 * Valid range is [CalenDateUtils::MinTime(), CalenDateUtils::MaxTime]
	 * @param aTime time to be checked
	 * @return aTime, if aTime in [CalenDateUtils::MinTime(), CalenDateUtils::MaxTime]
	 *         CalenDateUtils::MinTime(), if aTime < CalenDateUtils::MinTime()
	 *         CalenDateUtils::MaxTime(), if aTime > CalenDateUtils::MaxTime()
	 */
	static QDateTime limitToValidTime(const QDateTime& time)
		{
		Q_UNUSED(time);
		return QDateTime();
		}

	/**
	 * Return maximum allowed time. (31.
	 */
	static QDateTime maxTime()
		{
		return QDateTime();
		}

	/**
	 * Return minimum allowed time.
	 */
	static QDateTime minTime()
		{
		return QDateTime();
		}

	/**
	 * Return time of day as a minutes from midnight. Useful to get hours::minutes component of datetime, 
	 * regardless of date
	 */
	static int timeOfDay(const QDateTime& dateTime)
		{
		Q_UNUSED(dateTime);
		return 0;
		}

	/**
	 * Round QDateTime to previous full hour, e.g. RoundToPreviousHour( 23.11.2006 9:31 ) = 23.11.2006 9:00
	 */
	static QDateTime roundToPreviousHour(const QDateTime& dateTime)
		{
		Q_UNUSED(dateTime);
		return QDateTime();
		}

	/**
	 * Round QDateTimeIntervalMinutes to previous full hour, e.g. RoundToPreviousHour( 130 min ) = 120 min
	 */
	static int roundToPreviousHour(const int& minutes)
		{
		Q_UNUSED(minutes);
		return 0;
		}

	/**
	 * @return current time. 
	 */
	static QDateTime now()
		{
		return QDateTime();
		}

	/**
	 * @return today with time component set to 00:00. 
	 */
	static QDateTime today()
		{
		return QDateTime();
		}

	/**
	 * @return ETrue if given aTime is on today, EFalse otherwise
	 */
	static bool isOnToday(const QDateTime& time)
		{
		Q_UNUSED(time);
		return false;
		}

	/* 
	 * Given aDate = DD::MM::YY @ hh:mm:ss, it returns a QDateTime obj DD::MM::YY @ 08:00 am
	 * @param: aDate, which has the DD::MM::YY
	 */
	static QDateTime defaultTime(const QDateTime& date)
		{
		Q_UNUSED(date);
		return QDateTime();
		}

	static QDateTime futureOf(const QDateTime& dateTime, int numOfDays)
		{
		Q_UNUSED(dateTime);Q_UNUSED(numOfDays);
		return QDateTime();
		}
	};

#endif // __CALENDATEUTILS_H__

// End of File
