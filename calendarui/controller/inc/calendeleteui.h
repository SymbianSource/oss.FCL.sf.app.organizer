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
* Description:   Controller class for deleting related UI behavior
 *
*/



#ifndef CALENDELETEUI_H
#define CALENDELETEUI_H

// INCLUDES
#include <e32base.h>
#include <calcommon.h>
#include <eikenv.h>
#include <AknWaitDialog.h>
#include <calprogresscallback.h>
#include <calencommandhandler.h>
#include <calennotificationhandler.h>
#include <calenactionuiutils.h>

// FORWARD DECLARATIONS
class CAknWaitDialog;
class CCalenGlobalData;
class CCalenController;
class CCalInstanceView;
class CCalInstance;
class CCalEntry;
class CCalEntryView;

// CLASS DECLARATION
/**
 * CCalenDeleteUi encapsulates deleting related behavior
 * in Calendar application.
 */
NONSHARABLE_CLASS( CCalenDeleteUi ) : public CBase,
                                      public MCalProgressCallBack,
                                      public MCalenNotificationHandler,
                                      public MCalenCommandHandler,
                                      public MProgressDialogCallback
    {

    public: // public API
	
        /**
         * Symbian 1st phase constructor
         * @param aController Reference to CCalenController
         */
        static CCalenDeleteUi* NewL( CCalenController& aController );

        /**
         * Destructor
         */
        virtual ~CCalenDeleteUi();

    public: // From MCalenCommandHandler
        
        /**
         * Handles key presses.
         * 
         * @param aCommand The command Id of the key.
         */
        TBool HandleCommandL( const TCalenCommand& aCommand );
        
        /**
         * Allows extending this API without breaking BC.
         * 
         * @param aExtensionUid specifies
         * @return extension of the requested type
         */
        TAny* CalenCommandHandlerExtensionL( TUid aExtensionUid );

    public:  // New functions
        /**
         * UI sequence for deleting single entry. Shows UI queries and notes and
         * performs delete. Current entry is taken from Calendar Context
         */
        void DeleteCurrentEntryL();

        /**
         * UI sequence for deleting all entries. Shows UI queries and notes and
         * performs delete asynchronosuly.
         */
        void DeleteAllEntriesL();

        /**
         * UI sequence for deleting entries before given date.
         * Shows UI queries and notes and performs delete asynchronosuly.
         */
        void DeleteEntriesBeforeDateL();

        /**
         * UI sequence for deleting series repeating entry
         * Becasue a ThisAndAll repeat type has already been chosen by user,
         * no repeat query is required. Performs delete asynchronosuly.
         */
        void DeleteThisOrAllL( CalCommon::TRecurrenceRange aRepeatType );

    public:  // from MCalenNotificationHandler
        /**
         * Handle notifications
         * @param aNotification Notification to be handled
         */
        void HandleNotification(const TCalenNotification aNotification );

    private: // From MCalProgressCallBack
        /**
         * From MCalProgressCallBack
         */
        void Completed( TInt aError );

        /**
         * From MCalProgressCallBack
         */
        TBool NotifyProgress();

        /**
         * From MCalProgreeCallBack
         */
        void Progress( TInt aPercentageCompleted );

    private: // own methods
        /**
         * Handles deleting all entries in time range from aFirstDay to aLastDay.
         * Shows wait notification and given confirmation notifications.
         */
        void HandleDeleteMultipleEventsL( const TTime& aFirstDay,
                const TTime& aLastDay,
                TInt aConfNoteId );

        /**
         * Delete entries from given range
         */
        void DeleteDayRangeL( const TTime& aStart, const TTime& aEnd );

        /**
         * Delete entries than end at given midnight.
         * This is needed when deleting multiple entries, to handle
         * e.g all-day notes that extends to next day's midnight.
         */
        void DeleteEntriesEndingAtMidnightL( TTime aMidnight );

        /**
         * just a wrapper for leave
         */
        void DoCompletedL( TInt aFirstPassError );

        /**
         * Delete entry by passing CCalEntryView and CCalEntry
         * Will query user for confirmation before deletion
         * @return ETrue user confirms to delete EFalse otherwise 
         */
        TBool DeleteEntryL( CCalEntryView* aEntryView, CCalEntry* aEntry );

        /**
         * Delete entry without querying the user
         */
        TBool DeleteEntryWithoutQueryL();

        /**
         * Delete the given entry. Ask the user whether to delete the
         * instance or the entire series.
         */
        TBool DeleteSingleInstanceL( CCalInstance* aInstance );

        /**
         * Delete the given entry, using aRepeatType to determine
         * whether to delete the instance or the entire series.
         */
        TBool DeleteSingleInstanceL( CCalInstance* aInstance, 
                CalCommon::TRecurrenceRange aRepeatType );

        /**
         * Show the confirmation query with confirmation type and number of entries this query is for
         * @param aType Type of query: EDeleteEntry/ EDeleteTodo/ EDeleteTodos/ EDeleteAll
         */
        TInt ShowDeleteConfirmationQueryL( const CalenActionUiUtils::TDeleteConfirmationType aType,
                const TInt aCount=0 );

        /**
         * Delete single instance.
         * @param aInstance Pointer to the instance to be deleted
         * @param aHasRepeatType Whether or not the instance is repeating
         * @param aRepeatType Repeat type choosen to apply on delete
         * @return ETrue is user confirms to delete, EFalse otherwise
         */
        TBool DoDeleteSingleInstanceL( CCalInstance* aInstance,
                TBool aHasRepeatType,
                CalCommon::TRecurrenceRange aRepeatType );

        /**
         * Attempt to delete the instance using the Meeting Request utilities,
         * if MR viewers is enabled.
         * @param aInstance Pointer to the instance to be delete
         * @param aRepeatType Repeat Type choosen to apply on delete
         * @return ETrue if user confirms to delete, EFalse otherwise
         */
        TBool TryDeleteWithMrUtilsL( CCalInstance* aInstance, 
                CalCommon::TRecurrenceRange aRepeatType );

        /**
         * Leaving function, handle ECalenNotifyViewCreated
         */
        void HandleECalenNotifyViewCreatedL();

        /**
         * Multiple entries delete query
         */ 
        TInt ShowMultipleEntriesDeleteQueryL(TInt aCount);

    private: // own methods
        /**
         * C++ constructor
         */
        CCalenDeleteUi( CCalenController& aController );

        /**
         * Symbian 2nd phase constructor
         */
        void ConstructL();

    protected:  // Methods derived from MProgressDialogCallback
        /**
         * From MProgressDialogCallback
         * Callback method
         * called when a dialog is dismissed
         */
        void DialogDismissedL( const TInt aButtonId );

    private:

        /**
         * Delete entry using entry local uid
         */
        void DeleteEntryL(TCalLocalUid& aEntryLocalUid, TInt aColId );

        /**
         * Displays wait dialog while deleting marked entries
         */
        void DisplayWaitDialogL();

        /**
         * Dismiss wait dialog and show information note
         */
        void MarkedEntriesDeletedL();

        /**
         * Handles launching of the delete entries list query 
         */
        void HandleDeleteAllEntriesL();

    private: // data

        CEikonEnv* iEikEnv;
        CAknWaitDialog* iWaitDialog;

        TBool iIsDeleting; // True, if asynchronous delete is running
        TTime iStartTime;
        TTime iEndTime;
        // Confirmation note id is stored here. Note is shown when asynchronous
        // delete completes.
        TInt iConfirmationNoteId;
        CCalenGlobalData* iGlobalData;
        CCalenController& iController;

        // Stored command if the entry view
        // needs to constructed asyncronously
        TCalenCommand iStoredCommand;
        TInt iMutlipleContextIdsCount;
        TBool iMoreEntriesToDelete;
        TBool iDisplayQuery;
        TInt iEntriesToDelete;
        TInt iToShowDeleteNote;
        TInt iNumberOfCalendars;
        
        RArray<TInt> iDeleteColIds;
        CalCommon::TCalTimeRange *iDelAllRange;

    };

#endif // CALENDELETEUI_H

// End of File
