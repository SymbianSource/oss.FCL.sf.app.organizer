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
* Description:  Implementation of Notepad engine.
*
*/


// INCLUDE FILES
#include <e32std.h>
#include <gulutil.h>
#include <sysutil.h>
#include <eikenv.h>
#include <barsread.h>
#include "NpdLib.hrh"
#include "NpdModel_platsec.h"
#include "NpdApi.h"
#include "NpdLibPanic.h"
#include "AknUtils.h"

// CONSTANTS
_LIT(KNotepadMceFirstColumnData, "1");
_LIT(KNotepadSqlSelect, 
    "SELECT * FROM Table1 ORDER BY time DESC");
_LIT(KNotepadSqlMceSelect, 
    "SELECT * FROM Table1 ORDER BY memo ASC,key DESC");
_LIT(KNotepadSqlDeleteByKeysAppend, " OR key="); // 8 chars
const TInt KNotepadSqlDeleteByKeysAppendSize(18); // %d costs 10 chars in max
const TInt KNotepadItemArrayGranularity(10);
const TInt KNotepadMaxRunIntervalInMicroSeconds(500000); // 0.5 sec
const TInt KNotepadBuildItemArrayProgressCount(10);
const TInt KNotepadMaxDeleteCountInStep(300);
const TInt KNotepadDiskSpaceNeededForMultipleDeletion(8192);
const TInt KNotepadMaxCharactersForSort(100);

// assert KNotepadListEntryLength >= KNotepadMaxCharactersForSort + length of
//     1st column
const TInt KNotepadListEntryLength( 60 ); 

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CNotepadModel::NewL
// Constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CNotepadModel* CNotepadModel::NewL(RFs& /*aFs*/, TInt aResId)
    {
    CNotepadModel* self = new(ELeave) CNotepadModel();
    CleanupStack::PushL(self);
    self->ConstructL(aResId);
    CleanupStack::Pop(); //self
    return self;
    }

// -----------------------------------------------------------------------------
// CNotepadModel::~CNotepadModel
// Destructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CNotepadModel::~CNotepadModel()
    {
    delete iDatabaseChangeNotifier;
    delete iExecuter;
    iSavedDeleteKeys.Close();
    iDbUpdate.Close();
    iDbView.Close();
    iKeyArray.Close(); //delete iKeyArray;
    delete iItemArray;
    delete iTimeFormat;
    delete iDateFormat;
    }

// -----------------------------------------------------------------------------
// CNotepadModel::AddContentL
// API. CNotepadApi call this.
// -----------------------------------------------------------------------------
//
EXPORT_C void CNotepadModel::AddContentL(const TDesC& aText)
    {
    TInt key(KNotepadPseudoKeyIdForNewNote);
    InsertL(aText, key);
    }

// -----------------------------------------------------------------------------
// CNotepadModel::OpenL
// deprecated. (only for BC).
// -----------------------------------------------------------------------------
//
EXPORT_C void CNotepadModel::OpenL()
    {
    }

// -----------------------------------------------------------------------------
// CNotepadModel::Close
// deprecated. (only for BC).
// -----------------------------------------------------------------------------
//
EXPORT_C void CNotepadModel::Close() 
    {
    }

// -----------------------------------------------------------------------------
// CNotepadModel::IsOpen
// deprecated. (only for BC).
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CNotepadModel::IsOpen() 
    {
    return ETrue;
    } 

// -----------------------------------------------------------------------------
// CNotepadModel::SeekKey
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CNotepadModel::SeekKey(const TInt aKey)
    {
    return iKeyArray.Find(aKey) >= 0 ? // found?
        0 : 1;
    }

// -----------------------------------------------------------------------------
// CNotepadModel::ContentL
// -----------------------------------------------------------------------------
//
HBufC* CNotepadModel::ContentL(TInt aItemIndex)
    {
    return ContentByKeyL(iKeyArray[aItemIndex]);
    }

