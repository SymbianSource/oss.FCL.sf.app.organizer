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
* Description: STIF testclass CCalendarDateUtilsTest declaration.
*
*/

#ifndef CALENDARDATEUTILSTEST_H
#define CALENDARDATEUTILSTEST_H

//  INCLUDES
#include <StifLogger.h>
#include <TestScripterInternal.h>
#include <StifTestModule.h>
#include <TestclassAssert.h>

#include <aknview.h>
#include <bautils.h>

// For CalenDateUtils.
#include <calendateutils.h>

// CONSTANTS
//const ?type ?constant_var = ?constant;

// MACROS
//#define ?macro ?macro_def
#define DATEUTILS_TEST_CLASS_VERSION_MAJOR 1
#define DATEUTILS_TEST_CLASS_VERSION_MINOR 0
#define DATEUTILS_TEST_CLASS_VERSION_BUILD 0

// Logging path
_LIT( KCalendarDateUtilsTestLogPath, 
        "//logs//testframework//CalendarDateUtilsTest//" ); 
// Log file
_LIT( KCalendarDateUtilsTestLogFile, 
        "CalendarDateUtilsTest.txt" ); 
_LIT( KCalendarDateUtilsTestLogFileWithTitle, 
        "CalendarDateUtilsTest_[%S].txt" );

// FUNCTION PROTOTYPES
//?type ?function_name(?arg_list);

// FORWARD DECLARATIONS
//class ?FORWARD_CLASSNAME;

// DATA TYPES
//enum ?declaration
//typedef ?declaration
//extern ?data_type;

// CLASS DECLARATION

