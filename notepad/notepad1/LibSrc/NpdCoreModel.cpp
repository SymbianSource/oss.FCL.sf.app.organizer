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
* Description:  Implementation of core part of Notepad Model.
*
*/


// INCLUDE FILES
#include <txtglobl.h>
#include <barsread.h>
#include <eikenv.h>
#include "NpdModel_platsec.h"
#include "NpdLib.hrh"
#include "NpdLib.h"
#include "NpdUtil.h"
#include "NpdLibPanic.h"

// next is for KNotepadPseudoKeyIdForNewNote.
//
#include "NpdApi.h"

#include "AknUtils.h"
// CONSTANTS
_LIT(KNotepadKey, "key");
_LIT(KNotepadUpdateTime, "time");
_LIT(KNotepadMemo, "memo");
_LIT(KNotepadTableName, "Table1");
_LIT(KNotepadSqlInsert, "SELECT * FROM Table1");
_LIT(KNotepadSqlFormatSeek, "SELECT * FROM Table1 WHERE key=%d");

const TUid KSecureUid = {0x101F8878};
_LIT(KSecure,	"SECURE");


const TInt KNotepadSqlStringMaxLength(128);
const TInt KNotepadLanguagePostfixWidth(2); // TLanuguage is 00 -- 99
const TInt KNotepadModelMinimumEntrySize(24);
const TInt KNotepadModelMinimumDataFileSize(300);
const TInt KNotepadDiskSpaceNeededForSingleDeletion(8192);

const TInt KNotepadListEntryLength ( 60 );
// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CNotepadCoreModel::CNotepadCoreModel
// C++ constructor
// -----------------------------------------------------------------------------
//
CNotepadCoreModel::CNotepadCoreModel()
    : iEnv(CEikonEnv::Static()),
    iModeOp(ENotepadModelRowNop),
    iKey(-1),
    iNpdItemLastChanged(Time::MinTTime())
    {
    }

// -----------------------------------------------------------------------------
// CNotepadCoreModel::ConstructL
// second phase constructor.
// -----------------------------------------------------------------------------
//
void CNotepadCoreModel::ConstructL(TInt aResId)
    {
    iDataResourceId = aResId; // used by IsTemplates()
    iDataFile = iEnv->AllocReadResourceL(aResId);
    if ( IsTemplates() )
        {
        const TInt language( User::Language() );
        __ASSERT_DEBUG( language >= 0 && language <= 999, 
            User::Leave( KErrUnknown ) );
        __ASSERT_DEBUG( iDataFile->Length() >= KNotepadLanguagePostfixWidth,
            User::Leave( KErrUnknown ) );
        TInt width( KNotepadLanguagePostfixWidth );
        if ( language > 99 )
            {
            iDataFile = iDataFile->ReAllocL( iDataFile->Length() + 1 );
            width++;
            }
        TPtr ptr = iDataFile->Des();
        ptr.SetLength(iDataFile->Length() - KNotepadLanguagePostfixWidth);
        ptr.AppendNumFixedWidth( language, EDecimal, width );
        }
    User::LeaveIfError(iDbs.Connect());
    OpenDatabaseL();
    iDatabase.Compact();
    if ( iDatabase.IsDamaged() )
        {
        iDatabase.Recover();
        }
    }

// -----------------------------------------------------------------------------
// CNotepadCoreModel::~CNotepadCoreModel
// Destructor.
// -----------------------------------------------------------------------------
//
CNotepadCoreModel::~CNotepadCoreModel()
    {
    iDatabase.Close();
    iDbs.Close();
    delete iDataFile;
    }

