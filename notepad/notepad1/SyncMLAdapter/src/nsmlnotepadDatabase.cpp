/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  notepad database
*
*/


#include <sysutil.h>


#include <txtetext.h>
#include "nsmlnotepadDatabase.h"
#include <nsmlnotepaddatastoreformat.rsg>

#include "nsmlnotepadlogging.h"


// -----------------------------------------------------------------------------
// CNSmlNotepadDatabase::NewL
// -----------------------------------------------------------------------------
//
CNSmlNotepadDatabase* CNSmlNotepadDatabase::NewL()
	{
	_NOTEPAD_DBG_FILE("CNSmlNotepadDatabase::NewL(): begin");
	
	CNSmlNotepadDatabase *instance = new(ELeave) CNSmlNotepadDatabase();
	CleanupStack::PushL(instance);
	instance->ConstructL();
	CleanupStack::Pop(instance);
	
	_NOTEPAD_DBG_FILE("CNSmlNotepadDatabase::NewL(): end");
	return instance;
	}
// -----------------------------------------------------------------------------
// CNSmlNotepadDatabase::~CNSmlNotepadDatabase()
// -----------------------------------------------------------------------------
//
CNSmlNotepadDatabase::~CNSmlNotepadDatabase()
	{
	_NOTEPAD_DBG_FILE("CNSmlNotepadDatabase::~CNSmlNotepadDatabase(): begin");
	
    if(iOpened)
        {
    	iDatabase.Close();
        }
	
	if(iDataFile)
	{
		delete iDataFile;
		iDataFile = NULL;	
	}
	iTable.Close();	
	iDbs.Close();
	_NOTEPAD_DBG_FILE("CNSmlNotepadDatabase::~CNSmlNotepadDatabase(): end");
	}
// -----------------------------------------------------------------------------
// CNSmlNotepadDatabase::CNSmlNotepadDatabase()
// -----------------------------------------------------------------------------
//
CNSmlNotepadDatabase::CNSmlNotepadDatabase()
	{
	_NOTEPAD_DBG_FILE("CNSmlNotepadDatabase::CNSmlNotepadDatabase(): begin");
	
	iOpened = EFalse;
	
	_NOTEPAD_DBG_FILE("CNSmlNotepadDatabase::CNSmlNotepadDatabase(): end");
	}	
	
// -----------------------------------------------------------------------------
// CNSmlNotepadDatabase::ConstructL
// -----------------------------------------------------------------------------
//
void CNSmlNotepadDatabase::ConstructL()
	{
	_NOTEPAD_DBG_FILE("CNSmlNotepadDatabase::ConstructL: begin");
	
    User::LeaveIfError(iDbs.Connect());
    
    iDataFile = HBufC::NewL(KMaxFileLength);
    iDataFile->Des().Copy(KNpdDataFile);
	
    _NOTEPAD_DBG_FILE("CNSmlNotepadDatabase::ConstructL: end");
	}

// -----------------------------------------------------------------------------
// CNSmlNotepadDatabase::OpenL
// -----------------------------------------------------------------------------
//
void CNSmlNotepadDatabase::OpenL()
{
	_NOTEPAD_DBG_FILE("CCNSmlNotepadDatabase::OpenL(): begin");
	_NOTEPAD_DBG_FILE("CNSmlNotepadDatabase::OpenL(): end");
	OpenL(*iDataFile);
}
// -----------------------------------------------------------------------------
// TInt CNSmlNotepadDatabase::OpenL(const TDesC& aStoreName)
// -----------------------------------------------------------------------------
//
void CNSmlNotepadDatabase::OpenL(const TDesC& aStoreName)
    {
    _NOTEPAD_DBG_FILE("CNSmlNotepadDatabase::OpenL(): begin");
    
	TBuf<32> format;
    format.Copy(KSecure);
    format.Append(KSecureUid.Name());
    
	TInt err(iDatabase.Open(iDbs,aStoreName,format));
	RFs fsession; 
    switch ( err )
        {
        case KErrNone: // database open succeed
			iOpened = ETrue;
            break;
        case KErrArgument: // when the datafile is empty
        case KErrNotSupported: // UID mismatch, corrupted
        case KErrEof: // read error, corrupted
        case KErrCorrupt:
		    iDatabase.Close(); // indespensable for RFs::Delete()
		    User::LeaveIfError(fsession.Connect());
			fsession.Delete(*iDataFile);
			fsession.Close();
        case KErrNotFound:
			TRAP(err,CreateDBL());
			_NOTEPAD_DBG_FILE("CNSmlNotepadDatabase::OpenL(): after createDBL and before database open");
			if( err == KErrNone) 
				{
				iOpened = ETrue;
				}
			break;
         default:
            break;
        }
	_NOTEPAD_DBG_FILE("CNSmlNotepadDatabase::OpenL(): end");
	User::Leave(err);
    }



