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

#include "calenglobaldata.h"
#include "CalendarCommonUtilsTest.h"
#include "CalProgressCallBackListener.h"

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
void CCalendarCommonUtilsTest::Delete() 
    {

    }

// -----------------------------------------------------------------------------
// CCalendarCommonUtilsTest::RunMethodL
// Run specified method. Contains also table of test mothods and their names.
// -----------------------------------------------------------------------------
//
TInt CCalendarCommonUtilsTest::RunMethodL( 
    CStifItemParser& aItem ) 
    {

    static TStifFunctionInfo const KFunctions[] =
        {  
        // Copy this line for every implemented function.
        // First string is the function name used in TestScripter script file.
        // Second is the actual implementation member function. 
        // Example: ENTRY( "Example", CCalendarCommonUtilsTest::ExampleL ),
        
        /** Test functions for API CalenViewUtils */
        ENTRY( "TestDateQueryOK", 
                CCalendarCommonUtilsTest::TestViewUtilsDateQueryL ),
        ENTRY( "TestDateQueryCancel", 
                CCalendarCommonUtilsTest::TestViewUtilsDateQueryL ),

        /** Test functions for API CalenActionUiUtils */
        ENTRY( "TestActionDateQueryOK", 
                CCalendarCommonUtilsTest::TestActionUiDateQueryL ),
        ENTRY( "TestActionDateQueryCancel", 
                CCalendarCommonUtilsTest::TestActionUiDateQueryL ),
        ENTRY( "TestShowDeleteConfQueryL", 
                CCalendarCommonUtilsTest::TestShowDeleteConfQueryL ),
        ENTRY( "TestShowRepeatTypeQueryL", 
                CCalendarCommonUtilsTest::TestShowRepeatTypeQueryL ),
        ENTRY( "TestShowNewNoteQueryL", 
                CCalendarCommonUtilsTest::TestShowNewNoteQueryL ),
        ENTRY( "TestSetToDoCompleteStatusL", 
                CCalendarCommonUtilsTest::TestSetToDoCompleteStatusL ),
        ENTRY( "TestFindPossibleInstanceL", 
                CCalendarCommonUtilsTest::TestFindPossibleInstanceL ),

        /** Test functions for API CalenAgendaUtils */
        ENTRY( "TestCreateEntryIdListForDayL", 
                CCalendarCommonUtilsTest::TestCreateEntryIdListForDayL ),
        ENTRY( "TestFindEventsForDayRangeL", 
                CCalendarCommonUtilsTest::TestFindEventsForDayRangeL ),
        ENTRY( "TestFindTodosForDayRangeL", 
                CCalendarCommonUtilsTest::TestFindTodosForDayRangeL ),
        ENTRY( "TestRemoveEntriesEndingAtMidnightL", 
                CCalendarCommonUtilsTest::TestRemoveEntriesEndingAtMidnightL ),
        ENTRY( "TestEndsAtStartOfDayL", 
                CCalendarCommonUtilsTest::TestEndsAtStartOfDayL ),
        ENTRY( "TestDurationL", 
                CCalendarCommonUtilsTest::TestDurationL ),
        ENTRY( "TestEntryTimeL", 
                CCalendarCommonUtilsTest::TestEntryTimeL ),
        ENTRY( "TestIsTimedEntryL", 
                CCalendarCommonUtilsTest::TestIsTimedEntryL ),
        ENTRY( "TestIsRepeatingL", 
                CCalendarCommonUtilsTest::TestIsRepeatingL ),
        ENTRY( "TestIsEmptyTextL", 
                CCalendarCommonUtilsTest::TestIsEmptyTextL ),
                
        //ADD NEW ENTRY HERE
        // [test cases entries] - Do not remove

        };

    const TInt count = sizeof( KFunctions ) / 
                        sizeof( TStifFunctionInfo );

    return RunInternalL( KFunctions, count, aItem );

    }

