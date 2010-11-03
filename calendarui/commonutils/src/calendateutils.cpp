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
* Description:   	Utility class providing utility functions to know  beginning of the day, maximum/minimum date allowed in calendar, 
*			day range, on same day, on same month etc.
*
*/



//debug
#include "calendarui_debug.h"

#include <calendateutils.h>
#include <caltime.h> // For Min/MaxValidDate

//  LOCAL CONSTANTS AND MACROS
const TInt KDefaultStartTime(8);    // 8 am

// ============================ MEMBER FUNCTIONS ==============================

// ============================  CalenDateUtils  ==============================
// -----------------------------------------------------------------------------
// ?classname::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CalenDateUtils::OnSameDay( const TTime& aX, const TTime& aY )
    {
    TRACE_ENTRY_POINT;
    
    TDateTime x = aX.DateTime();
    TDateTime y = aY.DateTime();
    
    TRACE_EXIT_POINT;
    return x.Year()  == y.Year() 
        && x.Month() == y.Month()
        && x.Day()   == y.Day();
    }

// -----------------------------------------------------------------------------
// ?classname::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CalenDateUtils::OnSameMonth( const TTime& aX, const TTime& aY )
    {
    TRACE_ENTRY_POINT;
    
    TDateTime x = aX.DateTime();
    TDateTime y = aY.DateTime();
    
    TRACE_EXIT_POINT;
    return ( x.Year() == y.Year() && x.Month() == y.Month() );
    }

// -----------------------------------------------------------------------------
// ?classname::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TTime CalenDateUtils::BeginningOfDay( const TTime& aStartTime )
    {
    TRACE_ENTRY_POINT;
    
    TTime zero(TInt64(0));
    
    TRACE_EXIT_POINT;
    return zero + aStartTime.DaysFrom( zero );
    }

// -----------------------------------------------------------------------------
// ?classname::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TTime CalenDateUtils::DisplayTimeOnDay( const TTime& aStartTime,
                                                 const TTime& aDay )
    {
    TRACE_ENTRY_POINT;
    
    if( ! OnSameDay( aStartTime, aDay ) )
        {
        TRACE_EXIT_POINT;
        return BeginningOfDay( aDay );
        }
    else 
        {
        TRACE_EXIT_POINT;
        return aStartTime;
        }
    }

// -----------------------------------------------------------------------------
// ?classname::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CalenDateUtils::TimeRangesIntersect( const TTime& aXStart,
                                                    const TTime& aXEnd,
                                                    const TTime& aYStart,
                                                    const TTime& aYEnd )
    {
    TRACE_ENTRY_POINT;
    
    TRACE_EXIT_POINT;
    return (! ( aYEnd <= aXStart || aXEnd <= aYStart ))
        || (aXStart == aXEnd && aYStart <= aXStart && aXStart < aYEnd)
        || (aYStart == aYEnd && aXStart <= aYStart && aYStart < aXEnd)
        || (aXStart == aXEnd && aYStart == aYEnd && aXStart == aYStart);
    }

// -----------------------------------------------------------------------------
// ?classname::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CalenDateUtils::IsValidDay( const TTime& aTime )
    {
    TRACE_ENTRY_POINT;
    
    // Interim API supports range from 1900-2100, 
    TRACE_EXIT_POINT;
    return ( MinTime() <= aTime && aTime <= MaxTime() );
    }

