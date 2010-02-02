/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Controller class for editing related UI behavior
 *
*/



#ifndef __EDITUI_H__
#define __EDITUI_H__

// INCLUDES
#include <calentry.h>
#include <e32base.h>
#include <AknWaitDialog.h>
#include <MAgnEntryUi.h>
#include <calcommon.h>
#include <cmrmailboxutils.h>             // CMRMailboxUtils::TMailboxInfo
#include <calencommandhandler.h>
#include <calennotificationhandler.h>   // MCalenNotificationHandler

// FORWARD DECLARATIONS
class CCalenEditorsPlugin;
class CCalenView;
class CCalInstance;
class CCalenGlobalData;
class CCalenController;


// CLASS DECLARATION

/**
 * CCalenEditUi encapsulates editing related behavior in Calendar application.
 *
 */
NONSHARABLE_CLASS( CCalenEditUi ) : public CBase, 
                                    public MAgnEntryUiCallback,
                                    public MCalenNotificationHandler,
                                    public MCalenCommandHandler,
                                    public MProgressDialogCallback
    {
    
    public: // public API
        /**
         * Symbian 1st phase construction
         * @param aController Reference of the controller that owns the editui
         */
        static CCalenEditUi* NewL( CCalenController& aController );

        /**
         * Destructor
         */
        virtual ~CCalenEditUi();

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

    protected:  // From MProgressDialogCallback

        /**
         * From MProgressDialogCallback
         * Callback method
         * called when a dialog is dismissed
         */
        void DialogDismissedL( const TInt aButtonId );

    public:  // New functions

        /**
         * Creates a new entry of type aType and launches the editor
         * @param aType type of new entry
         */
        void EditNewEntryL( CCalEntry::TType aEntryType = CCalEntry::EAppt );

        /**
         * Creates a new meeting request and launches the editor
         */
        void EditNewMeetingRequestL();

        /**
         * Launches editor for aEntry
         * @param aEntry entry to be edited
         * @param aInstancetime Instance time of the entry
         * @param aEditorMode Edit mode, EViewEntry/EEditEntry
         */
        void EditEntryL( CCalEntry* aEntry,
                const TTime& aInstanceTime,
                MAgnEntryUi::TAgnEntryUiEditorMode aEditorMode = MAgnEntryUi::EViewEntry );
		
		/**
         * Function to tell whether editor is active or not
         * @return ETrue if editor is active else EFalse
         */
        TBool IsEditorActive();
		
    public:  // From MCalenNotificationHandler

        /** 
         * Handle notifications
         * @param aNotification Nofication to be handled
         */
        void HandleNotification(const TCalenNotification aNotification );

    protected: // From MAgnEntryUiCallback

        /**
         * Pure virtual function from MAgnEntryUiCallback
         * Not used
         * @param aCommandId Command id to be processed
         */
        TInt ProcessCommandWithResultL( TInt aCommandId );

        /**
         * Handle Command
         * @param aCommandId Command id to be handled
         */
        void ProcessCommandL( TInt aCommandId );

    private: // own methods

        /**
         * C++ constructor
         * @param aController Reference to the controller that owns the caleneditui
         */
        CCalenEditUi( CCalenController& aController );

        /**
         * Symbian 2nd phase constructor
         */
        void ConstructL();

        /**
         * Attempts to load the editors plugin into iEditorsPlugin. If failure occurs
         * (for example, if the user is prompted to select a default mailbox and hits
         * cancel), iEditorsPlugin is set to NULL.
         */
        void LoadEditorsPluginL();

        /**
         * If aComplete is ETrue, sets the todo as complete.
         * Otherwise sets the todo as incomplete.
         * @param aComplete Complete status to be set
         */
        void SetTodoCompleteL( TBool aComplete );

        /**
         * If aComplete is ETrue, sets the todo as complete.
         * Otherwise sets the todo as incomplete.
         * @param aComplete Complete status to be set
         */
        void SetTodoEntryCompleteL( TBool aComplete, TCalLocalUid aEntryUid,TCalCollectionId colId);

        /**
         * If aComplete is ETrue, sets the todo as complete.
         * Otherwise sets the todo as incomplete.
         * @param aComplete Complete status to be set
         */
        TBool SetMultipleTodoCompleteL(TBool aComplete);

        /**
         * Sends the current entry.
         */
        void SendEntryL();

        /**
         * Register for view created notification
         * @param aCommand Command that is going to be handled when view is created
         */
        void WaitForEntryViewNotificationL( const TCalenCommand& aCommand );

        /**
         * Leaving function that handles ECalenNotifyViewCreated
         */
        void HandleECalenNotifyViewCreatedL();

        void DisplayWaitDialogL();

        void MarkedEntriesCompletedL();

        /**
         * Calculate active time from context
         * 
         * @TTime returns the ttime 
         */
        TTime CalculateActiveTimeFromContextL();

    private: // data
        MAgnEntryUi*            iEditorsPlugin;
        CCalenGlobalData*       iGlobalData;
        CCalenController&       iController;
        CDesCArrayFlat*         iListItems;
        CArrayFix<TInt>*        iListItemCommands;
        CAknWaitDialog* iWaitDialog;

        // Stored command if the entry view 
        // needs to constructed asyncronously
        TCalenCommand iStoredCommand;

        TInt iMutlipleContextIdsCount;
        TInt iEntriesToComplete;
        TBool iMoreEntriesToComplete;
        TBool isEditorActive; // to inform framework if editor is active
               
    };

#endif // __EDITUI_H__

// End of File
