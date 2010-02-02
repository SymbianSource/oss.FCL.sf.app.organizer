/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: This file contains testclass implementation.
*
*/

// [INCLUDE FILES] - do not remove
#include <e32svr.h>
#include <StifParser.h>
#include <Stiftestinterface.h>

#include "CalendarDateUtilsTest.h"

// EXTERNAL DATA STRUCTURES
//extern  ?external_data;

// EXTERNAL FUNCTION PROTOTYPES  
//extern ?external_function( ?arg_type,?arg_type );

// CONSTANTS
//const ?type ?constant_var = ?constant;

// MACROS
//#define ?macro ?macro_def

// LOCAL CONSTANTS AND MACROS
//const ?type ?constant_var = ?constant;
//#define ?macro_name ?macro_def

// MODULE DATA STRUCTURES
//enum ?declaration
//typedef ?declaration

// LOCAL FUNCTION PROTOTYPES
//?type ?function_name( ?arg_type, ?arg_type );

// FORWARD DECLARATIONS
//class ?FORWARD_CLASSNAME;

//  LOCAL CONSTANTS AND MACROS
const TInt KDefaultStartTime( 8 );

// ============================= LOCAL FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// ?function_name ?description.
// ?description
// Returns: ?value_1: ?description
//          ?value_n: ?description_line1
//                    ?description_line2
// -----------------------------------------------------------------------------
//
/*
?type ?function_name(
    ?arg_type arg,  // ?description
    ?arg_type arg)  // ?description
    {

    ?code  // ?comment

    // ?comment
    ?code
    }
*/

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CCalendarCommonUtilsTest::Delete
// Delete here all resources allocated and opened from test methods. 
// Called from destructor. 
// -----------------------------------------------------------------------------
//
void CCalendarDateUtilsTest::Delete() 
    {
    // Nothing.
    }

// -----------------------------------------------------------------------------
// CCalendarCommonUtilsTest::RunMethodL
// Run specified method. Contains also table of test mothods and their names.
// -----------------------------------------------------------------------------
//
TInt CCalendarDateUtilsTest::RunMethodL( 
        CStifItemParser& aItem ) 
    {

    static TStifFunctionInfo const KFunctions[] =
        {
        // Copy this line for every implemented function.
        // First string is the function name used in TestScripter script file.
        // Second is the actual implementation member function. 
        // Example: ENTRY( "Example", CCalendarDateUtilsTest::ExampleL ),

        /** Test functions for API CalenDateUtils */
        ENTRY( "TestIsValidDay", 
                CCalendarDateUtilsTest::TestIsValidDayL ),
        ENTRY( "TestIsOnToday", 
                CCalendarDateUtilsTest::TestIsOnTodayL ),
        ENTRY( "TestToday", 
                CCalendarDateUtilsTest::TestTodayL ),
        ENTRY( "TestDefaultTime", 
                CCalendarDateUtilsTest::TestDefaultTimeL ),
        ENTRY( "TestMaxTime", 
                CCalendarDateUtilsTest::TestMaxTimeL ),
        ENTRY( "TestMinTime", 
                CCalendarDateUtilsTest::TestMinTimeL ),
        ENTRY( "TestNow", 
                CCalendarDateUtilsTest::TestNow ),
        ENTRY( "TestRoundToPreviousHour", 
                CCalendarDateUtilsTest::TestRoundToPreviousHourL ),
        ENTRY( "TestLimitToValidTime", 
                CCalendarDateUtilsTest::TestLimitToValidTimeL ),
        ENTRY( "TestOnSameDay", 
                CCalendarDateUtilsTest::TestOnSameDayL ),
        ENTRY( "TestOnSameMonth", 
                CCalendarDateUtilsTest::TestOnSameMonthL ),
        ENTRY( "TestBeginningOfDay", 
                CCalendarDateUtilsTest::TestBeginningOfDayL ),
        ENTRY( "TestDisplayTimeOnDay", 
                CCalendarDateUtilsTest::TestDisplayTimeOnDayL ),
        ENTRY( "TestTimeRangesIntersect", 
                CCalendarDateUtilsTest::TestTimeRangesIntersectL ),
        ENTRY( "TestTimeOfDay", 
                CCalendarDateUtilsTest::TestTimeOfDayL ),
        ENTRY( "TestIsNullTime", 
                CCalendarDateUtilsTest::TestIsNullTimeL ),

        //ADD NEW ENTRY HERE
        // [test cases entries] - Do not remove
        };

    const TInt count = sizeof( KFunctions ) / sizeof( TStifFunctionInfo );
    return RunInternalL( KFunctions, count, aItem );

    }

