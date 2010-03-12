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
*  Description :
 *
*/

#ifndef CALENATTACHMENTINFO_H
#define CALENATTACHMENTINFO_H

#include <apmstd.h>
#include <msvstd.h>

class CGulIcon;
class CCalenAttachmentModel;

// ========== CONSTANTS ====================================

// Farsi takes 14 bytes when attachment has > 1000 bytes
const TInt KCalenAttaSizeStringLength = 32;
// One KiloByte
const TInt KLocalKilo = 1024;

/**
* Attachment information 
*/
class CCalenAttachmentInfo : public CBase
    {
    public:
    
    enum TCalenAttachmentStoreType
        {
        ECalenNewAttachment,
        ECalenAttachmentFetchedFromEntry
        };
        
        /**
        * First phase construction
        * 
        * @param aAttachmentModel reference to the CCalenAttachmentModel
        * @param aFileName filename of the attachment
        * @param aSize size of the attachment
        * @param aFetched attachment fetched or not
        * @param aDataType mime type of the attachment
        * @param aStoreType storetype of the attachment info
        * @return returns pointer to the CCalenAttachmentInfo
        */
        IMPORT_C static CCalenAttachmentInfo* NewL( CCalenAttachmentModel& aAttachmentModel,
                                         const TFileName& aFileName,
                                         const TFileName& aSystemFileName,
                                         TInt aSize,
                                         TBool Fetched,
                                         const TDataType& aDataType,
                                         const TCalenAttachmentStoreType aStoreType );
        
        /**
        * Destructor.
        */
        ~CCalenAttachmentInfo();

    private:

        /**
        * Default Constructor.
        * 
        * @param aAttachmentModel reference to the CCalenAttachmentModel
        * @param aFileName filename of the attachment
        * @param aSize size of the attachment
        * @param aFetched attachment fetched or not
        * @param aDataType mime type of the attachment
        * @param aStoreType storetype of the attachment info
        */
        CCalenAttachmentInfo( CCalenAttachmentModel& aAttachmentModel,
                              const TFileName& aFileName,
                              const TFileName& aSystemFileName,
                              TInt aSize,
                              TBool aFetched,
                              const TDataType& aDataType,
                              const TCalenAttachmentStoreType aStoreType);
        
        /**
        * 2nd phase constructor.
        */
        void ConstructL();
        
    public:      
        
        /**
        * Returns reference to the filename this attachment represents.
        * 
        * @return returns filename of the attachment
        */
        IMPORT_C const TFileName& FileName() const;
        
        /**
        * Set the filename of this attachment.
        * 
        * @param aFileName filename of the attachment
        */
        IMPORT_C void SetFileName( const TFileName& aFileName );

        /**
        * Returns the size of the attachment.
        * 
        * @return returns size of the attachmen
        */
        IMPORT_C TInt Size() const;
        
        /**
         * Returns the size of the atatchment in the form of a string to be displayed on the UI
         * 
         * @return  returns size string
         */
        IMPORT_C HBufC* SizeString() const;
        
        /**
        * Returns ETrue if this attachment is fetched.
        * @return
        */
        IMPORT_C TBool IsFetched() const;

        /**
        * Sets the fetched attribute of this attachment.
        * @param aFetched
        */
        IMPORT_C void SetFetched( TBool aFetched );

        /**
        * Returns the datatype (MIME type) of this attachment.
        * @return
        */
        IMPORT_C const TDataType& DataType() const;

        /**
        * Get icon of this attachment.
        * @return
        */
        IMPORT_C CGulIcon* Icon() const;
        
        /**
         * Get the store type whether attachment is fetched from the entry
         * or attachment is new one
         * 
         * @return TCalenAttachmentStoreType 
         * ECalenNewAttachment if attachment is new
         * ECalenAttachmentFetchedFromEntry if attachment is fetched from entry  
         */
        IMPORT_C TCalenAttachmentStoreType StoreType() const;
        
		/**
        * Returns reference to the systemfilename this attachment represents.
        * 
        * @return returns systemfilename of the attachment
        */
        IMPORT_C const TFileName& SystemFileName() const;
        /**
         * Performs icon updating.
         */
        void DoUpdateIconL();
        
        /**
         * Sets the file handle.
         * @param aFile File handle to be set
         * 
         */
        void SetFileHandle(RFile& aFile);
        
        /**
         * Gets the file handle.
         * @param File handle.
         * 
         */
        void FileHandle( RFile& aFile );
        
        /**
         * Check whether the file handle is set or not.
         * @return The isSetFileHandle
         * 
         */
        TBool IsFileHandleSet();
        

    private:
        
        CCalenAttachmentModel& iAttachmentModel;
        TFileName iFileName;
        TFileName iSystemFileName;
        TInt iSize;
        TDataType iDataType;        
        CGulIcon* iIcon;
        TBool iIsFetched;
        TCalenAttachmentStoreType iAttachmentStoreType;
		RFile iFile;
        TBool isSetFileHandle;
    };

#endif // CALENATTACHMENTINFO_H