// -----------------------------------------------------------------------------
// CCalendarCommonUtilsTest::ExampleL
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CCalendarCommonUtilsTest::ExampleL( CStifItemParser& aItem )
    {

    // Print to UI
    _LIT( KCalendarCommonUtilsTest, "CalendarCommonUtilsTest" );
    _LIT( KExample, "In Example" );
    TestModuleIf().Printf( 0, KCalendarCommonUtilsTest, KExample );
    // Print to log file
    iLog->Log( KExample );

    TInt i = 0;
    TPtrC string;
    _LIT( KParam, "Param[%i]: %S" );
    while ( aItem.GetNextString ( string ) == KErrNone )
        {
        TestModuleIf().Printf( i, KCalendarCommonUtilsTest, 
                                KParam, i, &string );
        i++;
        }

    return KErrNone;

    }

// -----------------------------------------------------------------------------
// CCalendarCommonUtilsTest::TestViewUtilsDateQueryL
// Tests that the API returns the date entered by the user.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CCalendarCommonUtilsTest::TestViewUtilsDateQueryL( 
        CStifItemParser& aItem )
    {
    
    // Print to UI
    _LIT( KCalendarCommonUtilsTest, "CalendarCommonUtilsTest" );
    _LIT( KTestName, "In TestViewUtilsDateQueryL" );
    TestModuleIf().Printf( 0, KCalendarCommonUtilsTest, KTestName );
    // Print to log file
    iLog->Log( KTestName );
    
    TDateTime inputDate;
    TDateTime resultDate;
    // First parse the given input.
    ParseInputForDateL( aItem, inputDate, resultDate );
    TTime inputTime( inputDate );
    
    // Open the resource file.
    _LIT( KResource, "//resource//apps//calendar.rsc" );
    TFileName filename( KResource );

    // Get the nearest language file.
    BaflUtils::NearestLanguageFile( CCoeEnv::Static()->FsSession(), filename );
    // The offset holds the location of the resource file loaded in the memory.
    TInt fileOffset = CCoeEnv::Static()->AddResourceFileL( filename );

    // Run the test.
    CalenViewUtils::DateQueryL( inputTime, 0 );
    
    // Remove the resource file.
    CCoeEnv::Static()->DeleteResourceFile( fileOffset );
    
    inputDate = inputTime.DateTime();
    if( inputDate.Year() != resultDate.Year() || 
            inputDate.Year() != resultDate.Year() ||
            inputDate.Year() != resultDate.Year() )
        {
        return KErrGeneral;
        }
    
    return KErrNone;
    
    }

// -----------------------------------------------------------------------------
// CCalendarCommonUtilsTest::TestActionUiDateQueryL
// Tests that the API returns the date entered by the user.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CCalendarCommonUtilsTest::TestActionUiDateQueryL( 
        CStifItemParser& aItem )
    {
    
    // Print to UI
    _LIT( KCalendarCommonUtilsTest, "CalendarCommonUtilsTest" );
    _LIT( KTestName, "In TestActionUiDateQueryL" );
    TestModuleIf().Printf( 0, KCalendarCommonUtilsTest, KTestName );
    // Print to log file
    iLog->Log( KTestName );
    
    TDateTime inputDate;
    TDateTime resultDate;
    // First parse the given input.
    ParseInputForDateL( aItem, inputDate, resultDate );
    TTime inputTime( inputDate );
    
    // Open the resource file.
    _LIT( KResource, "//resource//apps//calendar.rsc" );
    TFileName filename( KResource );

    // Get the nearest language file.
    BaflUtils::NearestLanguageFile( CCoeEnv::Static()->FsSession(), filename );
    // The offset holds the location of the resource file loaded in the memory.
    TInt fileOffset = CCoeEnv::Static()->AddResourceFileL( filename );

    // Run the test.
    CalenActionUiUtils::DateQueryL( inputTime, 0 );
    
    // Remove the resource file.
    CCoeEnv::Static()->DeleteResourceFile( fileOffset );
    
    inputDate = inputTime.DateTime();
    if( inputDate.Year() != resultDate.Year() || 
            inputDate.Year() != resultDate.Year() ||
            inputDate.Year() != resultDate.Year() )
        {
        return KErrGeneral;
        }
    return KErrNone;
    
    }

