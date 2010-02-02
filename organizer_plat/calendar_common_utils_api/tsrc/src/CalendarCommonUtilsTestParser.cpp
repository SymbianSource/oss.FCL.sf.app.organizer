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
* Description: This file contains implementaion of parser functions for
*              CCalendarCommonUtilsTest. 
*
*/

// INCLUDES
#include <StifParser.h>
#include <Stiftestinterface.h>

#include "CalProgressCallBackListener.h"
#include "CalendarCommonUtilsTest.h"

// -----------------------------------------------------------------------------
// CCalendarCommonUtilsTest::ParseInputForDateL
// Parser utility returns the date from the configuration file.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCalendarCommonUtilsTest::ParseInputForDateL( 
        CStifItemParser& aItem, 
        TDateTime& aInput, 
        TDateTime& aResult )
    {
    
    // Print to UI
    _LIT( KCalendarCommonUtilsTest, "CalendarCommonUtilsTest" );
    _LIT( KTestName, "In ParseInputForDateL" );
    TestModuleIf().Printf( 0, KCalendarCommonUtilsTest, KTestName );
    // Print to log file
    iLog->Log( KTestName );
        
    TPtrC stringToParse;
    // Parsing type to be set before parsing a string. 
    // EQuoteStyleParsing: information between quotes is handled as a one string. 
    aItem.SetParsingType( CStifItemParser::EQuoteStyleParsing );
    aItem.GetNextString ( stringToParse );
        
    TLex stringData( stringToParse );
    TPtrC stringToken = stringData.NextToken();
    TChar separatorChar = ':';
    TDateTime someDate;
    
    while( 0 != stringToken.Length() )
        {
        TInt colonPos = stringToken.Locate( separatorChar );
        TPtrC16 tokenKey = stringToken.Left( colonPos );  
        TPtrC16 tokenValue = stringToken.Mid( colonPos + 1 );
        
        if( 0 == tokenKey.Compare( _L( "Year" ) ) )
            {
            TLex tempNum( tokenValue );
            TInt tempYear = 0;
            tempNum.Val( tempYear );
            someDate.SetYear( tempYear );
            }
        else if( 0 == tokenKey.Compare( _L( "Month" ) ) )
            {
            TLex tempNum( tokenValue );
            TInt tempMonth = 0;
            tempNum.Val( tempMonth );
            someDate.SetMonth( ( TMonth )tempMonth );
            }
        else if( 0 == tokenKey.Compare( _L( "Day" ) ) )
            {
            TLex tempNum( tokenValue );
            TInt tempDay = 0;
            tempNum.Val( tempDay );
            someDate.SetDay( tempDay );
            }
        else if( 0 == tokenKey.Compare( _L( "Result" ) ) )
            {
            aInput = someDate;
            }
        else if( 0 == tokenKey.Compare( _L( "End" ) ) )
            {
            aResult = someDate;
            }
        else
            {
            // Nothing.
            }
        // Get the next token.
        stringToken.Set( stringData.NextToken() );
        }
    
    }

// -----------------------------------------------------------------------------
// CCalendarCommonUtilsTest::ParseInputForReminderTypeL
// Parser utility returns the type of delete confirmation query.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CCalendarCommonUtilsTest::ParseInputForReminderTypeL( 
        CStifItemParser& aItem, 
        CalenActionUiUtils::TDeleteConfirmationType& aType )
    {
    
    // Print to UI
    _LIT( KCalendarCommonUtilsTest, "CalendarCommonUtilsTest" );
    _LIT( KTestName, "In ParseInputForDateL" );
    TestModuleIf().Printf( 0, KCalendarCommonUtilsTest, KTestName );
    // Print to log file
    iLog->Log( KTestName );

    TPtrC stringToParse;
    // Parsing type to be set before parsing a string. 
    // EQuoteStyleParsing: information between quotes is handled as a one string. 
    aItem.SetParsingType( CStifItemParser::EQuoteStyleParsing );
    aItem.GetNextString ( stringToParse );

    TLex stringData( stringToParse );
    TPtrC stringToken = stringData.NextToken();
    TChar separatorChar = ':';
    TInt expectedResult;
    
    while( 0 != stringToken.Length() )
        {
        TInt colonPos = stringToken.Locate( separatorChar );
        TPtrC16 tokenKey = stringToken.Left( colonPos );  
        TPtrC16 tokenValue = stringToken.Mid( colonPos + 1 );

        if( 0 == tokenKey.Compare( _L( "Delete" ) ) )
            {
            if( 0 == tokenValue.Compare( _L( "Entry" ) ) )
                {
                aType = CalenActionUiUtils::EDeleteEntry;
                }
            else if( 0 == tokenValue.Compare( _L( "TODO" ) ) )
                {
                aType = CalenActionUiUtils::EDeleteToDo;
                }
            else if( 0 == tokenValue.Compare( _L( "TODOs" ) ) )
                {
                aType = CalenActionUiUtils::EDeleteToDos;
                }
            else if( 0 == tokenValue.Compare( _L( "All" ) ) )
                {
                aType = CalenActionUiUtils::EDeleteAll;
                }
            else if( 0 == tokenValue.Compare( _L( "Unknown" ) ) )
                {
                aType = ( CalenActionUiUtils::TDeleteConfirmationType )KErrNotFound;
                }
            else
                {
                // Nothing.
                }
            }
        else if( 0 == tokenKey.Compare( _L( "Result" ) ) )
            {
            TLex tempNum( tokenValue );
            tempNum.Val( expectedResult );
            }
        else
            {
            // Nothing.
            }
        // Get the next token.
        stringToken.Set( stringData.NextToken() );
        }
    return expectedResult;
    
    }

