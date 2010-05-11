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
* Description:  Declaration of Base class of Notepad Dialogs.
*
*/


#ifndef NPDDIALOGBASE_H
#define NPDDIALOGBASE_H

// INCLUDES
#include <AknDialog.h>
#include <f32file.h>
#include "NpdLib.hrh"
#include <AknProgressDialog.h>
// FORWARD DECLARATIONS
class CAknTitlePane;
class CSendUi;
class CSendAppUi;
class CNotepadDialogBase;
class CEikonEnv;
class CNotepadModel;
class CEikEdwin;
class CEikMenuPane;
class CPlainText;
class RFile;
class TResourceReader;
class CAknProgressDialog;
class CMessageData;

class CNotepadListDialog;
// CLASS DECLARATION
/**
* Base class of three dialogs in Notepad library.
* CNotepadDialogBase is the base class of CNotepadListDialog, 
* CNotepadEditorDialog and CNotepadViewerDialog.
*
* @lib NpdLib.lib
* @see CNotepadListDialog, CNotepadEditorDialog, CNotepadViewerDialog.
*/
class CNotepadDialogBase : public CAknDialog
    {
    public:  // Constructors and destructor

        /**
        * Destructor.
        */
        IMPORT_C virtual ~CNotepadDialogBase();

    public:  // New functions

        /**
        * Execute dialog (Internal to NPDLIB).
        *
        * @return return code of CEikDailog::ExecuteLD.
        * @internal
        */
        IMPORT_C TInt ExecuteLD();

    protected: // Constructors

        /**
        * Second phase constructor.
        *
        * @param aReader resource reader already opened for 
        *   one of "STRUCT NOTEPAD_LIST_DIALOG", 
        *   "STRUCT NOTEPAD_EDITOR_DIALOG", or "STRUCT NOTEPAD_VIEWER_DIALOG".
        */
        void ConstructL(TResourceReader& aReader);

    protected: // New functions

        /**
        * Set text of title pane.
        * If aTitle is NULL, no operation is done (it never leaves nor panics).
        *
        * @param aTitle title string.
        */
        void SetTitleL( const TDesC* aTitle );

        /**
        * Set text of title pane.
        *
        * @param aFile file handle.
        */
		void SetFileHandleL(RFile& aFile);

        /**
        * Show confirmation query with specified prompt and return the answer.
        *
        * @param aPromptResId resource id for the confirmation prompt.
        * @return ETrue if query confirmed Yes, otherwise EFalse.
        */
        TBool ExecuteConfirmationQueryL(TInt aPromptResId);

        /**
        * Show confirmation query with specified prompt and return the answer.
        *
        * @param aPrompt prompt text for the confirmation.
        * @return ETrue if query confirmed Yes, otherwise EFalse.
        */
        TBool ExecuteConfirmationQueryL(TDesC* aPrompt = NULL);


        /**
        * Send text using SendUi.
        *
        * @param aSendUi CSendUi object.
        * @param aCommandId, command id of MTM.
        * @param aDesC text to send.
        * @param aPlainText CPlainText to send.
        * @param aFileName file name to send.
        */
        void SendAsL(
            CSendUi& aSendUi, 
            TInt aCommandId, 
            const TDesC* aDesC,
            const CPlainText* aPlainText = NULL,
            const TDesC* aFileName = NULL );

        /**
        * Send text using SendUi.
        *
        * @param aSendUi CSendUi object.
        * @param aCommandId, command id of MTM.
        * @param aDesC text to send.
        * @param aPlainText CPlainText to send.
        * @param aFileName file name to send.
        */
        void SendAsL(
            CSendUi& aSendUi, 
            TInt aCommandId, 
            TBool aFileExist,
            const TDesC* aDesC,
            const CPlainText* aPlainText = NULL );

        /**
        * Send items using SendUi.
        *
        * @param aSendUi CSendUi object.
        * @param aModel CNotepadModel object.
        * @param aCommandId, command id of MTM.
        * @param aKeys a key array of items to send.
        */
        TUid SendByKeysL(
            CSendUi& aSendUi, 
            TInt aCommandId, 
            CNotepadModel& aModel,
            const RArray<TInt>& aKeys );

        /**
        * Check this dialog if waiting.
        *
        * @return ETrue if this is a waiting dialog, otherwize EFalse.
        */
        inline TBool IsWaiting() const;

        /**
        * Check whether this dialog is for Notepad application.
        *
        * @return ETrue if this is for Notepad, otherwize EFalse.
        */
        inline TBool IsNotepad() const;

        /**
        * Check whether this dialog is for Templates application.
        *
        * @return ETrue if this is for Templates, otherwize EFalse.
        */
        inline TBool IsTemplates() const;

        /**
        * Check whether this dialog is modeless.
        *
        * @return ETrue if this is a modeless dialog, otherwize EFalse.
        */
        inline TBool IsModeless();
        
        /**
        * Check whether this dialog is for Notepad application ListDialog.
        *
        * @return ETrue if this is for for Notepad application ListDialog, otherwize EFalse.
        */
        inline TBool IsNoteListDialog() const;

        /**
        * Set iTitle text to title pane.
        * 
        */
        void RefreshTitleL();


        /**
        * Insert 'Send' menu item to aMenuPane just 
        * after aCommandId.
        * 
        * @param aSendUi CSendUi object.
        * @param aMenuPane menu pane to insert.
        * @param aCommandId new menu item is added after the position of this.
        */
        void InsertSendMenuItemAfterL(
            CSendUi& aSendUi,
            CEikMenuPane& aMenuPane, 
            TInt aCommandId );

        /**
        * For MCE, delayed exit.
        *
        * @return ETrue If delayed exit mode.
        */
        inline TBool IsExitDelayedBySendUi() const;

    protected:  // Functions from base classes

        /**
        * From CCoeControl.
        */
        void ActivateL();

        /**
        * From MEikCommandObserver
        * 
        * @param aCommandId command.
        */
        void ProcessCommandL(TInt aCommandId);

        /**
        * From MEikDialogPageObserver.
        * Create customized ListBox. 
        *
        * @param aControlType id of dialog line.
        * @return line control info.
        */
        SEikControlInfo CreateCustomControlL(TInt aControlType);

    private: // New functions

        /**
        * Send a file as an attachment.
        *
        * The MTM specified by aCommandId must have capability to send 
        * a file as attachment.
        *
        * @param aSendUi CSendUi object.
        * @param aCommandId a command id of send menu item.
        * @param aFileName a name of the file to be sent.
        */
        void SendFileAsAttachmentL( 
            CSendUi& aSendUi, 
            TInt aCommandId, 
            const TDesC& aFileName );

        /**
        * Send a file as an attachment.
        *
        * The MTM specified by aCommandId must have capability to send 
        * a file as attachment.
        *
        * @param aSendUi CSendUi object.
        * @param aCommandId a command id of send menu item.
        * @param aFileName a name of the file to be sent.
        */
        void SendFileAsAttachmentL( 
            CSendUi& aSendUi, 
            TInt aCommandId, 
            RFile& aFile );

        /**
        * For cleaning up the ENotepadSendUiRunning flag.
        */
        static void CleanupSendUiFlag(TAny *aSelf);

        /**
        * Reserved API entry.
        *
        * @internal
        */
        IMPORT_C void CNotepadDialogBase_Reserved();

    protected:  // Data

        TInt iResId; // dialog ResourceId
        CEikDialog** iSelfPtr;
        TBool iMskFlag;

    private:  // inner class

        /**
        * CTemporaryFiles is a class for temprary files.
        *
        * We must not only close but also 
        * 'Delete' the temorary files even when some function LEAVES while 
        * using it.
        */
        class CTemporaryFiles : public CDesC16ArrayFlat
            {
            public: //constructor, destructor
                CTemporaryFiles(TInt aGranularity, CEikonEnv& aEikonEnv);
                /**
		        * Destructor.
		        */
                virtual ~CTemporaryFiles();
            public: // new function
            /*
			* CNotepadDialogBase::CTemporaryFiles::AppendL
			
			* if you need some code conversion....
			* CPlainText::TImportExportParam param;
			* param.iForeignEncoding = KCharacterSetIdentifierUtf8; // target encoding
			* CPlainText::TImportExportResult result;
			* text->ExportTextL(0, stream, param, result);
			*/
                void AppendL(const CPlainText& aText);
            private: // data
                CEikonEnv& iEnv; // not own
            public:
                TBuf16<150> iSendFileName;
            };
    public:
		/**
        * CCreateAndAppendFiles is an active object to create and append
        * attachments.
        */
        NONSHARABLE_CLASS(CCreateAndAppendFiles) : public CActive, public MProgressDialogCallback
            {
            public: // constructor, destructor
            
            	/**
		        * CCreateAndAppendFiles* NewL
		        * @param aReader resource reader already opened for 
		        */
                static CCreateAndAppendFiles* NewL(
					CSendUi& aSendAppUi, 
					TInt aCommandId, 
					CNotepadModel& aModel,
					RArray<TInt> aKeys,
					CEikonEnv& aEikonEnv,
					TInt& aBaseFlags,
					CNotepadDialogBase* aNpdDlgBase,
					TUid aServiceUId);
              /**
		        * CCreateAndAppendFiles
		        * @param aReader resource reader already opened for 
		        */ 
              CCreateAndAppendFiles(
					CSendUi& aSendAppUi, 
					TInt aCommandId, 
					CNotepadModel& aModel,
					//RArray<TInt> aKeys,
					CEikonEnv& aEikonEnv,
					TInt& aBaseFlags,
					CNotepadDialogBase* aNpdDlgBase,
					TUid aServiceUId);
           		/**
		        * Second phase constructor.
		        * @param aReader resource reader already opened for 
		        */
                void ConstructL(RArray<TInt> aKeys);
                
                /**
		        * Destructor.
		        */
                 ~CCreateAndAppendFiles();
           		/**
		        * Creates CAknProgressDialog object and creates a progress bar
		        */
				void ShowProgressBarL();
				
				/**
		        * Deletes the CAknProgressDialog object if it is already created.
		        */
				void EndProgressDialog();
				
				/**
		        * Attaches the selected notes to the message asynchronously.
		        */
				void StartAttachingL();
				
				/**
		        * Call back function called when Dialog is dismissed.
		        */
				void DialogDismissedL( TInt aButtonId );
                void RunL();
                void DoCancel();
                
            private:
				/**
		        * release memory allocated in the phase of appending Notes to Messaging.
		        */                
                void ReleaseMemory();
                
    private: // data
				CSendUi& iSendAppUi;
				TInt iCommandId;
				CNotepadModel& iModel;
				RArray<TInt> iKeys;
				TInt iTotalCount;
				CNotepadDialogBase::CTemporaryFiles* iFiles;
				CPlainText* iText;
				TInt iCount;
				TBool iAttaching;
				TBool iCanceled;
				CEikonEnv& iEnv;
				TInt& iBaseFlags;
				CAknProgressDialog* iProgressBar;
				CMessageData* imessageData ;
				RFs iFs;
				CArrayFixFlat<RFile> *array;
				RFile ifile1;
				TUid iServiceUId;
				CNotepadListDialog *iListDialog; //Not owned 

            };

    private:  // Data

        enum TNotepadDialogBaseFlags
            {
            ENotepadSendUiRunning = 0x01,
            ENotepadSendUiDelayedExit = 0x02
            };

        TDesC* iTitle; // own
        HBufC* iSavedTitle; // own (copied)
        CAknTitlePane* iTitlePane; // not own (cached)
        RFile iFile;

        TInt iBaseFlags;
       public:
       
        CNotepadDialogBase::CCreateAndAppendFiles* iCreateFile;
    };

#include "NpdDialogBase.inl"
#endif // NPDDIALOGBASE_H

// End of File