// -----------------------------------------------------------------------------
// CNSmlNotepadDatabase::AddNoteL
// -----------------------------------------------------------------------------
//
void CNSmlNotepadDatabase::AddNoteL(CNpdItem& aNote, TInt &aKey)
    {
    _NOTEPAD_DBG_FILE("CNSmlNotepadDatabase::AddNoteL(): begin");
    
	if(!iOpened)
		{
		User::Leave(KErrNotReady);
		}

    if(!aNote.Content())
        {
        User::Leave(KErrArgument);
        }

	TInt err(KErrNone);
    RDbView dbView;
    CleanupClosePushL(dbView);

    err =  dbView.Prepare(
            iDatabase, TDbQuery(KNpdSqlInsert) );

	if(err == KErrNone) 
		{
        TRAP(err,
	    		dbView.InsertL();
		    	dbView.SetColL(ENotepadUpdateTime, aNote.LastModified());
			    dbView.SetColL(ENotepadMemo, aNote.Content()->Des());
			    dbView.PutL();
                );
	    if (err == KErrNone)
		    {
            aKey = dbView.ColInt32(ENotepadKey);
	        } 
        else 
            {
			dbView.Close();
			Rollback();
            }
		}

    CleanupStack::PopAndDestroy(); // dbView
    iDatabase.Compact();
	
	_NOTEPAD_DBG_FILE("CNSmlNotepadDatabase::AddNoteL(): end");
    User::Leave(err);
    }

// -----------------------------------------------------------------------------
// CNSmlNotepadDatabase::DeleteNoteL
// -----------------------------------------------------------------------------
//
void CNSmlNotepadDatabase::DeleteNoteL(const TInt aKey)
    {
    _NOTEPAD_DBG_FILE("CNSmlNotepadDatabase::DeleteNoteL(): begin");
    
	if(!iOpened)
		{
			User::Leave(KErrNotReady);
		}

    TInt err(KErrNone);
    TBuf<KNpdSqlDeleteHeadSize> sql;
    sql.Append(KNpdSqlDeleteHead);
    sql.AppendNum(aKey);

    TInt rowCount( iDatabase.Execute(sql) );

    if(rowCount > 0)
		{
			err = KErrNone;
		}
	else
		{
			_NOTEPAD_DBG_FILE("CNSmlNotepadDatabase::DeleteNoteL(): item not found");
			err = KErrNotFound;
		}

	_NOTEPAD_DBG_FILE("CNSmlNotepadDatabase::DeleteNoteL(): end");
	User::Leave(err);
    }


// -----------------------------------------------------------------------------
// CNSmlNotepadDatabase::Close
// -----------------------------------------------------------------------------
//
void CNSmlNotepadDatabase::Close()
	{
	_NOTEPAD_DBG_FILE("CNSmlNotepadDatabase::Close(): begin");
	
	if(iOpened)
		{
		iDatabase.Compact();
	    iDatabase.Close();
	    iOpened = EFalse;
        }
        
	_NOTEPAD_DBG_FILE("CNSmlNotepadDatabase::Close(): end");        
	}
	