// -----------------------------------------------------------------------------
// CCalendarCommonUtilsTest::ParseInputForRecurrenceRangeL
// Parser utility returns the action and the recurrence range for a given action
// selected by the user from a list query.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CalenActionUiUtils::TRepeatQueryType CCalendarCommonUtilsTest::ParseInputForRecurrenceRangeL( 
        CStifItemParser& aItem,
        CalCommon::TRecurrenceRange& aRange )
    {
    
    // Print to UI
    _LIT( KCalendarCommonUtilsTest, "CalendarCommonUtilsTest" );
    _LIT( KTestName, "In ParseInputForRecurrenceRangeL" );
    TestModuleIf().Printf( 0, KCalendarCommonUtilsTest, KTestName );
    // Print to log file
    iLog->Log( KTestName );
    
    TPtrC stringToParse;
    // Parsing type to be set before parsing a string. 
    // EQuoteStyleParsing: information between quotes is handled as a one string. 
    aItem.SetParsingType( CStifItemParser::EQuoteStyleParsing );
    aItem.GetNextString ( stringToParse );
    
    TLex stringData( stringToParse );
    TPtrC stringToken = stringData.NextToken();
    TChar separatorChar = ':';
    
    CalenActionUiUtils::TRepeatQueryType repeatQueryType;
    repeatQueryType = CalenActionUiUtils::ESave;
    
    while( 0 != stringToken.Length() )
        {
        TInt colonPos = stringToken.Locate( separatorChar );
        TPtrC16 tokenKey = stringToken.Left( colonPos );  
        TPtrC16 tokenValue = stringToken.Mid( colonPos + 1 );
        
        if( 0 == tokenKey.Compare( _L( "Result") ) )
            {
            if( 0 == tokenValue.Compare( _L( "ThisOnly" ) ) )
                {
                aRange = CalCommon::EThisOnly;
                }
            else if( 0 == tokenValue.Compare( _L( "ThisAndAll" ) ) )
                {
                aRange = CalCommon::EThisAndAll;
                }
            else if( 0 == tokenValue.Compare( _L( "ThisAndFuture" ) ) )
                {
                aRange = CalCommon::EThisAndFuture;
                }
            else if( 0 == tokenValue.Compare( _L( "ThisAndPrior" ) ) )
                {
                aRange = CalCommon::EThisAndPrior;
                }
            else
                {
                // Nothing.
                }
            }
        else if( 0 == tokenKey.Compare( _L( "Type" ) ) )
            {
            if( 0 == tokenValue.Compare( _L( "Save" ) ) )
                {
                repeatQueryType = CalenActionUiUtils::ESave;
                }
            else if( 0 == tokenValue.Compare( _L( "Delete" ) ) )
                {
                repeatQueryType = CalenActionUiUtils::EDelete;
                }
            else if( 0 == tokenValue.Compare( _L( "Edit" ) ) )
                {
                repeatQueryType = CalenActionUiUtils::EEdit;
                }
            else
                {
                // Nothing.
                }
            }
        else
            {
            // Nothing.
            }
        // Get the next token.
        stringToken.Set( stringData.NextToken() );
        }
    return repeatQueryType;

    }

// -----------------------------------------------------------------------------
// CCalendarCommonUtilsTest::ParseInputForNewNoteSelectionL
// Parser utility returns the expected selection to be made from the list query.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCalendarCommonUtilsTest::ParseInputForNewNoteSelectionL( 
        CStifItemParser& aItem,
        TInt& aExpectedSelection )
    {
    
    // Print to UI
    _LIT( KCalendarCommonUtilsTest, "CalendarCommonUtilsTest" );
    _LIT( KTestName, "In ParseInputForNewNoteSelectionL" );
    TestModuleIf().Printf( 0, KCalendarCommonUtilsTest, KTestName );
    // Print to log file
    iLog->Log( KTestName );

    TPtrC stringToParse;
    // Parsing type to be set before parsing a string. 
    // EQuoteStyleParsing: information between quotes is handled as a one string. 
    aItem.SetParsingType( CStifItemParser::EQuoteStyleParsing );
    aItem.GetNextString ( stringToParse );
    
    TLex stringData( stringToParse );
    TPtrC stringToken = stringData.NextToken();
    TChar separatorChar = ':';

    while( 0 != stringToken.Length() )
        {
        TInt colonPos = stringToken.Locate( separatorChar );
        TPtrC16 tokenKey = stringToken.Left( colonPos );  
        TPtrC16 tokenValue = stringToken.Mid( colonPos + 1 );
        
        if( 0 == tokenKey.Compare( _L( "Selection" ) ) )
            {
            TLex tempNum( tokenValue );
            tempNum.Val( aExpectedSelection );
            }
        else
            {
            // Nothing.
            }
        // Get the next token.
        stringToken.Set( stringData.NextToken() );
        }
        
    }

// -----------------------------------------------------------------------------
// CCalendarCommonUtilsTest::ParseInputForToDoStatusL
// Parser utility returns a entry (to-do) created with the expected status after
// changing the status of the to-do.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CCalEntry* CCalendarCommonUtilsTest::ParseInputForToDoStatusL( 
        CStifItemParser& aItem,
        CCalSession& /*aSession*/,
        CCalEntryView& aEntryView )
    {
    
    // Print to UI
    _LIT( KCalendarCommonUtilsTest, "CalendarCommonUtilsTest" );
    _LIT( KTestName, "In ParseInputForToDoStatusL" );
    TestModuleIf().Printf( 0, KCalendarCommonUtilsTest, KTestName );
    // Print to log file
    iLog->Log( KTestName );
    
    TPtrC stringToParse;
    // Parsing type to be set before parsing a string. 
    // EQuoteStyleParsing: information between quotes is handled as a one string. 
    aItem.SetParsingType( CStifItemParser::EQuoteStyleParsing );
    aItem.GetNextString ( stringToParse );

    TLex stringData( stringToParse );
    TPtrC stringToken = stringData.NextToken();
    TChar separatorChar = ':';
    
    // Construct the CCalenInterimUtils2 object.
    CCalenInterimUtils2* calInterimUtils = CCalenInterimUtils2::NewL();
    CCalEntry* newEntry = NULL;
    
    while( 0 != stringToken.Length() )
        {
        TInt colonPos = stringToken.Locate( separatorChar );
        TPtrC16 tokenKey = stringToken.Left( colonPos );  
        TPtrC16 tokenValue = stringToken.Mid( colonPos + 1 );

        if( 0 == tokenKey.Compare( _L( "Filter" ) ) )
            {
            // Create global UID.
            HBufC8* globalUid = calInterimUtils->GlobalUidL();
            CleanupStack::PushL( globalUid );
            // Create an entry.
            newEntry = CCalEntry::NewL( 
                    CCalEntry::ETodo, 
                    globalUid, 
                    CCalEntry::EMethodNone, 
                    0 );
            CleanupStack::Pop( globalUid );
            CleanupStack::PushL( newEntry );

            // Set initial start and end times from Calendar context.
            TTime homeTime;
            homeTime.HomeTime();
            TCalTime initialCalTime;
            initialCalTime.SetTimeLocalL( CalenDateUtils::BeginningOfDay( homeTime ) );
            newEntry->SetStartAndEndTimeL( initialCalTime, initialCalTime );
            
            // Set the status.
            if( 0 == tokenValue.Compare( _L( "EIncludeInCompletedTodos" ) ) )
                {
                newEntry->SetCompletedL( ETrue,initialCalTime );
                }
            else if( 0 == tokenValue.Compare( _L( "EIncludeCompletedTodos" ) ) )
                {
                newEntry->SetCompletedL( EFalse, initialCalTime );
                }
            
            // Store the above entry.
            CCalenInterimUtils2::StoreL( aEntryView, *newEntry );
            CleanupStack::Pop( newEntry );
            
            // Cleanup.
            delete calInterimUtils;
            }
        else
            {
            // Nothing.
            }
        // Get the next token.
        stringToken.Set( stringData.NextToken() );
        }
    return newEntry;
    
    }