// -----------------------------------------------------------------------------
// CCalendarCommonUtilsTest::TestShowDeleteConfQueryL
// Test function for verifying Dispaly of delete confirmation query.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CCalendarCommonUtilsTest::TestShowDeleteConfQueryL( CStifItemParser& aItem)
    {
    
    // Print to UI
    _LIT( KCalendarCommonUtilsTest, "CalendarCommonUtilsTest" );
    _LIT( KTestName, "In TestShowDeleteConfQueryL" );
    TestModuleIf().Printf( 0, KCalendarCommonUtilsTest, KTestName );
    // Print to log file
    iLog->Log( KTestName );
    
    // Initialization. Parse the input.
    CalenActionUiUtils::TDeleteConfirmationType confirmationType;
    TInt expectedResult;
    expectedResult= ParseInputForReminderTypeL( aItem, confirmationType );
    
    // Open the resource file.
    _LIT( KResource, "//resource//calencommonui.rsc" );
    TFileName filename( KResource );

    // Get the nearest language file.
    BaflUtils::NearestLanguageFile( CCoeEnv::Static()->FsSession(), filename );
    // The offset holds the location of the resource file loaded in the memory.
    TInt fileOffset = CCoeEnv::Static()->AddResourceFileL( filename );

    TInt gotResult = 0;
    // Make the call.
    TRAPD( error, 
            gotResult = CalenActionUiUtils::ShowDeleteConfirmationQueryL( 
                    confirmationType ) );
    
    // Remove the resource file.
    CCoeEnv::Static()->DeleteResourceFile( fileOffset );
    
    // Validate the result.
    if( expectedResult != gotResult )
        {
        if( expectedResult == error )
            {
            return KErrNone;
            }
        return KErrGeneral;
        }
    return KErrNone;
    
    }

// -----------------------------------------------------------------------------
// CCalendarCommonUtilsTest::TestShowRepeatTypeQueryL
// Test function for verifying Repeat type selected for the operation Save, Edit
// or Delete. Selection happens from a list.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CCalendarCommonUtilsTest::TestShowRepeatTypeQueryL( 
        CStifItemParser& aItem )
    {
    
    // Print to UI
    _LIT( KCalendarCommonUtilsTest, "CalendarCommonUtilsTest" );
    _LIT( KTestName, "In TestShowRepeatTypeQueryL" );
    TestModuleIf().Printf( 0, KCalendarCommonUtilsTest, KTestName );
    // Print to log file
    iLog->Log( KTestName );
    
    // Initialization, parse input.
    CalCommon::TRecurrenceRange expectedRecurrenceRange;
    CalenActionUiUtils::TRepeatQueryType repeatQueryType;
    repeatQueryType = ParseInputForRecurrenceRangeL( 
            aItem, 
            expectedRecurrenceRange );
    
    // Open the resource file.
    _LIT( KResource, "//resource//calencommonui.rsc" );
    TFileName filename( KResource );

    // Get the nearest language file.
    BaflUtils::NearestLanguageFile( CCoeEnv::Static()->FsSession(), filename );
    // The offset holds the location of the resource file loaded in the memory.
    TInt fileOffset = CCoeEnv::Static()->AddResourceFileL( filename );
    
    // Execute the test.
    CalCommon::TRecurrenceRange gotRecurrenceRange;
    TBool returnValue = CalenActionUiUtils::ShowRepeatTypeQueryL( 
            gotRecurrenceRange, 
            repeatQueryType );
    
    // Remove the resource file.
    CCoeEnv::Static()->DeleteResourceFile( fileOffset );
    
    if( gotRecurrenceRange != expectedRecurrenceRange )
        {
        return KErrGeneral;
        }
    return KErrNone;
    
    }

