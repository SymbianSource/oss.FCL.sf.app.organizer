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
* Description:  Declaration of Notepad Editor Dialog class.
*
*/


#ifndef NPDEDITORDIALOG_H
#define NPDEDITORDIALOG_H

// INCLUDES
#include <eikapp.h>
#include <eikdoc.h>
#include <e32std.h>
#include <coeccntx.h>
#include <aknappui.h>
#include <eikedwob.h>
#include "NpdDialogBase.h"
#include "NpdModelObserver.h"
#include <finditemengine.h>
#include <e32property.h> 

// FORWARD DECLARATIONS
class CNotepadModel;
class CNotepadEdwin;
class CNotepadEditorLaf;
class CSendUi;
class CNotepadListDialog;
class CAknAppUi;
class CIdle;
class CFindItemMenu;
class CAiwServiceHandler;

// CLASS DECLARATION
/**
* Dialog of editor mode.
* CNotepadEditorDialog is a dialog for both memo editor and template editor.
*  
* @lib NpdLib.lib
* @see CNotepadDialogBase
*/
class CNotepadEditorDialog : public CNotepadDialogBase, 
    public MNotepadModelObserver
    {
    public:  // Constructors and destructor
        /**
        * Constructor.
        *
        * @param aResId resource id of type STRUCT NOTEPAD_EDITOR_DIALOG.
        * @param aModel pointer to CNotepadModel, never NULL.
        * @param aSendUi aSendUi object.
        * @return new CNotepadDialogBase object.
        * @internal
        */
        IMPORT_C static CNotepadEditorDialog* NewL(
            TInt aResId, 
            CNotepadModel* aModel,
            CNotepadListDialog* aListDialog,
            CSendUi* aSendUi);

        /**
        *  Destructor.
        */
        IMPORT_C virtual ~CNotepadEditorDialog();

    public: // New functions

        #ifdef RD_INTELLIGENT_TEXT_INPUT   
        void FocusChanged(TDrawNow aDrawNow);
        #endif
        /**
        * Execute a dialog.
        *
        * @return exit status value of CEikDialog's ExecuteLD.
        * @internal
        */
        IMPORT_C TInt ExecuteLD();

        /**
        * Switch contents of the editor to text of specified item.
        *
        * @param aKey Unique key id of Notepad items.
        */
        void SwitchTextL(TInt aKey); 

        /**
        * Save current contents of the editor into database.
        *
        * @param aNeedListRefresh Refresh List mode if ETrue.
        * @return TNotepadModelRowResult ( return value of SaveL ).
        */
        TInt SaveOnExitL(TBool aNeedListRefresh);

        /**
        * Switch contents of the editor to text of specified item.
        * Current contents of the editor is saved before the switching.
        * 
        * This method is called when receiving a message from 
        * Pinboard while editor dialog is running.
        *
        * @param aKey unique memo id.
        * @param aAppUi base app ui.
        */
        void EditByKeyL(const TInt aKey, CAknAppUi& aAppUi);

        /**
        * Set a key event. This method is used when the editor is opened 
        * from list mode by pressing a number key.
        *
        * @param aKeyEvent number key event.
        */
        void SetInitialKeyEventL(const TKeyEvent& aKeyEvent);

    protected:  // Functions from base classes

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
        * From CEikDialog.
        * Override to implement dynamically changing decoration.
        *
        * @param aControlId id of editor dialog line.
        */
        void HandleControlStateChangeL(TInt aControlId);

        /**
        * From CEikDialog.
        */
        void PreLayoutDynInitL();

        /**
        * From CEikDialog.
        */
        TBool OkToExitL( TInt aButtonId ) ;

        /**
        * From CCoeControl.
        */
        void ActivateL();

        /**
        * From CCoeControl
        *
        * @param aKeyEvent key event which includes ScanCode etc.
        * @param aType event type ( up, in, down...).
        */
        TKeyResponse OfferKeyEventL(
            const TKeyEvent& aKeyEvent, 
            TEventCode aType);

        /**
        * From CCoeControl.
        *
        * @param aContext TCoeHelpContext.
        */
        void GetHelpContext(TCoeHelpContext& aContext) const;

        /**
        * From CCoeControl
        *
        * @param aType event type.
        */
		void HandleResourceChange(TInt aType);

        /**
        * From CCoeControl.
        * Called when SizeChanged.
        */
        void SizeChanged();

    private: // Constructors

        /**
        * C++ constructor.
        */
        CNotepadEditorDialog(
            CNotepadModel& aModel, 
            CNotepadListDialog* aListDialog );

        /**
        * Internal base constructor.
        *
        * @param aReader resource reader already opened for 
        *                "STRUCT NOTEPAD_EDITOR_DIALOG".
        */
        void ConstructL(TResourceReader& aReader);

    private: // New functions.

        /**
        * Save current content of editor into database.
        *
        * @return TNotepadModelRowResult
        */
        TInt SaveL();


        /**
        * Called on ENotepadCmdAdd.
        */
        void OnCmdAddL();

        /**
        * Called on ENotepadCmdNextMemo
        */
        void OnCmdNextMemoL();

        /**
        * Called on ENotepadCmdPreviousMemo
        */
        void OnCmdPreviousMemoL();

        /**
        * Called on ENotepadCmdDelete
        */
        void OnCmdDeleteL();


        /** 
        * Number key callback function for deferred key event (for FEP).
        *
        * @param a pointer of CNotepadEditorDialog.
        * @return always EFalse.
        */
        static TInt NumberKeyCallBack(TAny *aSelf);


        /**
        * Check the condition if the Notepad editor is empty or not.
        */
  	     inline TBool IsEmpty() const;


        /**
        * Resource id of prompt text when deleting a single item.
        *
        * @return TBUF resource id.
        */
        inline TInt QueryDeleteSingleResId() const;

        /**
        * Create iSequence.
        */
        void CreateSequenceL();

        /**
        * Sync between iSequence and iModel->iKeyArray.
        *
        * @param aForceSync if EFalse syncing is done only when the last 
        *   SyncSequenceL failed on LEAVE, otherwise syncing is done always.
        */
        void SyncSequenceL(const TBool aForceSync);

        /**
        * Check wether we are at the end of Sequence.
        *
        * @return ETrue if sequence exists and current is last.
        */
        TBool IsSequenceAtLast() const;

        /**
        * Check wether we are at the top of Sequence.
        *
        * @return ETrue if sequence exists and current is first.
        */
        TBool IsSequenceAtFirst() const;

        /**
        * The current index in Sequence of the item now editing.
        *
        * @return the current index if IsModeEditing, 
        *   KNotepadInvalidSequenceIndex if IsModeAdding.
        */
        TInt CurrentSequence() const;

        /**
        * Check if current mode is Adding.
        *
        * @return ETrue iff the current mode is Adding.
        */
        inline TBool IsModeAdding() const;

        /**
        * Check if current mode is Editing.
        *
        * @return ETrue iff the current mode is Editing.
        */
        inline TBool IsModeEditing() const;

        /**
        * Key id of the item next to the item now editing.
        *
        * If the mode is Adding and iSequence is empty, 
        * return KNotepadPseudoKeyIdForNewNote.
        *
        * If the mode is Adding and iSequence is not empty, 
        * return a key of the first item in iSequence.
        *
        * If the mode is Editing and the item now editing is the last item
        * in iSequence, return the key now editing.
        *
        * If the mode is Editing and the item now editing is not the last 
        * item in iSequence, return the key of the next item.
        *
        * @return Key id of the item next to the item now editing.
        */
        TInt NextKeyInSequence() const;

        /**
        * Key id of the item previous to the item now editing.
        *
        * If the mode is Adding, return KNotepadPseudoKeyIdForNewNote.
        *
        * If the mode is Editing and the item now editing is the first item
        * in iSequence, return the key now editing.
        *
        * If the mode is Editing and the item now editing is not the first 
        * item in iSequence, return the key of the previous item.
        *
        * @return Key id of the item previous to the item now editing.
        */
        TInt PreviousKeyInSequence() const;

		/**
		* Changes for Find Item.
		* To search a given buffer for phone numbers, email ids, URLs.
		**/
		TInt DoSearchL(CFindItemEngine::TFindItemSearchCase aCase);
		/**
		* test whether the text contains the linefeed character.
		* @return ETrue if the test contains linefeed character, EFalse if not.
		*/
		TBool IsContainLinefeedSymbol(const TDesC& aDes);

    private: // reserved

        /**
        * Reserved.
        */
        IMPORT_C void CNotepadEditorDialog_Reserved();

    private:
        enum TNotepadEditorFlags
            {
            ENotepadFindItemRuning = 0x01,
            ENotepadMenuByOkKey = 0x02,
            ENotepadRequireSequenceUpdate = 0x04,
            ENotepadCatchSaveOnExitCallback = 0x08
            };
        CNotepadEdwin* iEditor; // not own (CEikDialog own)
        CNotepadModel& iModel; // never own
        CSendUi* iSendUi; // not own
        CNotepadListDialog* iListDialog;
        TInt iKey;
        CIdle* iIdle;
        TKeyEvent iEvent; // moved to here from ListDialog
        TUint iFlags;
        CArrayFixFlat<TInt>* iSequence; // own, map SequenceIndex -> Key.
        CFindItemMenu*  iFindItemMenu; //Manual Find Item
        TBool iRedrawBackground;
        RProperty iQwertyModeStatusProperty;
        CAiwServiceHandler* iServiceHandler; // Own: AIW
        CActiveSchedulerWait iStoreWait;
        TBool iTaskSwapperFlag;
        TBool iExitByTerminateKey;
    };

#include "NpdEditorDialog.inl"
#endif // NPDEDITORDIALOG_H

// End of File