// -----------------------------------------------------------------------------
// CCalendarCommonUtilsTest::ParseInputForFindInstanceL
// Parser utility returns a entry.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CCalEntry* CCalendarCommonUtilsTest::ParseInputForFindInstanceL( 
        CStifItemParser& aItem,
        CCalSession& /*aSession*/, 
        CCalEntryView& aEntryView )
    {
    
    // Print to UI
    _LIT( KCalendarCommonUtilsTest, "CalendarCommonUtilsTest" );
    _LIT( KTestName, "In ParseInputForToDoStatusL" );
    TestModuleIf().Printf( 0, KCalendarCommonUtilsTest, KTestName );
    // Print to log file
    iLog->Log( KTestName );

    TPtrC stringToParse;
    // Parsing type to be set before parsing a string. 
    // EQuoteStyleParsing: information between quotes is handled as a one string. 
    aItem.SetParsingType( CStifItemParser::EQuoteStyleParsing );
    aItem.GetNextString ( stringToParse );

    TLex stringData( stringToParse );
    TPtrC stringToken = stringData.NextToken();
    TChar separatorChar = ':';

    // Construct the CCalenInterimUtils2 object.
    CCalenInterimUtils2* calInterimUtils = CCalenInterimUtils2::NewL();
    CCalEntry* newEntry = NULL;
    CCalEntry::TType entryType = CCalEntry::EAppt;
    TCalLocalUid localUid = 0;
    TDateTime tempDateTime;
    
    while( 0 != stringToken.Length() )
        {
        TInt colonPos = stringToken.Locate( separatorChar );
        TPtrC16 tokenKey = stringToken.Left( colonPos );
        TPtrC16 tokenValue = stringToken.Mid( colonPos + 1 );
        
        if( 0 == tokenKey.Compare( _L( "Year" ) ) )
            {
            TLex tempNum( tokenValue );
            TInt tempYear = 0;
            tempNum.Val( tempYear );
            tempDateTime.SetYear( tempYear );
            }
        else if( 0 == tokenKey.Compare( _L( "Month" ) ) )
            {
            TLex tempNum( tokenValue );
            TInt tempMonth = 0;
            tempNum.Val( tempMonth );
            tempDateTime.SetMonth( ( TMonth )( tempMonth - 1 ) );
            }
        else if( 0 == tokenKey.Compare( _L( "Day" ) ) )
            {
            TLex tempNum( tokenValue );
            TInt tempDay = 0;
            tempNum.Val( tempDay );
            tempDateTime.SetDay( tempDay - 1 );
            }
        else if( 0 == tokenKey.Compare( _L( "Hours" ) ) )
            {
            TLex tempNum( tokenValue );
            TInt tempHours = 0;
            tempNum.Val(tempHours);
            tempDateTime.SetHour( tempHours );
            }
        else if( 0 == tokenKey.Compare( _L( "Minutes" ) ) )
            {
            TLex tempNum( tokenValue );
            TInt tempMinutes = 0;
            tempNum.Val( tempMinutes );
            tempDateTime.SetMinute( tempMinutes );
            }
        else if(0 == tokenKey.Compare( _L( "Seconds" ) ) )
            {
            TLex tempNum( tokenValue );
            TInt tempSeconds = 0;
            tempNum.Val( tempSeconds );
            tempDateTime.SetSecond( tempSeconds );
            }
        else if( 0 == tokenKey.Compare( _L( "EntryType" ) ) )
            {
            entryType = GetEntryTypeFromString( tokenValue );
            }
        else if( 0 == tokenKey.Compare( _L( "Next" ) ) )
            {
            // Create global UID.
            HBufC8* globalUid = calInterimUtils->GlobalUidL();
            CleanupStack::PushL( globalUid );
            // Construct the CCalEntry object.
            newEntry = CCalEntry::NewL( 
                    entryType, 
                    globalUid, 
                    CCalEntry::EMethodNone, 
                    0 );
            CleanupStack::Pop( globalUid );
            CleanupStack::PushL( newEntry );

            // Set initial start and end times from Calendar context.
            TTime focusedTime( tempDateTime );
            TCalTime initialCalTime;
            if( CalenAgendaUtils::IsTimedEntryL( newEntry->EntryTypeL() ) )
                {
                initialCalTime.SetTimeLocalL( focusedTime );
                }
            else
                {
                initialCalTime.SetTimeLocalFloatingL( 
                        CalenDateUtils::BeginningOfDay( focusedTime ) );
                }
            newEntry->SetStartAndEndTimeL( initialCalTime, initialCalTime );

            if( CCalEntry::EAnniv == newEntry->EntryTypeL() )
                {
                TCalRRule repeatRule = TCalRRule::EYearly;
                TCalTime startDate;
                repeatRule.SetDtStart( initialCalTime );
                // For every year.
                repeatRule.SetInterval( 1 );
                newEntry->SetRRuleL( repeatRule );
                }
            // Get the instance time and store the entry.
            TTime instanceTime = newEntry->StartTimeL().TimeLocalL();
            CCalenInterimUtils2::StoreL( aEntryView, *newEntry );
            // Get the local UID.
            localUid = newEntry->LocalUidL();
            CleanupStack::PopAndDestroy( newEntry );
            }
        else if( 0 == tokenKey.Compare( _L( "Result" ) ) )
            {
            TLex tempNum( tokenValue );
            TInt tempResult = 0;
            tempNum.Val( tempResult );
            TInt andResult = tempResult && localUid;
            if( 0 <= andResult )
                {
                newEntry = aEntryView.FetchL( localUid );
                }
            else
                {
                // Get the global UID.
                HBufC8* globalUid = calInterimUtils->GlobalUidL();
                CleanupStack::PushL( globalUid );
                newEntry = CCalEntry::NewL( 
                        CCalEntry::EAppt, 
                        globalUid, 
                        CCalEntry::EMethodNone, 
                        0 );
                CleanupStack::Pop( globalUid );
                }
            }
        else
            {
            // Nothing.
            }
        
        // Get the next token.
        stringToken.Set( stringData.NextToken() );
        }
        
    // Cleanup.
    delete calInterimUtils;
    
    return newEntry;
    
    }

// -----------------------------------------------------------------------------
// CCalendarCommonUtilsTest::GetEntryTypeFromString
// Returns the entry type of a given entry.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CCalEntry::TType CCalendarCommonUtilsTest::GetEntryTypeFromString( 
        TPtrC16& aString )
    {
    
    // Print to UI
    _LIT( KCalendarCommonUtilsTest, "CalendarCommonUtilsTest" );
    _LIT( KTestName, "In GetEntryTypeFromString" );
    TestModuleIf().Printf( 0, KCalendarCommonUtilsTest, KTestName );
    // Print to log file
    iLog->Log( KTestName );
    
    CCalEntry::TType entryType = CCalEntry::EAppt;

    if( 0 == aString.Compare( _L( "EAppt" ) ) )
        {
        entryType = CCalEntry::EAppt;
        }
    else if( 0 == aString.Compare( _L( "ETodo" ) ) )
        {
        entryType = CCalEntry::ETodo;
        }
    else if( 0 == aString.Compare( _L( "EEvent" ) ) )
        {
        entryType = CCalEntry::EEvent;
        }
    else if( 0 == aString.Compare( _L( "EReminder" ) ) )
        {
        entryType = CCalEntry::EReminder;
        }
    else if( 0 == aString.Compare( _L( "EAnniv" ) ) )
        {
        entryType = CCalEntry::EAnniv;
        }
    return entryType;
    
    }