// -----------------------------------------------------------------------------
// CNotepadModel::IndexOf
// -----------------------------------------------------------------------------
//
TInt CNotepadModel::IndexOf(const TInt aKey) const
    {
    TInt returnIndex(KNotepadModelInvalidIndex);
    if ( aKey != KNotepadPseudoKeyIdForNewNote )
        {
        TInt index(iKeyArray.Find(aKey));
        if ( index >= 0 )
            {
            returnIndex = index;
            }
        }
    return returnIndex;
    }

// -----------------------------------------------------------------------------
// CNotepadModel::HandleDateChangedL
// -----------------------------------------------------------------------------
//
void CNotepadModel::HandleDateChangedL(TBool aSyncNow)
    {
    iFlag |= ENotepadRequireDbViewUpdate | ENotepadRequireItemArrayUpdate;
    if ( aSyncNow )
        {
        SyncL(EFalse);
        }
    }

// -----------------------------------------------------------------------------
// CNotepadModel::DeleteByKeysL
// -----------------------------------------------------------------------------
//
void CNotepadModel::DeleteByKeysL(const RArray<TInt>& aKeys)
    {
    __ASSERT_DEBUG( aKeys.Count() > 0, 
        Panic(ENotepadLibraryPanicInvalidArgument) );
    TRAPD( err, PrepareToDeleteByKeysL(aKeys); ); // execute RDbUpdate
    if ( err != KErrNone )
        {
        AbortDeletionL();
        User::Leave(err);
        }
    iFlag |= ENotepadIsDeleting;
    iFlag &= ~ENotepadIsRemovingLinks;
    iExecuter->Start(TCallBack(DeleteCallBack, this));
    }

// -----------------------------------------------------------------------------
// CNotepadModel::CancelDeletion
// -----------------------------------------------------------------------------
//
void CNotepadModel::CancelDeletion()
    {
    iDbUpdate.Close();
    delete iExecuter;
    iExecuter = NULL;
    iFlag &= ~ENotepadIsDeleting;
    iFlag &= ~ENotepadIsRemovingLinks;
    iFlag |= ENotepadRequireDbViewUpdate;
    if ( iDatabase.IsDamaged() )
        {
        iDatabase.Recover();
        }
    iSavedDeleteKeys.Reset();
	}

// -----------------------------------------------------------------------------
// CNotepadModel::SyncL
// Sync non-updated data.
// -----------------------------------------------------------------------------
//
void CNotepadModel::SyncL( const TBool aForceSync )
    {
    // If iExecuter is creating iDbView, cancel the process and restart 
    // to create iDbView. If iExecuter is deleting items, this method does nothing.
    // (ie. Creating iDbView is postponed until the deleting process 
    // will be finished.
	if ( !aForceSync && (iFlag & ENotepadItemArrayUpdateOnly) )
	    {
	    iFlag |= ENotepadItemArrayUpdateOnly;
    	if ( iModelObserver )
        	{
        	iModelObserver->HandleNotepadModelEventL(
          			MNotepadModelObserver::EStartItemArrayChange );
        	}
	    TInt err(KErrNone);
		TRAP(err,DoUpdateNotepadItemArrayAndKeyArrayL(*iItemArray,
													  iKeyArray,
													  *iTimeFormat));
		if ( err != KErrNone)
			{
			iItemArray->Reset();
			iKeyArray.Reset();
			if ( iModelObserver )
				{
			    iModelObserver->HandleNotepadModelEventL(
			        MNotepadModelObserver::EAbortItemArrayChange ); // not leave
			    }
			User::Leave(err);
		    }
		iFlag &= ~ENotepadItemArrayUpdateOnly; // now iItemArray is up to date
		if ( iModelObserver )
			{
			iModelObserver->HandleNotepadModelEventL(
			        MNotepadModelObserver::ECompleteItemArrayChange );
		    }
	    }
    else if(aForceSync || ( iFlag & ENotepadRequireDbViewUpdate ))
        {
        iFlag |= (ENotepadRequireDbViewUpdate | ENotepadRequireItemArrayUpdate);
        iDbView.Close();
        TDbTextComparison textComparison(EDbCompareCollated);
        User::LeaveIfError( iDbView.Prepare( iDatabase, 
            ( IsTemplates() ? TDbQuery(KNotepadSqlMceSelect, textComparison)
                : TDbQuery(KNotepadSqlSelect, textComparison) ), 
        RDbView::EReadOnly ) );
        User::LeaveIfError( iDbView.EvaluateAll() );
        iFlag &= ~ENotepadRequireDbViewUpdate; // iDbView is now up to date
        BuildItemArrayL(ETrue); // force update
        }
    else 
        {
        BuildItemArrayL(aForceSync);
        }
    return;
    }