// -----------------------------------------------------------------------------
// CCalendarDateUtilsTest::ExampleL
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CCalendarDateUtilsTest::ExampleL( CStifItemParser& aItem )
    {

    // Print to UI
    _LIT( KCalendarDateUtilsTest, "CalendarDateUtilsTest" );
    _LIT( KExample, "In Example" );
    TestModuleIf().Printf( 0, KCalendarDateUtilsTest, KExample );
    // Print to log file
    iLog->Log( KExample );

    TInt index = 0;
    TPtrC parseString;
    _LIT( KParam, "Param[%i]: %S" );
    while( KErrNone == aItem.GetNextString ( parseString ) )
        {
        TestModuleIf().Printf( index, KCalendarDateUtilsTest, 
                                KParam, index, &parseString );
        index++;
        }

    return KErrNone;

    }

// -----------------------------------------------------------------------------
// CCalendarDateUtilsTest::TestIsValidDayL
// Test case to verify if a given day is within a specified range or not.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CCalendarDateUtilsTest::TestIsValidDayL( CStifItemParser& aItem )
    {
    
    // Print to UI
    _LIT( KCalendarDateUtilsTest, "CalendarDateUtilsTest" );
    _LIT( KExample, "In TestIsValidDayL" );
    TestModuleIf().Printf( 0, KCalendarDateUtilsTest, KExample );
    // Print to log file
    iLog->Log( KExample );
    
    // Initialization. Parse the input from the configration file.
    TDateTime dateTime;
    TBool expectedResult = ParseInputL( aItem, dateTime );
    TTime testTime = dateTime;

    // Make the call to the API.
    TBool gotResult = CalenDateUtils::IsValidDay( testTime );

    // Validate the result.
    if( gotResult != expectedResult )
        {
        return KErrGeneral;
        }
    return KErrNone;

    }

// -----------------------------------------------------------------------------
// CCalendarDateUtilsTest::TestIsOnTodayL
// Test case to verify if a given time is within today.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CCalendarDateUtilsTest::TestIsOnTodayL( CStifItemParser& aItem )
    {
    
    // Print to UI
    _LIT( KCalendarDateUtilsTest, "CalendarDateUtilsTest" );
    _LIT( KExample, "In TestIsOnTodayL" );
    TestModuleIf().Printf( 0, KCalendarDateUtilsTest, KExample );
    // Print to log file
    iLog->Log( KExample );
    
    // Initialization. Parse the input from the configuration file.
    TDateTime dateTime;
    TBool expectedResult = ParseInputL( aItem, dateTime );
    TTime testTime( dateTime );

    // Make the call to the API.
    TBool gotResult = CalenDateUtils::IsOnToday( testTime );

    // Validate teh result.
    if( gotResult != expectedResult )
        {
        return  KErrGeneral;
        }
    return KErrNone;

    }

// -----------------------------------------------------------------------------
// CCalendarDateUtilsTest::TestTodayL
// Test case to verify whether the day obtained is today or not.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CCalendarDateUtilsTest::TestTodayL( CStifItemParser& aItem )
    {
    
    // Print to UI
    _LIT( KCalendarDateUtilsTest, "CalendarDateUtilsTest" );
    _LIT( KExample, "In TestTodayL" );
    TestModuleIf().Printf( 0, KCalendarDateUtilsTest, KExample );
    // Print to log file
    iLog->Log( KExample );

    // Initialization. Parse the input from the configuration file.
    TDateTime dateTime;
    ParseInputL( aItem, dateTime ) ;
    TTime testTime( dateTime );

    // Make the call to the API.
    TTime timeToday = CalenDateUtils::Today();

    // Validate the result.
    // For verifying the result, please ensure in the cfg file that the result 
    // date given is the present day.
    if( timeToday != testTime )
        {
        return KErrGeneral;
        }
    return KErrNone;

    }