// -----------------------------------------------------------------------------
// CCalendarCommonUtilsTest::ParseInputForCreateEntryIdListL
// Parser utility, creates entries and returns the number of entries created.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CCalendarCommonUtilsTest::ParseInputForCreateEntryIdListL( 
        CStifItemParser& aItem,
        CCalProgressCallBackListener& aListener, 
        TDateTime& aDateTime )
    {
    
    // Print to UI
    _LIT( KCalendarCommonUtilsTest, "CalendarCommonUtilsTest" );
    _LIT( KTestName, "In ParseInputForCreateEntryIdListL" );
    TestModuleIf().Printf( 0, KCalendarCommonUtilsTest, KTestName );
    // Print to log file
    iLog->Log( KTestName );

    TPtrC stringToParse;
    // Parsing type to be set before parsing a string. 
    // EQuoteStyleParsing: information between quotes is handled as a one string. 
    aItem.SetParsingType( CStifItemParser::EQuoteStyleParsing );
    aItem.GetNextString ( stringToParse );

    TLex stringData( stringToParse );
    TPtrC stringToken = stringData.NextToken();
    TChar separatorChar = ':';
    
    // First delete all previous entries created.
    aListener.DeleteAllEntriesL();
    
    // Construct the CCalenInterimUtils2 object.
    CCalenInterimUtils2* calInterimUtils = CCalenInterimUtils2::NewL();
    CCalEntryView* calEntryView = aListener.CalEntryView();
    TInt entryCount = 0;
    TDateTime tempDateTime;
    CCalEntry::TType entryType = CCalEntry::EAppt;
    
    while( 0 != stringToken.Length() )
        {
        TInt colonPos = stringToken.Locate( separatorChar );
        TPtrC16 tokenKey = stringToken.Left( colonPos );
        TPtrC16 tokenValue = stringToken.Mid( colonPos + 1 );

        if( 0 == tokenKey.Compare( _L( "Year" ) ) )
            {
            TLex tempNum( tokenValue );
            TInt tempYear = 0;
            tempNum.Val( tempYear );
            tempDateTime.SetYear( tempYear );
            }
        else if( 0 == tokenKey.Compare( _L( "Month" ) ) )
            {
            TLex tempNum( tokenValue );
            TInt tempMonth = 0;
            tempNum.Val( tempMonth );
            tempDateTime.SetMonth( ( TMonth )( tempMonth - 1 ) );
            }
        else if( 0 == tokenKey.Compare( _L( "Day" ) ) )
            {
            TLex tempNum( tokenValue );
            TInt tempDay = 0;
            tempNum.Val( tempDay );
            tempDateTime.SetDay( tempDay - 1 );
            }
        else if( 0 == tokenKey.Compare( _L( "Hours" ) ) )
            {
            TLex tempNum( tokenValue );
            TInt tempHours = 0;
            tempNum.Val(tempHours);
            tempDateTime.SetHour( tempHours );
            }
        else if( 0 == tokenKey.Compare( _L( "Minutes" ) ) )
            {
            TLex tempNum( tokenValue );
            TInt tempMinutes = 0;
            tempNum.Val( tempMinutes );
            tempDateTime.SetMinute( tempMinutes );
            }
        else if(0 == tokenKey.Compare( _L( "Seconds" ) ) )
            {
            TLex tempNum( tokenValue );
            TInt tempSeconds = 0;
            tempNum.Val( tempSeconds );
            tempDateTime.SetSecond( tempSeconds );
            }
        else if( 0 == tokenKey.Compare( _L( "EntryType" ) ) )
            {
            entryType = GetEntryTypeFromString( tokenValue );
            }
        else if( 0 == tokenKey.Compare( _L( "Next" ) ) )
            {
            // Create global UID.
            HBufC8* globalUid = calInterimUtils->GlobalUidL();
            CleanupStack::PushL( globalUid );
            // Construct the CCalEntry object.
            CCalEntry* newEntry = CCalEntry::NewL(
                    entryType, 
                    globalUid, 
                    CCalEntry::EMethodNone, 
                    0 );
            CleanupStack::Pop( globalUid );
            CleanupStack::PushL( newEntry );

            // Set initial start and end times from Calendar context.
            TTime focusedTime( tempDateTime );
            TCalTime initialCalTime;
            if( CalenAgendaUtils::IsTimedEntryL( newEntry->EntryTypeL() ) )
                {
                initialCalTime.SetTimeLocalL( focusedTime );
                }
            else
                {
                initialCalTime.SetTimeLocalFloatingL( 
                        CalenDateUtils::BeginningOfDay( focusedTime ) );
                }
            newEntry->SetStartAndEndTimeL( initialCalTime, initialCalTime );

            if( CCalEntry::EAnniv == newEntry->EntryTypeL() )
                {
                TCalRRule repeatRule = TCalRRule::EYearly;
                TCalTime startDate;
                repeatRule.SetDtStart( initialCalTime );
                // For every year.
                repeatRule.SetInterval( 1 );
                newEntry->SetRRuleL( repeatRule );
                }
            // Get the instance time and store the entry.
            TTime instanceTime = newEntry->StartTimeL().TimeLocalL();
            CCalenInterimUtils2::StoreL( *calEntryView, *newEntry );

            CleanupStack::PopAndDestroy( newEntry );
            }
        else if( 0 == tokenKey.Compare( _L( "Result" ) ) )
            {
            TLex tempNum( tokenValue );
            TInt tempResult = 0;
            tempNum.Val( tempResult );
            entryCount = tempResult;
            }
        else if( 0 == tokenKey.Compare( _L( "NextResult" ) ) )
            {
            aDateTime.SetYear( tempDateTime.Year() );
            aDateTime.SetMonth( tempDateTime.Month() );
            aDateTime.SetDay( tempDateTime.Day() );
            }
        else
            {
            // Nothing.
            }

        // Get the next token.
        stringToken.Set( stringData.NextToken() );
        }
    // Cleanup.
    delete calInterimUtils;
    
    return entryCount;
    
    }