// -----------------------------------------------------------------------------
// CNotepadModel::HandleDatabaseChangedL
// -----------------------------------------------------------------------------
//
void CNotepadModel::HandleDatabaseChangedL(TInt aStatus)
    {
    if ( ( ModeOp() == CNotepadCoreModel::ENotepadModelRowDeleted ) ||
    	 ( ModeOp() == CNotepadCoreModel::ENotepadModelRowAdded ) ||
    	 ( ModeOp() == CNotepadCoreModel::ENotepadModelRowUpdated ) )
    	{
 	  	iFlag |=ENotepadItemArrayUpdateOnly;
    	}
    else
    	{
    	iFlag |= ENotepadRequireDbViewUpdate; // delayed sync	
    	}
    if ( iModelObserver 
        && !(IsDeleting() && !(iFlag & ENotepadIsRemovingLinks)) )
        {
        iModelObserver->HandleNotepadModelEventL(
            MNotepadModelObserver::EDatabaseChanged, aStatus );
        }
    }

// -----------------------------------------------------------------------------
// CNotepadModel::SetNotepadModelObserver
// -----------------------------------------------------------------------------
//
void CNotepadModel::SetNotepadModelObserver(
    MNotepadModelObserver* aObserver )
    {
    iModelObserver = aObserver;
    }

// -----------------------------------------------------------------------------
// CNotepadModel::MdcaCount
// from MDesCArray
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CNotepadModel::MdcaCount() const
    {
    return iItemArray->Count();
    }

// -----------------------------------------------------------------------------
// CNotepadModel::MdcaPoint
// from MDesCArray
// -----------------------------------------------------------------------------
//
EXPORT_C TPtrC CNotepadModel::MdcaPoint(TInt aIndex) const
    {
    return (*iItemArray)[aIndex];
    }

// -----------------------------------------------------------------------------
// CNotepadModel::CNotepadModel
// C++ constructor
// -----------------------------------------------------------------------------
//
CNotepadModel::CNotepadModel()
    : CNotepadCoreModel(),
    iListingStyle(ENotepadListingStyleNone)
    {
    }

// -----------------------------------------------------------------------------
// CNotepadModel::ConstructL
// second phase constructor.
// -----------------------------------------------------------------------------
//
void CNotepadModel::ConstructL(TInt aResId)
    {
    TResourceReader rr;
    iEnv->CreateResourceReaderLC(rr, aResId); // Push rr
    CNotepadCoreModel::ConstructL(rr.ReadInt32()); // LLINK data_file
    iListingStyle 
        = rr.ReadInt16(); // WORD  listing_style;
    TInt dateResId(rr.ReadInt32());               // LLINK date_format
    TInt timeResId(rr.ReadInt32());               // LLINK time_format
    CleanupStack::PopAndDestroy(); // rr
    if (dateResId)
        {
        iDateFormat = iEnv->AllocReadResourceL(dateResId);
        }
    if (timeResId)
        {
        iTimeFormat = iEnv->AllocReadResourceL(timeResId);
        }
    iItemArray = new(ELeave) CDesCArrayFlat(KNotepadItemArrayGranularity);
    iDatabaseChangeNotifier = 
        CNotepadModel::CDatabaseChangeNotifier::NewL(iDatabase, *this);

    SyncL(ETrue);
    }

