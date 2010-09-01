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
*  Description : class declaration of CCalenAttachmentModel 
 *
*/

#ifndef CALENATTACHMENTMODEL_H
#define CALENATTACHMENTMODEL_H

#include <apgcli.h>
#include <calentry.h>
#include "calenattachmentinfo.h"

class CGulIcon;
class CCalenAttachmentInfo;
class CFbsBitmap;

/**
 * Attachment model observer.
 */
class MCalenAttachmentModelObserver
    {
    public:
    
        enum TCalenAttachmentCommand
            {
            ECalenAttachmentAdded = 0,
            ECalenAttachmentRemoved
            };

    public:

        /**
        * This function that is called when attachment model adds or removes
        * attachments from its list.
        *
        * @param aCommand 
        */
        virtual void NotifyChanges( TCalenAttachmentCommand aCommand ,
                                    TInt aAttachmentIndex) = 0;
        
        /**
        * This function that is called when attachment model needs to
        * get access to an attachment file
        * 
        * @param aIndex Index of the attachment for which file handle is requested
        * @return (read-only) file handle
        */
        virtual RFile GetAttachmentFileL(TInt aIndex) = 0;
    };

/**
 * Attachment model.
 */
class CCalenAttachmentModel : public CBase
    {
    public:

        /**
         * First phase constructor
         * 
         * @return returns pointer to CCalenAttachmentModel
         */
        IMPORT_C static CCalenAttachmentModel* NewL();

        /**
         * Destructor.
         */
        ~CCalenAttachmentModel();
        
    private:

        /**
         * Default Constructor.
         */
        CCalenAttachmentModel();

        /**
         * 2nd phase constructor.
         */
        void ConstructL();        

    public:
        
        /**
         * Sets attachment model observer.
         * 
         * @param aObserver pointer to the MCalenAttachmentModelObserver
         */
        IMPORT_C void SetObserver(MCalenAttachmentModelObserver* aObserver);
        
        /**
         * Returns number of attachments in the internal array of attachments.
         * 
         * @return TInt number of attachments
         */
        IMPORT_C TInt NumberOfItems() const;

        /**
         * Returns reference to attachment info at given index.
         * 
         * @param aIndex index for which attachment info is needed
         * @return return attachment info
         */
        IMPORT_C CCalenAttachmentInfo& AttachmentInfoAt(TInt aIndex) const;

        /**
         * Adds attachment to the internal array. Notifies attachment model
         * observer with ECalenAttachmentAdded parameter.
         * 
         * @param aFullName file name of the attachment to be added
         */
        IMPORT_C void AddAttachmentL(const TDesC& aFullName, const TDesC& aSystemFileName);

        /**
         * Adds attachment to the internal array. Notifies attachment model
         * observer with ECalenAttachmentAdded parameter.
         * 
         * @param aFullName file name of the attachment to be added
         * @param aSize size of the attachment file
         * @param aFetched fetched or not
         * @param aDataType mime type of the attachment
         */
        IMPORT_C void AddAttachmentL( const TDesC& aFullName, const TDesC& aSystemFileName, TInt aSize,
                   TBool aFetched, const TDataType& aDataType, RFile &aFile);
        
        /**
         * Deletes attachment from internal array. Notifies attachment model
         * observer with ECalenAttachmentRemoved parameter.
         * 
         * @param aIndex index for which attachment to be deleted
         */
        IMPORT_C TBool DeleteAttachment(TInt aIndex);

        /**
         * Reset the attachment model (empties the internal array).
         */
        IMPORT_C void Reset();

        /**
         * Loads resources, i.e. bitmaps & layouts
         */
        IMPORT_C void LoadResourcesL();

        /*
         * Get file handle from "model observer"
         * 
         * @param aIndex index of the attachment info
         * @return RFile returns the file handle
         */
        IMPORT_C RFile GetAttachmentFileL( TInt aIndex );
        
        /**
         * Get the attachment lists
         * 
         * @param aAttachmentList fills the attachment list
         */
        IMPORT_C void GetAttachmentListL(
                    RPointerArray<CCalenAttachmentInfo> &aAttachmentList);
        
        /**
         * Returns pointer to bitmap of the application that handles given file.
         * 
         * @param aAttaInfo reference to the attachment info
         * @return returns CGulIcon
         */
        IMPORT_C CGulIcon* BitmapForFileL(CCalenAttachmentInfo& aAttaInfo);
        
        /**
         * Updates attachment mode with the attahcments of entr if it has any
         * 
         * @param   None
         * @return  None
         */
        IMPORT_C void CheckForExistingAttachmentsL(CCalEntry* entry);
        
        /**
         * Set the flag to ETrue if the attachment(Photo, audio and vedio files) is open.
         * 
         * @param   aEmbeddedFileOpened, set to ETrue when attachment(Photo, audio and vedio files) is Open
         * @return  None
         */
        IMPORT_C void AttachmentOpen(TBool& aEmbeddedFileOpened);
        
        /**
         * return ETrue if the atacchment is opened.
         * 
         * @param   None
         * @return  None
         */
        IMPORT_C TBool IsAttachmentOpen();
        
    private:

        /**
         * Returns pointer to bitmap of given application.
         * 
         * @param aAppUid UID of the application for which bitmap is requested
         * @return
         */
        CGulIcon* BitmapForAppL( const TUid &aAppUid );

        /**
         * Creates new attachment info object.
         * 
         * @param aFileName filename of the attachment
         * @param aSize size of the attachment
         * @param aFetched attacment fetched or not
         * @param aDataType mime type of the attachment
         * @return
         */
        CCalenAttachmentInfo* CreateNewInfoL( const TDesC& aFileName,
                                              const TDesC& aSystemFileName,
                                              TInt aSize,
                                              TBool aFetched,
                                              const TDataType& aDataType,
        const CCalenAttachmentInfo::TCalenAttachmentStoreType aStoreType);
        
        /**
         * Deletes attachment from given index.
         *
         * @param aIndex Index of the deleted attachment.
         */
        TBool DoDeleteAttachmentL( TInt aIndex );
        /**
         * @brief Checks if the total attachment size plus the new file size exceeds 1MB .
         *
         * @param aNewFileSize Size of the selected attachment.
         */
        TBool CheckFileSizeLimitL( TInt aNewFileSize );
        
    private:
        RPointerArray<CCalenAttachmentInfo> iAttachInfoArray;
        MCalenAttachmentModelObserver* iAttachmentModelObserver;
        RApaLsSession iAppList;
        TSize iIconSize;
        TBool isAttachmentModelCleared; // Flag to tell if attachment model is reset. If yes, then only it will be populated again
        TBool iEmbeddedFileOpened;
    };

#endif // CALENATTACHMENTMODEL_H

// End of File