// -----------------------------------------------------------------------------
// CNSmlNotepadDatabase::GetNoteL
// -----------------------------------------------------------------------------
//
void CNSmlNotepadDatabase::GetNoteL(const TInt aKey, CNpdItem& aItem)
    {
    _NOTEPAD_DBG_FILE("CNSmlNotepadDatabase::GetNoteL(): begin");
    
	TInt err(KErrNone);

	if(!iOpened)
		{
		_NOTEPAD_DBG_FILE("CNSmlNotepadDatabase::GetNoteL(): not opened");
		User::Leave(KErrNotReady);
		}

    TBuf<KNpdSqlStringMaxLength> sql;
    sql.Format(KNpdSqlFormatSeek, aKey);

    RDbView dbView;
    CleanupClosePushL(dbView);
  
	User::LeaveIfError(dbView.Prepare(
        iDatabase, TDbQuery(sql), RDbView::EReadOnly ) );

    User::LeaveIfError(dbView.EvaluateAll());
 
	TInt isAtRow(dbView.FirstL());
    if ( isAtRow )
        {
        _NOTEPAD_DBG_FILE("CNSmlNotepadDatabase::GetNoteL(): if entered");
        HBufC* content = NULL;
        dbView.GetL();

		TInt key = dbView.ColInt32(ENotepadKey);
		TTime modified = dbView.ColTime(ENotepadUpdateTime);

        RDbColReadStream blob;
        blob.OpenLC(dbView, ENotepadMemo);
        TInt colLength( dbView.ColLength(ENotepadMemo) );
        content = HBufC::NewLC(colLength);
        TPtr ptr = content->Des();

        blob.ReadL(ptr, colLength);
		
		aItem.Set(key,modified, content);
        CleanupStack::Pop(content); // content
        CleanupStack::PopAndDestroy(); // blob
        }
	else 
		{
		err = KErrNotFound;
		}

    CleanupStack::PopAndDestroy(); // dbView/
    
    _NOTEPAD_DBG_FILE("CNSmlNotepadDatabase::GetNoteL(): end");
    User::Leave(err);
    }

// -----------------------------------------------------------------------------
// CNSmlNotepadDatabase::UpdateNoteL
// -----------------------------------------------------------------------------
//
void CNSmlNotepadDatabase::UpdateNoteL(const TInt aKey, CNpdItem& aItem)
	{
	TInt err(KErrNone);
	_NOTEPAD_DBG_FILE("CNSmlNotepadDatabase::UpdateNoteL(): begin");

	if(!iOpened)
		{
		_NOTEPAD_DBG_FILE("CNSmlNotepadDatabase::UpdateNoteL(): not ready");
		User::Leave(KErrNotReady);
		}

    TBuf<KNpdSqlStringMaxLength> sql;
    sql.Format(KNpdSqlFormatSeek, aKey);

    RDbView dbView;
    CleanupClosePushL(dbView);
  
	User::LeaveIfError(dbView.Prepare(
        iDatabase, TDbQuery(sql), RDbView::EUpdatable ) );

    User::LeaveIfError(dbView.EvaluateAll());
 
	TInt isAtRow(dbView.FirstL());
    if ( isAtRow )
		{
		
		TRAP(err,
			dbView.UpdateL();
			dbView.SetColL(ENotepadUpdateTime, aItem.LastModified());
			dbView.SetColL(ENotepadMemo, aItem.Content()->Des());
			dbView.PutL();
		);
	    if (err != KErrNone)
		    {
			dbView.Close();
			Rollback();
	        } 
		}
	else 
		{
		err = KErrNotFound;
		}

    CleanupStack::PopAndDestroy(); // dbView
    iDatabase.Compact();

	_NOTEPAD_DBG_FILE("CNSmlNotepadDatabase::UpdateNoteL(): end");
	User::Leave(err);
	}
// -----------------------------------------------------------------------------
// CNSmlNotepadDatabase::Reset
// -----------------------------------------------------------------------------
//
TInt CNSmlNotepadDatabase::Reset()
	{
	_NOTEPAD_DBG_FILE("CNSmlNotepadDatabase::Reset(): begin");
	
    TInt err(KErrNone);

	if(!iOpened)
		{
		return KErrNotReady;
		}

    TBuf<KNpdSqlDeleteHeadSize> sql;
    sql.Append(KNpdSqlDeleteAll);
 
	TInt rowCount( iDatabase.Execute(sql) );

    if(rowCount < 0)
        {
        err = rowCount;
        }
    
   	if (err == KErrNone && iDatabase.InTransaction())
    	{
	    err = iDatabase.Commit();
	    }
   
    if (err == KErrNone && rowCount > 0) 
        {
        err = iDatabase.Compact();
        }

    _NOTEPAD_DBG_FILE("CNSmlNotepadDatabase::Reset(): begin");
    return err;
	}