// -----------------------------------------------------------------------------
// CNotepadCoreModel::ContentByKeyL
// -----------------------------------------------------------------------------
//
HBufC* CNotepadCoreModel::ContentByKeyL(const TInt aKey)
    {
    __ASSERT_DEBUG( aKey >= 0, Panic(ENotepadLibraryPanicInvalidKey) );
    TBuf<KNotepadSqlStringMaxLength> sql;
    sql.Format(KNotepadSqlFormatSeek, aKey);
    RDbView dbView;
    CleanupClosePushL(dbView);
    User::LeaveIfError( dbView.Prepare(
        iDatabase, TDbQuery(sql), RDbView::EReadOnly ) );
    User::LeaveIfError(dbView.EvaluateAll());
    HBufC* content = NULL;
    TInt isAtRow(dbView.FirstL());
    User::LeaveIfError(isAtRow);
    if ( isAtRow )
        {
        dbView.GetL();
        RDbColReadStream blob;
        blob.OpenLC(dbView, ENotepadMemo);
        TInt colLength( dbView.ColLength(ENotepadMemo) );
        content = HBufC::NewLC(colLength);
        TPtr ptr = content->Des();

        blob.ReadL(ptr, colLength);
        CleanupStack::Pop(); // content
        CleanupStack::PopAndDestroy(); // blob
        }
    CleanupStack::PopAndDestroy(); // dbView
    return content;
    }

// -----------------------------------------------------------------------------
// CNotepadCoreModel::CountL
// -----------------------------------------------------------------------------
//
TInt CNotepadCoreModel::CountL()
    {
    RDbTable table;
    User::LeaveIfError( table.Open(
        iDatabase, KNotepadTableName, RDbRowSet::EReadOnly ) );
    CleanupClosePushL(table);
    TInt count( table.CountL() );
    CleanupStack::PopAndDestroy(); // table
    return count;
    }

// -----------------------------------------------------------------------------
// CNotepadCoreModel::InsertL
// Never leave once the database has committed.
// -----------------------------------------------------------------------------
//
TInt CNotepadCoreModel::InsertL(const TDesC& aDes, TInt& aKey)
    {
    if ( NotepadUtil::IsEmpty(aDes) )
        {
        return ENotepadModelRowNop;
        }
    aKey = KNotepadPseudoKeyIdForNewNote;
    return InsertOrUpdateL(aDes, aKey);
    }

// -----------------------------------------------------------------------------
// CNotepadCoreModel::UpdateL
// Never leave once the database has committed.
// -----------------------------------------------------------------------------
//
TInt CNotepadCoreModel::UpdateL(const TDesC& aDes, TInt& aKey)
    {
    __ASSERT_DEBUG( aKey >= 0, Panic(ENotepadLibraryPanicInvalidKey) );
    TInt resultStatus( NotepadUtil::IsEmpty(aDes) ? DeleteL(aKey) :
        InsertOrUpdateL(aDes, aKey) );
    if ( resultStatus == ENotepadModelRowDeleted )
        {
        aKey = KNotepadPseudoKeyIdForNewNote;
        }
    return resultStatus;
    }

// -----------------------------------------------------------------------------
// CNotepadCoreModel::DeleteL
// Never leave once the database has committed.
// -----------------------------------------------------------------------------
//
TInt CNotepadCoreModel::DeleteL(TInt aKey)
    {
    TInt resultStatus(ENotepadModelRowNop);
    iModeOp = ENotepadModelRowNop;
    RFs fileSession;
   	User::LeaveIfError(fileSession.Connect());
   	CleanupClosePushL(fileSession);
    TInt retval = fileSession.ReserveDriveSpace( KDefaultDrive, KNotepadDiskSpaceNeededForSingleDeletion );
    if ( retval == KErrNone )
    {
    	retval = fileSession.GetReserveAccess( KDefaultDrive );
    }
    TBuf<KNotepadSqlDeleteHeadSize> sql;
    sql.Append(KNotepadSqlDeleteHead);
    sql.AppendNum(aKey);

    TInt rowCount( iDatabase.Execute(sql) );
    User::LeaveIfError( rowCount );
    if ( rowCount > 0 ) 
        {
        resultStatus = ENotepadModelRowDeleted;
    	if ( !IsTemplates() )
    		{
        	iModeOp = ENotepadModelRowDeleted;
    		}
        TInt err(KErrNone);
        err = iDatabase.Compact();
        if ( err != KErrNone )
            {
            iEnv->HandleError(err);
            }
        }
    if ( retval == KErrNone )
    {
    	retval = fileSession.ReleaseReserveAccess( KDefaultDrive );
    }
	fileSession.Close();
	CleanupStack::PopAndDestroy();
    iKey = aKey;
    return resultStatus;
    }