// -----------------------------------------------------------------------------
// CCalendarCommonUtilsTest::TestShowNewNoteQueryL
// Test function for verifying the item selected by the user for creating a 
// new reminder.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CCalendarCommonUtilsTest::TestShowNewNoteQueryL( CStifItemParser& aItem )
    {
    
    // Print to UI
    _LIT( KCalendarCommonUtilsTest, "CalendarCommonUtilsTest" );
    _LIT( KTestName, "In TestShowRepeatTypeQueryL" );
    TestModuleIf().Printf( 0, KCalendarCommonUtilsTest, KTestName );
    // Print to log file
    iLog->Log( KTestName );
    
    // Initialization. Parse the input required.
    TInt expectedSelection = 0;
    ParseInputForNewNoteSelectionL( aItem, expectedSelection );
    
    // Construct the calendar global data.
    CCalProgressCallBackListener *callBack = NULL;
    callBack = CCalProgressCallBackListener::NewL();
    CCalenGlobalData* globalData = CCalenGlobalData::NewL( *callBack );
    
    // Open the resource file.
    _LIT( KResource, "//resource//calencommonui.rsc" );
    TFileName filename( KResource );

    // Get the nearest language file.
    BaflUtils::NearestLanguageFile( CCoeEnv::Static()->FsSession(), filename );
    // The offset holds the location of the resource file loaded in the memory.
    TInt fileOffset = CCoeEnv::Static()->AddResourceFileL( filename );

    // Make the call.
    CDesCArrayFlat* listItem = NULL;
    CArrayFixFlat< TInt >* listItemCommands = NULL;
    TInt selectionMade = CalenActionUiUtils::ShowNewNoteQueryL( 
            listItem, 
            listItemCommands );
    
    // Remove the resource file.
    CCoeEnv::Static()->DeleteResourceFile( fileOffset );
    // Cleanup.
    delete callBack;
    globalData->Release();
    
    // Validate.
    if( selectionMade != expectedSelection )
        {
        return KErrGeneral;
        }
    if( -3 == expectedSelection && selectionMade == expectedSelection )
        {
        return KErrNone;
        }
    return KErrNone;
    
    }

// -----------------------------------------------------------------------------
// CCalendarCommonUtilsTest::TestSetToDoCompleteStatusL
// Test function for verifying the status change for todos.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CCalendarCommonUtilsTest::TestSetToDoCompleteStatusL( 
        CStifItemParser& aItem )
    {
    
    // Print to UI
    _LIT( KCalendarCommonUtilsTest, "CalendarCommonUtilsTest" );
    _LIT( KTestName, "In TestShowRepeatTypeQueryL" );
    TestModuleIf().Printf( 0, KCalendarCommonUtilsTest, KTestName );
    // Print to log file
    iLog->Log( KTestName );
    
    // Initialization.
    CCalProgressCallBackListener* listenerObject = NULL;
    listenerObject = CCalProgressCallBackListener::NewL();
    // Get the CCalEntryView object.
    CCalSession* calSession = listenerObject->CalSession();
    CCalEntryView* entryView = listenerObject->CalEntryView();
    CCalInstanceView* instanceView = listenerObject->CalInstanceView();
    
    // Parse the input from the configuration file.
    TBool entryStatus;
    CCalEntry* createdEntry = ParseInputForToDoStatusL( 
            aItem,
            *calSession,
            *entryView ); 

    // If to-do is completed then restore it 
    // and if it's not completed then complete it.
    if( CCalEntry::ETodoCompleted == createdEntry->StatusL() )
        {
        entryStatus = EFalse;
        }
    else
        {
        entryStatus = ETrue;
        }
    
    // The expected result.
    CCalEntry::TStatus expectedResult = createdEntry->StatusL();
    
    // Make the call to the API.
    CalenActionUiUtils::SetToDoCompleteStatusL( 
            *entryView, 
            createdEntry, 
            entryStatus );
    
    CCalEntry::TStatus gotResult = createdEntry->StatusL();
    
    // Cleanup.
    delete createdEntry;
    
    // Validate the result.
    if( expectedResult == gotResult)
        {
        return KErrGeneral;
        }
    return KErrNone;
    
    }