// -----------------------------------------------------------------------------
// CCalendarDateUtilsTest::TestDefaultTimeL
// Test case to verify the default time.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CCalendarDateUtilsTest::TestDefaultTimeL( CStifItemParser& aItem )
    {
    
    // Print to UI
    _LIT( KCalendarDateUtilsTest, "CalendarDateUtilsTest" );
    _LIT( KExample, "In TestDefaultTimeL" );
    TestModuleIf().Printf( 0, KCalendarDateUtilsTest, KExample );
    // Print to log file
    iLog->Log( KExample );

    // Initialization. Parse the input from the configuration file.
    TDateTime dateTime;
    ParseInputL( aItem, dateTime ) ;
    TTime testTime( dateTime );

    // Make the call to the API.
    TTime resultTime = CalenDateUtils::DefaultTime( testTime );

    // Validate the result.
    if( KDefaultStartTime != resultTime.DateTime().Hour() )
        {
        return KErrGeneral;
        }
    return KErrNone;
    
    }

// -----------------------------------------------------------------------------
// CCalendarDateUtilsTest::TestMaxTimeL
// Test case to obtain the maximum time and verify the same.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CCalendarDateUtilsTest::TestMaxTimeL( CStifItemParser& aItem )
    {
    
    // Print to UI
    _LIT( KCalendarDateUtilsTest, "CalendarDateUtilsTest" );
    _LIT( KExample, "In TestMaxTimeL" );
    TestModuleIf().Printf( 0, KCalendarDateUtilsTest, KExample );
    // Print to log file
    iLog->Log( KExample );

    // Initialization. Parse the input from the configuration file.
    TDateTime dateTime;
    ParseInputL( aItem, dateTime ) ;
    TTime testTime( dateTime );

    // Make the call to the API.
    TTime resultTime = CalenDateUtils::MaxTime();

    // Validate the result.
    if( resultTime != testTime )
        {
        return KErrGeneral;
        }
    return KErrNone;

    }

// -----------------------------------------------------------------------------
// CCalendarDateUtilsTest::TestMinTimeL
// Test case to obtain the minimum time and verify the same.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CCalendarDateUtilsTest::TestMinTimeL( CStifItemParser& aItem )
    {

    // Print to UI
    _LIT( KCalendarDateUtilsTest, "CalendarDateUtilsTest" );
    _LIT( KExample, "In TestMinTimeL" );
    TestModuleIf().Printf( 0, KCalendarDateUtilsTest, KExample );
    // Print to log file
    iLog->Log( KExample );

    // Initialization. Parse the input from the configuration file.
    TDateTime dateTime;
    ParseInputL( aItem, dateTime ) ;
    TTime testTime( dateTime );

    // Make the call to the API.
    TTime resultTime = CalenDateUtils::MinTime();

    // Validate the result.
    if( resultTime != testTime )
        {
        return KErrGeneral;
        }
    return KErrNone;

    }

// -----------------------------------------------------------------------------
// CCalendarDateUtilsTest::TestNow
// Test case to obtain the System time and verify the same.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CCalendarDateUtilsTest::TestNow( CStifItemParser& /*aItem*/ )
    {
    
    // Print to UI
    _LIT( KCalendarDateUtilsTest, "CalendarDateUtilsTest" );
    _LIT( KExample, "In TestNow" );
    TestModuleIf().Printf( 0, KCalendarDateUtilsTest, KExample );
    // Print to log file
    iLog->Log( KExample );

    // Initialization.
    TTime homeTime;
    homeTime.HomeTime();
    
    // Make the call to the API.
    TTime resultTime = CalenDateUtils::Now();
    
    // Validate the result.
    if( resultTime != homeTime )
        {
        return KErrGeneral;
        }
    return KErrNone;
    
    }

