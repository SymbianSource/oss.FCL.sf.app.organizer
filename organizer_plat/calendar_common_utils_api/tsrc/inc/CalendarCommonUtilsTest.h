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
* Description: STIF testclass declaration
*
*/

#ifndef CALENDARCOMMONUTILSTEST_H
#define CALENDARCOMMONUTILSTEST_H

//  INCLUDES
#include <StifLogger.h>
#include <TestScripterInternal.h>
#include <StifTestModule.h>
#include <TestclassAssert.h>

// For CalenViewUtils.
#include <calenviewutils.h>
#include <aknview.h>
#include <bautils.h>

// For CalenActionUiUtils.
#include <calenactionuiutils.h>
#include <calentry.h>
#include <calentryview.h>
#include <caleninterimutils2.h>
#include <calendateutils.h>
#include <caleninstanceid.h>
#include <calrrule.h>

// For CalenAgendaUtils.
#include <calenagendautils.h>

// For CalenDateUtils.
#include <calendateutils.h>

// CONSTANTS
//const ?type ?constant_var = ?constant;

// MACROS
//#define ?macro ?macro_def
#define COMMONUTILS_TEST_CLASS_VERSION_MAJOR 1
#define COMMONUTILS_TEST_CLASS_VERSION_MINOR 0
#define COMMONUTILS_TEST_CLASS_VERSION_BUILD 0

// Logging path
_LIT( KCalendarCommonUtilsTestLogPath, 
        "//logs//testframework//CalendarCommonUtilsTest//" ); 
// Log file
_LIT( KCalendarCommonUtilsTestLogFile, 
        "CalendarCommonUtilsTest.txt" ); 
_LIT( KCalendarCommonUtilsTestLogFileWithTitle, 
        "CalendarCommonUtilsTest_[%S].txt" );

// FUNCTION PROTOTYPES
//?type ?function_name(?arg_list);

// FORWARD DECLARATIONS
//class ?FORWARD_CLASSNAME;
class CCalendarCommonUtilsTest;
class CCalProgressCallBackListener;
// For CalenActionUiUtils.
class CCalEntry;
class CCalSession;
class CCalEntryView;

// DATA TYPES
//enum ?declaration
//typedef ?declaration
//extern ?data_type;

// CLASS DECLARATION