// -----------------------------------------------------------------------------
// ?classname::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CalenDateUtils::GetDayRangeL( const TTime& aStartDay,
                                            const TTime& aEndDay,
                                            CalCommon::TCalTimeRange& aRange )
    {
    TRACE_ENTRY_POINT;

    TDateTime start( aStartDay.DateTime() );
    TDateTime end( aEndDay.DateTime() );

    start.SetHour( 0 );
    start.SetMinute( 0 );
    start.SetSecond( 0 );
    start.SetMicroSecond( 0 );

    end.SetHour( 23 );
    end.SetMinute( 59 );
    end.SetSecond( 59 );
    end.SetMicroSecond( 0 );

    // prevent overflow
    TCalTime endDate;
    endDate.SetTimeLocalL( LimitToValidTime( TTime( end ) ) );

    TCalTime startDate;
    startDate.SetTimeLocalL( LimitToValidTime( TTime( start ) ) );

    CalCommon::TCalTimeRange dayrange( startDate, endDate );

    aRange = dayrange;
    
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// ?classname::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CalenDateUtils::IsNullTimeL( TCalTime& aTime )
    {
    TRACE_ENTRY_POINT;
    
    TRACE_EXIT_POINT;
    return( aTime.TimeLocalL() == Time::NullTTime() );
    }

// -----------------------------------------------------------------------------
// ?classname::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TTime CalenDateUtils::LimitToValidTime( const TTime& aTime )
    {
    TRACE_ENTRY_POINT;
    
    TTime valid = aTime;
    valid = valid > MaxTime() ? MaxTime() : valid;
    valid = valid < MinTime() ? MinTime() : valid;
    
    TRACE_EXIT_POINT;
    return valid;
    }

// -----------------------------------------------------------------------------
// ?classname::?member_function
// Returns maximum time allowed, 31.12.2100 0:00 is max so 30.12.2100 is last actual
// date to be used.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TTime CalenDateUtils::MaxTime()
    {
    TRACE_ENTRY_POINT;
    
    TTime time( TCalTime::MaxTime() - TTimeIntervalMinutes( 1 ) );
    
    TRACE_EXIT_POINT;
    return time;
    }

// -----------------------------------------------------------------------------
// ?classname::?member_function
// Returns minimum time allowed, 1.1.1900 0:00 is min
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TTime CalenDateUtils::MinTime()
    {
    TRACE_ENTRY_POINT;
    
    TRACE_EXIT_POINT;
    return TCalTime::MinTime();
    }

// -----------------------------------------------------------------------------
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TTimeIntervalMinutes CalenDateUtils::TimeOfDay( const TTime& aDateTime )
    {
    TRACE_ENTRY_POINT;
    
    TTime midnight = CalenDateUtils::BeginningOfDay( aDateTime );
    TTimeIntervalMinutes result;
    aDateTime.MinutesFrom( midnight, result );
    
    TRACE_EXIT_POINT;
    return result;
    }

// -----------------------------------------------------------------------------
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TTime CalenDateUtils::RoundToPreviousHour( const TTime& aTime ) 
    {
    TRACE_ENTRY_POINT;

    TDateTime dt = aTime.DateTime();
    dt.SetMinute(0);
    dt.SetSecond(0);
    dt.SetMicroSecond(0);

    TRACE_EXIT_POINT;
    return TTime( dt );
    }

// -----------------------------------------------------------------------------
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TTimeIntervalMinutes CalenDateUtils::RoundToPreviousHour( const TTimeIntervalMinutes& aMinutes )
    {
    TRACE_ENTRY_POINT;

    TRACE_EXIT_POINT;
    return TTimeIntervalMinutes( (aMinutes.Int() / 60) * 60 );
    }

// -----------------------------------------------------------------------------
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TTime CalenDateUtils::Now()
    {
    TRACE_ENTRY_POINT;

    TTime now;
    now.HomeTime();

    TRACE_EXIT_POINT;
    return now;
    }

// -----------------------------------------------------------------------------
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TTime CalenDateUtils::Today()
    {
    TRACE_ENTRY_POINT;

    TRACE_EXIT_POINT;
    return CalenDateUtils::BeginningOfDay( Now() );
    }

// -----------------------------------------------------------------------------
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CalenDateUtils::IsOnToday( const TTime& aTime )
    {
    TRACE_ENTRY_POINT;

    TRACE_EXIT_POINT;
    return CalenDateUtils::OnSameDay( aTime, Today() );
    }

// -----------------------------------------------------------------------------
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TTime CalenDateUtils::DefaultTime( const TTime& aDate )
    {
    TRACE_ENTRY_POINT;

    TTime dateTime( Time::NullTTime() );
    // DD:MM:YY @ hh:mm:ss
    dateTime = CalenDateUtils::BeginningOfDay( aDate ); 
    // DD:MM:YY @ 00:00:00
    dateTime += TTimeIntervalHours( KDefaultStartTime ); // DD:MM:YY @ 08:00 am
        
    TRACE_EXIT_POINT;
    return dateTime;
    }      

// End of File