// -----------------------------------------------------------------------------
// CCalendarDateUtilsTest::TestRoundToPreviousHourL
// Test case to obtain the time and check if the minute is rounded off to the 
// previous hour.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CCalendarDateUtilsTest::TestRoundToPreviousHourL( CStifItemParser& aItem )
    {
    
    // Print to UI
    _LIT( KCalendarDateUtilsTest, "CalendarDateUtilsTest" );
    _LIT( KExample, "In TestRoundToPreviousHourL" );
    TestModuleIf().Printf( 0, KCalendarDateUtilsTest, KExample );
    // Print to log file
    iLog->Log( KExample );

    // Initialization. Parse input from the configuration file.
    TDateTime dateTime;
    ParseInputL( aItem, dateTime );
    TTime testTime = dateTime;
    
    // Make the call to the API.
    TTime resultTime = CalenDateUtils::RoundToPreviousHour( testTime );

    // Validate the result.
    if( NULL != resultTime.DateTime().Minute() )
        {
        return KErrGeneral;
        }
    return KErrNone;

    }

// -----------------------------------------------------------------------------
// CCalendarDateUtilsTest::TestLimitToValidTimeL
// Test case to verify if a given time is limited to its nearest range value.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CCalendarDateUtilsTest::TestLimitToValidTimeL( CStifItemParser& aItem )
    {
    
    // Print to UI
    _LIT( KCalendarDateUtilsTest, "CalendarDateUtilsTest" );
    _LIT( KExample, "In TestLimitToValidTimeL" );
    TestModuleIf().Printf( 0, KCalendarDateUtilsTest, KExample );
    // Print to log file
    iLog->Log( KExample );
    
    // Initialization. Parse input from configuration file.
    TDateTime dateTime;
    TTime expectedResult;
    ParseInputL( aItem, dateTime );
    TTime testTime( dateTime );
    if( testTime < CalenDateUtils::MinTime())
        {
        expectedResult = CalenDateUtils::MinTime();
        }
    else if( testTime > CalenDateUtils::MaxTime())
        {
        expectedResult = CalenDateUtils::MaxTime();
        }

    // Make the call to the API.
    TTime gotResult = CalenDateUtils::LimitToValidTime( testTime );

    // Validate the obtained result.
    if( gotResult != expectedResult )
        {
        return KErrGeneral;
        }
    return KErrNone;
    
    }

// -----------------------------------------------------------------------------
// CCalendarDateUtilsTest::TestOnSameDayL
// Test case to verify if given two dates are same or not.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CCalendarDateUtilsTest::TestOnSameDayL( CStifItemParser& aItem )
    {
    
    // Print to UI
    _LIT( KCalendarDateUtilsTest, "CalendarDateUtilsTest" );
    _LIT( KExample, "In TestOnSameDayL" );
    TestModuleIf().Printf( 0, KCalendarDateUtilsTest, KExample );
    // Print to log file
    iLog->Log( KExample );
        
    // Initialization. Parse the input from the configuration file.
    RArray< TDateTime > dateTimeArray;
    RArray< TTime > timeArray;
    TBool expectedResult = ParseInputWithMultipleTTimesL( aItem, dateTimeArray );
    TInt dateTimeCount = dateTimeArray.Count();
    for( TInt index = 0; index < dateTimeCount; index++ )
        {
        TDateTime tempDateTime = dateTimeArray[ index ];
        TTime tempTime( tempDateTime );
        timeArray.Append( tempTime );
        }
    
    // Make the call to the API.
    TBool gotResult = CalenDateUtils::OnSameDay( 
            timeArray[ 0 ], 
            timeArray[ 1 ] );
    // Cleanup.
    dateTimeArray.Close();
    timeArray.Close();
    
    // Validate the result.
    if( gotResult != expectedResult )
        {
        return KErrGeneral;
        }
    return KErrNone;
    
    }

