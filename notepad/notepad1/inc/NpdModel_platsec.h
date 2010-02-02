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
* Description:  Declaration of Notepad Model class.
*
*/


#ifndef NPDMODEL_PLATSEC_H
#define NPDMODEL_PLATSEC_H

// INCLUDES
#include <d32dbms.h>
#include "NpdCoreModel.h"
#include "NpdModelObserver.h"

// CONSTANTS
const TInt KNotepadModelInvalidIndex(-1);
const TInt KNotepadInvalidKeyId(-1);

// FORWARD DECLARATIONS
class RFs;
class MNotepadModelObserver;

// CLASS DECLARATION
/**
* Engine of NpdLib.
* CNotepadModel is the engine of Notepad Library.
* This class has an array of texts for listing data in a text list box.
*  
* @lib NpdLib.lib
* @see CNotepadCoreModel, CNotepadTableModel
*/
class CNotepadModel : public CNotepadCoreModel,
    public MDesCArray
    {
    public: // Constuctor and destructor
        enum TNotepadModelFlag
            {
            ENotepadRequireDbViewUpdate = 0x01,
            ENotepadRequireItemArrayUpdate = 0x02,
            ENotepadIsDeleting = 0x04,
            ENotepadIsRemovingLinks = 0x08,
            ENotepadItemArrayUpdateOnly = 0x10
            };

        /**
        * Constructor.
        *
        * @param aFs file server session.
        * @param aResId resource id of type STRUCT NOTEPAD_MODEL.
        * @return new CNotepadModel object.
        */
        IMPORT_C static CNotepadModel* NewL(RFs& aFs, TInt aResId);

        /**
        * Destructor.
        */
        IMPORT_C virtual ~CNotepadModel();

    public: // New functions

        /**
        * Save text as a memo.
        * API for SaveAsMemo.
        *
        * @param aText text to save
        */
        IMPORT_C void AddContentL(const TDesC& aText);

        /**
        * Open (or create) a datafile.
        *
        * If already opened, do nothing.
        * If there is no datafile, create a new datafile.
        *
        * @deprecated
        * @internal
        */
        IMPORT_C void OpenL();

        /**
        * Close a datafile.
        *
        * @deprecated
        * @internal
        */
        IMPORT_C void Close();

        /**
        * Return ETrue if it is opened now.
        *
        * @return ETrue if opened.
        * @internal
        * @deprecated
        */
        IMPORT_C TBool IsOpen();

        // Query

        /**
        * Return key UID of specified item.
        *
        * @return key of specified item.
        */
        inline TInt Key(TInt aItemIndex) const;

        /**
        * Return iKeyArray (for Editor to create a sequence).
        *
        * @return internal iKeyArray.
        */
        inline const RArray<TInt>& KeyArray() const;

        /**
        * Seek a specified key UID in item array.
        *
        * @param aKey key to seek.
        * @return 0 if found.
        */
        IMPORT_C TInt SeekKey(const TInt aKey);

        /**
        * Return content of the item.
        *
        * Caller own a returned object.
        *
        * @param aItemIndex target index.
        * @return text content of specified item.
        */
        HBufC* ContentL( TInt aItemIndex );

        /**
        * Return an index of the key.
        *
        * @param aKey unique key id.
        * @return index if found, KNotepadModelInvalidIndex if not found
        */
        TInt IndexOf(const TInt aKey) const;

        /**
        * Return ETrue if deleting process is running.
        * 
        * @return ETrue if iExecuter is running for deletion.
        */
        inline TBool IsDeleting() const;

        // Update

        /**
        * Refresh the text array.
        *
        * This function should be called if date has changed.
        */
        void HandleDateChangedL(TBool aSyncNow);

        /**
        * Start deleting process.
        * This method gives an incremental version of deletion.
        *
        * @param aKeys keys of items to be deleted.
        */
        void DeleteByKeysL(const RArray<TInt>& aKeys);

        /**
        * Cancel deleting process if running.
        */
        void CancelDeletion();

        /**
        * Sync iDbView, iItemArray and iKeyArray.
        *
        * @param aForceSync force sync.
        * @return ETrue if any change on iDbView, iItemArray or iKeyArray.
        */
        /*TBool*/ void SyncL( const TBool aForceSync );

        /**
        * Refresh the cached data accroding to changes of the database.
        *
        * This function is called from iDatabaseChangeNotifer.
        */
        void HandleDatabaseChangedL(TInt aStatus);

        /**
        * Set a Notepad model observer.
        *
        * @param aObserver an object which implements MNotepadModelObserver.
        */
        void SetNotepadModelObserver(MNotepadModelObserver* aObserver);

    public:  // Functions from base classes

        /**
        * From MDesCArray.
        * Return the number of items.
        */
        IMPORT_C TInt MdcaCount() const;

        /**
        * From MDesCArray.
        * Return the text of specified item.
        */
        IMPORT_C TPtrC MdcaPoint(TInt aIndex) const;

    private: // Constructors

        /**
        * Default C++ constructor.
        */
        CNotepadModel();

        /**
        * Second phase constructor.
        *
        * @param aResId resource id of type STRUCT NOTEPAD_MODEL
        */
        void ConstructL(TInt aResId);

    private: // New functions

        /**
        * Prepare to start deletion.
        * Create a SQL statement and set it on iDbUpdate.
        *
        * @param aKeys keys of items to be deleted.
        */
        void PrepareToDeleteByKeysL(const RArray<TInt>& aKeys);

        void ExecuteDeleteStepL();

        /**
        * Should be called if deleting process was aborted.
        */
        void AbortDeletionL();

        /**
        * Should be called if deleting process has completed.
        */
        void CompleteDeletionL();

        /**
        * Callback function while iExecuter is running for deletion.
        *
        * @param aSelf an object of this class.
        */
        static TInt DeleteCallBack(TAny *aSelf);

        /**
        * Callback function while iExecuter is running for deletion.
        */
        TInt DoDeleteCallBackL();

        /**
        * Callback function while iExecuter is running for removal of links.
        */
        TInt DoRemoveLinkCallBackL();

        /**
        * Create iItemArray and iKeyArray.
        */
        void BuildItemArrayL( const TBool aForceSync );

        /**
        * Implementation of BuildItemArrayL.
        *
        */
        void DoBuildItemArrayL();

        /**
        * Implementation of BuildItemArrayL when Notepad.
        *
        */
        void DoBuildNotepadItemArrayL(TPtr& aBuf);

        /**
        * Implementation of BuildItemArrayL when Tempaltes.
        *
        */
        void DoBuildTemplateItemArrayL(TPtr& aBuf);

        /**
        * Reserved.
        * @internal
        */
        IMPORT_C void CNotepadModel_Reserved();   
	
	public:

        /**
        * Sets the item array flags.
        *
        */
        void SetItemArrayFlags(); 

        /**
        * Retruns the item array flags.
        *
        */
		TUint ItemArrayFlags();

    private: // Inner class

        /**
        * Inner calss to observe changes of the database.
        *
        */
        class CDatabaseChangeNotifier : public CActive
            {
            public: // constructor, destructor
                static CDatabaseChangeNotifier* NewL(
                    RDbDatabase& aDatabase,
                    CNotepadModel& aModel );
                ~CDatabaseChangeNotifier();
            private: // constructor
                CDatabaseChangeNotifier(CNotepadModel& aModel);
                void ConstructL(RDbDatabase& aDatabase);
            private: // from CActive
                void DoCancel();
                void RunL();
            private: // data
                RDbNotifier iDbNotifier;
                CNotepadModel& iModel;
            };

    private: // Data

        TUint iFlag;
        RDbUpdate iDbUpdate;
        RArray<TInt> iSavedDeleteKeys;
        CNotepadModel::CDatabaseChangeNotifier* iDatabaseChangeNotifier; //own
        RDbView iDbView;
        CIdle* iExecuter; // own
        CDesCArray* iItemArray; // own
        RArray<TInt> iKeyArray; // map ItemIndex -> Key.
        MNotepadModelObserver* iModelObserver; // not own
        TInt iProgressCount; // for progress infomation callback
        TInt iListingStyle;
        HBufC* iDateFormat;
        HBufC* iTimeFormat;
        TInt iStepCount;
        RFs iFileSession;
        TInt iRetval;
    };

#include "NpdModel.inl"
#endif // NPDMODEL_PLATSEC_H

// End of File
