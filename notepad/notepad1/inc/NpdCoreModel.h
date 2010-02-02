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
* Description:  Declaration of core part of Notepad Model class.
*
*/


#ifndef NPDCOREMODEL_H
#define NPDCOREMODEL_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>
#include <badesca.h>
#include <d32dbms.h>

// CONSTANTS
_LIT(KNotepadSqlDeleteHead, "DELETE FROM Table1 WHERE key="); //39 chars
const TInt KNotepadSqlDeleteHeadSize(49); // %d costs 10 chars in max

// FORWARD DECLARATIONS
class CFileStore;
class CDaUserColSet;
class CEikonEnv;
class CPlainText;

// CLASS DECLARATION
/**
* Core part of the Notepad Engine.
* CNotepadCoreModel is a core part of Notepad engine.
* This class manages a DBMS session.
*  
* @lib NpdLib.lib
* @see CNotepadModel, CNotepadTableModel
*/
class CNotepadCoreModel : public CBase
    {
    public:
        enum TNotepadModelRowResult
            {
            ENotepadModelRowNop = 0,
            ENotepadModelRowDeleted,
            ENotepadModelRowAdded,
            ENotepadModelRowUpdated
            };
        
    public:  // Constructors and destructor

        /**
        * Default c++ constructor.
        */
        CNotepadCoreModel();

        /**
        * second phase constructor.
        *
        * @param aResId resource id of data file name.
        */
        void ConstructL(TInt aResId);

        /**
        * Destructor.
        *
        */
        virtual ~CNotepadCoreModel();

    public: // New functions

        /**
        * Return contents of a specified key.
        *
        * Caller own a returned object.
        *
        * @param aKey unique key id of Notepad.
        * @return text content of specified item.
        */
        HBufC* ContentByKeyL(const TInt aKey);

        /**
        * Return the number of items in the database.
        *
        * @return Count of items.
        */
        TInt CountL();

        /**
        * Insert a text to the database as a new row.
        * If a given text is regarded as empty, nothing is done.
        *
        * @param aDes descripter to save. if NULL, aText will be used.
        * @param aText text to save. if NULL, aDes will be used.
        * @return ENotepadModelRowNop if a given text is regarded as empty,
        *  ENotepadModelRowAdded if really added.
        */
        TInt InsertL(const TDesC& aDes, TInt& aKey);

        /**
        * Update an entry.
        * If a given text is regarded as empty, the row is removed.
        * If a given text is regarded as unchanged, nothing is done.
        * 
        * @param aText text to overwrite.
        * @param aKey entry key to update.
        * @return ENotepadModelRowNop if a given text is regarded as unchanged,
        *  ENotepadModelRowDeleted if a given text is regarded as empty, and
        *  ENotepadModelRowUpdated if really updated.
        */
        TInt UpdateL(const TDesC& aDes, TInt& aKey);

        /**
        * Delete a item by Key.
        *
        * @param aKey key to delete. 
        * @return If the key were not found, return ENotepadModelRowNop.
        */
        TInt DeleteL(TInt aKey);

        /**
        * Make compaction of the database.
        *
        * @return the return value of RDbDatabase::Compact.
        */
        TInt Compact();

        /**
        * Sets the iModOp when a note is inserted/deleted/updated.
        *
        * @param aModeOp Insert/Delete/Update Operation.
        */
        void SetModeOp(CNotepadCoreModel::TNotepadModelRowResult aModeOp);

    protected: // New functions

        /**
        * Read text content of aKey into aDes.
        * 
        * @param aDes buffer for label.
        * @param aKey key id to read.
        */
        void AppendContentAsLabelL(TDes& aDes, const TInt aKey);

        /**
        * Read text content into aDes.
        * 
        * @param aDes buffer for label.
        * @param aDbView RDbView already opened for GetL.
        */
        void AppendContentAsLabelL(TDes& aDes, RDbView& aDbView);

        /*
        * Ask if the model is running for Templates.
        */
        inline TBool IsTemplates() const;

        /*
        * Returs the value of iModOp.
        *
        * @return value of the operation.
        */
        CNotepadCoreModel::TNotepadModelRowResult ModeOp();

        /**
        * Updates the ItemArray and the KeyArray when a particular
        * Note is either Inserted or Deleted or Updated.
        *
        * @param aItemArray Array of items to be displayed in the listbox.
        * @param aKeyArray key array.
        * @param aTimeFormat Time format.
        */
		void DoUpdateNotepadItemArrayAndKeyArrayL(CDesCArray& aItemArray,RArray<TInt>& aKeyArray,HBufC& aTimeFormat);

    private:  // New functions

        /**
        * Open (or create) a database.
        *
        * If already opened, do nothing.
        * If there is no datafile, create a new datafile.
        *
        * @internal
        */
        void OpenDatabaseL();

        /**
        * Create and initialize database file.
        *
        */
        void CreateDataFileL();

        /**
        * Initialize database file.
        *
        */
        void InitDataFileL();

        /**
        * Insert factory setting of templates.
        *
        */
        void SetDefaultTemplatesL();

        /**
        * Create database table definition.
        *
        * @return created table column definition.
        */
        CDbColSet* CreateDbColSetLC() const;

        /**
        * Recover DB and Leave.
        *
        */
        void Rollback();

        /**
        * Common part of NewRowL and UpdateRowL.
        *
        * @param aItemIndex Update if aItemIndex>=0, otherwize Insert.
        * @param aText content.
        */
        TInt InsertOrUpdateL(const TDesC& aDes, TInt& aKey);

        /**
        * Delete database file.
        *
        */
        void DeleteFileL();

        /**
        * Check the database table.
        *
        * @return ETrue if incorrect table, otherwise EFalse.
        */
        TBool IsInvalidTableL();

        /**
        * Gets the Index where the new/updated note needs to be inserted.
        *
        * @return Index where the note needs to be inserted.
        */
		TInt GetInsertionIndex(const TDesC& aDes,
								CDesCArray& aItemArray);

    protected:  // Data

        enum TNotepadColumnType
            {
            ENotepadKey = 1, // this must be 1 (DBMS/SQL restriction)
            //ENotepadUpdateDate,
            ENotepadUpdateTime,
            ENotepadMemo,
            ENotepadColCountPlusOne // sentinel
            };

        RDbNamedDatabase iDatabase;
        CEikonEnv* iEnv; // cache

    private:     // Data
        TInt iDataResourceId;
        HBufC* iDataFile; // own
        RDbs iDbs;
       	TNotepadModelRowResult iModeOp;
       	TInt iKey;
       	TTime iNpdItemLastChanged;
    };

#include "NpdCoreModel.inl"
#endif      // NPDCOREMODEL_H

// End of File
