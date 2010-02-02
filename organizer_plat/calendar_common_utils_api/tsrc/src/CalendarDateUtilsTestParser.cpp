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
*              CCalendarDateUtilsTest. 
*
*/

// INCLUDES
#include <StifParser.h>
#include <Stiftestinterface.h>

#include "CalendarDateUtilsTest.h"

// -----------------------------------------------------------------------------
// CCalendarDateUtilsTest::ParseInputL
// Parser utility returns the date/time read from the configuration file.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CCalendarDateUtilsTest::ParseInputL( 
        CStifItemParser& aItem, 
        TDateTime& aDateTime )
    {
    
    // Print to UI
    _LIT( KCalendarCommonUtilsTest, "CalendarCommonUtilsTest" );
    _LIT( KTestName, "In ParseInputL" );
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
    
    TBool expectedResult = ETrue;
    
    while( 0 != stringToken.Length() )
        {
        TInt colonPos = stringToken.Locate( separatorChar );
        TPtrC16 tokenKey = stringToken.Left( colonPos );  
        TPtrC16 tokenValue = stringToken.Mid( colonPos + 1 );
        
        if( 0 == tokenKey.Compare( _L( "Year" ) ) )
            {
            TLex tempLex( tokenValue );
            TInt tempInt = 0;
            tempLex.Val( tempInt );
            aDateTime.SetYear( tempInt );
            }
        else if( 0 == tokenKey.Compare( _L( "Month" ) ) )
            {
            TLex tempLex( tokenValue );
            TInt tempInt = 0;
            tempLex.Val( tempInt );
            aDateTime.SetMonth( ( TMonth )tempInt );
            }
        else if( 0 == tokenKey.Compare( _L( "Day" ) ) )
            {
            TLex tempLex( tokenValue );
            TInt tempInt = 0;
            tempLex.Val( tempInt );
            aDateTime.SetDay( tempInt );
            }
        else if( 0 == tokenKey.Compare( _L( "Hours" ) ) )
            {
            TLex tempLex( tokenValue );
            TInt tempInt = 0;
            tempLex.Val( tempInt );
            aDateTime.SetHour( tempInt );
            }
        else if( 0 == tokenKey.Compare( _L( "Minutes" ) ) )
            {
            TLex tempLex( tokenValue );
            TInt tempInt = 0;
            tempLex.Val( tempInt );
            aDateTime.SetMinute( tempInt );
            }
        else if( 0 == tokenKey.Compare( _L( "Seconds" ) ) )
            {
            TLex tempLex( tokenValue );
            TInt tempInt = 0;
            tempLex.Val( tempInt );
            aDateTime.SetSecond( tempInt );
            }
        else if( 0 == tokenKey.Compare( _L( "Result" ) ) )
            {
            TLex tempLex( tokenValue );
            TInt tempInt = 0;
            tempLex.Val( tempInt );
            expectedResult = tempInt;
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
// CCalendarDateUtilsTest::ParseInputWithMultipleTTimesL
// Parser utility returns an array of date/time read from the configuration 
// file.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CCalendarDateUtilsTest:: ParseInputWithMultipleTTimesL( 
        CStifItemParser& aItem, 
        RArray< TDateTime >& aDateTime )
    {
    
    // Print to UI
    _LIT( KCalendarCommonUtilsTest, "CalendarCommonUtilsTest" );
    _LIT( KTestName, "In ParseInputWithMultipleTTimesL" );
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
    
    TBool expectedResult = ETrue;
    TDateTime tempDateTime;
    
    while( 0 != stringToken.Length() )
        {
        TInt colonPos = stringToken.Locate( separatorChar );
        TPtrC16 tokenKey = stringToken.Left( colonPos );  
        TPtrC16 tokenValue = stringToken.Mid( colonPos + 1 );
        
        if( 0 == tokenKey.Compare( _L( "Year" ) ) )
            {
            TLex tempLex( tokenValue );
            TInt tempInt = 0;
            tempLex.Val( tempInt );
            tempDateTime.SetYear( tempInt );
            }
        else if( 0 == tokenKey.Compare( _L( "Month" ) ) )
            {
            TLex tempLex( tokenValue );
            TInt tempInt = 0;
            tempLex.Val( tempInt );
            tempDateTime.SetMonth( ( TMonth )tempInt );
            }
        else if( 0 == tokenKey.Compare( _L( "Day" ) ) )
            {
            TLex tempLex( tokenValue );
            TInt tempInt = 0;
            tempLex.Val( tempInt );
            tempDateTime.SetDay( tempInt );
            }
        else if( 0 == tokenKey.Compare( _L( "Hours" ) ) )
            {
            TLex tempLex( tokenValue );
            TInt tempInt = 0;
            tempLex.Val( tempInt );
            tempDateTime.SetHour( tempInt );
            }
        else if( 0 == tokenKey.Compare( _L( "Minutes" ) ) )
            {
            TLex tempLex( tokenValue );
            TInt tempInt = 0;
            tempLex.Val( tempInt );
            tempDateTime.SetMinute( tempInt );
            }
        else if( 0 == tokenKey.Compare( _L( "Seconds" ) ) )
            {
            TLex tempLex( tokenValue );
            TInt tempInt = 0;
            tempLex.Val( tempInt );
            tempDateTime.SetSecond( tempInt );
            }
        else if( 0 == tokenKey.Compare( _L( "Result" ) ) )
            {
            TLex tempLex( tokenValue );
            TInt tempInt = 0;
            tempLex.Val( tempInt );
            expectedResult = tempInt;
            }
        else if( 0 == tokenKey.Compare( _L( "Next" ) ) )
            {
            aDateTime.Append( tempDateTime );
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

//  [End of File] - Do not remove
