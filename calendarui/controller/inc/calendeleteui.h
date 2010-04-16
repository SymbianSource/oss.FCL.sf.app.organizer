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
* Description:  Controller class for deleting related UI behavior
 *
*/


#ifndef CALENDELETEUI_H
#define CALENDELETEUI_H

// System Includes
#include <QObject>
#include <e32base.h>
#include <hbdatetimepicker.h>
#include <agendautil.h>

//User Includes
#include "calenactionuiutils.h"
#include "calennotificationhandler.h"
#include "calencommandhandler.h"


// FORWARD DECLARATIONS
class CCalenController;


// CLASS DECLARATION
/**
 * CalenDeleteUi encapsulates deleting related behavior
 * in Calendar application.
 */
 class CalenDeleteUi : public QObject,
                          public MCalenNotificationHandler,
                          public MCalenCommandHandler                               
                                      
    {
    Q_OBJECT

public: // public API
    /**
     * Symbian 1st phase constructor
     * @param aController Reference to CCCalenController
     */
    static CalenDeleteUi* NewL(CCalenController& aController );

    /**
     * Destructor
     */
    virtual ~CalenDeleteUi();

public:  // MCalenCommandHandler
    /**
     * Handles action ui commands
     * @param aCommand Command to be handled
     */
    TBool HandleCommandL( const TCalenCommand& aCommand );

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
     void DeleteThisOrAllL( AgendaUtil::RecurrenceRange aRepeatType );
     
public:  // from MCalenNotificationHandler
    /**
     * Handle notifications
     * @param aNotification Notification to be handled
     */
    void HandleNotification(const TCalenNotification aNotification );

private: // own methods
    /**
     * Handles deleting all entries in time range from aFirstDay to aLastDay.
     * Shows wait notification and given confirmation notifications.
     */
    void HandleDeleteMultipleEventsL( const QDateTime& aFirstDay,
                                      const QDateTime& aLastDay,
                                      int aConfNoteId );

    /**
     * Delete entries from given range
     */
    void DeleteDayRangeL( const QDateTime& aStart, const QDateTime& aEnd );

    /**
     * Delete entries than end at given midnight.
     * This is needed when deleting multiple entries, to handle
     * e.g all-day notes that extends to next day's midnight.
     */
    void deleteEntriesEndingAtMidnight( QDateTime aMidnight );

    /**
     * Delete entry by passing CCalEntryView and CCalEntry
     * Will query user for confirmation before deletion
     * @return ETrue user confirms to delete EFalse otherwise 
     */
    TBool DeleteEntryL( AgendaEntry& aEntry );
    
    /**
     * Delete entry without querying the user
     */
    TBool DeleteEntryWithoutQueryL();

    /**
     * Delete the given entry. Ask the user whether to delete the
     * instance or the entire series.
     */
    TBool DeleteSingleInstanceL( AgendaEntry& aInstance );

    /**
     * Delete the given entry, using aRepeatType to determine
     * whether to delete the instance or the entire series.
     */
    TBool DeleteSingleInstanceL( AgendaEntry& aInstance, 
            AgendaUtil::RecurrenceRange aRepeatType );
    
    /**
     * Delete single instance.
     * @param aInstance Pointer to the instance to be deleted
     * @param aHasRepeatType Whether or not the instance is repeating
     * @param aRepeatType Repeat type choosen to apply on delete
     * @return ETrue is user confirms to delete, EFalse otherwise
     */
    TBool DoDeleteSingleInstanceL( AgendaEntry& aInstance,
                                   bool aHasRepeatType,
                                   AgendaUtil::RecurrenceRange aRepeatType );
    
    /**
     * Leaving function, handle ECalenNotifyViewCreated
     */
    void HandleECalenNotifyViewCreatedL();
    
   /**
    * Multiple entries delete query
    */ 
    TInt ShowMultipleEntriesDeleteQueryL(int aCount);
    
   /**
    * Get the date from user.
    */
    void dateQuery();
    
    /**
     * Shows a confirmation query to the user if
     * he/she wants to delete the particular instance
     * or the entire series of a repeating entry
     */
    void showRepeatingEntryDeleteQuery();

private: // own methods
    /**
     * C++ constructor
     */
    CalenDeleteUi( CCalenController& aController, QObject *parent = 0 );
    
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
    void DialogDismissedL( const int aButtonId );

public slots:

    void doCompleted( int aFirstPassError );
    void getSelectedDate();
    
private slots:
    void handleRepeatedEntryDelete(int index);
    void entryDeleted(ulong id);

private:
    
    /**
     * Delete entry using entry local uid
     */
    void DeleteEntryL(ulong& aEntryLocalUid);
    
    /**
     * Displays wait dialog while deleting marked entries
     */
    void DisplayWaitDialogL();
    
    /**
     * Dismiss wait dialog and show information note
     */
    void MarkedEntriesDeletedL();
    
private: // data
    
    bool iIsDeleting; // True, if asynchronous delete is running
    QDateTime iStartTime;
    QDateTime iEndTime;
    QDateTime mDateTime; // Get the date selected by user.
    HbDateTimePicker *mDatePicker;
    // Confirmation note id is stored here. Note is shown when asynchronous
    // delete completes.
    int iConfirmationNoteId;
    CCalenController& iController;

    // Stored command if the entry view
    // needs to constructed asyncronously
    TCalenCommand iStoredCommand;
    int iMutlipleContextIdsCount;
    bool iMoreEntriesToDelete;
    bool iDisplayQuery;
    bool mIsDateValid;
    int iEntriesToDelete;
    };

#endif // CALENDELETEUI_H

// End of File