// -----------------------------------------------------------------------------
// CNotepadCoreModel::Compact
// -----------------------------------------------------------------------------
//
TInt CNotepadCoreModel::Compact()
    {
    return iDatabase.Compact();
    }

// -----------------------------------------------------------------------------
// CNotepadCoreModel::SetModeOp
// -----------------------------------------------------------------------------
//
void CNotepadCoreModel::SetModeOp(CNotepadCoreModel::TNotepadModelRowResult aModeOp)
	{
	iModeOp = aModeOp;
	}

// -----------------------------------------------------------------------------
// CNotepadCoreModel::AppendContentAsLabelL
// -----------------------------------------------------------------------------
//
void CNotepadCoreModel::AppendContentAsLabelL(TDes& aDes, const TInt aKey)
    {
    __ASSERT_DEBUG( aKey >= 0, Panic(ENotepadLibraryPanicInvalidKey) );

    TBuf<KNotepadSqlStringMaxLength> sql;
    sql.Format(KNotepadSqlFormatSeek, aKey);
    RDbView dbView;
    CleanupClosePushL(dbView);
    User::LeaveIfError( dbView.Prepare(
        iDatabase, TDbQuery(sql), RDbView::EReadOnly ) );
    User::LeaveIfError(dbView.EvaluateAll());
    dbView.FirstL();
    dbView.GetL();
    AppendContentAsLabelL(aDes, dbView);
    CleanupStack::PopAndDestroy(); // dbView
    }

// -----------------------------------------------------------------------------
// CNotepadCoreModel::AppendContentAsLabelL
// -----------------------------------------------------------------------------
//
void CNotepadCoreModel::AppendContentAsLabelL(TDes& aDes, RDbView& aDbView)
    {
    __ASSERT_DEBUG( aDbView.AtRow(), 
        Panic(ENotepadLibraryPanicInvalidRowInView) );

    HBufC* hbuf = HBufC::NewLC(aDes.MaxLength());
    TPtr buf = hbuf->Des();
    RDbColReadStream blob;
    blob.OpenLC(aDbView, ENotepadMemo);
    const TInt colLength( aDbView.ColLength(ENotepadMemo) );
    TInt unreadLength( colLength );
    TInt orgLength(aDes.Length());
    while ( aDes.Length() < aDes.MaxLength() && unreadLength > 0 )
        {
        blob.ReadL(buf, 
            unreadLength < buf.MaxLength() ? unreadLength : buf.MaxLength() );
        unreadLength -= buf.Length();
        NotepadUtil::AppendLabel(aDes, buf, (aDes.Length() == orgLength));
        }
    CleanupStack::PopAndDestroy(2); // blob, hbuf
    }

// -----------------------------------------------------------------------------
// CNotepadCoreModel::ModeOp
// -----------------------------------------------------------------------------
//
CNotepadCoreModel::TNotepadModelRowResult CNotepadCoreModel::ModeOp()
	{
	return iModeOp;
	}