// -----------------------------------------------------------------------------
// CNotepadModel::PrepareToDeleteByKeysL
// -----------------------------------------------------------------------------
//
void CNotepadModel::PrepareToDeleteByKeysL(const RArray<TInt>& aKeys)
    {
    CancelDeletion();
    User::LeaveIfError(iFileSession.Connect());
   	CleanupClosePushL(iFileSession);
    iRetval = iFileSession.ReserveDriveSpace( KDefaultDrive, KNotepadDiskSpaceNeededForMultipleDeletion );
    if ( iRetval == KErrNone )
    {
    	iRetval = iFileSession.GetReserveAccess( KDefaultDrive );
    }
    // the priority of iExecuter must be lower than that of CAknProgressDialog
    iExecuter = CIdle::NewL(CActive::EPriorityLow); 
    const TInt count( aKeys.Count() );
    for (TInt i(0); i < count; i++)
        {
        User::LeaveIfError(iSavedDeleteKeys.Append(aKeys[i]));
        }
    iProgressCount = 0;
    ExecuteDeleteStepL();
    iRetval = iFileSession.ReleaseReserveAccess( KDefaultDrive );
    iFileSession.Close();
	CleanupStack::PopAndDestroy();
    }

// -----------------------------------------------------------------------------
// CNotepadModel::ExecuteDeleteStepL
// -----------------------------------------------------------------------------
//
void CNotepadModel::ExecuteDeleteStepL()
    {
    
    iStepCount = iSavedDeleteKeys.Count();
    if ( iStepCount > KNotepadMaxDeleteCountInStep )
        {
        iStepCount = KNotepadMaxDeleteCountInStep;
        }
    HBufC* hbuf = HBufC::NewLC( KNotepadSqlDeleteHeadSize + 
        ( iStepCount - 1 )* KNotepadSqlDeleteByKeysAppendSize);
    TPtr sql = hbuf->Des();
    sql.Append(KNotepadSqlDeleteHead);
    sql.AppendNum(iSavedDeleteKeys[0]);
    if ( IsTemplates() )
        {
        iSavedDeleteKeys.Remove(0);
        }
    else // If Notepad, Remove is postponed until remove link phase
        {
        for (TInt i(1); i < iStepCount; i++)
            {
            sql.Append(KNotepadSqlDeleteByKeysAppend);
            sql.AppendNum(iSavedDeleteKeys[i]);
            }
        }
    User::LeaveIfError(iDbUpdate.Execute(iDatabase, sql));
    CleanupStack::PopAndDestroy(); // hbuf
    }

// -----------------------------------------------------------------------------
// CNotepadModel::AbortDeletionL
// -----------------------------------------------------------------------------
//
void CNotepadModel::AbortDeletionL()
    {
    CancelDeletion();
    if ( iModelObserver )
        {
        iModelObserver->HandleNotepadModelEventL(
            MNotepadModelObserver::EAbortDeletion );
        }
    }

// -----------------------------------------------------------------------------
// CNotepadModel::CompleteDeletionL
// -----------------------------------------------------------------------------
//
void CNotepadModel::CompleteDeletionL()
    {
    CancelDeletion();
    if ( iModelObserver )
        {
        iModelObserver->HandleNotepadModelEventL(
            MNotepadModelObserver::ECompleteDeletion);
        }
    SyncL(ETrue);
    iItemArray->Compress();
    }