// -----------------------------------------------------------------------------
// CCalendarCommonUtilsTest::TestFindPossibleInstanceL
// Test function for verifying the status change for todos.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CCalendarCommonUtilsTest::TestFindPossibleInstanceL( 
        CStifItemParser& aItem )
    {
    
    // Print to UI
    _LIT( KCalendarCommonUtilsTest, "CalendarCommonUtilsTest" );
    _LIT( KTestName, "In TestFindPossibleInstanceL" );
    TestModuleIf().Printf( 0, KCalendarCommonUtilsTest, KTestName );
    // Print to log file
    iLog->Log( KTestName );
    
    // Initialization.
    CCalProgressCallBackListener* listenerObject = NULL;
    listenerObject = CCalProgressCallBackListener::NewL();
    // Get the Calendar session, entry view, instance view.
    CCalSession* calSession = listenerObject->CalSession();
    CCalEntryView* calEntryView = listenerObject->CalEntryView();
    CCalInstanceView* calInstanceView = listenerObject->CalInstanceView();
    // Parse the input from the configuration file.    
    CCalEntry* createdEntry = ParseInputForFindInstanceL( 
            aItem, 
            *calSession, 
            *calEntryView );
    TCalLocalUid expectedResult = createdEntry->LocalUidL();
    
    // Create the cal instance id.
    TCalenInstanceId calenInstanceId = TCalenInstanceId::CreateL( 
            *createdEntry, createdEntry->StartTimeL() );
    TCalLocalUid gotResult = 0;
    
    // Make the call to the API to get the CCalInstance object.
    CCalInstance* calInstance = CalenActionUiUtils::FindPossibleInstanceL( 
            calenInstanceId, 
            *calInstanceView );
    // Get the result.
    if( calInstance )
        {
        gotResult = calInstance->Entry().LocalUidL();
        }
    
    // Cleanup.
    delete createdEntry;
    delete calInstance;
    
    // Validate the result.
    if( expectedResult != gotResult )
        {
        return KErrGeneral;
        }
    return KErrNone;
    
    }

// -----------------------------------------------------------------------------
// CCalendarCommonUtilsTest::TestCreateEntryIdListForDayL
// Test function for verifying list of entries created for a given day.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CCalendarCommonUtilsTest::TestCreateEntryIdListForDayL( 
        CStifItemParser& aItem )
    {
    
    // Print to UI
    _LIT( KCalendarCommonUtilsTest, "CalendarCommonUtilsTest" );
    _LIT( KTestName, "In TestCreateEntryIdListForDayL" );
    TestModuleIf().Printf( 0, KCalendarCommonUtilsTest, KTestName );
    // Print to log file
    iLog->Log( KTestName );
    
    // Initialization. Get Calendar session, entry view and instance view.
    CCalProgressCallBackListener* listenerObject = NULL;
    listenerObject = CCalProgressCallBackListener::NewL();
    CCalInstanceView* calInstanceView = listenerObject->CalInstanceView();
    RPointerArray< CCalInstance > calInstanceList;
    
    // Parse the input from the configuration file.
    TDateTime dateTime;
    TInt expectedResult = ParseInputForCreateEntryIdListL( 
            aItem, 
            *listenerObject, 
            dateTime );

    // Make the call to the API.
    CalenAgendaUtils::CreateEntryIdListForDayL( 
            calInstanceList, 
            calInstanceView, 
            TTime( dateTime ) );
    
    TInt gotResult = calInstanceList.Count();
    // Cleanup.
    calInstanceList.ResetAndDestroy();
    listenerObject->DeleteAllEntriesL();

    // Validate the result.
    if( gotResult != expectedResult )
        {
        return KErrGeneral;
        }
    return KErrNone;

    }

// -----------------------------------------------------------------------------
// CCalendarCommonUtilsTest::TestFindEventsForDayRangeL
// Test function which verifies the number of events returned for a given day
// range.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CCalendarCommonUtilsTest::TestFindEventsForDayRangeL( 
        CStifItemParser& aItem )
    {
    
    // Print to UI
    _LIT( KCalendarCommonUtilsTest, "CalendarCommonUtilsTest" );
    _LIT( KTestName, "In TestFindEventsForDayRangeL" );
    TestModuleIf().Printf( 0, KCalendarCommonUtilsTest, KTestName );
    // Print to log file
    iLog->Log( KTestName );
    
    // Initialization. Construct the listener object.
    CCalProgressCallBackListener* listenerObject = NULL;
    listenerObject = CCalProgressCallBackListener::NewL();
    CCalInstanceView* calInstanceView = listenerObject->CalInstanceView();
    // Parse the input from configuration file.
    TDateTime startDateTime;
    TDateTime endDateTime;
    CalCommon::TCalViewFilter viewFilter;
    RPointerArray< CCalInstance > instanceList;
    
    TInt expectedResult = ParseInputForFindEventsForDayRangeL( aItem, 
            *listenerObject, 
            startDateTime, 
            endDateTime, 
            viewFilter );
    
    // Make the call to the API.
    CalenAgendaUtils::FindEventsForDayRangeL( 
            calInstanceView, 
            instanceList, 
            viewFilter, 
            TTime( startDateTime ), 
            TTime( endDateTime ) );
    TInt gotResult = instanceList.Count();
    
    // Cleanup.
    instanceList.ResetAndDestroy();
    listenerObject->DeleteAllEntriesL();
    
    // Validate the result.
    if( gotResult != expectedResult )
        {
        return KErrGeneral;
        }
    return KErrNone;

    }