/**
*  CCalendarDateUtilsTest test class for STIF Test Framework TestScripter.
*  ?other_description_lines
*
*  @lib CalendarCommonUtilsTest.lib
*  @since ?Series60_version
*/
NONSHARABLE_CLASS( CCalendarDateUtilsTest ) : public CScriptBase
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CCalendarDateUtilsTest* NewL( CTestModuleIf& aTestModuleIf );

        /**
        * Destructor.
        */
        virtual ~CCalendarDateUtilsTest();

    public: // New functions

        /**
        * ?member_description.
        * @since ?Series60_version
        * @param ?arg1 ?description
        * @return ?description
        */
        //?type ?member_function( ?type ?arg1 );

    public: // Functions from base classes

        /**
        * From CScriptBase Runs a script line.
        * @since ?Series60_version
        * @param aItem Script line containing method name and parameters
        * @return Symbian OS error code
        */
        virtual TInt RunMethodL( CStifItemParser& aItem );

    protected:  // New functions

        /**
        * ?member_description.
        * @since ?Series60_version
        * @param ?arg1 ?description
        * @return ?description
        */
        //?type ?member_function( ?type ?arg1 );

    protected:  // Functions from base classes

        /**
        * From ?base_class ?member_description
        */
        //?type ?member_function();

    private:

        /**
        * C++ default constructor.
        */
        CCalendarDateUtilsTest( CTestModuleIf& aTestModuleIf );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

        // Prohibit copy constructor if not deriving from CBase.
        // ?classname( const ?classname& );
        // Prohibit assigment operator if not deriving from CBase.
        // ?classname& operator=( const ?classname& );

        /**
        * Frees all resources allocated from test methods.
        * @since ?Series60_version
        */
        void Delete();

        /**
        * Method used to log version of test class
        */
        void SendTestClassVersion();
        
        /**
        * Test methods are listed below. 
        */
        /**
        * Example test method.
        * @since ?Series60_version
        * @param aItem Script line containing parameters.
        * @return Symbian OS error code.
        */
        virtual TInt ExampleL( CStifItemParser& aItem );
        
        /**
        * Test funtion to test the API CalenDateUtils::IsValidDay().
        * @param aItem Script line containing parameters.
        */
        TInt TestIsValidDayL( CStifItemParser& aItem );
        
        /**
        * Test funtion to test the API CalenDateUtils::IsOnToday().
        * @param aItem Script line containing parameters.
        */
        TInt TestIsOnTodayL( CStifItemParser& aItem );
        
        /**
        * Test funtion to test the API CalenDateUtils::Today().
        * @param aItem Script line containing parameters.
        */
        TInt TestTodayL( CStifItemParser& aItem );
        
        /**
        * Test funtion to test the API CalenDateUtils::DefaultTime().
        * @param aItem Script line containing parameters.
        */
        TInt TestDefaultTimeL( CStifItemParser& aItem );
        
        /**
        * Test funtion to test the API CalenDateUtils::MaxTime().
        * @param aItem Script line containing parameters.
        */
        TInt TestMaxTimeL( CStifItemParser& aItem );
        
        /**
        * Test funtion to test the API CalenDateUtils::MinTime().
        * @param aItem Script line containing parameters.
        */
        TInt TestMinTimeL( CStifItemParser& aItem );
        
        /**
        * Test funtion to test the API CalenDateUtils::Now().
        * @param aItem Script line containing parameters.
        */
        TInt TestNow( CStifItemParser& aItem );
        
        /**
        * Test funtion to test the API CalenDateUtils::RoundToPreviousHour().
        * @param aItem Script line containing parameters.
        */
        TInt TestRoundToPreviousHourL( CStifItemParser& aItem );
        
        /**
        * Test funtion to test the API CalenDateUtils::LimitToValidTime().
        * @param aItem Script line containing parameters.
        */
        TInt TestLimitToValidTimeL( CStifItemParser& aItem );
        
        /**
        * Test funtion to test the API CalenDateUtils::OnSameDay().
        * @param aItem Script line containing parameters.
        */
        TInt TestOnSameDayL( CStifItemParser& aItem );
        
        /**
        * Test funtion to test the API CalenDateUtils::OnSameMonth().
        * @param aItem Script line containing parameters.
        */
        TInt TestOnSameMonthL( CStifItemParser& aItem );
        
        /**
        * Test funtion to test the API CalenDateUtils::BeginningOfDay().
        * @param aItem Script line containing parameters.
        */
        TInt TestBeginningOfDayL( CStifItemParser& aItem );
        
        /**
        * Test funtion to test the API CalenDateUtils::DisplayTimeOnDay().
        * @param aItem Script line containing parameters.
        */
        TInt TestDisplayTimeOnDayL( CStifItemParser& aItem );
        
        /**
        * Test funtion to test the API CalenDateUtils::TimeRangesIntersect().
        * @param aItem Script line containing parameters.
        */
        TInt TestTimeRangesIntersectL( CStifItemParser& aItem );
        
        /**
        * Test funtion to test the API CalenDateUtils::TimeOfDay().
        * @param aItem Script line containing parameters.
        */
        TInt TestTimeOfDayL( CStifItemParser& aItem );
        
        /**
        * Test funtion to test the API CalenDateUtils::IsNullTimeL().
        * @param aItem Script line containing parameters.
        */
        TInt TestIsNullTimeL( CStifItemParser& aItem );  
        
    private:    // New functions
        
        /**
        * The parser utility, used to Parse input from configuration file.
        * @param aItem Script line containing parameters.
        */
        TBool ParseInputL(CStifItemParser& aItem, TDateTime& aTime);
        
       
        /**
        * The parser utility, used to Parse input from configuration file.
        * @param aItem Script line containing parameters.
        */
        TBool ParseInputWithMultipleTTimesL( 
                CStifItemParser& aItem, 
                RArray< TDateTime >& aTime );
        
    public:     // Data
        // ?one_line_short_description_of_data
        //?data_declaration;

    protected:  // Data
        // ?one_line_short_description_of_data
        //?data_declaration;

    private:    // Data
        
        // ?one_line_short_description_of_data
        //?data_declaration;

        // Reserved pointer for future extension
        //TAny* iReserved;

    public:     // Friend classes
        //?friend_class_declaration;
    protected:  // Friend classes
        //?friend_class_declaration;
    private:    // Friend classes
        //?friend_class_declaration;

    };

#endif      // CALENDARDATEUTILSTEST_H

// End of File