// -----------------------------------------------------------------------------
// CNotepadModel::DeleteCallBack
//
// be sure that this function never leave after AbortDeletionL
// because iExecuter is deleted in AbortDeletionL.
// -----------------------------------------------------------------------------
//
TInt CNotepadModel::DeleteCallBack(TAny *aSelf)
    {
    TInt needsRepeat(FALSE);
    CNotepadModel* self = STATIC_CAST(CNotepadModel*, aSelf);
    TRAPD(err, 
        needsRepeat = ( self->iFlag & ENotepadIsRemovingLinks ) ? 
            self->DoRemoveLinkCallBackL() : 
            self->DoDeleteCallBackL(); 
        );
    if ( err != KErrNone )
        {
        self->iEnv->HandleError(err);
        TRAP(err, self->AbortDeletionL(););
        if ( err != KErrNone )
            {
            self->iEnv->HandleError(err);
            }
        }
    return needsRepeat;
    }

// -----------------------------------------------------------------------------
// CNotepadModel::DoDeleteCallBackL
// -----------------------------------------------------------------------------
//
TInt CNotepadModel::DoDeleteCallBackL()
    {
    TInt needsRepeat(TRUE);
    TInt increment(0);
    TBool deleteFinished(EFalse);
    const TInt stat(iDbUpdate.Next());
    if ( stat == 0 ) // current step has completed
        {
        iDbUpdate.Close();
        if ( IsTemplates() )
            {
            if ( iSavedDeleteKeys.Count() == 0 ) // MCE, finish all
                {
                needsRepeat = FALSE;
                CompleteDeletionL();
                deleteFinished = ETrue;
                }
            else // do next step
                {
                increment = iStepCount - iProgressCount;

                iProgressCount += increment;
                if ( iModelObserver )
                    {
                    iModelObserver->HandleNotepadModelEventL(
                            MNotepadModelObserver::EProgressDeletion,
                            1 );
                    }

                ExecuteDeleteStepL();
                }
            }
        else
            {
            // switch to remove link phase
            iFlag |= ENotepadIsRemovingLinks; 
            increment = iStepCount - iProgressCount;
            if ( iSavedDeleteKeys.Count() == iStepCount ) // final step
                {
                deleteFinished = ETrue;
                }
            }
        }
    if ( stat > 0 )
        {
        increment = iDbUpdate.RowCount() - iProgressCount;
        }            
    
    if ( deleteFinished || ( stat == 0 && 
        SysUtil::FFSSpaceBelowCriticalLevelL(&(iEnv->FsSession())) ) )
        {
        User::LeaveIfError(iDatabase.Compact());
        }
    User::LeaveIfError(stat);
    return needsRepeat;
    }

// -----------------------------------------------------------------------------
// CNotepadModel::DoRemoveLinkCallBackL
// -----------------------------------------------------------------------------
//
TInt CNotepadModel::DoRemoveLinkCallBackL()
    {
    TInt needsRepeat(TRUE);
    TBool hasFinished(EFalse);
    TBool timeUp(EFalse);
    TInt count(0);
    TTime beginning;
    TTime now;
    TTimeIntervalMicroSeconds maxInterval(
        KNotepadMaxRunIntervalInMicroSeconds );
    beginning.UniversalTime();
    while ( !hasFinished && !timeUp )
        {
        if ( iStepCount == 0 ) // current step has completed
            {
            hasFinished = ETrue;
            }
        else
            {
            iSavedDeleteKeys.Remove(0);
            count++;
            iStepCount--;
            now.UniversalTime();
            timeUp = ( now.MicroSecondsFrom(beginning) > maxInterval );
            }
        }
    if ( hasFinished ) // current step has finished
        {
        if ( iSavedDeleteKeys.Count() == 0 ) // finish all
            {
            needsRepeat = FALSE;
            CompleteDeletionL();
            }
        else // switch to delete phase again
            {
            iFlag &= ~ENotepadIsRemovingLinks;
            ExecuteDeleteStepL();
            }
        }
    if ( needsRepeat && count > 0 )
        {
        iModelObserver->HandleNotepadModelEventL(
            MNotepadModelObserver::EProgressDeletion, count); 
        }
    return needsRepeat;
    }

