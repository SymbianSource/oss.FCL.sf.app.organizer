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
* Description:  command line parse utils
 *
*/


//debug
#include "calendarui_debug.h"

// INCLUDE FILES
#include "calencmdlineparser.h"

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CCalenCmdLineParser::CCalenCmdLineParser
// Constructor
// (other items were commented in a header).
// Status : Draft/Proposal/Approved
// ---------------------------------------------------------
//
CCalenCmdLineParser::CCalenCmdLineParser()
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------
// CCalenCmdLineParser::NewL
// Static constructor
// (other items were commented in a header).
// Status : Draft/Proposal/Approved
// ---------------------------------------------------------
//
CCalenCmdLineParser* CCalenCmdLineParser::NewL()
    {
    TRACE_ENTRY_POINT;

    CCalenCmdLineParser* self = new( ELeave ) CCalenCmdLineParser();

    TRACE_EXIT_POINT;
    return self;
    }

// ---------------------------------------------------------
// CCalenCmdLineParser::~CCalenCmdLineParser
// Destructor
// (other items were commented in a header).
// Status : Draft/Proposal/Approved
// ---------------------------------------------------------
//
CCalenCmdLineParser::~CCalenCmdLineParser()
    {
    TRACE_ENTRY_POINT;

    iStrings.ResetAndDestroy();
    delete iCommandLine;

    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------
// CCalenCmdLineParser::AddL
// Add string to the list. In this case it can be descriptor.
// (other items were commented in a header).
// Status : Draft/Proposal/Approved
// ---------------------------------------------------------
//
void CCalenCmdLineParser::AddL( const TDesC& aString )
    {
    TRACE_ENTRY_POINT;

    if( aString.Length() )
        {
        HBufC* b = aString.AllocLC();
        iStrings.AppendL( b );
        CleanupStack::Pop( b );
        }

    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------
// CCalenCmdLineParser::ItemL
// Return item at specific position
// (other items were commented in a header).
// Status : Draft/Proposal/Approved
// ---------------------------------------------------------
//
TPtr CCalenCmdLineParser::ItemL( TInt aIndex )
    {
    TRACE_ENTRY_POINT;

    if( aIndex >= iStrings.Count() )
        {
        User::Leave( KErrArgument );
        }

    TRACE_EXIT_POINT;
    return iStrings[aIndex]->Des();
    }

// ---------------------------------------------------------
// CCalenCmdLineParser::ParseStringL
// Parse given sting in to substrings which are added to the list
// (other items were commented in a header).
// Status : Draft/Proposal/Approved
// ---------------------------------------------------------
//
void CCalenCmdLineParser::ParseStringL( const TDesC& aString,
                                                        const TDesC& aDelimiter )
    {
    TRACE_ENTRY_POINT;

    // create copy of string to be parsed
    HBufC* copy = aString.AllocLC();

    // clean possible previous items
    iStrings.ResetAndDestroy();

    TInt stop;

    while( ( stop = copy->Des().Find( aDelimiter ) ) != KErrNotFound )
        {
        HBufC* current = HBufC::NewLC( stop );
        current->Des().Copy( copy->Des().Ptr(), stop );
        AddL( current->Des() );
        CleanupStack::PopAndDestroy( current );
        copy->Des().Delete( 0, stop + aDelimiter.Length() );
        }

    AddL( copy->Des() ); // add the last parameter
    CleanupStack::PopAndDestroy( copy );

    if( iStrings.Count() > ECalCmdMaxParamCount )
        {
        User::Leave( KErrArgument );
        }

    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------
// CCalenCmdLineParser::ParseCommandLineL
// Parse commandline parameters and store in iStrings
// (other items were commented in a header).
// Status : Draft/Proposal/Approved
// ---------------------------------------------------------
//
void CCalenCmdLineParser::ParseCommandLineL( const TDesC& aCommandLine )
    {
    TRACE_ENTRY_POINT;

    iCmdLineParameters.iCommandType = 0;
    iCmdLineParameters.iLocalUid = 0;
    iCmdLineParameters.iFlag = 0;
    iCmdLineParameters.iTime = Time::NullTTime();

    HBufC* buf = aCommandLine.AllocLC();

    TPtrC spc = KSpace();

    ParseStringL( buf->Des(), spc );

    if( iStrings.Count() > ECalCmdMaxParamCount || !iStrings.Count() )
        {
        User::Leave( KErrArgument );
        }

    iCmdLineParameters.iCommandType = DesToCmdParamTypeL( ItemL(ECalCmdLineCmd) );
    User::LeaveIfError( iCmdLineParameters.iCommandType );

    // If we are NOT opening using a Local UID we need to add an extra (empty)
    // string into the array to keep the date parsing indexes correct eg:
    // MONTH        2006    11  15
    // LUID     1   2006    11  15
    if( ( iCmdLineParameters.iCommandType != EStartTypeUid ) && 
        ( iCmdLineParameters.iCommandType != EStartTypeUidViewer )&&
        (iCmdLineParameters.iCommandType != EStartTypeUidAlarmViewer) &&
        (iCmdLineParameters.iCommandType != EStartTypeUidAlarmViewerNoSnooze))
        {
        // Create a string and add to array at position ECalCmdLineUid
        TBufC<ECalCmdLineUid> dummyBuf( KSpace );
        HBufC* dummyString = dummyBuf.AllocLC();
        iStrings.InsertL( dummyString, ECalCmdLineUid );
        CleanupStack::Pop( dummyString );
        }
    else
        {
        // Get the UID
        TLex uid;    
        uid.Assign( ItemL( ECalCmdLineUid ) );
        uid.Val( iCmdLineParameters.iLocalUid, EDecimal );
        }

    if( ( iCmdLineParameters.iCommandType != EStartTypeNewMeetingRequest ) && 
        ( iCmdLineParameters.iCommandType != EStartTypeNewMeeting ) &&
        ( iCmdLineParameters.iCommandType != EStartTypeNewAnniv ) &&
        ( iCmdLineParameters.iCommandType != EStartTypeNewTodo ) )
        {
        // Create a string and add to array at position ECalCmdLineFlag
        TBufC<1> dummyBuf( KSpace );
        HBufC* dummyString = dummyBuf.AllocLC();
        iStrings.InsertL( dummyString, ECalCmdLineFlag );
        CleanupStack::Pop( dummyString );
        }
    else
        {
        // Get the flag. This flag is used whether to exit application on dialog close or not.
        TLex uid;    
        uid.Assign( ItemL( ECalCmdLineFlag ) );
        uid.Val( iCmdLineParameters.iFlag );
        }
    
    if( ( iCmdLineParameters.iCommandType != EStartTypeUidViewer ) && 
        ( iCmdLineParameters.iCommandType != EStartTypeUidAlarmViewer ) &&
        ( iCmdLineParameters.iCommandType != EStartTypeUidAlarmViewerNoSnooze ) )
        {
        // Create a string and add to array at position ECalCmdLineFlag
        TBufC<1> dummyBuf( KSpace );
        HBufC* dummyString = dummyBuf.AllocLC();
        iStrings.InsertL( dummyString, ECalCmdCalFileName );
        CleanupStack::Pop( dummyString );
        }
    else
        {
        // Get the Calen database name of the calendar entry.        
        iCmdLineParameters.iCalenFileName = ItemL(ECalCmdCalFileName);
        }
    // Get the time if there is one
    if( iStrings.Count() > ECalCmdLineFlag )
        {
        iCmdLineParameters.iTime = TimeFromParamsL();
        }
    else if( iCmdLineParameters.iCommandType == EStartTypeDate )
        {
        // time is needed for EStartTypeDate!
        User::Leave( KErrArgument );
        }
    else
        {
        // Do nothing.
        // final else added to remove lint warning
        }

    CleanupStack::PopAndDestroy( buf );

    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------
// CCalenCmdLineParser::TimeFromParamsL
// Get time value from parameters
// (other items were commented in a header).
// ---------------------------------------------------------
//
TTime CCalenCmdLineParser::TimeFromParamsL()
    {
    TRACE_ENTRY_POINT;

    TLex lex;
    TInt year, month(1), day(1), hour(0), min(0), sec(0), msec(0);

    TInt argCount = iStrings.Count();
    argCount = argCount - 2;
    switch( argCount )
        {
        // Set as many time properties as we can find.
        case ECalCmdLineMicroSeconds:
            {
            // Add microseconds - unlikely anyone would bother with these though
            lex.Assign( ItemL( ECalCmdLineMicroSeconds ) );
            lex.Val( msec );
            }
            //lint -fallthrough            
        case ECalCmdLineSeconds:
            {
            // Add seconds
            lex.Assign(ItemL( ECalCmdLineSeconds ) );
            lex.Val( sec );
            }
            //lint -fallthrough            
        case ECalCmdLineMinute:
            {
            // Add minutes
            lex.Assign( ItemL( ECalCmdLineMinute ) );
            lex.Val( min );
            }
            //lint -fallthrough            
        case ECalCmdLineHour:
            {
            // Add hours
            lex.Assign( ItemL( ECalCmdLineHour ) );
            lex.Val( hour );
            }
            //lint -fallthrough            
        case ECalCmdLineDay:
            {
            // Add day
            lex.Assign( ItemL(ECalCmdLineDay ) );
            lex.Val( day );
            }
            //lint -fallthrough            
        case ECalCmdLineMonth:
            {
            // Add month
            lex.Assign( ItemL( ECalCmdLineMonth ) );
            lex.Val( month );
            }
            //lint -fallthrough            
        case ECalCmdLineYear:
            {
            // Add year
            lex.Assign( ItemL( ECalCmdLineYear ) );
            lex.Val( year );
            }
            break;
        
        default:
            // No time component found....
            TRACE_EXIT_POINT;
            return Time::NullTTime();
        }

    // prevent panic during TDateTime construction!
    month = month > 0 && month <= 12 ? month : 1;

    TInt maxDaysInMonth( Time::DaysInMonth( year, TMonth(month-1) ) );
    day  = day > 0 && day <= maxDaysInMonth ? day : 1;

    hour = hour >= 0 && hour <= 23 ? hour : 0;
    min  = min >= 0 && min <= 59 ? min : 0;
    sec  = sec >= 0 && sec <= 59 ? sec : 0;
    msec = msec >= 0 && msec <= 999999 ? msec : 0;

    TDateTime dt( year, TMonth( --month ), --day, hour, min, sec, msec );

    TRACE_EXIT_POINT;
    return TTime( dt );
    }

// ---------------------------------------------------------
// CCalenCmdLineParser::CommandLineParameters
// Return commandline parameters
// (other items were commented in a header).
// Status : Draft/Proposal/Approved
// ---------------------------------------------------------
//
TCalenCmdParameters CCalenCmdLineParser::CommandLineParameters() 
    {
    TRACE_ENTRY_POINT;

    TRACE_EXIT_POINT;
    return iCmdLineParameters;
    }

// ---------------------------------------------------------
// CCalenCmdLineParser::DesToCmdParamTypeL
// Convert descriptor to parameter type
// (other items were commented in a header).
// Status : Draft/Proposal/Approved
// ---------------------------------------------------------
//
TInt CCalenCmdLineParser::DesToCmdParamTypeL( const TDesC& aCmdParamType )
    {
    TRACE_ENTRY_POINT;

    HBufC* temp = aCmdParamType.AllocLC();
    TPtr des = temp->Des();
    des.UpperCase();

    TInt res( 0 );

    if( des.Find( KCommandDefault ) != KErrNotFound )
        {
        res = EStartTypeToday;
        }
    else if( des.Find( KCommandNewMeetingRequest ) != KErrNotFound )
        {
        res = EStartTypeNewMeetingRequest;
        }
    else if( des.Find( KCommandNewMeeting ) != KErrNotFound )
        {
        res = EStartTypeNewMeeting;
        }
    else if( des.Find( KCommandNewAnniv ) != KErrNotFound )
        {
        res = EStartTypeNewAnniv;
        }        
    else if( des.Find( KCommandNewTodo ) != KErrNotFound )
        {
        res = EStartTypeNewTodo;
        }
    else if( des.Find( KCommandViewer ) != KErrNotFound )
        {
        res = EStartTypeUidViewer;
        }        
    else if( des.Find(KCommandAlarmViewerNoSnooze) != KErrNotFound )
        {
        res = EStartTypeUidAlarmViewerNoSnooze;
        }
    else if( des.Find(KCommandAlarmViewer) != KErrNotFound )
        {
        res = EStartTypeUidAlarmViewer;
        }
    else if( des.Find( KCommandUid ) != KErrNotFound )
        {
        res = EStartTypeUid;
        }
    else if( des.Find( KCommandTime ) != KErrNotFound )
        {
        res = EStartTypeDate;
        }
    else if( des.Find( KCommandToDo ) != KErrNotFound )
        {
        res = EStartTypeToDo;
        }
    else if( des.Find( KCommandDay ) != KErrNotFound )
        {
        res = EStartTypeDay;
        }
    else if( des.Find( KCommandWeek ) != KErrNotFound )
        {
        res = EStartTypeWeek;
        }
    else if( des.Find( KCommandMonth ) != KErrNotFound )
        {
        res = EStartTypeMonth;
        }
    else if( des.Find( KCommandDefaultView ) != KErrNotFound )
        {
        res = EStartTypeDefault;
        }
    else
        {
        res = -1;
        }

    CleanupStack::PopAndDestroy( temp );

    TRACE_EXIT_POINT;
    return res;
    }

// End of File