// -----------------------------------------------------------------------------
// CCalendarCommonUtilsTest::TestFindTodosForDayRangeL
// Test function for verifying the number of todos returned by the API for a 
// given day range. Note that the TO date should be equal to or greater than
// the current day.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CCalendarCommonUtilsTest::TestFindTodosForDayRangeL( 
        CStifItemParser& aItem )
    {
    
    // Print to UI
    _LIT( KCalendarCommonUtilsTest, "CalendarCommonUtilsTest" );
    _LIT( KTestName, "In TestFindTodosForDayRangeL" );
    TestModuleIf().Printf( 0, KCalendarCommonUtilsTest, KTestName );
    // Print to log file
    iLog->Log( KTestName );
    
    // Initialization. Construct the listener object.
    CCalProgressCallBackListener* listenerObject = NULL;
    listenerObject = CCalProgressCallBackListener::NewL();
    CCalInstanceView* calInstanceView = listenerObject->CalInstanceView();
    TDateTime startDateTime;
    TDateTime endDateTime;
    CalCommon::TCalViewFilter viewFilter;
    RPointerArray< CCalInstance > instanceList;

    // Parse the input from configuration file.
    TInt expectedResult = ParseInputForFindEventsForDayRangeL( aItem, 
            *listenerObject, 
            startDateTime, 
            endDateTime, 
            viewFilter );
    
    // Make the call to the API.
    CalenAgendaUtils::FindTodosForDayRangeL( 
            calInstanceView, 
            instanceList, 
            TTime( startDateTime ), 
            TTime( endDateTime ) );
    TInt gotResult = instanceList.Count();
    
    // Cleanup.
    instanceList.ResetAndDestroy();
    listenerObject->DeleteAllEntriesL();
    
    // Validate the result.
    if( gotResult != expectedResult )
        {
        return KErrGeneral;
        }    
    return KErrNone;

    }

// -----------------------------------------------------------------------------
// CCalendarCommonUtilsTest::TestRemoveEntriesEndingAtMidnightL
// Test if the entries that are ending at midnight of a given day are deleted.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CCalendarCommonUtilsTest::TestRemoveEntriesEndingAtMidnightL( 
        CStifItemParser& aItem )
    {
    
    // Print to UI
    _LIT( KCalendarCommonUtilsTest, "CalendarCommonUtilsTest" );
    _LIT( KTestName, "In TestRemoveEntriesEndingAtMidnightL" );
    TestModuleIf().Printf( 0, KCalendarCommonUtilsTest, KTestName );
    // Print to log file
    iLog->Log( KTestName );
    
    // Initialization. Construct the listener object.
    CCalProgressCallBackListener* listenerObject = NULL;
    listenerObject = CCalProgressCallBackListener::NewL();
    CCalInstanceView* calInstanceView = listenerObject->CalInstanceView();
    TDateTime startDateTime;
    TDateTime endDateTime;
    TDateTime dateTime;
    TInt numberOfEntries;
    RPointerArray< CCalInstance > instanceList;
    
    // Parse the input from configuration file.
    TInt expectedResult = ParseInputForRemoveEntriesBeforeMidnightL( 
            aItem, 
            *listenerObject, 
            dateTime );
    
    startDateTime = dateTime;
    endDateTime = dateTime;
    startDateTime.SetDay( endDateTime.Day() - 1 );
    
    // Make the call to the API. 
    CalenAgendaUtils::FindEventsForDayRangeL( 
            calInstanceView, 
            instanceList,
            CalCommon::EIncludeAll, 
            TTime( startDateTime ),
            TTime( endDateTime )
            );
    // Get the total count of events.
    numberOfEntries = instanceList.Count();
    CalenAgendaUtils::RemoveEntriesEndingAtMidnightL( 
            instanceList, 
            TTime( dateTime ) );
    
    TInt gotResult = numberOfEntries - instanceList.Count();
    // Cleanup.
    instanceList.ResetAndDestroy();
    listenerObject->DeleteAllEntriesL();
    
    // Validate the result.
    if( gotResult != expectedResult )
        {
        return KErrGeneral;
        }
    return KErrNone;

    }

