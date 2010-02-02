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



#ifndef __CALCMDLINEPARSER_H__
#define __CALCMDLINEPARSER_H__


//  INCLUDES
#include <e32base.h>
#include <calcommon.h>

// DATA TYPES
struct TCalenCmdParameters
    {
    TInt iCommandType;
    TUint32 iLocalUid;
    TInt iFlag;
    TBuf<256> iCalenFileName;
    TTime iTime;
    };


// CONSTANTS
_LIT( KComma, "," );
_LIT( KSpace, " " );

// Launch calendar to the Day view with todays date
_LIT( KCommandDefault,               "TODAY" );

// Command line format is:
// CMD YEAR MONTH DAY HOUR MINUTE SECONDS MICROSECONDS
// Date components MUST be specified in the above order
// but date components are now optional
// For example:
// DAY 2006
// Would open Calendar day view on 1st January 2006
//
// DAY 2006 03
// Would open Calendar day view on the 1st March 2006
//
// DAY 2006 03 13
// Would open Calendar day view on 13th March 2006
_LIT( KCommandTime,                 "DATE");

// Launch calendar from the command line to new entry editors
_LIT( KCommandNewMeeting,           "NEW_MEETING" );
_LIT( KCommandNewMeetingRequest,    "NEW_MEETING_REQUEST" );
_LIT( KCommandNewAnniv,             "NEW_ANNIV" );
_LIT( KCommandNewTodo,              "NEW_TODO" );
_LIT( KCommandNewEntry,             "NEW_ENTRY" );

// Launch Calendar from the command line using the Local UID of
// an existing entry. The entry can be opened in either the editor or
// the viewer.  Can be followed by time parameters to open a specific
// instance of a repeating entry.
_LIT( KCommandUid,                  "LUID" );
_LIT( KCommandViewer,               "LUIDVIEWER" );
_LIT( KCommandAlarmViewer,          "LUIDALARMVIEWER");
_LIT( KCommandAlarmViewerNoSnooze,  "LUIDALARMVIEWER_NOSNOOZE");

// Launch calendar from the command line to a specific view. Can be
// followed by time parameters to open the view on a specific day
_LIT( KCommandDefaultView,          "DEFAULT" );
_LIT( KCommandDay,                  "DAY" );
_LIT( KCommandWeek,                 "WEEK" );
_LIT( KCommandMonth,                "MONTH" );
_LIT( KCommandToDo,                 "TODO" );

// CLASS DECLARATION

/**
* CCalCmdLineParser is used to parse and set commandlines for Calendar.
* Command line will take parameters like Agenda Instance ID which will
* make easy way to start calendar with some specific event active.
*/
NONSHARABLE_CLASS( CCalenCmdLineParser ) : public CBase
    {
public:
    // Assumed indexes of command line components in the string array
    enum TCalCmdParam
        {
        ECalCmdLineCmd,             // Command index
        ECalCmdLineUid,             // Local UID of an entry
        ECalCmdLineFlag,            // Flag index
        ECalCmdCalFileName,           // Calendar DB name
        ECalCmdLineYear,            // Year index (optional)
        ECalCmdLineMonth,           // Month index (optional)
        ECalCmdLineDay,             // Day index (optional)
        ECalCmdLineHour,            // Hour index (optional)
        ECalCmdLineMinute,          // Minute index (optional)
        ECalCmdLineSeconds,         // Second index (optional)
        ECalCmdLineMicroSeconds,    // Microsecond index (optional)
        ECalCmdMaxParamCount        // Maximum number of allowed parameters
        };

    // Calendar start up mode as determined by ECalCmdLineCmd
    // and optional date components
    enum TCalStartType
        {
        EStartTypeToday=2005,       // start in Day view using todays date
        EStartTypeUid,              // start by local uid to entry editor
        EStartTypeUidViewer,        // start by local uid to entry viewer
        EStartTypeUidAlarmViewer,   // start by local uid to entry viewer with alarm softkeys
        EStartTypeUidAlarmViewerNoSnooze,  // start by local uid to entry viewer without snooze key
        EStartTypeDate,             // start by date (requires at least 1 date component)
        EStartTypeDefault,          // start in default view
        EStartTypeDay,              // start in Day view
        EStartTypeToDo,             // start in Todo view
        EStartTypeWeek,             // start in Week view
        EStartTypeMonth,            // start in Month view
        EStartTypeNewMeeting,       // start new Meeting editor
        EStartTypeNewMeetingRequest,// start new Meeting request editor (if available)
        EStartTypeNewAnniv,         // start new Anniversary editor
        EStartTypeNewTodo,          // start new Todo editor
        };

public:
    /**
     * Static constructor
     * @return new instance of CCalCmdLineParser
     **/
    static CCalenCmdLineParser* NewL();

    /**
     * parse commandline
     * @param aCommandLine is descriptor containing the command line to be parsed
     **/
    void ParseCommandLineL( const TDesC& aCommandLine );

    /**
     * @return the actual parameters
     **/
     TCalenCmdParameters CommandLineParameters();

    /**
     * Destructor
     **/
    virtual ~CCalenCmdLineParser();

private:
    /**
     * Constructor
     **/
    CCalenCmdLineParser();

    /**
     * Add descriptor to the list
     * @param aString is the string to be added into the list
     **/
    void AddL( const TDesC& aString );

    /**
     * Return item at position
     * @param aIndex is the index of desired item
     * @return Pointer descriptor of desired line
     **/
    TPtr ItemL( TInt aIndex );

    /**
     * Parse given string to invidual list items
     * @param aString is the string to be parsed
     * @param aDelimiter is the delimiter used to parse lines<P>
     * Common delimiter is comma but can be also space etc.
     **/
    void ParseStringL( const TDesC& aString, const TDesC& aDelimiter );

    /**
     * Convert Descriptor to parameter type
     **/
    TInt DesToCmdParamTypeL( const TDesC& aCmdParamType );

    /**
     * Extract TTime from param strings.
     * @return A TTime object.
     **/
    TTime TimeFromParamsL( void );

private:
    // Storage for the strings
    RPointerArray<HBufC> iStrings;

    // Command line
    HBufC* iCommandLine;

    // Storage for the strings
    TCalenCmdParameters iCmdLineParameters;

    };

#endif // __CALCMDLINEPARSER_H__


// End of File