// -----------------------------------------------------------------------------
// CCalendarCommonUtilsTest::ParseInputForCreateEntryIdListL
// Parser utility, creates entries and returns the number of entries created.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CCalendarCommonUtilsTest::ParseInputForFindEventsForDayRangeL( 
        CStifItemParser& aItem, 
        CCalProgressCallBackListener& aListener, 
        TDateTime& aStartDateTime, 
        TDateTime& aEndDateTime, 
        CalCommon::TCalViewFilter& aViewFilter )
    {
    
    // Print to UI
    _LIT( KCalendarCommonUtilsTest, "CalendarCommonUtilsTest" );
    _LIT( KTestName, "In ParseInputForFindEventsForDayRangeL" );
    TestModuleIf().Printf( 0, KCalendarCommonUtilsTest, KTestName );
    // Print to log file
    iLog->Log( KTestName );

    TPtrC stringToParse;
    // Parsing type to be set before parsing a string. 
    // EQuoteStyleParsing: information between quotes is handled as a one string. 
    aItem.SetParsingType( CStifItemParser::EQuoteStyleParsing );
    aItem.GetNextString ( stringToParse );

    TLex stringData( stringToParse );
    TPtrC stringToken = stringData.NextToken();
    TChar separatorChar = ':';

    // First delete all previous entries created.
    aListener.DeleteAllEntriesL();
    
    // Construct the CCalenInterimUtils2 object.
    CCalenInterimUtils2* calInterimUtils = CCalenInterimUtils2::NewL();
    CCalEntryView* calEntryView = aListener.CalEntryView();
    TInt eventCount = 0;
    TDateTime tempDateTime;
    
    while( 0 != stringToken.Length() )
        {
        TInt colonPos = stringToken.Locate( separatorChar );
        TPtrC16 tokenKey = stringToken.Left( colonPos );
        TPtrC16 tokenValue = stringToken.Mid( colonPos + 1 );

        if( 0 == tokenKey.Compare( _L( "Year" ) ) )
            {
            TLex tempNum( tokenValue );
            TInt tempYear = 0;
            tempNum.Val( tempYear );
            tempDateTime.SetYear( tempYear );
            }
        else if( 0 == tokenKey.Compare( _L( "Month" ) ) )
            {
            TLex tempNum( tokenValue );
            TInt tempMonth = 0;
            tempNum.Val( tempMonth );
            tempDateTime.SetMonth( ( TMonth )( tempMonth - 1 ) );
            }
        else if( 0 == tokenKey.Compare( _L( "Day" ) ) )
            {
            TLex tempNum( tokenValue );
            TInt tempDay = 0;
            tempNum.Val( tempDay );
            tempDateTime.SetDay( tempDay - 1 );
            }
        else if( 0 == tokenKey.Compare( _L( "EntryType" ) ) )
            {
            // Get the entry type first.
            CCalEntry::TType entryType = GetEntryTypeFromString( tokenValue );

            // Create global UID.
            HBufC8* globalUid = calInterimUtils->GlobalUidL();
            CleanupStack::PushL( globalUid );
            // Construct the CCalEntry object.
            CCalEntry* newEntry = CCalEntry::NewL(
                    entryType, 
                    globalUid, 
                    CCalEntry::EMethodNone, 
                    0 );
            CleanupStack::Pop( globalUid );
            CleanupStack::PushL( newEntry );

            // Set initial start and end times from Calendar context.
            TTime focusedTime( tempDateTime );
            TCalTime initialCalTime;
            if( CalenAgendaUtils::IsTimedEntryL( newEntry->EntryTypeL() ) )
                {
                initialCalTime.SetTimeLocalL( focusedTime );
                }
            else
                {
                initialCalTime.SetTimeLocalFloatingL( 
                        CalenDateUtils::BeginningOfDay( focusedTime ) );
                }
            newEntry->SetStartAndEndTimeL( initialCalTime, initialCalTime );

            if( CCalEntry::EAnniv == newEntry->EntryTypeL() )
                {
                TCalRRule repeatRule = TCalRRule::EYearly;
                TCalTime startDate;
                repeatRule.SetDtStart( initialCalTime );
                // For every year.
                repeatRule.SetInterval( 1 );
                newEntry->SetRRuleL( repeatRule );
                }
            // Get the instance time and store the entry.
            TTime instanceTime = newEntry->StartTimeL().TimeLocalL();
            CCalenInterimUtils2::StoreL( *calEntryView, *newEntry );

            CleanupStack::PopAndDestroy( newEntry );
            }
        else if( 0 == tokenKey.Compare( _L( "Result" ) ) )
            {
            TLex tempNum( tokenValue );
            TInt tempResult = 0;
            tempNum.Val( tempResult );
            eventCount = tempResult;
            }
        else if( 0 == tokenKey.Compare( _L( "StartDate" ) ) )
            {
            aStartDateTime.SetYear( tempDateTime.Year() );
            aStartDateTime.SetMonth( tempDateTime.Month() );
            aStartDateTime.SetDay( tempDateTime.Day() );
            }
        else if( 0 == tokenKey.Compare( _L( "EndDate" ) ) )
            {
            aEndDateTime.SetYear( tempDateTime.Year() );
            aEndDateTime.SetMonth( tempDateTime.Month() );
            aEndDateTime.SetDay( tempDateTime.Day() );
            }
        else if( 0 == tokenKey.Compare( _L( "Filter" ) ) )
            {
            aViewFilter = GetFilterTypeFromValue( tokenValue );
            }
        else
            {
            // Nothing.
            }
        // Get the next token.
        stringToken.Set( stringData.NextToken() );
        }
    // Cleanup.
    delete calInterimUtils;
    
    return eventCount;
    
    }

// -----------------------------------------------------------------------------
// CCalendarCommonUtilsTest::GetFilterTypeFromValue
// Returns the view filter used for getting the events for a given range.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CalCommon::TCalViewFilter CCalendarCommonUtilsTest::GetFilterTypeFromValue( 
        TPtrC16& aType )
    {
    
    // Print to UI
    _LIT( KCalendarCommonUtilsTest, "CalendarCommonUtilsTest" );
    _LIT( KTestName, "In GetFilterTypeFromValue" );
    TestModuleIf().Printf( 0, KCalendarCommonUtilsTest, KTestName );
    // Print to log file
    iLog->Log( KTestName );

    CalCommon::TCalViewFilter filterType = 0;
    
    if( 0 == aType.Compare( _L( "EIncludeAppts" ) ) )
        {
        filterType = CalCommon::EIncludeAppts;
        }
    else if( 0 == aType.Compare( _L( "EIncludeReminder" ) ) )
        {
        filterType = CalCommon::EIncludeReminder;
        }
    else if( 0 == aType.Compare( _L( "EIncludeEvents" ) ) )
        {
        filterType = CalCommon::EIncludeEvents;
        }
    else if( 0 == aType.Compare( _L( "EIncludeAnnivs" ) ) )
        {
        filterType = CalCommon::EIncludeAnnivs;
        }
    else if( 0 == aType.Compare( _L( "EIncludeCompletedTodos" ) ) )
        {
        filterType = CalCommon::EIncludeCompletedTodos;
        }
    else if( 0 == aType.Compare( _L( "EIncludeIncompletedTodos" ) ) )
        {
        filterType = CalCommon::EIncludeIncompletedTodos;
        }
    else if( 0 == aType.Compare( _L( "EIncludeAll" ) ) )
        {
        filterType = CalCommon::EIncludeAll;
        }
    
    return filterType;
    
    }