// -----------------------------------------------------------------------------
// CCalendarCommonUtilsTest::TestEndsAtStartOfDayL
// Test function to verify if an entry ends at midnight, i.e., begining of next
// day.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CCalendarCommonUtilsTest::TestEndsAtStartOfDayL( 
        CStifItemParser& aItem )
    {
    
    // Print to UI
    _LIT( KCalendarCommonUtilsTest, "CalendarCommonUtilsTest" );
    _LIT( KTestName, "In TestEndsAtStartOfDayL" );
    TestModuleIf().Printf( 0, KCalendarCommonUtilsTest, KTestName );
    // Print to log file
    iLog->Log( KTestName );
    
    // Initialization. Construct the listener object.
    CCalProgressCallBackListener* listenerObject = NULL;
    listenerObject = CCalProgressCallBackListener::NewL();
    CCalInstanceView* calInstanceView = listenerObject->CalInstanceView();
    TDateTime startDateTime;
    TDateTime endDateTime;
    TDateTime dateTime;
    RPointerArray< CCalInstance > instanceList;
    // Parse the input from configuration file.
    TInt expectedResult = ParseInputForRemoveEntriesBeforeMidnightL( 
            aItem, 
            *listenerObject, 
            dateTime );

    startDateTime = dateTime;
    endDateTime = dateTime;
    startDateTime.SetDay( endDateTime.Day() - 1 );

    // Make the call to the API. 
    CalenAgendaUtils::FindEventsForDayRangeL( 
            calInstanceView, 
            instanceList,
            CalCommon::EIncludeAll, 
            TTime( startDateTime ),
            TTime( endDateTime ) );
    TBool gotResult = CalenAgendaUtils::EndsAtStartOfDayL( 
            instanceList[ 0 ], 
            TTime( dateTime ) );
    
    // Cleanup.
    instanceList.ResetAndDestroy();
    listenerObject->DeleteAllEntriesL();
    
    // Validate the result.
    if( gotResult != expectedResult )
        {
        return KErrNone;
        }
    return KErrNone;

    }

// -----------------------------------------------------------------------------
// CCalendarCommonUtilsTest::TestDurationL
// Tests if the API returns the correct duration of an entry.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CCalendarCommonUtilsTest::TestDurationL( 
        CStifItemParser& aItem )
    {
    
    // Print to UI
    _LIT( KCalendarCommonUtilsTest, "CalendarCommonUtilsTest" );
    _LIT( KTestName, "In TestDurationL" );
    TestModuleIf().Printf( 0, KCalendarCommonUtilsTest, KTestName );
    // Print to log file
    iLog->Log( KTestName );
    
    // Initialization. Construct the listener object.
    CCalProgressCallBackListener* listenerObject = NULL;
    listenerObject = CCalProgressCallBackListener::NewL();
    // Parse input from configuration file.
    TInt expectedResult;
    TTime dummyTime;
    CCalEntry* testEntry = ParseInputForEntryL( 
            aItem, 
            *listenerObject, 
            expectedResult,
            dummyTime );
    
    // Make the call to the API.
    TTimeIntervalMinutes entryDuration = CalenAgendaUtils::DurationL( 
            *testEntry );
    
    // Cleanup.
    delete testEntry;
    listenerObject->DeleteAllEntriesL();
    delete listenerObject;
    
    // Validate the result.
    TInt gotResult = entryDuration.Int();
    if( gotResult != expectedResult )
        {
        return KErrGeneral;
        }
    return KErrNone;

    }