// -----------------------------------------------------------------------------
// CNotepadCoreModel::DoUpdateNotepadItemArrayAndKeyArrayL
// -----------------------------------------------------------------------------
//
void CNotepadCoreModel::DoUpdateNotepadItemArrayAndKeyArrayL(CDesCArray& aItemArray,
															RArray<TInt>& aKeyArray,
															HBufC& aTimeFormat)
	{
	switch(iModeOp)
		{
		//delete the items from list arrays
		case ENotepadModelRowDeleted:
			{
			TInt index = aKeyArray.Find(iKey);
			if(index != KErrNotFound) //if found
				{
				aItemArray.Delete(index);
				aItemArray.Compress();
				aKeyArray.Remove(index);
				}
			iModeOp = ENotepadModelRowNop;
			}
			break;
		//add new item in list arrays			
		case ENotepadModelRowAdded:
			{
			TTime time;
			time = iNpdItemLastChanged;
    		TTimeIntervalMinutes offset;
    		TTime now;
    		now.HomeTime();
    		TTime gmt;
    		gmt.UniversalTime();
    		now.MinutesFrom(gmt, offset);
    		time += offset; // including summertime adjustment
	 		HBufC* hbuf = HBufC::NewLC(KNotepadListEntryLength);
	    	TPtr lBuf = hbuf->Des();
	        lBuf.SetLength(0);
	        time.FormatL( lBuf, aTimeFormat);
	        AknTextUtils::LanguageSpecificNumberConversion(lBuf);
	        lBuf.Append(KColumnListSeparator);
	        AppendContentAsLabelL(lBuf, iKey);
	        TInt lIndex = GetInsertionIndex(lBuf,aItemArray);
	        aItemArray.InsertL(lIndex,lBuf);
	        User::LeaveIfError(aKeyArray.Insert(iKey,lIndex));
	        CleanupStack::PopAndDestroy(); // hbuf
			iModeOp = ENotepadModelRowNop;
			}
			break;
		//update the existing item.
		case ENotepadModelRowUpdated:
			{
			TTime time = iNpdItemLastChanged;
    		TTimeIntervalMinutes offset;
    		TTime now;
    		now.HomeTime();
    		TTime gmt;
    		gmt.UniversalTime();
    		now.MinutesFrom(gmt, offset);
    		time += offset; // including summertime adjustment
	 		HBufC* hbuf = HBufC::NewLC(KNotepadListEntryLength);
	    	TPtr lBuf = hbuf->Des();
	        lBuf.SetLength(0);
	        time.FormatL( lBuf, aTimeFormat);
	        lBuf.Append(KColumnListSeparator);
	        AppendContentAsLabelL(lBuf, iKey);
	        AknTextUtils::LanguageSpecificNumberConversion(lBuf);
	        TInt index = aKeyArray.Find(iKey);
	        if(index != KErrNotFound) //if found
				{
				aItemArray.Delete(index);
				aItemArray.Compress();
				aKeyArray.Remove(index);
				}
	        TInt lIndex = GetInsertionIndex(lBuf,aItemArray);
	        aItemArray.InsertL(lIndex,lBuf);
	        User::LeaveIfError(aKeyArray.Insert(iKey,lIndex));
	        CleanupStack::PopAndDestroy(); // hbuf
			iModeOp = ENotepadModelRowNop;	        
			}
			break;
		default:
			iModeOp = ENotepadModelRowNop;
			break;
		}
	}

// -----------------------------------------------------------------------------
// CNotepadCoreModel::OpenDatabaseL
// -----------------------------------------------------------------------------
//
void CNotepadCoreModel::OpenDatabaseL()
    {
    __ASSERT_DEBUG(iDataFile, Panic(ENotepadLibraryPanicNullDataFileName));
    __ASSERT_DEBUG(iDbs.ResourceCount() == 0, 
        Panic(ENotepadLibraryPanicDatabaseAlreadyOpened));
        
       TBuf<32> format;
       format.Copy(KSecure);
       format.Append(KSecureUid.Name());

    TInt err(iDatabase.Open(iDbs, *iDataFile,format));
    if ( err == KErrNone && IsInvalidTableL() )
        {
        err = KErrCorrupt;
        }
    switch ( err )
        {
        case KErrArgument: // when the datafile is empty
        case KErrNotSupported: // UID mismatch, corrupted
        case KErrEof: // read error, corrupted
        case KErrCorrupt:
            // delete corrupted file
            NotepadUtil::ShowCorruptedNoteL(*iEnv);
            DeleteFileL();
            // go through into next case
        case KErrNotFound:
            // try to create a new data file
            iDatabase.Close();
            CreateDataFileL();
            User::LeaveIfError(iDatabase.Open(iDbs, *iDataFile,format));
            // go through into next case
        case KErrNone: // database open succeed
            break;
        default:
            User::Leave(err);
            break;
        }
    }

