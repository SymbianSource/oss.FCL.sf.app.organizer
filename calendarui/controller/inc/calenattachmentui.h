/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:
*
*/

#ifndef CALENATTACHMENTUI_H
#define CALENATTACHMENTUI_H

#include <e32base.h>
#include <MMGFetchVerifier.h>
#include <MAknFileSelectionObserver.h>
#include <AknWaitNoteWrapper.h>
#include <ConeResLoader.h>
#include <calencommandhandler.h>
#include <calencommands.hrh>
#include <calennotificationhandler.h>

#include "calenattachmentutils.h"
#include "calenattachmentmodel.h"

class CCalenController;
class MCalenCommandHandler;
class CCalenViewAttachmentsDialog;
class TContentProtection;

#include <AknServerApp.h>
/**
 * Handles attachment related commands
 */
class CCalenAttachmentUi : public CBase,
                           public MCalenCommandHandler,
                           public MCalenNotificationHandler,
                           public MMGFetchVerifier,
                           public MAknFileSelectionObserver,
                           public MCalenAttachmentModelObserver,
                           public MProgressDialogCallback
    {
    public:
        
        /**
         * First phase constructor
         * 
         * @param aController A reference to calencontroller
         * @return pointer to CCalenAttachmentUi
         */
        static CCalenAttachmentUi* NewL( CCalenController& aController );
        
        /**
         * Virtual destructor
         */
        virtual ~CCalenAttachmentUi();

    private:
        
        /**
         * C++ constructor, non-leaving
         * 
         * @param aController A reference to the calencontroller
         */
        CCalenAttachmentUi(CCalenController& aController);
        
        /**
         * 2nd phase constructor
         */
        void ConstructL();
        
        void CheckDRMStatusL(const TDesC& aFileName,TBool& aProtection);
        
    public:
        
        /**
         * From MCalenCommandHandler
         * Handles the attachement commands
         * 
         * @param aCommand Command to be handled
         * @return returns whether command is successfully handled or not
         */
        TBool HandleCommandL(const TCalenCommand& aCommand);
        
        /**
         * From MCalenCommandHandler.
         * Allows extending this API without breaking BC.
         * 
         * @param aExtensionUid specifies
         * @return extension of the requested type
         */
        TAny* CalenCommandHandlerExtensionL( TUid aExtensionUid );
        
        /** 
         * From MCalenNotificationHandler
         * Handle notifications
         * 
         * @param aNotification Nofication to be handled
         */
        void HandleNotification(const TCalenNotification aNotification );
        
        /**
         * From MMGFetchVerifier
         * Interface for verifying fetch file selection
         * 
         * @param aSelectedFiles Holds the list of selected files
         * @return TBool returns whether file is selected or not
         */
        TBool VerifySelectionL(const MDesCArray* aSelectedFiles);
        
        /**
         * From MAknFileSelectionObserver
         * Get's called when user selects an file.
         * 
         * @param aDriveAndPath Drive and full path of the file.
         * @param aEntry An entry containing all information of selected item.
         * @return Returns ETrue if it is ok to exit from dialog.
         */
        TBool OkToExitL( const TDesC& aDriveAndPath, const TEntry& aEntry );
        
        /**
         * From MCalenAttachmentModelObserver
         * This function that is called when attachment model adds or removes
         * attachments from its list.
         *
         * @param aCommand TCalenAttachmentCommand whether attachment is added
         *                 or removed 
         * @param attachmentIndex index of the attachment info
        */
        void NotifyChanges( TCalenAttachmentCommand aCommand ,
                          TInt aAttachmentIndex );
        
        /**
         * From MCalenAttachmentModelObserver
         * This function that is called when attachment model needs to
         * get access to an attachment file
         * 
         * @param aIndex index of the attachment file
         * @return returns the file handle
         */
        RFile GetAttachmentFileL(TInt aIndex);
        
        /**
         * From MProgressDialogCallback
         * 
         * called when dialog is dismissed
         */
        void DialogDismissedL( TInt aButtonId );

    private:
        
        /**
         * Used to fetch the attachment
         *
         * @param aFetchType, EImage,ENote,EAudio,EVideo
         * @return TBool returns ETrue if file is fetched successfully
         * otherwise returns EFalse
         */
        TBool FetchFileL( 
                CCalenAttachmentUtils::TCalenAttachmentFileType aFetchType);
        
        /**
         * Handles ECalenAddAttachment command 
         */
        void HandleAddAttachmentL();
        
        /**
         * Check for esisting attachments from the calentry     
         */
        void CheckForExistingAttachmentsL();
        
        /**
         * Copy fetched files from gallery/notes in to temporary folder
         * 
         * @param aTempPath 
         * @param aSourceFilePath
         */
        void CopyFetchedFilesL( TFileName& aTempPath, const TDesC& aSourceFilePath);
       
        /**
         * Add an attachment based on the file path
         * 
         * @param aSourcepath path of the file being attached
         */
        void AddAttachmentL(const TDesC& aSourceFilePath);
        
        /**
         * Remove the attachment from the calendar entry

         * @param aAttachmentIndex index of the attachment to be rmeoved 
         */
        void RemoveAttachmentFromEntryL(TInt aAttachmentIndex);
        
        /**
         * Add attachments to the calendar entry
         */
        void AddAttachmentsToEntryL();
        
        /** 
         * Handle notifications
         * 
         * @param aNotification Nofication to be handled
         */
        void DoHandleNotificationL(const TCalenNotification aNotification );
        
        /**
         * Handles ECalenViewAttachmentList command 
         */
        void DoAttachmentL();
        
        /**
         * Show wait note
         */
        void ShowWaitNoteL( TBool aCanCancel );
        
        /**
         * Compares the names of attachment being alredy added to editor, and 
         * the other one which user is trying to add now.
         */
        TBool IsDuplicateNameL(const TDesC& aSelectedFile);
        
        /**
         * Compares the binary data for the text files.
         */
        TBool CompareContentOfTextFilesL(const TDesC& aSelectedFile,const TDesC& aAlreadyAttachedFile);
        
        /**
         * Remove the temporary file after attaching the attachtment to tht entry.
         */
        void RemoveTemporaryFilesL();
        
    private:
        
        // reference to the calencontroller
        CCalenController& iController;
        CCalenAttachmentModel* iAttachmentModel;
        //own: attachment dilalog title
        HBufC* iTitle;
        //Not owned
        CCalenViewAttachmentsDialog* iAttachmentdlg;
        
        // Internal Flag
        TBool iAttachmentLoadingOn;
        
        // Own: Wait dialog
        CAknWaitDialog* iWaitDialog;
        
        TInt iSelectedCommand;         //command used to differntiate between the type of attachment user is trying to add. 
        TBool iAddAttachmentFromViewer;//shows attachment got added from attachment list view.
        TBool iShowToolbar;            //To update the toolbar visiblity.
        TBool iViewerActive;           //To avoid the problem of status pane change.
        TBool iSFound;                 //To check wether name is duplicated or not.
        TBool iAttachmentAlreadyExists;
        TBool iAttachmentInfoIntialized;
    };

#endif // CALENATTACHMENTUI_H