// -----------------------------------------------------------------------------
// CCalendarCommonUtilsTest::ParseInputForRemoveEntriesBeforeMidnightL
// Parser utility, creates entries and returns the number of entries created.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CCalendarCommonUtilsTest::ParseInputForRemoveEntriesBeforeMidnightL( 
        CStifItemParser& aItem, 
        CCalProgressCallBackListener& aListener, 
        TDateTime& aDateTime )
    {
    
    // Print to UI
    _LIT( KCalendarCommonUtilsTest, "CalendarCommonUtilsTest" );
    _LIT( KTestName, "In ParseInputForRemoveEntriesBeforeMidnightL" );
    TestModuleIf().Printf( 0, KCalendarCommonUtilsTest, KTestName );
    // Print to log file
    iLog->Log( KTestName );

    TPtrC stringToParse;
    // Parsing type to be set before parsing a string. 
    // EQuoteStyleParsing: information between quotes is handled as a one string. 
    aItem.SetParsingType( CStifItemParser::EQuoteStyleParsing );
    aItem.GetNextString ( stringToParse );

    TLex stringData( stringToParse );
    TPtrC stringToken = stringData.NextToken();
    TChar separatorChar = ':';
    
    // First delete all entires.
    aListener.DeleteAllEntriesL();
    
    // Construct the CCalenInterimUtils2 object.
    CCalenInterimUtils2* calInterimUtils = CCalenInterimUtils2::NewL();
    CCalEntryView* calEntryView = aListener.CalEntryView();
    CCalEntry::TType entryType = CCalEntry::EAppt;
    TInt eventCount;
    TDateTime tempDateTime;
    TDateTime startDateTime;
    TDateTime endDateTime;
    
    while( 0 != stringToken.Length() )
        {
        TInt colonPos = stringToken.Locate( separatorChar );
        TPtrC16 tokenKey = stringToken.Left( colonPos );
        TPtrC16 tokenValue = stringToken.Mid( colonPos + 1 );

        if( 0 == tokenKey.Compare( _L( "Year" ) ) )
            {
            TLex tempNum( tokenValue );
            TInt tempYear = 0;
            tempNum.Val( tempYear );
            tempDateTime.SetYear( tempYear );
            }
        else if( 0 == tokenKey.Compare( _L( "Month" ) ) )
            {
            TLex tempNum( tokenValue );
            TInt tempMonth = 0;
            tempNum.Val( tempMonth );
            tempDateTime.SetMonth( ( TMonth )( tempMonth - 1 ) );
            }
        else if( 0 == tokenKey.Compare( _L( "Day" ) ) )
            {
            TLex tempNum( tokenValue );
            TInt tempDay = 0;
            tempNum.Val( tempDay );
            tempDateTime.SetDay( tempDay - 1 );
            }
        else if( 0 == tokenKey.Compare( _L( "Hours" ) ) )
            {
            TLex tempNum( tokenValue );
            TInt tempHours = 0;
            tempNum.Val(tempHours);
            tempDateTime.SetHour( tempHours );
            }
        else if( 0 == tokenKey.Compare( _L( "Minutes" ) ) )
            {
            TLex tempNum( tokenValue );
            TInt tempMinutes = 0;
            tempNum.Val( tempMinutes );
            tempDateTime.SetMinute( tempMinutes );
            }
        else if(0 == tokenKey.Compare( _L( "Seconds" ) ) )
            {
            TLex tempNum( tokenValue );
            TInt tempSeconds = 0;
            tempNum.Val( tempSeconds );
            tempDateTime.SetSecond( tempSeconds );
            }
        else if( 0 == tokenKey.Compare( _L( "StartDate" ) ) )
            {
            startDateTime = tempDateTime;
            }
        else if( 0 == tokenKey.Compare( _L( "EndDate" ) ) )
            {
            endDateTime = tempDateTime;
            }
        else if( 0 == tokenKey.Compare( _L( "EntryType" ) ) )
            {
            entryType = GetEntryTypeFromString( tokenValue );
            }
        else if( 0 == tokenKey.Compare( _L( "Next" ) ) )
            {
            // Create global UID.
            HBufC8* globalUid = calInterimUtils->GlobalUidL();
            CleanupStack::PushL( globalUid );
            // Construct the CCalEntry object.
            CCalEntry* newEntry = CCalEntry::NewL(
                    entryType, 
                    globalUid, 
                    CCalEntry::EMethodNone, 
                    0 );
            CleanupStack::Pop( globalUid );
            CleanupStack::PushL( newEntry );

            // Set initial start and end times from Calendar context.
            TTime focusedTime( tempDateTime );
            TCalTime initialCalTime;
            if( CalenAgendaUtils::IsTimedEntryL( newEntry->EntryTypeL() ) )
                {
                initialCalTime.SetTimeLocalL( focusedTime );
                }
            else
                {
                initialCalTime.SetTimeLocalFloatingL( 
                        CalenDateUtils::BeginningOfDay( focusedTime ) );
                }
            
            if( CCalEntry::EAppt == newEntry->EntryTypeL() )
                {
                TCalTime startCalTime;
                TCalTime endCalTime;
                startCalTime.SetTimeLocalL( TTime( startDateTime ) );
                endCalTime.SetTimeLocalL( TTime( endDateTime ) );
                newEntry->SetStartAndEndTimeL( startCalTime, endCalTime );
                }
            else
                {
                newEntry->SetStartAndEndTimeL( initialCalTime, initialCalTime );
                }

            if( CCalEntry::EAnniv == newEntry->EntryTypeL() )
                {
                TCalRRule repeatRule = TCalRRule::EYearly;
                TCalTime startDate;
                repeatRule.SetDtStart( initialCalTime );
                // For every year.
                repeatRule.SetInterval( 1 );
                newEntry->SetRRuleL( repeatRule );
                }
            // Get the instance time and store the entry.
            TTime instanceTime = newEntry->StartTimeL().TimeLocalL();
            CCalenInterimUtils2::StoreL( *calEntryView, *newEntry );

            CleanupStack::PopAndDestroy( newEntry );
            }
        else if( 0 == tokenKey.Compare( _L( "Result" ) ) )
            {
            TLex tempNum( tokenValue );
            tempNum.Val( eventCount );
            }
        else if( 0 == tokenKey.Compare( _L( "NextResult" ) ) )
            {
            aDateTime.SetYear( tempDateTime.Year() );
            aDateTime.SetMonth( tempDateTime.Month() );
            aDateTime.SetDay( tempDateTime.Day() );
            }
        else
            {
            // Nothing.
            }
        // Get the next token.
        stringToken.Set( stringData.NextToken() );
        }
    // Cleanup.
    delete calInterimUtils;

    return eventCount;
    
    }