// -----------------------------------------------------------------------------
// CNotepadModel::BuildItemArrayL
// -----------------------------------------------------------------------------
//
void CNotepadModel::BuildItemArrayL( const TBool aForceSync )
    {
    if ( !aForceSync && !(iFlag & ENotepadRequireItemArrayUpdate) )
        {
        return;
        }

    iFlag |= ENotepadRequireItemArrayUpdate;
    if ( iModelObserver )
        {
        iModelObserver->HandleNotepadModelEventL(
            MNotepadModelObserver::EStartItemArrayChange );
        }
    iItemArray->Reset();
    iKeyArray.Reset();
    TRAPD( err, DoBuildItemArrayL() );
    if ( err != KErrNone)
        {
        iItemArray->Reset();
        iKeyArray.Reset();
        if ( iModelObserver )
            {
            iModelObserver->HandleNotepadModelEventL(
                MNotepadModelObserver::EAbortItemArrayChange ); // not leave
            }
        User::Leave(err);
        }
    iFlag &= ~ENotepadRequireItemArrayUpdate; // now iItemArray is up to date
    if ( iModelObserver )
        {
        iModelObserver->HandleNotepadModelEventL(
            MNotepadModelObserver::ECompleteItemArrayChange );
        }
    }

// -----------------------------------------------------------------------------
// CNotepadModel::DoBuildItemArrayL
// -----------------------------------------------------------------------------
//
void CNotepadModel::DoBuildItemArrayL()
    {
    HBufC* hbuf = HBufC::NewLC(KNotepadListEntryLength);
    TPtr buf = hbuf->Des();
    if ( IsTemplates() )
        {
        DoBuildTemplateItemArrayL(buf);
        }
    else
        {
        DoBuildNotepadItemArrayL(buf);
        }
    CleanupStack::PopAndDestroy(); // hbuf
    }