// -----------------------------------------------------------------------------
// CNSmlNotepadDatabase::IsOpen
// -----------------------------------------------------------------------------
//
TBool CNSmlNotepadDatabase::IsOpen()
	{
	_NOTEPAD_DBG_FILE("CNSmlNotepadDatabase::IsOpen(): begin");
	_NOTEPAD_DBG_FILE("CNSmlNotepadDatabase::IsOpen(): end");
    return iOpened;
    }
// -----------------------------------------------------------------------------
// void CNSmlNotepadDatabase::GetDefaultDatastoreName(TDes& aStoreName)
// -----------------------------------------------------------------------------
//    
void CNSmlNotepadDatabase::GetDefaultDatastoreName(TDes& aStoreName)
{
	_NOTEPAD_DBG_FILE("CCNSmlNotepadDatabase::GetDefaultDatastoreName(): begin");
	
	aStoreName.Copy(*iDataFile);
	
	_NOTEPAD_DBG_FILE("CNSmlNotepadDatabase::GetDefaultDatastoreName(): end");
} 
// -----------------------------------------------------------------------------
// CDesCArray* CNSmlNotepadDatabase::ListDatabasesL()
// -----------------------------------------------------------------------------
//
CDesCArray* CNSmlNotepadDatabase::ListDatabasesL()
{
	_NOTEPAD_DBG_FILE("CNSmlNotepadDatabase::ListDatabasesL(): begin");
	
	CDesCArrayFlat *arr = new (ELeave)CDesCArrayFlat(1);
	CleanupStack::PushL(arr);
	arr->AppendL(*iDataFile);
	CleanupStack::Pop(arr);
	
	_NOTEPAD_DBG_FILE("CNSmlNotepadDatabase::ListDatabasesL(): end");
	return arr;	
} 
// -----------------------------------------------------------------------------
// TInt CNSmlNotepadDatabase::FirstL()
// -----------------------------------------------------------------------------
//
TInt CNSmlNotepadDatabase::FirstL()
{
	_NOTEPAD_DBG_FILE("CNSmlNotepadDatabase::FirstL(): begin");
	TInt ret = -1;
	if(!iOpened) OpenL();
    iTable.Close();
	TInt err = iTable.Open(iDatabase,KNpdTableName,RDbRowSet::EReadOnly);
	if(err == KErrNone)
	{
		iTable.BeginningL();	
		if(iTable.FirstL())
		{
			_NOTEPAD_DBG_FILE("CNSmlNotepadDatabase::FirstL(): if firstL");		
			iTable.GetL();
			_NOTEPAD_DBG_FILE("CNSmlNotepadDatabase::FirstL(): after GetL");		
			ret = iTable.ColInt(1);	
		}
		_NOTEPAD_DBG_FILE("CNSmlNotepadDatabase::FirstL(): end");
	}
  	return ret;
}
// -----------------------------------------------------------------------------
// TInt CNSmlNotepadDatabase::NextL()
// -----------------------------------------------------------------------------
//
TInt CNSmlNotepadDatabase::NextL()
{
	_NOTEPAD_DBG_FILE("NSmlNotepadDatabase::NextL(): begin");
	
	TInt ret = -1;
	if(iTable.AtRow())
	{
		if(iTable.NextL())
		{
			iTable.GetL();
			ret = iTable.ColInt(1);	
		}
	}
	
	_NOTEPAD_DBG_FILE("NSmlNotepadDatabase::NextL(): end");
	return ret;
}
// -----------------------------------------------------------------------------
// TInt CNSmlNotepadDatabase::LastL()
// -----------------------------------------------------------------------------
//
TInt CNSmlNotepadDatabase::LastL()
{
	_NOTEPAD_DBG_FILE("CNSmlNotepadDatabase::LastL(): begin");
	
	TInt ret = -1;
	if(iTable.LastL())
	{
		iTable.GetL();
		ret = iTable.ColInt(1);	
	}
	
	_NOTEPAD_DBG_FILE("CNSmlNotepadDatabase::LastL(): end");
	return ret;	
}
  	