// -----------------------------------------------------------------------------
// CCalendarDateUtilsTest::TestOnSameDayL
// Test case to verify if months of given two dates are same or not.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CCalendarDateUtilsTest::TestOnSameMonthL( CStifItemParser& aItem )
    {
    
    // Print to UI
    _LIT( KCalendarDateUtilsTest, "CalendarDateUtilsTest" );
    _LIT( KExample, "In TestOnSameMonthL" );
    TestModuleIf().Printf( 0, KCalendarDateUtilsTest, KExample );
    // Print to log file
    iLog->Log( KExample );

    // Initialization. Parse the input from the configuration file.
    RArray< TDateTime > dateTimeArray;
    RArray< TTime > timeArray;
    TBool expectedResult = ParseInputWithMultipleTTimesL( aItem, dateTimeArray );
    TInt dateTimeCount = dateTimeArray.Count();
    for( TInt index = 0; index < dateTimeCount; index++ )
        {
        TDateTime tempDateTime = dateTimeArray[ index ];
        TTime tempTime( tempDateTime );
        timeArray.Append( tempTime );
        }

    // Make the call to the API.
    TBool gotResult = CalenDateUtils::OnSameMonth( 
            timeArray[ 0 ], 
            timeArray[ 1 ] );
    // Cleanup.
    dateTimeArray.Close();
    timeArray.Close();

    // Validate the result.
    if( gotResult != expectedResult )
        {
        return KErrGeneral;
        }
    return KErrNone;

    }

// -----------------------------------------------------------------------------
// CCalendarDateUtilsTest::TestBeginningOfDayL
// Test case to verify if the given date's time is changed to beginning of the 
// day or not.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CCalendarDateUtilsTest::TestBeginningOfDayL( CStifItemParser& aItem )
    {
    
    // Print to UI
    _LIT( KCalendarDateUtilsTest, "CalendarDateUtilsTest" );
    _LIT( KExample, "In TestBeginningOfDayL" );
    TestModuleIf().Printf( 0, KCalendarDateUtilsTest, KExample );
    // Print to log file
    iLog->Log( KExample );

    // Initialization. Parse input from the configuration file.
    TDateTime dateTime;
    ParseInputL( aItem, dateTime );
    TTime testTime = dateTime;

    // Make the call to the API.
    TTime resultTime = CalenDateUtils::BeginningOfDay( testTime );

    // Validate the result.
    if( NULL != resultTime.DateTime().Minute() ||
            NULL != resultTime.DateTime().Hour() )
        {
        return KErrGeneral;
        }
    return KErrNone;
    
    }

// -----------------------------------------------------------------------------
// CCalendarDateUtilsTest::TestDisplayTimeOnDayL
// Test case to verify if given two dates are same or not. 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CCalendarDateUtilsTest::TestDisplayTimeOnDayL( CStifItemParser& aItem )
    {
    
    // Print to UI
    _LIT( KCalendarDateUtilsTest, "CalendarDateUtilsTest" );
    _LIT( KExample, "In TestDisplayTimeOnDayL" );
    TestModuleIf().Printf( 0, KCalendarDateUtilsTest, KExample );
    // Print to log file
    iLog->Log( KExample );

    // Initialization. Parse the input from the configuration file.
    TInt returnValue = KErrNone;
    RArray< TDateTime > dateTimeArray;
    RArray< TTime > timeArray;
    ParseInputWithMultipleTTimesL( aItem, dateTimeArray );
    TInt dateTimeCount = dateTimeArray.Count();
    for( TInt index = 0; index < dateTimeCount; index++ )
        {
        TDateTime tempDateTime = dateTimeArray[ index ];
        TTime tempTime( tempDateTime );
        timeArray.Append( tempTime );
        }

    // Make the call to the API.
    TTime gotResult = CalenDateUtils::DisplayTimeOnDay( 
            timeArray[ 0 ], 
            timeArray[ 1 ] );
    
    // Verify the result.
    if( CalenDateUtils::OnSameDay( timeArray[ 0 ], timeArray[ 1 ] ) )
        {
        if( gotResult.DateTime().Hour() != timeArray[0].DateTime().Hour() )
            {
            returnValue = KErrGeneral;
            }
        }
    else if( NULL != gotResult.DateTime().Hour() )
        {
        returnValue = KErrGeneral;
        }
    
    // Cleanup.
    dateTimeArray.Close();
    timeArray.Close();

    return returnValue;

    }

