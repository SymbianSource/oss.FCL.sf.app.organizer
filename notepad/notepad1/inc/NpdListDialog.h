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
* Description:  Declaration of Notepad List Dialog class.
*
*/


#ifndef NPDLISTDIALOG_H
#define NPDLISTDIALOG_H

// INCLUDES
#include <eiklbo.h>
#include <coeaui.h>
#include <AknProgressDialog.h>
#include <basched.h>
#include "NpdDialogBase.h"
#include "NpdModelObserver.h"
#include <AiwServiceHandler.h>      //For Application InterWorking
#include <AiwCommon.h>

// FORWARD DECLARATIONS
class CAknColumnListBox;
class CNotepadModel;
class CNotepadEditorDialog;
class CSendUi;
class CEnvironmentChangeNotifier;
class CAknProgressDialog;
class CAknAppUi;

// CLASS DECLARATION
/**
* Dialog of list mode.
* CNotepadListDialog is a dialog for both list of memos and list of templates.
*  
* @lib NpdLib.lib
* @see CNotepadDialogBase
*/
class CNotepadListDialog : public CNotepadDialogBase, 
    public MEikListBoxObserver,
    public MCoeViewObserver,
    public MNotepadModelObserver,
    public MProgressDialogCallback,
    public MAiwNotifyCallback
    {
    public: // Constructor and destructor
         enum TNotepadListBoxEvent
        		{
        		EOpenListBoxItem = 1,
        		EOpenListBoxContextMenu
        		};
        /**
        * Constructor.
        *
        * @param aResId resource id of type STRUCT NOTEPAD_LIST_DIALOG
        * @param aSelfPtr pointer to pointer of this
        * @return Newly constructed CNotepadListDialog object.
        * @internal
        */
        IMPORT_C static CNotepadListDialog* NewL( 
            TInt aResId, 
            CEikDialog** aSelfPtr );

        /**
        * Destructor.
        */
        IMPORT_C virtual ~CNotepadListDialog();

    public: // New functions

        /**
        * Force to switch to editor mode.
        *
        * This API function is used when Notepad application is 
        * opened from Pinboard application.
        *
        * When this API is called, Notepad status is changed to editor mode
        * and a memo whose key is aKey is displayed.
        *
        * If a memo whose key is equal to aKey doesn't exist or
        * aKey is equal to KNotepadPseudoKeyIdForNewNote(-1),
        * an empty Notepad editor for a new memo is displayed.
        * 
        * @param aKey a unique memo id number.
        */
        IMPORT_C void EditByKeyL(const TInt aKey);

        /**
        * If editor is running, save its contents if need.
        * This is called from CNotepadAppUi on shutdown.
        *
        * @internal
        * @see CNotepadEditorDialog::SaveL
        */
        IMPORT_C void SaveL();

        /**
        * Prepare to switch from editor mode to list mode.
        * This function is called when the editor has closed by
        * the right softkey 'Done'.
        *
        * @param aStatOfSave status of CNotepadEditorDialog::SaveL.
        * @param aKey a unique key id of the editing item.
        * @param aEagerSync if this is ETrue, it is necessary to refresh
        * CNotepadModel immediately.
        */
        void HandleEditorExitL( 
            TInt aStatOfSave, 
            const TInt aKey, 
            const TBool aEagerSync );

        /**
        * Clear the iEditorDialog. 
        * Called only from destructor of CNotepadEditorDialog.
        */
        void EditorDeleted();
        
        /**
        * Handle list box events
        * @param aEvent Listbox event type
        */ 
        void HandleListBoxPointerEventL(TNotepadListBoxEvent aEvent);
        
        /**
        * From MiddleSoftKeyL
        * changes middle soft key
        */
		void MiddleSoftKeyL();
		/*
		*
		*/
	    void ClearSaveOrMarkedListItemsL(TBool aFlag);

    private: // Constructor

        /**
        * Second phase constructor.
        *
        * @param aReader resource reader already opened for 
        *                "STRUCT NOTEPAD_LIST_DIALOG".
        */
        void ConstructL(TResourceReader& aReader);

    private: // New functions

        /**
        * Start editor dialog.
        *
        */
        void StartEditorL();

        /**
        * Switch to empty editor mode.
        */
  
  		void OnCmdAddL(); 

        /**
        * Switch to editor mode.
        *
        * @param aIndex ListBox item index to edit.
        */
        void OnCmdOpenL( TInt aIndex );

        /** 
        * Show delete confirmation query and return the answer.
        *
        * @param aMemos # of items to delete.
        * @return ETrue if query confirmed Yes, otherwise EFalse.
        */
        TBool ConfirmDeleteL(TInt aMemos);

        /** 
        * Create key array of selected items.
        */
        void CreateKeyArrayOfSelectionsL();

        /** 
        * Delete memos.
        */
        void OnCmdDeleteL();


        /** 
        * Process Send Command.
        *
        */
        void OnCmdSendL(TInt aCommandId);

        /** 
        * Create iProgressDialog.
        *
        * @param aCount number of items to be processed.
        * @param aLabelResId a label text resource id.
        */
        void CreateProgressDialogL(
            const TInt aCount, 
            const TInt aLabelResId = 0 );




        /**
        * Main part of EditByKeyL.
        *
        * @param aKey a unique memo id number.
        */
        void DoEditByKeyL(const TInt aKey);

        /**
        * Resource id of prompt text when deleting a single item.
        *
        * @return TBUF resource id.
        */
        inline TInt QueryDeleteSingleResId() const;

        /**
        * Resource id of prompt text when deleting a single item.
        *
        * @return TBUF resource id.
        */
        inline TInt QueryDeleteMultipleResId() const;

        /**
        * Environment change callback entry function.
        *
        * @param aThisPtr pointer to this.
        * @return always TRUE (callback should be called again).
        */
        static TInt HandleEnvironmentChangedL(TAny* aThisPtr);

        /**
        * Environment change callback entry function.
        *
        * @return always TRUE (callback should be called again).
        */
        TInt DoHandleEnvironmentChangedL();

        /**
        */
        void SaveCurrentAndSelectionsL();

        /**
        */
        void RestoreCurrentAndSelectionsL();

        /**
        */
        void RestoreCurrent();

        /**
        */
        void RestoreSelectionsL();

        /**
        */
        void ClearSavedCurrentAndSelections();

        /**
        */
        void AdjustTopItemIfNeed();

        /**
        */
        TBool AvoidSyncing() const;

        /**
        */
        inline static TInt LevelOfActiveScheduler();

        /**
        * Implementation of HandleViewEventL.
        * 
        * @param aEvent view event type.
        */
        void DoHandleViewEventL(const TVwsViewEvent& aEvent);

    private:  // Functions from base classes

        /**
        * From MNotepadModelObserver.
        */
        void HandleNotepadModelEventL(TEvent aEvent, TInt aParam = 0);

        /**
        * From MEikMenuObserver.
        * 
        * @param aResourceId resource id.
        * @param menu pane object.
        */
        void DynInitMenuPaneL(TInt aResourceId,CEikMenuPane* aMenuPane);


		/**
        * From MEikMenuObserver.
        * 
        * @param aResourceId resource id.
        * @param menu Bar object.
        */
        void DynInitMenuBarL(TInt aResourceId,CEikMenuBar* aMenuBar);
        
        
        /**
        * From MEikCommandObserver
        * 
        * @param aCommandId command.
        */
        void ProcessCommandL(TInt aCommandId);

        /**
        * From MCoeViewObserver
        * 
        * @param aEvent view event type.
        */
        void HandleViewEventL(const TVwsViewEvent& aEvent);

        /**
        * From MProgressDialogCallback.
        * 
        * @param aButtonId button id which is derived from OkToExitL.
        */
        void DialogDismissedL( TInt aButtonId );

        /**
        * From CEikDialog.
        */
        void PreLayoutDynInitL();

        /**
        * From CEikDialog.
        */
        void PostLayoutDynInitL();

        /**
        * From CEikDialog.
        */
        TBool OkToExitL( TInt aButtonId );

        /**
        * From MEikListBoxObserver.
        *
        * @param aListBox List box object.
        * @param aEventType event type.
        */
        void HandleListBoxEventL(
            CEikListBox* aListBox, 
            TListBoxEvent aEventType );

        /**
        * From CCoeControl.
        *
        * @param aContext TCoeHelpContext.
        */
        void GetHelpContext(TCoeHelpContext& aContext) const;

        /**
        * From CCoeControl
        *
        * @param aKeyEvent key event which includes ScanCode etc.
        * @param aType event type ( up, in, down...).
        */
        TKeyResponse OfferKeyEventL(
            const TKeyEvent& aKeyEvent, 
            TEventCode aType);

    private: // From MAiwNotifyCallback
        /**
         * Handles any notification caused by asynchronous ExecuteCommandL
         * or event.
         * @param aCmdId Service command associated to the event.
         * @param aEventId Occured event
         * @param aEventParamList Event parameters, if any, as defined per
         * each event.
         * @param aInParamList Input parameters, if any, given in the
         *        related HandleCommmandL
         * @return error code for the callback
         */
        TInt HandleNotifyL( TInt aCmdId,
                            TInt aEventId,
                            CAiwGenericParamList& aEventParamList,
                            const CAiwGenericParamList& aInParamList
                          );

        /**
        * From CEikColumnListBox/CCoeControl
        *
        * @param aType event type.
        */
		void HandleResourceChange(TInt aType);

    private:  // Reserved

        /**
        * Reserved API entry.
        *
        * @internal
        */
        IMPORT_C void CNotepadListDialog_Reserved();   
        
   		CAiwGenericParamList* AiwSyncParamListLC(TInt /*aApplicationId*/);

    private:  // Inner class

        /**
        * CNotepadIdle override CIdle to add Restart function and 
        * internal state.
        */
        class CNotepadIdle : public CIdle
            {
            public: // Constructor and destructor
                static CNotepadIdle* NewLC(const TInt aMarkCount);
                ~CNotepadIdle();
            public: // New functions
                inline TInt MarkCountWhenStarted() const;
            private: // Constructors 
                inline CNotepadIdle(const TInt aMarkCount);
                void ConstructL();
            private: // Data
                TInt iMarkCount;
            };

        class CKludgeScheduler : public CBaActiveScheduler
            {
            public:
                inline TInt PublicLevel() const;
            };

        class CNotepadProgressDialog : public CAknProgressDialog
            {
            public: // Constructor and destructor
                CNotepadProgressDialog(
                    CEikDialog** aSelfPtr,
                    TBool aVisibilityDelayOff );
                ~CNotepadProgressDialog();
            public: // New functions
                void PrepareL( const TInt aCount, const TInt aLabelResId );
                void IncrementL( const TInt aIncrement );
                inline CEikProgressInfo* ProgressInfo() const;
            public: // From CAknProgressDialog
                void ProcessFinishedL();
            private: // Data
                CEikProgressInfo* iProgressInfo; // not own, set in PrepareL
                CArrayFix<TInt>* iInts; // own
            };

    private: // Data

        /**
        * Flags set to iFlags.
        */
        enum TNotepadListFlags
            {
            ENotepadObserveView = 0x01,
            ENotepadItemAdded = 0x02,
            ENotepadMenuByOkKey = 0x04,
            ENotepadEverEnvironmentChanged = 0x08,
            ENotepadEverViewDeactivated = 0x10,
            ENotepadIsBackground = 0x20
            };

        CNotepadModel* iModel; // own
        CAknColumnListBox* iListBox; // not own (dialog own)
        TInt iEditorResId;
        CNotepadEditorDialog* iEditorDialog; // own
        CSendUi* iSendUi; // own
        TInt iSavedKey;
        TUint iFlags;
        CAknAppUi* iBaseAppUi; // not own. to know whether SendUi is running
        CNotepadIdle* iIdle; // own
        CNotepadProgressDialog* iProgressDialog; // own
        RArray<TInt> iSavedSelectedKeys; // save keys to delete
        RArray<TInt> iSavedKeysAboveCurrent;
        CEnvironmentChangeNotifier* iEnvironmentChangeNotifier; // own
        TInt iBaseLevelOfActiveScheduler;
        CAiwServiceHandler* iServiceHandler;    /// Own: AIW
        TBool iSendingEmail;
        TBool iClockValueChange;
        TBool shiftflag;
        TBool iEditorFlag;
        TBool iNeedSave;
        TTimeFormat iPreTimeFormat;
        TTimeFormat iCurrentTimeFormat; 
    };

#include "NpdListDialog.inl"
#endif // NPDLISTDIALOG_H

// End of File
