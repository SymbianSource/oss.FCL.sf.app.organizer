/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Wrapper class for launching S60 Calendar application
*
*/



#ifndef CALENLAUNCHER_H
#define CALENLAUNCHER_H

/**
* Include files
*/
#include <calentry.h>   // CCalEntry::TType

/**
* S60 Month view uid
*/
const TUid KCalMonthViewUid    = { 0x10283360 };

/**
* S60 Week view uid
*/
const TUid KCalWeekViewUid     = { 0x10283361 };

/**
* S60 Day view uid
*/
const TUid KCalDayViewUid      = { 0x10283362 };

/**
* S60 Todo view uid
*/
const TUid KCalTodoViewUid     = { 0x10283363 };

/**
* S60 Default view uid
* This is the default view as set by the user in the Calendar settings
*/
const TUid KCalDefaultViewUid   = { 0x10283364 };

/**
* CalenLauncher allows external applications to start the S60 Calendar
* application in any of its views.
*
* Available launch options are:
* - Launch the Calendar and edit a new entry
* - Launch the Calendar and view an existing entry
* - Launch the Calendar and view an instance of a repeating entry
* - Launch the Calendar and edit an existing entry
* - Launch the Calendar and edit an instance of a repeating entry
* - Launch the Calendar to the default view
* - Launch the Calendar to the specified view
* - Launch the Calendar to the specified view at a given time and date
*
* @code
* // To create a new meeting
* CalenLauncher::NewEntryL( CCalEntry::EAppt );
*
* // To create a new todo
* CalenLauncher::NewEntryL( CCalEntry::ETodo );
*
* // To launch Calendar in the month view
* CalenLauncher::LaunchL( KCalMonthViewUid );
* @endcode
*/
NONSHARABLE_CLASS( CalenLauncher )
    {
    public:
        // internal viewer flags - do not use
        enum
            {
            EAlarmViewer         = 0x01,
            EAlarmViewerNoSnooze = 0x02
            };

    public:
        /**
        * Opens Calendar directly to the new entry editor.
        * Calendar will be closed when the editor is closed, even if it was
        * already open.
        * Any open dialogs (eg entry viewer or editor) will be closed.
        * @param aEntryType Type of entry editor to open
        * @param aFlags Reserved for future use
        */
        IMPORT_C static void NewEntryL( const CCalEntry::TType& aEntryType,
                                        TInt aFlags = 0 );

        /**
        * Opens a Calendar entry directly in the entry viewer.  If the
        * entry is a repeating entry then entire series will be opened.
        * The entry editor may be launched from the entry viewer.
        * Calendar will be closed when the editor is closed, even if it was
        * already open.
        * Any open dialogs (eg entry viewer or editor) will be closed.
        * @param aUid LocalUid of the entry to view
        * @param aFlags Reserved for future use
        */
        IMPORT_C static void ViewEntryL( const TCalLocalUid& aUid,
                                         TInt aFlags = 0 );

        /**
        * Opens an instance of a repeating Calendar entry directly in the
        * entry viewer.
        * The entry editor may be launched from the entry viewer.
        * Calendar will be closed when the editor is closed, even if it was
        * already open.
        * Any open dialogs (eg entry viewer or editor) will be closed.
        * @param aUid LocalUid of the entry to view
        * @param aInstanceTime Instance time of the repeated entry
        * @param aFlags Reserved for future use
        */
        IMPORT_C static void ViewEntryL( const TCalLocalUid& aUid,
                                         const TCalTime& aInstanceTime,
                                         TInt aFlags = 0 );

		/**
        * Opens an instance of a repeating Calendar entry directly in the
        * entry viewer.
        * The entry editor may be launched from the entry viewer.
        * Calendar will be closed when the editor is closed, even if it was
        * already open.
        * Any open dialogs (eg entry viewer or editor) will be closed.
        * @param aUid LocalUid of the entry to view
        * @param aInstanceTime Instance time of the repeated entry
        * @param aCalFileName Calendar DB name of the repeated entry.
        * @param aFlags Reserved for future use
        */
        IMPORT_C static void ViewEntryL( const TCalLocalUid& aUid,
                                         const TCalTime& aInstanceTime,
                                         const TDesC& aCalFileName,
                                         TInt aFlags = 0 );



        /**
        * Opens a Calendar entry directly in the entry editor.  If the
        * entry is a repeating entry then entire series will be opened.
        * Calendar will be closed when the editor is closed, even if it was
        * already open.
        * Any open dialogs (eg entry viewer or editor) will be closed.
        * @param aUid LocalUid of the entry to view
        * @param aFlags Reserved for future use
        */
        IMPORT_C static void EditEntryL( const TCalLocalUid& aUid,
                                         TInt aFlags = 0 );

        /**
        * Opens an instance of a repeating Calendar entry directly in the
        * entry editor.
        * Calendar will be closed when the editor is closed, even if it was
        * already open.
        * Any open dialogs (eg entry viewer or editor) will be closed.
        * @param aUid LocalUid of the entry to view
        * @param aInstanceTime Instance time of the repeated entry
        * @param aFlags Reserved for future use
        */
        IMPORT_C static void EditEntryL( const TCalLocalUid& aUid,
                                         const TCalTime& aInstanceTime,
                                         TInt aFlags = 0 );

        /**
        * Launches Calendar in the specified view.
        * If Calendar is already running it will be brought to the foreground
        * in the specified view.
        * Any open dialogs (eg entry viewer or editor) will be closed.
        * @param aViewUid UID of the Calendar view to use.
        * @param aFlags Reserved for future use
        */
        IMPORT_C static void LaunchL( const TUid& aViewUid, TInt aFlags = 0 );

        /**
        * Launches Calendar in the specified view, focused to the specified time
        * If Calendar is already running it will be brought to the foreground
        * in the specified view.
        * Any open dialogs (eg entry viewer or editor) will be closed.
        * @param aViewUid UID of the Calendar view to use.
        * @param aFocusTime Initial focus time
        * @param aFlags Reserved for future use
        */
        IMPORT_C static void LaunchL( const TUid& aViewUid,
                                      const TCalTime& aFocusTime,
                                      TInt aFlags = 0 );
    };

#endif // CALENLAUNCHER_H

// End of File