// -----------------------------------------------------------------------------
// CCalendarCommonUtilsTest::ParseInputForEntryForDurationL
// Parser utility, creates an entry and returns the same.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CCalEntry* CCalendarCommonUtilsTest::ParseInputForEntryL( 
        CStifItemParser& aItem,
        CCalProgressCallBackListener& aListener, 
        TInt& aExpectedResult, 
        TTime& aEntryTime )
    {
    
    // Print to UI
    _LIT( KCalendarCommonUtilsTest, "CalendarCommonUtilsTest" );
    _LIT( KTestName, "In ParseInputForEntryForDurationL" );
    TestModuleIf().Printf( 0, KCalendarCommonUtilsTest, KTestName );
    // Print to log file
    iLog->Log( KTestName );

    TPtrC stringToParse;
    // Parsing type to be set before parsing a string. 
    // EQuoteStyleParsing: information between quotes is handled as a one string. 
    aItem.SetParsingType( CStifItemParser::EQuoteStyleParsing );
    aItem.GetNextString ( stringToParse );

    TLex stringData( stringToParse );
    TPtrC stringToken = stringData.NextToken();
    TChar separatorChar = ':';
    
    // First delete all entires.
    aListener.DeleteAllEntriesL();
    
    // Construct the CCalenInterimUtils2 object.
    CCalenInterimUtils2* calInterimUtils = CCalenInterimUtils2::NewL();
    CCalEntryView* calEntryView = aListener.CalEntryView();
    CCalEntry* newEntry = NULL;
    CCalEntry::TType entryType = CCalEntry::EAppt;
    TBool isRRule = EFalse;
    TBool isRDate = EFalse; 
    TCalRRule::TType typeRRule = TCalRRule::EInvalid;
    TDateTime tempDateTime;
    TDateTime startDateTime;
    TDateTime endDateTime;
    
    while( 0 != stringToken.Length() )
        {
        TInt colonPos = stringToken.Locate( separatorChar );
        TPtrC16 tokenKey = stringToken.Left( colonPos );
        TPtrC16 tokenValue = stringToken.Mid( colonPos + 1 );

        if( 0 == tokenKey.Compare( _L( "Year" ) ) )
            {
            TLex tempNum( tokenValue );
            TInt tempYear = 0;
            tempNum.Val( tempYear );
            tempDateTime.SetYear( tempYear );
            }
        else if( 0 == tokenKey.Compare( _L( "Month" ) ) )
            {
            TLex tempNum( tokenValue );
            TInt tempMonth = 0;
            tempNum.Val( tempMonth );
            tempDateTime.SetMonth( ( TMonth )( tempMonth - 1 ) );
            }
        else if( 0 == tokenKey.Compare( _L( "Day" ) ) )
            {
            TLex tempNum( tokenValue );
            TInt tempDay = 0;
            tempNum.Val( tempDay );
            tempDateTime.SetDay( tempDay - 1 );
            }
        else if( 0 == tokenKey.Compare( _L( "Hours" ) ) )
            {
            TLex tempNum( tokenValue );
            TInt tempHours = 0;
            tempNum.Val(tempHours);
            tempDateTime.SetHour( tempHours );
            }
        else if( 0 == tokenKey.Compare( _L( "Minutes" ) ) )
            {
            TLex tempNum( tokenValue );
            TInt tempMinutes = 0;
            tempNum.Val( tempMinutes );
            tempDateTime.SetMinute( tempMinutes );
            }
        else if(0 == tokenKey.Compare( _L( "Seconds" ) ) )
            {
            TLex tempNum( tokenValue );
            TInt tempSeconds = 0;
            tempNum.Val( tempSeconds );
            tempDateTime.SetSecond( tempSeconds );
            }
        else if( 0 == tokenKey.Compare( _L( "StartDate" ) ) )
            {
            startDateTime = tempDateTime;
            }
        else if( 0 == tokenKey.Compare( _L( "EndDate" ) ) )
            {
            endDateTime = tempDateTime;
            }
        else if( 0 == tokenKey.Compare( _L( "EntryType" ) ) )
            {
            entryType = GetEntryTypeFromString( tokenValue );
            }
        else if( 0 == tokenKey.Compare( _L( "ResultDate" ) ) )
            {
            aEntryTime = tempDateTime;
            }
        else if( 0 == tokenKey.Compare( _L( "RRule" ) ) )
            {
            isRRule = ETrue;
            typeRRule = CheckRRuleFromString( tokenValue );
            }
        else if( 0 == tokenKey.Compare( _L( "RDate" ) ) )
            {
            TLex tempNum( tokenValue );
            TInt rDate = 0;
            tempNum.Val( rDate );
            isRDate = rDate;
            }
        else if( 0 == tokenKey.Compare( _L( "NextResult" ) ) )
            {
            // Create global UID.
            HBufC8* globalUid = calInterimUtils->GlobalUidL();
            CleanupStack::PushL( globalUid );
            // Construct the CCalEntry object.
            newEntry = CCalEntry::NewL( 
                    entryType, 
                    globalUid, 
                    CCalEntry::EMethodNone, 
                    0 );
            CleanupStack::Pop( globalUid );
            CleanupStack::PushL( newEntry );

            // Set initial start and end times from Calendar context.
            TTime focusedTime( tempDateTime );
            TCalTime initialCalTime;
            if( CalenAgendaUtils::IsTimedEntryL( newEntry->EntryTypeL() ) )
                {
                initialCalTime.SetTimeLocalL( focusedTime );
                }
            else
                {
                initialCalTime.SetTimeLocalFloatingL( 
                        CalenDateUtils::BeginningOfDay( focusedTime ) );
                }

            if( CCalEntry::EAppt == newEntry->EntryTypeL() )
                {
                TCalTime startCalTime;
                TCalTime endCalTime;
                startCalTime.SetTimeLocalL( TTime( startDateTime ) );
                endCalTime.SetTimeLocalL( TTime( endDateTime ) );
                newEntry->SetStartAndEndTimeL( startCalTime, endCalTime );
                if( isRRule )
                    {
                    TCalRRule calRRule;
                    calRRule.SetDtStart( startCalTime );
                    startDateTime.SetYear( startDateTime.Year() + 5 );
                    TCalTime untilCalTime;
                    untilCalTime.SetTimeLocalL( startDateTime );
                    calRRule.SetType( typeRRule );
                    calRRule.SetUntil( untilCalTime );
                    calRRule.SetInterval( 1 );

                    if( TCalRRule::EWeekly == typeRRule )
                        {
                        calRRule.SetInterval( 7 );
                        RArray< TDay > dayArray;
                        CleanupClosePushL( dayArray );
                        dayArray.Append( 
                                calRRule.DtStart().TimeLocalL().DayNoInWeek() );
                        calRRule.SetByDay( dayArray );
                        CleanupStack::PopAndDestroy( &dayArray );
                        }
                    else if( TCalRRule::EMonthly == typeRRule )
                        {
                        RArray<TInt> dateArray;
                        CleanupClosePushL( dateArray );
                        dateArray.Append( 
                                calRRule.DtStart().TimeLocalL().DayNoInMonth() );
                        calRRule.SetByMonthDay( dateArray );
                        CleanupStack::PopAndDestroy( &dateArray );
                        }
                    newEntry->SetRRuleL( calRRule );
                    }

                if( isRDate )
                    {
                    RArray< TCalTime > rDate;
                    rDate.Append( startCalTime );
                    newEntry->SetRDatesL( rDate );
                    }
                }
            else
                {
                newEntry->SetStartAndEndTimeL( initialCalTime, initialCalTime );
                }

            if( CCalEntry::EAnniv == newEntry->EntryTypeL() )
                {
                TCalRRule repeatRule = TCalRRule::EYearly;
                TCalTime startDate;
                repeatRule.SetDtStart( initialCalTime );
                // For every year.
                repeatRule.SetInterval( 1 );
                newEntry->SetRRuleL( repeatRule );
                }
            CleanupStack::Pop( newEntry );
            }
        else if(0 == tokenKey.Compare( _L( "Result" ) ) )
            {
            TLex tempNum( tokenValue );
            tempNum.Val( aExpectedResult );
            }
        else
            {
            // Nothing.
            }
        // Get the next token.
        stringToken.Set( stringData.NextToken() );
        }
    // Cleanup.
    delete calInterimUtils;
    
    return newEntry;
    
    }