// -----------------------------------------------------------------------------
// CNotepadModel::DoBuildNotepadItemArrayL
// -----------------------------------------------------------------------------
//
void CNotepadModel::DoBuildNotepadItemArrayL(
    TPtr& aBuf)
    {
    TTimeIntervalMinutes offset;
    TTime now;
    now.HomeTime();
    TTime gmt;
    gmt.UniversalTime();
    now.MinutesFrom(gmt, offset);
    TInt currentDay(now.DateTime().Day());
    TInt currentMonth(now.DateTime().Month());
    TInt currentYear(now.DateTime().Year());

    RArray<TInt> minutesArrayOfToday;
    CleanupClosePushL(minutesArrayOfToday);
    TInt minutesInToday(0);

    TInt firstIndexOfTheDay(KNotepadModelInvalidIndex);
    TDateTime prevDateTime;

    RArray<TPtrC> textArrayOfTheDay;
    CleanupClosePushL(textArrayOfTheDay);
    iDbView.FirstL();
    while (iDbView.AtRow())
        {
        iDbView.GetL();
        TInt thisKey(iDbView.ColInt(ENotepadKey));
        TTime time = iDbView.ColTime(ENotepadUpdateTime);
        time += offset; // including summertime adjustment
        TDateTime dateTime = time.DateTime(); // cache
        TBool isToday( dateTime.Day() == currentDay
             && dateTime.Month() == currentMonth
             && dateTime.Year() == currentYear );
        if ( isToday )
            {
            minutesInToday = dateTime.Hour() * 60 + dateTime.Minute();
            }
        aBuf.SetLength(0);
        TInt textOffset(0);
        switch (iListingStyle)
            {
            case ENotepadListingStyleNP: // list of memos
                time.FormatL( aBuf, isToday ? *iTimeFormat : *iDateFormat );
                aBuf.Append(KColumnListSeparator);
                textOffset = aBuf.Length();
                // through into next case
            case ENotepadListingStyleFetch: // fetch a memo
                AppendContentAsLabelL(aBuf, iDbView);
                break;
            default:
                break;
            }

        TPtrC thisText = 
            aBuf.Mid(textOffset).Left(KNotepadMaxCharactersForSort);
        if ( iItemArray->Count() == 0 || 
             dateTime.Day() != prevDateTime.Day() ||
             dateTime.Month() != prevDateTime.Month() ||
             dateTime.Year() != prevDateTime.Year() )
            {
            // first entry or date of the item has changed
            firstIndexOfTheDay = iItemArray->Count();
            textArrayOfTheDay.Reset();
            if ( isToday )
                {
                User::LeaveIfError(minutesArrayOfToday.Append(minutesInToday));
                }
            // INSERT POINT
            AknTextUtils::LanguageSpecificNumberConversion(aBuf);
            iItemArray->AppendL(aBuf);
            User::LeaveIfError( textArrayOfTheDay.Append(
                (*iItemArray)[firstIndexOfTheDay].Mid(textOffset).Left(
                    KNotepadMaxCharactersForSort) ) );
            User::LeaveIfError(iKeyArray.Append(thisKey));
            }
        else
            {
            // calculate an insertion point
            TInt low(0); // this will be result insertion point
            TInt high( textArrayOfTheDay.Count() - 1 );
            TInt mid(0);
            while ( high >= low )
                {
                mid = (high + low) / 2;
                if ( isToday ? ( minutesInToday < minutesArrayOfToday[mid] ||
                    ( minutesInToday == minutesArrayOfToday[mid] &&
                        thisText.CompareC(textArrayOfTheDay[mid]) > 0 ) ) :
                    thisText.CompareC(textArrayOfTheDay[mid]) > 0 )
                    {
                    low = mid + 1;
                    }
                else
                    {
                    high = mid - 1;
                    }
                }
            // low is result
            if ( isToday )
                {
                User::LeaveIfError(minutesArrayOfToday.Insert(
                    minutesInToday, low ) );
                }
            // INSERT POINT
            AknTextUtils::LanguageSpecificNumberConversion(aBuf);
            iItemArray->InsertL(firstIndexOfTheDay + low, aBuf);
            User::LeaveIfError( textArrayOfTheDay.Insert(
                (*iItemArray)[firstIndexOfTheDay + low].Mid(textOffset).Left(
                    KNotepadMaxCharactersForSort), 
                low ) );
            User::LeaveIfError(
                iKeyArray.Insert(thisKey, firstIndexOfTheDay + low) );
            }
        if ( iModelObserver && 
            iItemArray->Count() == KNotepadBuildItemArrayProgressCount )
            {
            iModelObserver->HandleNotepadModelEventL(
                MNotepadModelObserver::EProgressItemArrayChange );
            }
        prevDateTime = dateTime;
        iDbView.NextL();
        }
    CleanupStack::PopAndDestroy(2); // minutesArrayOfToday, textArrayOfTheDay
    }

// -----------------------------------------------------------------------------
// CNotepadModel::DoBuildTemplateItemArrayL
// -----------------------------------------------------------------------------
//
void CNotepadModel::DoBuildTemplateItemArrayL(
    TPtr& aBuf)
    {
    iDbView.FirstL();
    while (iDbView.AtRow())
        {
        iDbView.GetL();
        TInt thisKey(iDbView.ColInt(ENotepadKey));
        aBuf.SetLength(0);
        switch (iListingStyle)
            {
            case ENotepadListingStyleMC:
                aBuf.Append(KNotepadMceFirstColumnData);
                aBuf.Append(KColumnListSeparator);
                // pass through into next case
            case ENotepadListingStyleFetchMC: // fetch template
                AppendContentAsLabelL(aBuf, iDbView);
                break;
            default:
                break;
            }
        // INSERT POINT
        iItemArray->AppendL(aBuf);
        User::LeaveIfError(iKeyArray.Append(thisKey));
        if ( iModelObserver && 
            iItemArray->Count() == KNotepadBuildItemArrayProgressCount )
            {
            iModelObserver->HandleNotepadModelEventL(
                MNotepadModelObserver::EProgressItemArrayChange );
            }
        iDbView.NextL();
        }
    }