// -----------------------------------------------------------------------------
// CNSmlNotepadDatabase::CreateColSetLC
// -----------------------------------------------------------------------------
//
CDbColSet* CNSmlNotepadDatabase::CreateColSetLC()
    {
    _NOTEPAD_DBG_FILE("CNSmlNotepadDatabase::CreateColSetLC: begin");
    
    CDbColSet* columns = CDbColSet::NewLC(); 
    TDbCol col(KNpdKey, EDbColInt32);
    col.iAttributes = TDbCol::ENotNull | TDbCol::EAutoIncrement;
    columns->AddL(col);

    col.iName = KNpdUpdateTime;
    col.iType = EDbColDateTime;
    col.iAttributes = TDbCol::ENotNull;
    columns->AddL(col);

    col.iName = KNpdMemo;
    col.iType = EDbColLongText;
    col.iAttributes = TDbCol::ENotNull;
    columns->AddL(col);

	_NOTEPAD_DBG_FILE("CNSmlNotepadDatabase::CreateColSetLC: end");
    return columns; // columns stays on CleanupStack
    }

// -----------------------------------------------------------------------------
// CNSmlNotepadDatabase::CreateDBL
// -----------------------------------------------------------------------------
//
void CNSmlNotepadDatabase::CreateDBL()
	{
	_NOTEPAD_DBG_FILE("CNSmlNotepadDatabase::CreateDBL(): begin");
	
    iDatabase.Close();
	TBuf<32> format;
    format.Copy(KSecure);
    format.Append(KSecureUid.Name());
	User::LeaveIfError(iDatabase.Create(iDbs, *iDataFile,format));
	CDbColSet* columns = CreateColSetLC();
	User::LeaveIfError(iDatabase.CreateTable(KNpdTableName, *columns));
	CleanupStack::PopAndDestroy(columns); // columns
    User::Leave(KErrNone);
    _NOTEPAD_DBG_FILE("CNSmlNotepadDatabase::CreateDBL(): end");
	}

// -----------------------------------------------------------------------------
// CNSmlNotepadDatabase::Rollback
// -----------------------------------------------------------------------------
//
void CNSmlNotepadDatabase::Rollback()
    {
    _NOTEPAD_DBG_FILE("CNSmlNotepadDatabase::Rollback(): begin");
    
    iDatabase.Rollback();
    if ( iDatabase.IsDamaged() )
        {
        iDatabase.Recover();
        }
        
    _NOTEPAD_DBG_FILE("CNSmlNotepadDatabase::Rollback(): end");
    }






// -----------------------------------------------------------------------------
// CNpdItem implementations
// -----------------------------------------------------------------------------
//

// -----------------------------------------------------------------------------
// CNpdItem::~CNpdItem
// -----------------------------------------------------------------------------
//
CNpdItem::~CNpdItem()
	{
	_NOTEPAD_DBG_FILE("CNpdItem::~CNpdItem: begin");
	delete iContent;
	_NOTEPAD_DBG_FILE("CNpdItem::~CNpdItem: end");
	}
// -----------------------------------------------------------------------------
// CNpdItem::NewL
// -----------------------------------------------------------------------------
//
CNpdItem* CNpdItem::NewLC()
	{
	_NOTEPAD_DBG_FILE("CNpdItem::NewLC: begin");
	CNpdItem* tmp = new (ELeave) CNpdItem();
	CleanupStack::PushL(tmp);
	//tmp->ConstructL(0, TTime(), NULL);
	_NOTEPAD_DBG_FILE("CNpdItem::NewLC: end");
	return tmp;
	}