// -----------------------------------------------------------------------------
// CNotepadCoreModel::CreateDataFileL
// -----------------------------------------------------------------------------
//
void CNotepadCoreModel::CreateDataFileL()
    {
    NotepadUtil::LeaveIfCriticalLevelL(*iEnv, 
        KNotepadModelMinimumDataFileSize );
    iDatabase.Close(); // for safty
    //User::LeaveIfError(iDatabase.Create(iEnv->FsSession(), *iDataFile));
    TBuf<32> format;
       format.Copy(KSecure);
       format.Append(KSecureUid.Name());
	TInt rr(0);
    TRAP(rr,iDatabase.Create(iDbs,*iDataFile,format); )

    TRAP(rr, InitDataFileL(); );
    if ( rr != KErrNone )
        {
        DeleteFileL(); // including iDatabase.Close()
        User::Leave(rr);
        }
    iDatabase.Close();
    }

// -----------------------------------------------------------------------------
// CNotepadCoreModel::InitDataFileL
//
// [Q] CreateIndex is needed?
// [A] No.
// We made a comparison experiment whether the data file with the index 
// has better performance than without it.
// 
// Pre-condition: The "C:\System\Data\Notepad.dat" does not exist and 
//     the folder "C:\System\favorites" should be empty.
// 
//                             Without Index         With Index   
// 1. Create 300 memos             200 sec             218 sec
//   (using NpdTest.app)
// 2. Notepad.dat size             9546 b              9546 b
//    after 1.
// 3. Boot up time of Notepad      2.2 sec             2.4 sec
//    with the 300 memos.
// 4. Delete the 300 memos.         18 sec              18 sec
//
// So our conclusion is that the data file without the index is better.
// -----------------------------------------------------------------------------
//
void CNotepadCoreModel::InitDataFileL()
    {
    // create table
    CDbColSet* columns = CreateDbColSetLC();
    User::LeaveIfError(iDatabase.CreateTable(KNotepadTableName, *columns));
    CleanupStack::PopAndDestroy(); // columns
    if ( IsTemplates() )
        {
        SetDefaultTemplatesL();
        }
    }

// -----------------------------------------------------------------------------
// CNotepadCoreModel::SetDefaultTemplatesL
// -----------------------------------------------------------------------------
//
void CNotepadCoreModel::SetDefaultTemplatesL()
    {
    CDesCArray* data = iEnv->ReadDesCArrayResourceL(
        R_NOTEPAD_DEFAULT_TEMPLATES );
    CleanupStack::PushL(data);
    const TInt count(data->Count());

    TInt estimatedSize(0);
    for (TInt i(0); i < count; i++)
        {
        estimatedSize += KNotepadModelMinimumEntrySize + (*data)[i].Size();
        }
    NotepadUtil::LeaveIfCriticalLevelL(*iEnv, estimatedSize);

    TTime gmt;
    gmt.UniversalTime();
    gmt.RoundUpToNextMinute();
    gmt -= TTimeIntervalMinutes(1);

    RDbView dbView;
    CleanupClosePushL(dbView);
    User::LeaveIfError( dbView.Prepare(
        iDatabase, TDbQuery(KNotepadSqlInsert), RDbView::EInsertOnly ) );

    User::LeaveIfError(iDatabase.Begin());
    TRAPD(r,
        for ( TInt i(0); i < count; i++ )
            {
            TPtrC entry((*data)[i]);
            if ( !NotepadUtil::IsEmpty(entry) )
                {
                dbView.InsertL();
                dbView.SetColL(ENotepadUpdateTime, gmt);
                dbView.SetColL(ENotepadMemo, entry);
                dbView.PutL();
                }
            }
        User::LeaveIfError(iDatabase.Commit());
        );
    if (r != KErrNone)
        {
        Rollback();
        User::Leave(r);
        } 
    CleanupStack::PopAndDestroy(2); // dbView, data
    User::LeaveIfError(iDatabase.Compact()); 
    }