// -----------------------------------------------------------------------------
// CNotepadModel::CNotepadModel_Reserved
// -----------------------------------------------------------------------------
//
EXPORT_C void CNotepadModel::CNotepadModel_Reserved()
    {
    }

// -----------------------------------------------------------------------------
// CNotepadModel::SetItemArrayFlags
// -----------------------------------------------------------------------------
//
void CNotepadModel::SetItemArrayFlags()
	{
	if(iFlag&ENotepadItemArrayUpdateOnly)
		{

		iFlag |= ENotepadRequireDbViewUpdate;
		iFlag |= ENotepadRequireItemArrayUpdate;
		}
	}

// -----------------------------------------------------------------------------
// CNotepadModel::ItemArrayFlags
// -----------------------------------------------------------------------------
//
TUint CNotepadModel::ItemArrayFlags()
	{
	return iFlag;
	}

// -----------------------------------------------------------------------------
// CNotepadModel::CDatabaseChangeNotifier::NewL
// -----------------------------------------------------------------------------
//
CNotepadModel::CDatabaseChangeNotifier* 
    CNotepadModel::CDatabaseChangeNotifier::NewL(
    RDbDatabase& aDatabase,
    CNotepadModel& aModel )
    {
    CDatabaseChangeNotifier* self = 
        new(ELeave) CDatabaseChangeNotifier(aModel);
    CleanupStack::PushL(self);
    self->ConstructL(aDatabase);
    CleanupStack::Pop(); // self
    return self; 
    }

// -----------------------------------------------------------------------------
// CNotepadModel::CDatabaseChangeNotifier::~CDatabaseChangeNotifier
// -----------------------------------------------------------------------------
//
CNotepadModel::CDatabaseChangeNotifier::~CDatabaseChangeNotifier()
    {
    Cancel();
    iDbNotifier.Close(); 
    }

// -----------------------------------------------------------------------------
// CNotepadModel::CDatabaseChangeNotifier::CDatabaseChangeNotifier
// -----------------------------------------------------------------------------
//
CNotepadModel::CDatabaseChangeNotifier::CDatabaseChangeNotifier(
    CNotepadModel& aModel )
    :CActive(CActive::EPriorityLow), iModel(aModel)
    {
    CActiveScheduler::Add(this);
    }

// -----------------------------------------------------------------------------
// CNotepadModel::CDatabaseChangeNotifier::ConstructL
// -----------------------------------------------------------------------------
//
void CNotepadModel::CDatabaseChangeNotifier::ConstructL(
    RDbDatabase& aDatabase ) 
    { 
    User::LeaveIfError(iDbNotifier.Open(aDatabase)); 
    iDbNotifier.NotifyChange(iStatus);
    SetActive();
    }

// -----------------------------------------------------------------------------
// CNotepadModel::CDatabaseChangeNotifier::DoCancel
// -----------------------------------------------------------------------------
//
void CNotepadModel::CDatabaseChangeNotifier::DoCancel()
    { 
    iDbNotifier.Cancel(); 
    }

// -----------------------------------------------------------------------------
// CNotepadModel::CDatabaseChangeNotifier::RunL
// -----------------------------------------------------------------------------
//
void CNotepadModel::CDatabaseChangeNotifier::RunL()
    {
    // enum TEvent {EClose,EUnlock,ECommit,ERollback,ERecover};
    TInt status(iStatus.Int());
    TInt err(KErrNone);
    if (status == RDbNotifier::ECommit)
        {
        TRAP(err, iModel.HandleDatabaseChangedL(status););
        }
    // Restart this even if err != KErrNone
    iDbNotifier.NotifyChange(iStatus);
    SetActive();
    User::LeaveIfError(err);
    }

// End of File  