// -----------------------------------------------------------------------------
// CNpdItem::NewL
// -----------------------------------------------------------------------------
//
CNpdItem* CNpdItem::NewLC(TInt aKey, const TTime& aLastModified, HBufC *aContent)
	{
	_NOTEPAD_DBG_FILE("CNpdItem::NewLC(TInt aKey, const TTime& aLastModified, HBufC *aContent): begin");
	CNpdItem* tmp = new (ELeave) CNpdItem(aKey,aLastModified,aContent);
	CleanupStack::PushL(tmp);
	//tmp->ConstructL(aKey, aLastModified, aContent);
	_NOTEPAD_DBG_FILE("CNpdItem::NewLC(TInt aKey, const TTime& aLastModified, HBufC *aContent): end");
	return tmp;
	}

// -----------------------------------------------------------------------------
// CNpdItem::CNpdItem()
// -----------------------------------------------------------------------------
//
CNpdItem::CNpdItem() : iKey(0), iLastModified(Time::MinTTime()), iContent(NULL)
	{
	_NOTEPAD_DBG_FILE("NpdItem::CNpdItem(): begin");
	_NOTEPAD_DBG_FILE("NpdItem::CNpdItem(): end");
	}

// -----------------------------------------------------------------------------
// CNpdItem::CNpdItem(TInt aKey,const TTime& aLastModified, HBufC *aContent)
// -----------------------------------------------------------------------------
//
CNpdItem::CNpdItem(TInt aKey, 
					const TTime& aLastModified, 
					HBufC *aContent) : iKey(aKey), 
										iLastModified(aLastModified), 
										iContent(aContent)
	{
	_NOTEPAD_DBG_FILE("NpdItem::CNpdItem(): begin");
	_NOTEPAD_DBG_FILE("NpdItem::CNpdItem(): end");
	}
	

// -----------------------------------------------------------------------------
// CNpdItem::Key
// -----------------------------------------------------------------------------
//
TInt CNpdItem::Key() const
	{
	_NOTEPAD_DBG_FILE("CNpdItem::Key(): begin");
	_NOTEPAD_DBG_FILE("CNpdItem::Key(): end");
	return iKey;
	}
// -----------------------------------------------------------------------------
// CNpdItem::LastModified
// -----------------------------------------------------------------------------
//
TTime CNpdItem::LastModified() const
	{
	_NOTEPAD_DBG_FILE("CNpdItem::LastModified(): begin");
	_NOTEPAD_DBG_FILE("CNpdItem::LastModified(): end");
	return iLastModified;
	}
// -----------------------------------------------------------------------------
// CNpdItem::Content
// -----------------------------------------------------------------------------
//
HBufC* CNpdItem::Content()
	{
	_NOTEPAD_DBG_FILE("NpdItem::Content(): begin");
	_NOTEPAD_DBG_FILE("NpdItem::Content(): end");
	return iContent;
	}
// -----------------------------------------------------------------------------
// CNpdItem::Set
// -----------------------------------------------------------------------------
//
void CNpdItem::Set(TInt aKey, const TTime& aLastModified, HBufC *aContent)
	{
	_NOTEPAD_DBG_FILE("CNpdItem::Set(): begin");
	iKey = aKey;
	iLastModified = aLastModified;
    SetContent(aContent);
    _NOTEPAD_DBG_FILE("CNpdItem::Set(): end");
	}

// -----------------------------------------------------------------------------
// CNpdItem::SetContent
// -----------------------------------------------------------------------------
//
void CNpdItem::SetContent(HBufC *aContent)
	{
	_NOTEPAD_DBG_FILE("CNpdItem::SetContent(): begin");
    iContent = aContent;
    _NOTEPAD_DBG_FILE("CNpdItem::SetContent(): end");
	}

// -----------------------------------------------------------------------------
// CNpdItem::SetModified
// -----------------------------------------------------------------------------
//
void CNpdItem::SetModified(const TTime& aLastModified)
	{
	_NOTEPAD_DBG_FILE("CNpdItem::SetModified(): begin");
	iLastModified = aLastModified;
	_NOTEPAD_DBG_FILE("CNpdItem::SetModified(): end");
	}

// End of File
