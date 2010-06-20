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
* Description:  Declaration of Notepad Viewer Dialog class.
*
*/


#ifndef NPDVIEWERDIALOG_H
#define NPDVIEWERDIALOG_H

// INCLUDES
#include <charconv.h>
#include "NpdDialogBase.h"
#include <centralrepository.h>
#include <cenrepnotifyhandler.h>
#include <itemfinderobserver.h>

// FORWARD DECLARATIONS
class CNotepadRichTextEditor;
class CEikDialog;
class CSendUi;
class CItemFinder;
class CFindItemMenu;
class CPlainText;

// CLASS DECLARATION
/**
*  Dialog of viewer mode.
*  CNotepadViewerDialog is a dialog for Notepad viewer.
*  
* @see CNotepadDialogBase
*/
class CNotepadViewerDialog : public CNotepadDialogBase,
                             public MCenRepNotifyHandlerCallback,
                             public MAknItemFinderObserver

    {
    public: // Constructor and destructor

        /**
        * Constructor.
        *
        * @param resource id of type STRUCT NOTEPAD_VIEWER_DIALOG
        * @param aSelfPtr pointer to pointer of this
        * @return Newly constructed CNotepadViewerDialog object.
        * @internal
        */
        IMPORT_C static CNotepadViewerDialog* NewL(
            const TInt aResId, 
            CEikDialog** aSelfPtr );

        /**
        * Destructor.
        */
        IMPORT_C virtual ~CNotepadViewerDialog();

    public: // New functions

        /**
        * Execute a dialog.
        *
        * @return exit status value of CEikDialog's ExecuteLD.
        * @internal
        */
        //IMPORT_C TInt ExecuteLD();

        /**
        * Set information of the file to be shown.
        *
        * @param aFileName text file to view.
        * @param aTitle  TitlePane string (if NULL, Title will be FileName).
        * @param aGuessEncoding guess encoding if ETrue (ignore aEncoding).
        * @param aEncoding encoding of the File
        *   (default ISO-8859-1. This is ignored if aGuessEncoding is ETrue).
        * @internal
        */
        IMPORT_C void SetFileL(
            const TDesC& aFilename, 
            const TDesC* aTitle = NULL,
            TBool aGuessEncoding = EFalse,
            TUint aEncoding = KCharacterSetIdentifierIso88591);

        /**
        * Set information of the file to be shown.
        *
        * @param aFile file handle.
        * @param aTitle  TitlePane string (if NULL, Title will be FileName).
        * @param aGuessEncoding guess encoding if ETrue (ignore aEncoding).
        * @param aEncoding encoding of the File.
        * @internal
        */
        IMPORT_C void SetFileL(
            RFile& aFile, 
            const TDesC* aTitle = NULL,
            TBool aGuessEncoding = EFalse,
            TUint aEncoding = KCharacterSetIdentifierIso88591);

        /**
        * Load contents of the file into the viewer.
        *
        * @internal
        */
        IMPORT_C void LoadFileL();

        /**
        * Load contents of the file into the viewer.
        *
        * @param aFile file handle.
        * @internal
        */
        IMPORT_C void LoadFileL(RFile& aFile);

        /**
        * Set information of the file to be shown and load it.
        *
        * @param aFileName text file to view.
        * @param aTitle  TitlePane string (if NULL, Title will be FileName).
        * @param aGuessEncoding guess encoding if ETrue (ignore aEncoding).
        * @param aEncoding encoding of the File
        * @internal
        */
        IMPORT_C void OpenFileL(
            const TDesC& aFilename, 
            const TDesC* aTitle = NULL,
            TBool aGuessEncoding = EFalse,
            TUint aEncoding = KCharacterSetIdentifierIso88591);
 
        /**
        * Set information of the file to be shown and load it.
        *
        * @param aFile file handle.
        * @param aTitle  TitlePane string (if NULL, Title will be FileName).
        * @param aGuessEncoding guess encoding if ETrue (ignore aEncoding).
        * @param aEncoding encoding of the File
        * @internal
        */
        IMPORT_C void OpenFileL(
            RFile& aFile, 
            const TDesC* aTitle = NULL,
            TBool aGuessEncoding = EFalse,
            TUint aEncoding = KCharacterSetIdentifierIso88591);
            
        /**
        * Set information of the descriptor to be shown.
        *
        * @param aFileName text file to view.
        */
        void InitDescriptorViewerL(
            HBufC** aContentPointer, 
            TInt& aReturnStatus,
            TBool aReadOnly, 
            const TDesC& aText, 
            const TDesC& aTitle, 
            const TDesC& aDelConfirmation 
            );

        /**
        * Set the state of automatic highlighting
        * @param aSwitchON, ETrue if automatic highlighting is to used, otherwise EFalse
        */
        void SetAutomaticHighlightL( const TBool aSwitchON );

        /**
        * Reads the shared data value used to initialize automatic highlighting
        * @return TBool, ETrue if automatic highlighting is to be used
        */
        void ReadAutoHlCenRepValueAndSetNotifyL();

    protected:  // Functions from base classes

        /**
        * From CEikDialog.
        */
        TBool OkToExitL( TInt aButtonId ) ;

        /**
        * From MCenRepNotifyHandlerCallback
        * Handles the incoming notifications of key changes
        * @param aId, Key that has changed
        * @param aNewValue, New value of the key    
        */
        void HandleNotifyInt( TUint32 aId, TInt aNewValue );

        /**
        * From MCenRepNotifyHandlerCallback
        * Handles the notifier errors
        * @param aId, Key that has changed
        * @param aNewValue, New value of the key    
        */
        void HandleNotifyError( 
            TUint32 aId, 
            TInt aError, 
            CCenRepNotifyHandler* aHandler );

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
    public:
        void HandleDialogPointerEventL( const TPointerEvent& aPointerEvent );
        
        void HandleFindItemEventL(
                const CItemFinder::CFindItemExt& aItem,
                MAknItemFinderObserver::TEventFlag aEvent,
                TUint aFlags );


    private: // New function

        /**
        * Show "Attachement saved" note.
        */
        void ShowConfirmationNoteL();
        /**
        * Handle Phone number calls
        */
        void HandleNumberCallL();

    private:  // Functions from base classes
    
        void ConstructL(TResourceReader& rr);

        /**
        * From CEikDialog.
        */
        void PostLayoutDynInitL();

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
        * From MEikCommandObserver
        * 
        * @param aCommandId command.
        */
        void ProcessCommandL(TInt aCommandId);

        /**
        * From MEikMenuObserver.
        * 
        * @param aResourceId resource id.
        * @param menu pane object.
        */
        void DynInitMenuPaneL(TInt aResourceId, CEikMenuPane* aMenuPane);
        
        /**
        * From MEikMenuObserver.
        * 
        * @param aResourceId resource id.
        * @param menu bar object.
        */
        void DynInitMenuBarL(TInt aResourceId,CEikMenuBar* aMenuBar);

    private: // Reserved

        /**
        * Reserved API entry.
        *
        * @internal
        */
        IMPORT_C void CNotepadViewerDialog_Reserved();   

    private:
        
        /**
        * Callback function for CPeriodic
        *
        * @param TAny* args.
        */
        static TInt TimerCallbackL(TAny* aPtr);    
        
        /**
        * Called in callback function to exit the dialog
        */
        void ExitDialogOnTimerExpireL();

    private: // Data

        enum TNotepadViewerFlags
            {
            ENotepadMenuByOkKey = 0x02,
            EMenuByItemActication = 0x04
            };
        CNotepadRichTextEditor* iEditor; // not own (dialog own)
        HBufC* iFilename;  // own
        RFile  * iFile;
        HBufC** iContentPointer; // not owned
		TBool iFileExist;
        TPtrC iContent; 
        TPtrC iTitle;
        TPtrC iDelConfirmation;
        TInt* iReturnValue;
        TUint iEncoding;
        TInt iStatus;
        TBool iGuessEncoding;

        // Used to flag whether viewer is handling file or descriptor passed
        // from calling application. This changes option menu items somewhat.
        TBool iDescriptorViewer; 

        // Used in conjunction with iDescriptorViewer flag to determine
        // whether "Edit" option menu item should be visible.
        TBool iEditable;

        CSendUi* iSendUi; // own
        TUint16 iFlags; 

        CItemFinder* iAutoFinder; // own
        CFindItemMenu* iFindMenu; // own
        TBool iTaskSwapperFlag;

        // flag for forward locked content
        TBool iForwardLocked;
        TBool iAutomaticHlValue;
        TBool iAutomaticHlInitialized;
        // Central Repository session
        CRepository* iCenRepSession;
        // Notifier to listen changes of offline state
        CCenRepNotifyHandler* iNotifier;
        CPeriodic*  iPeriodicTimer;
    };

#endif // NPDVIEWERDIALOG_H

// End of File