// -----------------------------------------------------------------------------
// CNotepadCoreModel::CreateDbColSetLC
// -----------------------------------------------------------------------------
//
CDbColSet* CNotepadCoreModel::CreateDbColSetLC() const
    {
    CDbColSet* columns = CDbColSet::NewLC(); 
    TDbCol col(KNotepadKey, EDbColInt32);
    col.iAttributes = TDbCol::ENotNull | TDbCol::EAutoIncrement;
    columns->AddL(col);

    col.iName = KNotepadUpdateTime;
    col.iType = EDbColDateTime;
    col.iAttributes = TDbCol::ENotNull;
    columns->AddL(col);

    col.iName = KNotepadMemo;
    col.iType = EDbColLongText;
    col.iAttributes = TDbCol::ENotNull;
    columns->AddL(col);

    return columns; // columns keeps on CleanupStack
    }

// -----------------------------------------------------------------------------
// CNotepadCoreModel::Rollback
// -----------------------------------------------------------------------------
//
void CNotepadCoreModel::Rollback()
    {
    iDatabase.Rollback();
    if ( iDatabase.IsDamaged() )
        {
        iDatabase.Recover();
        }
    }

// -----------------------------------------------------------------------------
// CNotepadCoreModel::InsertOrUpdateL
// Never leave once the database has committed.
// Including: FFS-full check.
// Not including: empty check.
// -----------------------------------------------------------------------------
//
TInt CNotepadCoreModel::InsertOrUpdateL(const TDesC& aDes, TInt& aKey)
    {
    NotepadUtil::LeaveIfCriticalLevelL(*iEnv, 
        KNotepadModelMinimumEntrySize + aDes.Size());

    RDbView dbView;
    CleanupClosePushL(dbView);
    TBool doInsert( aKey == KNotepadPseudoKeyIdForNewNote );
    if ( !doInsert )
        {
        TBuf<KNotepadSqlStringMaxLength> sql;
        sql.Format(KNotepadSqlFormatSeek, aKey);
        User::LeaveIfError( dbView.Prepare(
            iDatabase, TDbQuery(sql) ) );
        User::LeaveIfError(dbView.EvaluateAll());
        TInt isAtRow(dbView.FirstL());
        User::LeaveIfError(isAtRow);
        if ( !isAtRow )
            {
            doInsert = ETrue;
            }
        }
    if ( doInsert )
        {
        User::LeaveIfError( dbView.Prepare(
            iDatabase, TDbQuery(KNotepadSqlInsert) ) );
        }

    TTime gmt;
    gmt.UniversalTime();
    gmt.RoundUpToNextMinute();
    gmt -= TTimeIntervalMinutes(1);

    User::LeaveIfError(iDatabase.Begin());
    TRAPD(r,
        if ( doInsert )
            {
            dbView.InsertL();
            }
        else
            {
            dbView.UpdateL();
            }
        dbView.SetColL(ENotepadUpdateTime, gmt);
        dbView.SetColL(ENotepadMemo, aDes);
        dbView.PutL();
        dbView.GetL();
        User::LeaveIfError(iDatabase.Commit());
        );
    if (r != KErrNone)
        {
        dbView.Close();
        Rollback();
        User::Leave(r);
        } 
    // Database has committed

    aKey = dbView.ColInt32(ENotepadKey);

    CleanupStack::PopAndDestroy(); // dbView

    TInt err(iDatabase.Compact());
    if ( err != KErrNone )
        {
        iEnv->HandleError(err);
        }
	iNpdItemLastChanged = gmt;
	iKey = aKey;
    if ( !IsTemplates() )
    	{
		iModeOp = (doInsert?ENotepadModelRowAdded : ENotepadModelRowUpdated);
    	}
    return ( doInsert ? ENotepadModelRowAdded : ENotepadModelRowUpdated );
    }

// -----------------------------------------------------------------------------
// CNotepadCoreModel::DeleteFileL
// -----------------------------------------------------------------------------
//
void CNotepadCoreModel::DeleteFileL()
    {
    __ASSERT_DEBUG( iDataFile, Panic(ENotepadLibraryPanicNullDataFileName) );
    iDatabase.Close(); // indespensable for RFs::Delete()
    User::LeaveIfError( iEnv->FsSession().Delete( *iDataFile ) );
    }