/**
*  CCalendarCommonUtilsTest test class for STIF Test Framework TestScripter.
*  ?other_description_lines
*
*  @lib ?library
*  @since ?Series60_version
*/
NONSHARABLE_CLASS(CCalendarCommonUtilsTest) : public CScriptBase
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CCalendarCommonUtilsTest* NewL( CTestModuleIf& aTestModuleIf );

        /**
        * Destructor.
        */
        virtual ~CCalendarCommonUtilsTest();

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
        CCalendarCommonUtilsTest( CTestModuleIf& aTestModuleIf );

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
        * Method used to log version of test class
        */
        void SendTestClassVersion();

        /**
        * Test funtion to test the API CalenViewUtils::DateQueryL().
        * @param aItem Script line containing parameters.
        */
        TInt TestViewUtilsDateQueryL( CStifItemParser& aItem );
        
        /**
        * Test function to test the API CalenActionUiUtils::DateQueryL().
        * @param aItem Script line containing parameters.
        */
        TInt TestActionUiDateQueryL( CStifItemParser& aItem );
        
        /**
        * Test function to verify the API 
        * CalenActionUiUtils::ShowDeleteConfirmationQueryL().
        * @param aItem Script line containing parameters.
        */
        TInt TestShowDeleteConfQueryL( CStifItemParser& aItem );
        
        /**
        * Test function to verify the API
        * CalenActionUiUtils::ShowRepeatTypeQueryL().
        * @param aItem Script line containing parameters.
        */
        TInt TestShowRepeatTypeQueryL( CStifItemParser& aItem );
        
        /**
        * Test function to verify the API
        * CalenActionUiUtils::ShowNewNoteQueryL().
        * @param aItem Script line containing parameters.
        */
        TInt TestShowNewNoteQueryL( CStifItemParser& aItem );
        
        /**
        * Test function to verify the API
        * CalenActionUiUtils::SetToDoCompleteStatusL().
        * @param aItem Script line containing parameters.
        */
        TInt TestSetToDoCompleteStatusL( CStifItemParser& aItem );
        
        /**
        * Test function to verify the API
        * CalenActionUiUtils::FindPossibleInstanceL().
        * @param aItem Script line containing parameters.
        */
        TInt TestFindPossibleInstanceL( CStifItemParser& aItem );
        
        /**
        * Test function to verify the API
        * CalenAgendaUtils::CreateEntryIdListForDayL().
        * @param aItem Script line containing parameters.
        */
        TInt TestCreateEntryIdListForDayL( CStifItemParser& aItem );
        
        /**
        * Test function to verify the API
        * CalenAgendaUtils::FindEventsForDayRangeL().
        * @param aItem Script line containing parameters.
        */
        TInt TestFindEventsForDayRangeL( CStifItemParser& aItem );
        
        /**
        * Test function to verify the API
        * CalenAgendaUtils::FindTodosForDayRangeL().
        * @param aItem Script line containing parameters.
        */
        TInt TestFindTodosForDayRangeL( CStifItemParser& aItem );
        
        /**
        * Test function to verify the API
        * CalenAgendaUtils::RemoveEntriesEndingAtMidnightL().
        * @param aItem Script line containing parameters.
        */
        TInt TestRemoveEntriesEndingAtMidnightL( CStifItemParser& aItem );
        
        /**
        * Test function to verify the API
        * CalenAgendaUtils::EndsAtStartOfDayL().
        * @param aItem Script line containing parameters.
        */
        TInt TestEndsAtStartOfDayL( CStifItemParser& aItem );
        
        /**
        * Test function to verify the API
        * CalenAgendaUtils::DurationL().
        * @param aItem Script line containing parameters.
        */
        TInt TestDurationL( CStifItemParser& aItem );
        
        /**
        * Test function to verify the API
        * CalenAgendaUtils::EntryTimeL().
        * @param aItem Script line containing parameters.
        */
        TInt TestEntryTimeL( CStifItemParser& aItem );
        
        /**
        * Test function to verify the API
        * CalenAgendaUtils::IsTimedEntryL().
        * @param aItem Script line containing parameters.
        */
        TInt TestIsTimedEntryL( CStifItemParser& aItem );
        
        /**
        * Test function to verify the API
        * CalenAgendaUtils::IsRepeatingL().
        * @param aItem Script line containing parameters.
        */
        TInt TestIsRepeatingL( CStifItemParser& aItem );
        
        /**
        * Test function to verify the API
        * CalenAgendaUtils::IsEmptyText().
        * @param aItem Script line containing parameters.
        */
        TInt TestIsEmptyTextL( CStifItemParser& aItem );
        
        //ADD NEW METHOD DEC HERE
        //[TestMethods] - Do not remove
        
    private:    // New functions
        
        /**
        * The parser utility, used to Parse input from configuration file.
        * @param aItem Script line containing parameters.
        */
        void ParseInputForDateL( CStifItemParser& aItem, 
                TDateTime& aInput,
                TDateTime& aResult );
        
        /**
        * The parser utility, used to parse input from configuration file.
        * @param aItem Script line containing parameters.
        */
        TInt ParseInputForReminderTypeL( CStifItemParser& aItem, 
                CalenActionUiUtils::TDeleteConfirmationType& aReminderType );
        
        /**
        * The parser utility, used to Parse input from configuration file.
        * @param aItem Script line containing parameters.
        * @param aRange The calendar entry recurrence range.
        */
        CalenActionUiUtils::TRepeatQueryType ParseInputForRecurrenceRangeL( 
                CStifItemParser& aItem,
                CalCommon::TRecurrenceRange& aRange );
        
        /**
        * The parser utility, used to Parse input from configuration file.
        * @param aItem Script line containing parameters.
        */
        void ParseInputForNewNoteSelectionL( CStifItemParser& aItem,
                TInt& aExpectedSelection );
        
        /**
        * The parser utility, used to parse input from configuratoin file.
        * @param aItem Script list containing parameters.
        */
        CCalEntry* ParseInputForToDoStatusL( CStifItemParser& aItem,
                CCalSession& aSession,
                CCalEntryView& aEntryView );
        
        /**
        * The parser utility, used to parse input from configuratoin file.
        * @param aItem Script list containing parameters.
        */
        CCalEntry* ParseInputForFindInstanceL( CStifItemParser& aItem,
                CCalSession& aSession, 
                CCalEntryView& aEntryView );
        
        /**
        * Returns the calendar entry type based on the text in the string.
        * @return CCalEntry::TType The type of entry to be crated.
        */
        CCalEntry::TType GetEntryTypeFromString( TPtrC16& aString );
        
        /**
        * The parser utility, used to parse input from configuration file.
        * @param aItem Script list containing parameters.
        */
        TInt ParseInputForCreateEntryIdListL( 
                CStifItemParser& aItem, 
                CCalProgressCallBackListener& aListener, 
                TDateTime& aDateTime );
        
        /**
        * The parser utility, used to parse input from configuration file.
        * @param aItem Script list containing parameters.
        */
        TInt ParseInputForFindEventsForDayRangeL( 
                CStifItemParser& aItem, 
                CCalProgressCallBackListener& aListener, 
                TDateTime& aStartDateTime, 
                TDateTime& aEndDateTime, 
                CalCommon::TCalViewFilter& aViewFilter );
        
        /**
        * Returns the view filter.
        * @return CalCommon::TCalViewFilter.
        */
        CalCommon::TCalViewFilter GetFilterTypeFromValue( TPtrC16& aType );
        
        /**
        * The parser utility, used to parse input from configuration file.
        * @param aItem Script list containing parameters.
        */
        TInt ParseInputForRemoveEntriesBeforeMidnightL( 
                CStifItemParser& aItem, 
                CCalProgressCallBackListener& aListener, 
                TDateTime& aStartDateTime );
        
        /**
        * The parser utility, used to parse input from configuration file.
        * @param aItem Script list containing parameters.
        */
        CCalEntry* ParseInputForEntryL( 
                CStifItemParser& aItem,
                CCalProgressCallBackListener& aListener, 
                TInt& aExpectedResult, 
                TTime& aEntryTime );
        
        /**
        * Returns the RRule type.
        * @return TCalRRule::TType.
        */
        TCalRRule::TType CheckRRuleFromString( TPtrC16& aType );
        
        /**
        * The parser utility, used to parse input from configuration file.
        * @param aItem Script list containing parameters.
        */
        TBool ParseInputForEntryTypeL( 
                CStifItemParser& aItem, 
                CCalEntry::TType& aEntryType );
        
        /**
        * The parser utility, used to parse input from configuration file.
        * @param aItem Script list containing parameters.
        */
        TBool ParseInputForIsEmptyTextL( 
                CStifItemParser& aItem, 
                TPtrC& aEmptyText );
        
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

#endif      // CALENDARCOMMONUTILSTEST_H

// End of File