// -----------------------------------------------------------------------------
// CCalendarCommonUtilsTest::TestEntryTimeL
// Test if the entry time returned for a given entry is correct or not.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CCalendarCommonUtilsTest::TestEntryTimeL( 
        CStifItemParser& aItem )
    {
    
    // Print to UI
    _LIT( KCalendarCommonUtilsTest, "CalendarCommonUtilsTest" );
    _LIT( KTestName, "In TestEntryTimeL" );
    TestModuleIf().Printf( 0, KCalendarCommonUtilsTest, KTestName );
    // Print to log file
    iLog->Log( KTestName );
    
    // Initialization. Construct the listener object.
    CCalProgressCallBackListener* listenerObject = NULL;
    listenerObject = CCalProgressCallBackListener::NewL();
    // Parse input from configuration file.
    TInt dummyInt;
    TTime expectedResult;
    CCalEntry* testEntry = ParseInputForEntryL( 
            aItem, 
            *listenerObject, 
            dummyInt,
            expectedResult );

    // Make the call to the API.
    TTime gotResult = CalenAgendaUtils::EntryTimeL( *testEntry );

    // Cleanup.
    delete testEntry;
    listenerObject->DeleteAllEntriesL();
    delete listenerObject;

    // Validate the result.
    if( gotResult != expectedResult )
        {
        return KErrGeneral;
        }
    return KErrNone;

    }

// -----------------------------------------------------------------------------
// CCalendarCommonUtilsTest::TestIsTimedEntryL
// Test if a given entry is timed or not.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CCalendarCommonUtilsTest::TestIsTimedEntryL( 
        CStifItemParser& aItem )
    {
    
    // Print to UI
    _LIT( KCalendarCommonUtilsTest, "CalendarCommonUtilsTest" );
    _LIT( KTestName, "In TestIsTimedEntryL" );
    TestModuleIf().Printf( 0, KCalendarCommonUtilsTest, KTestName );
    // Print to log file
    iLog->Log( KTestName );
    
    // Initialization. Parse the input.
    CCalEntry::TType entryType;
    TBool expectedResult = ParseInputForEntryTypeL( aItem, entryType );
    
    // Make the call to the API.
    TBool gotResult = CalenAgendaUtils::IsTimedEntryL( entryType );
    
    // Validate the result.
    if( gotResult != expectedResult )
        {
        return KErrGeneral;
        }
    return KErrNone;

    }

// -----------------------------------------------------------------------------
// CCalendarCommonUtilsTest::TestIsRepeatingL
// Test if a given entry is repeating or not.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CCalendarCommonUtilsTest::TestIsRepeatingL( 
        CStifItemParser& aItem )
    {
    
    // Print to UI
    _LIT( KCalendarCommonUtilsTest, "CalendarCommonUtilsTest" );
    _LIT( KTestName, "In TestIsRepeatingL" );
    TestModuleIf().Printf( 0, KCalendarCommonUtilsTest, KTestName );
    // Print to log file
    iLog->Log( KTestName );
    
    // Initialization. Construct the listener object.
    CCalProgressCallBackListener* listenerObject = NULL;
    listenerObject = CCalProgressCallBackListener::NewL();
    // Parse input from configuration file.
    TInt dummyInt;
    TTime dummyTime;
    CCalEntry* testEntry = ParseInputForEntryL( 
            aItem, 
            *listenerObject, 
            dummyInt,
            dummyTime );
    TBool expectedResult = dummyInt ? ETrue : EFalse;
    
    // Make the call to the API.
    TBool gotResult = CalenAgendaUtils::IsRepeatingL( *testEntry );
    
    // Cleanup.
    delete testEntry;
    delete listenerObject;
    
    // Validate the result.
    if( gotResult != expectedResult )
        {
        return KErrGeneral;
        }
    return KErrNone;

    }

// -----------------------------------------------------------------------------
// CCalendarCommonUtilsTest::TestIsEmptyTextL
// Test if a given text is empty or not.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CCalendarCommonUtilsTest::TestIsEmptyTextL( 
        CStifItemParser& aItem )
    {
    
    // Print to UI
    _LIT( KCalendarCommonUtilsTest, "CalendarCommonUtilsTest" );
    _LIT( KTestName, "In TestIsEmptyTextL" );
    TestModuleIf().Printf( 0, KCalendarCommonUtilsTest, KTestName );
    // Print to log file
    iLog->Log( KTestName );
    
    // Initialization. Parse the input.
    TPtrC emptyText;
    TBool expectedResult = ParseInputForIsEmptyTextL( aItem, emptyText );

    // Make the call to the API.
    TBool gotResult = CalenAgendaUtils::IsEmptyText( emptyText );

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