// -----------------------------------------------------------------------------
// CCalendarCommonUtilsTest::CheckRRuleFromString
// Returns the RRule type.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TCalRRule::TType CCalendarCommonUtilsTest::CheckRRuleFromString( 
        TPtrC16& aType )
    {
    
    // Print to UI
    _LIT( KCalendarCommonUtilsTest, "CalendarCommonUtilsTest" );
    _LIT( KTestName, "In CheckRRuleFromString" );
    TestModuleIf().Printf( 0, KCalendarCommonUtilsTest, KTestName );
    // Print to log file
    iLog->Log( KTestName );
    
    TCalRRule::TType rRule = TCalRRule::EInvalid;
    
    if( 0 == aType.Compare( _L( "EDaily" ) ) )
        {
        rRule = TCalRRule::EDaily;
        }
    else if( 0 == aType.Compare( _L( "EWeekly" ) ) )
        {
        rRule =TCalRRule::EWeekly;
        }
    else if( 0 == aType.Compare( _L( "EYearly" ) ) )
        {
        rRule =TCalRRule::EYearly;
        }
    else if( 0 == aType.Compare( _L( "EMonthly" ) ) )
        {
        rRule =TCalRRule::EMonthly;
        }
    else
        {
        // Nothing.
        }
    return rRule;
    
    }

// -----------------------------------------------------------------------------
// CCalendarCommonUtilsTest::ParseInputForEntryTypeL
// Parser utility, returns an entry type.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CCalendarCommonUtilsTest::ParseInputForEntryTypeL( 
        CStifItemParser& aItem, 
        CCalEntry::TType& aEntryType )
    {
    
    // Print to UI
    _LIT( KCalendarCommonUtilsTest, "CalendarCommonUtilsTest" );
    _LIT( KTestName, "In ParseInputForEntryTypeL" );
    TestModuleIf().Printf( 0, KCalendarCommonUtilsTest, KTestName );
    // Print to log file
    iLog->Log( KTestName );
    
    TPtrC stringToParse;
    // Parsing type to be set before parsing a string. 
    // EQuoteStyleParsing: information between quotes is handled as a one string. 
    aItem.SetParsingType( CStifItemParser::EQuoteStyleParsing );
    aItem.GetNextString ( stringToParse );

    TLex stringData( stringToParse );
    TPtrC stringToken = stringData.NextToken();
    TChar separatorChar = ':';
    
    TInt expectedResult;
    
    while( 0 != stringToken.Length() )
        {
        TInt colonPos = stringToken.Locate( separatorChar );
        TPtrC16 tokenKey = stringToken.Left( colonPos );
        TPtrC16 tokenValue = stringToken.Mid( colonPos + 1 );
        
        if( 0 == tokenKey.Compare( _L( "EntryType" ) ) )
            {
            aEntryType = GetEntryTypeFromString( tokenValue );
            }
        else if( 0 == tokenKey.Compare( _L( "Result" ) ) )
            {
            TLex tempNum( tokenValue );
            tempNum.Val( expectedResult );
            }
        // Get the next token.
        stringToken.Set( stringData.NextToken() );
        }
    
    return expectedResult ? ETrue : EFalse;
    
    }

// -----------------------------------------------------------------------------
// CCalendarCommonUtilsTest::ParseInputForIsEmptyTextL
// Parser utility, returns a text descriptor.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CCalendarCommonUtilsTest:: ParseInputForIsEmptyTextL( 
        CStifItemParser& aItem, 
        TPtrC& aEmptyText )
    {
    
    // Print to UI
    _LIT( KCalendarCommonUtilsTest, "CalendarCommonUtilsTest" );
    _LIT( KTestName, "In ParseInputForIsEmptyTextL" );
    TestModuleIf().Printf( 0, KCalendarCommonUtilsTest, KTestName );
    // Print to log file
    iLog->Log( KTestName );

    TPtrC stringToParse;
    // Parsing type to be set before parsing a string. 
    // EQuoteStyleParsing: information between quotes is handled as a one string. 
    aItem.SetParsingType( CStifItemParser::EQuoteStyleParsing );
    aItem.GetNextString ( stringToParse );

    TLex stringData( stringToParse );
    TPtrC stringToken = stringData.NextToken();
    TChar separatorChar = ':';

    TInt expectedResult = 0;

    while( 0 != stringToken.Length() )
        {
        TInt colonPos = stringToken.Locate( separatorChar );
        TPtrC16 tokenKey = stringToken.Left( colonPos );
        TPtrC16 tokenValue = stringToken.Mid( colonPos + 1 );

        if( 0 == tokenKey.Compare( _L( "Text" ) ) )
            {
            aEmptyText.Set( tokenValue );
            }
        else if( 0 == tokenKey.Compare( _L( "Result" ) ) )
            {
            TLex tempNum( tokenValue );
            tempNum.Val( expectedResult );
            }
        // Get the next token.
        stringToken.Set( stringData.NextToken() );
        }
    
    return expectedResult ? ETrue : EFalse;
    
    }

//  [End of File] - Do not remove