// -----------------------------------------------------------------------------
// CNotepadCoreModel::IsInvalidTableL()
// -----------------------------------------------------------------------------
//
TBool CNotepadCoreModel::IsInvalidTableL()
    {
    TBool isInvalid(EFalse);
    RDbTable table;
    TInt err(table.Open(iDatabase, KNotepadTableName, RDbRowSet::EReadOnly));
 
	if ( err == KErrNotFound )
        {
        isInvalid = ETrue;
        }
    else if ( err == KErrNone )
        {
        if ( table.ColCount() > ENotepadColCountPlusOne - 1 )
            {
            table.Close();
            CDbColSet* cols = CreateDbColSetLC();
            TInt error(iDatabase.AlterTable(KNotepadTableName, *cols));
            CleanupStack::PopAndDestroy(); // cols
            if ( error == KErrArgument ) // for fail safe
                {
                isInvalid = ETrue;
                error = KErrNone;
                }
            User::LeaveIfError(error);
            }
        else if ( table.ColCount() < ENotepadColCountPlusOne - 1 )
            {
            isInvalid = ETrue;
            }
        else if ( table.ColType(ENotepadKey) != EDbColInt32 ||
            table.ColType(ENotepadUpdateTime) != EDbColDateTime ||
            table.ColType(ENotepadMemo) != EDbColLongText )
            {
            isInvalid = ETrue;
            }
        table.Close();
        }
    return isInvalid;
    }

// -----------------------------------------------------------------------------
// CNotepadCoreModel::GetInsertionIndex
// -----------------------------------------------------------------------------
//
TInt CNotepadCoreModel::GetInsertionIndex(const TDesC& aDes,
										CDesCArray& aItemArray)
	{
	_LIT(KCmpColumnListSeparator,"*\t*");
	TBuf<255> timeBuf;
	TBuf<255> contentBuf;
	TBuf<255> actualTimeBuf;
	timeBuf.Copy(aDes.Left( aDes.Match(KCmpColumnListSeparator) ));
	TBuf<255> cmpBuf;
    cmpBuf.Copy(_L("*"));
    cmpBuf.Append(timeBuf);
    cmpBuf.Append(_L("*"));
    TBuf<255> actualBuf;
    TInt insertIndex=0;
    TInt offset = KErrNotFound;
    TInt cmpOffsetListItem = KErrNotFound;
    for(insertIndex= 0;insertIndex<aItemArray.MdcaCount();insertIndex++)
    	{
    	actualBuf.Copy(aItemArray.MdcaPoint(insertIndex));
		actualTimeBuf.Copy(actualBuf.Left( actualBuf.Match(KCmpColumnListSeparator) ));
    	offset = actualBuf.Match(cmpBuf);
    	if(offset!=KErrNotFound)
    		{
    		cmpOffsetListItem = actualBuf.Match(KCmpColumnListSeparator);
    		if(cmpOffsetListItem!=KErrNotFound)
    			{
    			actualBuf.Copy(actualBuf.Right(actualBuf.Length()-cmpOffsetListItem-1));	
    			contentBuf.Copy(aDes.Right( aDes.Length()-aDes.Match(KCmpColumnListSeparator)-1 ));
    			if(actualBuf.Compare(contentBuf)>0)
    				{
    				break;
    				}
    			}
    		}
    	else
    		{
    		TTime time1;
    		time1.Parse(timeBuf);
    		TTime time2;
    		time2.Parse(actualTimeBuf);
    		TDateTime dateTime2 = time2.DateTime();
    		if ( !dateTime2.Year() )
    			{
    			if ( time1 > time2 )
    				break;
    			}
    		else
    			{
    			TTime now;
    			now.HomeTime();
    			if ( now > time2 )
    				break;
    			}
    		}
    	}
    return insertIndex;
	}
// End of File  
