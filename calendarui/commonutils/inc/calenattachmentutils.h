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
*  Description : attachment utils apis for fetching the attachments from other
*              : apps
 *
*/

#ifndef CALENATTACHMENTUTILS_H
#define CALENATTACHMENTUTILS_H

#include <e32std.h>
#include <e32base.h>
#include <apparc.h>
#include <badesca.h>
#include <AknServerApp.h>
#include <MediaFileTypes.hrh>
#include <MMGFetchVerifier.h>
#include <MAknFileFilter.h>
#include <MAknFileSelectionObserver.h>
#include <AknWaitNoteWrapper.h>
#include <ConeResLoader.h>
#include "calenattachmentmodel.h"

class CAiwGenericParamList;
class CCoeEnv;

/**
 * Attachment utils used for attachment support 
 */
class CCalenAttachmentUtils : public CBase
    {
    public:
        enum TCalenAttachmentFileType
            {
            EUnknown = 0,
            EImage,
            ENote,
            EAudio,
            EVideo
            };
    public:

        /**
         * Fetches an attachment of given type from external application.
         * 
         * @param aType Fetch type.
         * @param aSelectedFiles  OUT: array of selected files.
         * @param aParams Generic AIW parameters
         * @param aCheckDiskSpace 
         * @param aMultiSelect if ETrue Media Gallery allows multiselect in file list.
         * @param aVerifier To verify selection.       
         * @return EFalse if fetch canceled.
         */
        IMPORT_C static TBool FetchFileL( TCalenAttachmentFileType aType,
                                          CDesCArray& aSelectedFiles,
                                          CAiwGenericParamList* aParams,
                                          TBool aCheckDiskSpace = EFalse,
                                          TBool aMultiSelect = EFalse,
                                          MMGFetchVerifier* aVerifier = NULL );

        /**
         * Fetches an any attachment file from external application.
         * 
         * @param aFileName OUT: filename of the fetched file.
         * @param aFilter To filter shown files.
         * @param aVerifier To verify selection.       
         * @return EFalse if fetch canceled.
         */
        IMPORT_C static TBool FetchAnyFileL( TFileName& aFileName,
                                  MAknFileSelectionObserver* aVerifier = NULL,
                                  MAknFileFilter* aFilter = NULL );

        /**
         * Returns temp path used by calendar unified editor and optionally
         * appends file name to it.
         * 
         * @param aTempPath  generated temp path and filename.
         * @param aFileName  optionally appended file name.
         */
        IMPORT_C static void GetCalenEditorTempPath( TFileName& aTempPath,
                                               const TDesC* aFileName = NULL );
        
        /**
         * Gets mime type of the given file handle
         * 
         * @param RFile File handle
         * @return TDataType returns file's mime type
         */
        IMPORT_C static TDataType GetMimeType(RFile aFile);
        
        /**
         * Gets mime type of the given file name
         * 
         * @param aFileName name of the file
         * @return TDataType returns file's mime type
         */
        IMPORT_C static TDataType GetMimeTypeL(const TDesC& aFileName);
        
        /**
         * Gets file size in bytes based on the file name 
         * 
         * @param aFileName name of the file of which size is required
         * @return TInt returns file size in bytes
         */
        IMPORT_C static TInt GetFileSizeL(const TDesC& aFileName);
        
		/**
         * Opens the attachment  
         * 
         * @param RFile	File handler to be opened
		 * @param aExitObserver	Exit observer that listens for exiting from the attahcment viewer
         * @return None
         */
        IMPORT_C static void OpenAttachmentL(RFile& file, MAknServerAppExitObserver& aExitObserver);
        
		/**
         * Saves attachment either to the phone memory or memory card as chosen by the user
         * 
         * @param RFile	File handler to be saved
		  * @param aExitObserver	Exit observer that listens for exiting from the save UI dialogs
         * @return TInt returns Error code if any
         */
        IMPORT_C static TInt SaveAttachmentL(RFile& aFile, MAknServerAppExitObserver& aExitObserver);
        
    private:
        
        /**
         * Fetches notes from notepad app
         * 
         * @aParam aSelectedFiles has selected files
         */
        static TBool FetchNoteL(CDesCArray& aSelectedFiles);
        
        /**
         * Fetch the file which is already existing
         * 
         * @param aFileType Type of the file to be fetched 
         * @param aSelectedFiles selecetd files after fetching
         * @param aMultiSelect for selecting multile files/or single file
         * @param aVerifier verifier for call back if file is selected
         */     
        static TBool FetchExistingL( TMediaFileType aFileType,
                                     CDesCArray& aSelectedFiles,
                                     TBool aMultiSelect,
                                     MMGFetchVerifier* aVerifier );
        
        /**
         * Generates a legal filename from given text buffer.
         * 
         * @param aFileName  OUT: generated filename.
         * @param aBuffer    text buffer.
         * @param aMaxLength maximum length for filename.
         * @param aExt       optional file extension.
         */
        static void GetFileNameFromBuffer( TFileName& aFileName,
                                           const TDesC& aBuffer,
                                           TInt aMaxLength,
                                           const TDesC* aExt = NULL );
    };

#endif