// -----------------------------------------------------------------------------
// CCalendarDateUtilsTest::TestTimeRangesIntersectL
// Test case to verify if given time values are intersecting or not. 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CCalendarDateUtilsTest::TestTimeRangesIntersectL( CStifItemParser& aItem )
    {
    
    // Print to UI
    _LIT( KCalendarDateUtilsTest, "CalendarDateUtilsTest" );
    _LIT( KExample, "In TestTimeRangesIntersectL" );
    TestModuleIf().Printf( 0, KCalendarDateUtilsTest, KExample );
    // Print to log file
    iLog->Log( KExample );

    // Initialization. Parse the input from the configuration file.
    RArray< TDateTime > dateTimeArray;
    RArray< TTime > timeArray;
    TBool expectedResult = ParseInputWithMultipleTTimesL( aItem, dateTimeArray );
    TInt dateTimeCount = dateTimeArray.Count();
    for( TInt index = 0; index < dateTimeCount; index++ )
        {
        TDateTime tempDateTime = dateTimeArray[ index ];
        TTime tempTime( tempDateTime );
        timeArray.Append( tempTime );
        }

    // Make the call to the API.
    TBool gotResult = CalenDateUtils::TimeRangesIntersect( 
            timeArray[ 0 ], 
            timeArray[ 1 ],
            timeArray[ 2 ], 
            timeArray[ 3 ] );
    // Cleanup.
    dateTimeArray.Close();
    timeArray.Close();

    // Validate the result.
    if( gotResult != expectedResult )
        {
        return KErrGeneral;
        }
    return KErrNone;
    
    }

// -----------------------------------------------------------------------------
// CCalendarDateUtilsTest::TestTimeOfDayL
// Test case to verify if the API returns the correct minutes from midnight for
// a given time.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CCalendarDateUtilsTest::TestTimeOfDayL( CStifItemParser& aItem )
    {
    
    // Print to UI
    _LIT( KCalendarDateUtilsTest, "CalendarDateUtilsTest" );
    _LIT( KExample, "In TestTimeOfDayL" );
    TestModuleIf().Printf( 0, KCalendarDateUtilsTest, KExample );
    // Print to log file
    iLog->Log( KExample );

    // Initialization. Parse input from the configuration file.
    TDateTime dateTime;
    ParseInputL( aItem, dateTime );
    TTime testTime = dateTime;

    // Make the call to the API.
    TTimeIntervalMinutes diffFromMid = CalenDateUtils::TimeOfDay( testTime );
    TInt expectedResult = dateTime.Hour() * 60 + 
                        dateTime.Minute() + 
                        dateTime.Second() / 60;
    TInt gotResult = diffFromMid.Int();
    
    // Validate the result.
    if( gotResult != expectedResult )
        {
        return KErrGeneral;
        }
    return KErrNone;
    
    }

// -----------------------------------------------------------------------------
// CCalendarDateUtilsTest::TestIsNullTimeL
// Test case to verify if the given time value is NULL or not.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CCalendarDateUtilsTest::TestIsNullTimeL( CStifItemParser& aItem )
    {
    
    // Print to UI
    _LIT( KCalendarDateUtilsTest, "CalendarDateUtilsTest" );
    _LIT( KExample, "In TestIsNullTimeL" );
    TestModuleIf().Printf( 0, KCalendarDateUtilsTest, KExample );
    // Print to log file
    iLog->Log( KExample );

    // Initialization. Parse input from the configuration file.
    TDateTime dateTime;
    TInt expectedResult = ParseInputL( aItem, dateTime );
    TTime testTime = dateTime;
    TCalTime calTime;
    calTime.SetTimeLocalL( dateTime );
    
    // Make the call to the API.
    TBool gotResult = CalenDateUtils::IsNullTimeL( calTime );

    // Validate the result.
    if( gotResult != expectedResult )
        {
        return KErrGeneral;
        }
    return KErrNone;
    
    }

// -----------------------------------------------------------------------------
// CCalendarCommonUtilsTest::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
/*
TInt CCalendarCommonUtilsTest::?member_function(
   CItemParser& aItem )
   {

   ?code

   }
*/

// ========================== OTHER EXPORTED FUNCTIONS =========================
// None

//  [End of File] - Do not remove
