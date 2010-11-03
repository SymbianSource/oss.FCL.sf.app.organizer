/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: Implementation for meeting request utils calendar    
*              database manager
*/



// ----------------------------------------------------------------------------
// INCLUDE FILES
// ----------------------------------------------------------------------------
//
#include "CMRUtilsCalDbMgr.h"
#include "CMRUtilsCalDbNormal.h"
#include "CMRUtilsCalDbTombs.h"
#include "CMRUtilsCmdIterationAO.h"
#include "CMRUtilsGraveyardCleaner.h"
#include "MREntryConsultant.h"
#include "MRHelpers.h"
#include "ICalUILog.h"
#include "CMRUtilsInternal.h"
#include <ct/RCPointerArray.h>
#include <CalSession.h>
#include <CalEntryView.h>
#include <CalInstance.h>
#include <CalInstanceView.h>
#include <CalEntry.h>
#include <CalCommon.h>
#include <CalUser.h>
#include <CalenInterimUtils2.h>

// CONSTANTS
/// Unnamed namespace for local definitions
namespace {

const TInt KUidListGranularity = 10;

enum TPanicCode
    {
    EPanicIllegalFetchParams = 1,
    EPanicAccessedWhenUnavailable,
    EPanicAsyncOpAlreadyExists,
    EPanicIllegalEntryStatus,
    EPanicIllegalResurrect,
    EPanicUnexpectedUidValue
    };

_LIT( KPanicMsg, "CMRUtilsCalDbMgr" );

void Panic( TPanicCode aReason )
    {
    User::Panic( KPanicMsg, aReason );
    }
    
}  // namespace

// ----------------------------------------------------------------------------
// MEMBER FUNCTIONS
// ----------------------------------------------------------------------------
//

// ----------------------------------------------------------------------------
// CMRUtilsCalDbMgr::NewL
// ----------------------------------------------------------------------------
//
CMRUtilsCalDbMgr* CMRUtilsCalDbMgr::NewL(
    CCalSession& aCalSession,
    MMRUtilsObserver& aObserver )
	{
	CMRUtilsCalDbMgr* self = new( ELeave ) CMRUtilsCalDbMgr( aCalSession,
	                                                         aObserver );
	CleanupStack::PushL( self );
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}

// ----------------------------------------------------------------------------
// CMRUtilsCalDbMgr::CMRUtilsCalDbMgr
//
// Constructor.
// ----------------------------------------------------------------------------
//
CMRUtilsCalDbMgr::CMRUtilsCalDbMgr(
    CCalSession& aCalSession,
    MMRUtilsObserver& aObserver )
    : iCurrentAsyncOp( CMRUtils::ENoOperation ),
      iCalSession( aCalSession ),
      iObserver( aObserver )    
    {    
    }
    
// ----------------------------------------------------------------------------
// CMRUtilsCalDbMgr::~CMRUtilsCalDbMgr
//
// Destructor.
// ----------------------------------------------------------------------------
//        
CMRUtilsCalDbMgr::~CMRUtilsCalDbMgr()
    {
    delete iGraveyardCleaner;
    delete iCmdIterator;
    delete iNormalDb;
    delete iTombsDb;
    }
    
// ----------------------------------------------------------------------------
// CMRUtilsCalDbMgr::ConstructL
// ----------------------------------------------------------------------------
//    
void CMRUtilsCalDbMgr::ConstructL()
    {
    iNormalDb = CMRUtilsCalDbNormal::NewL( iCalSession, *this, *this );
    iTombsDb = CMRUtilsCalDbTombs::NewL( *this, *this );    
    iCmdIterator = CMRUtilsCmdIterationAO::NewL( *this );
    }

// ----------------------------------------------------------------------------
// CMRUtilsCalDbMgr::HandleCalDbStatus
// ----------------------------------------------------------------------------
//    
void CMRUtilsCalDbMgr::HandleCalDbStatus(
    const CMRUtilsCalDbBase* /*aNotifier*/,
    MMRUtilsCalDbObserver::TDbStatus aStatus )
    {
    // If there is any change in db status, we must stop cleaning:
    delete iGraveyardCleaner;
    iGraveyardCleaner = NULL;
    
    if ( aStatus == EReseting )
        { // reset db is ongoing, engine becomes unavailable
        iObserver.HandleCalEngStatus( MMRUtilsObserver::ENotReady );
        }
    else if ( iNormalDb->DbStatus() == MMRUtilsCalDbObserver::EFinishedOk &&
              iTombsDb->DbStatus() == MMRUtilsCalDbObserver::EFinishedOk )
        {
        iObserver.HandleCalEngStatus( MMRUtilsObserver::EAvailable );        
        // Both databases are available so we can start idle cleanup
        // We can ignore the error if it fails, try again next time
        TRAP_IGNORE(
            iGraveyardCleaner =
                CMRUtilsGraveyardCleaner::NewL( *iNormalDb, *iTombsDb );
            iGraveyardCleaner->StartIdleCleanupL( Time::NullTTime() );
            )
        }
    else if ( iNormalDb->DbStatus() == MMRUtilsCalDbObserver::EFinishedOk &&
              iTombsDb->DbStatus() == MMRUtilsCalDbObserver::EFinishedError )
        {
        iObserver.HandleCalEngStatus(
            MMRUtilsObserver::EAvailableWithoutTombs );
        }
    else if ( iNormalDb->DbStatus() == MMRUtilsCalDbObserver::EFinishedError )
        {
        iObserver.HandleCalEngStatus( MMRUtilsObserver::ENotAvailable );
        }
        
    // In other cases either db is not yet finished        
    }
    
// ----------------------------------------------------------------------------
// CMRUtilsCalDbMgr::Progress
// ----------------------------------------------------------------------------
//
void CMRUtilsCalDbMgr::Progress( TInt /*aPercentageCompleted*/ )
    {
    // Not interested in progress currently
    }
    
// ----------------------------------------------------------------------------
// CMRUtilsCalDbMgr::NotifyProgress
// ----------------------------------------------------------------------------
//
TBool CMRUtilsCalDbMgr::NotifyProgress()
    {
    // Not interested in progress currently
    return EFalse;
    }

// ----------------------------------------------------------------------------
// CMRUtilsCalDbMgr::Completed
// ----------------------------------------------------------------------------
//
void CMRUtilsCalDbMgr::Completed( TInt aError )
    {
    iObserver.HandleOperation( iCurrentAsyncOp, 100, aError );
    iCurrentAsyncOp = CMRUtils::ENoOperation;
    }

// ----------------------------------------------------------------------------
// CMRUtilsCalDbMgr::NormalDbEntryView
// ----------------------------------------------------------------------------
//
CCalEntryView* CMRUtilsCalDbMgr::NormalDbEntryView()
    { // NormalDbEntryView will only be called if engine is available
    return iNormalDb->EntryView();
    }
     
// ----------------------------------------------------------------------------
// CMRUtilsCalDbMgr::NormalDbInstanceView
// ----------------------------------------------------------------------------
//
CCalInstanceView* CMRUtilsCalDbMgr::NormalDbInstanceView()
    { // NormalDbInstanceView will only be called if engine is available
    return iNormalDb->InstanceView();
    }

// ----------------------------------------------------------------------------
// CMRUtilsCalDbMgr::FetchEntriesL
// ----------------------------------------------------------------------------
//
void CMRUtilsCalDbMgr::FetchEntriesL(
    RPointerArray<CCalEntry>& aCalEntryArray,
    const CalCommon::TCalTimeRange& aTimeRange )
    {
    __ASSERT_DEBUG( aCalEntryArray.Count() == 0,
                    Panic( EPanicIllegalFetchParams ) );
    aCalEntryArray.ResetAndDestroy();
    // caller definitely wants to get full copies -> ETrue as 3rd argument:
    iNormalDb->FetchWithRangeL( aCalEntryArray,
                                aTimeRange,
                                ETrue );    
    }

// ----------------------------------------------------------------------------
// CMRUtilsCalDbMgr::FetchEntryL
// ----------------------------------------------------------------------------
//
CCalEntry* CMRUtilsCalDbMgr::FetchEntryL(
    const TDesC8& aUid,
    const TCalTime& aRecurrenceId )
    {
    __ASSERT_DEBUG( aUid.Length() > 0,
                    Panic( EPanicIllegalFetchParams ) );
    
    RCPointerArray<CCalEntry> tmpFetchArray;
    CleanupClosePushL( tmpFetchArray ); 
    
    CCalEntry* retVal = NULL;

    TInt index( -1 );
    if ( EntryExistsInDbL( aUid,
                           aRecurrenceId,
                           *iNormalDb,
                           tmpFetchArray,
                           index ) )
        {        
        retVal = tmpFetchArray[index];        
        tmpFetchArray.Remove( index ); // ownership transferred to retVal
        }        
        
    CleanupStack::PopAndDestroy(); // tmpFetchArray
    return retVal;
    }        

// ----------------------------------------------------------------------------
// CMRUtilsCalDbMgr::StoreEntryL
// ----------------------------------------------------------------------------
//          
TInt CMRUtilsCalDbMgr::StoreEntryL(
    CCalEntry& aCalEntry,
    TBool aToNormalDb )
    {
    TInt retVal( KErrNone );   
    
    CMRUtilsCalDbBase* dbToStore = aToNormalDb ? iNormalDb : iTombsDb;    
    CCalenInterimUtils2::StoreL( *( dbToStore->EntryView() ),
                                 aCalEntry,
                                 ETrue );
    
    return retVal;
    }        

// ----------------------------------------------------------------------------
// CMRUtilsCalDbMgr::UpdateEntryL
// ----------------------------------------------------------------------------
//          
TInt CMRUtilsCalDbMgr::UpdateEntryL( const CCalEntry& aCalEntry )
    {
    RPointerArray<CCalEntry> calEntryTmpArray( 1 ); // entry not own
    CleanupClosePushL( calEntryTmpArray ); 
    calEntryTmpArray.AppendL( &aCalEntry );
    TInt numSuccessfulEntry( 0 );
    // entry view surely exists when status is 'available':
    iNormalDb->EntryView()->UpdateL( calEntryTmpArray, numSuccessfulEntry );
    TInt retVal( numSuccessfulEntry == 1 ? KErrNone : KErrGeneral );
    CleanupStack::PopAndDestroy(); // calEntryTmpArray, only close array
    return retVal;    
    }        

// ----------------------------------------------------------------------------
// CMRUtilsCalDbMgr::DeleteEntryL
// ----------------------------------------------------------------------------
//          
TInt CMRUtilsCalDbMgr::DeleteEntryL( const TDesC8& aUid )
    {  
    CDesC8ArrayFlat* uidArray = new( ELeave ) CDesC8ArrayFlat( 1 );
    CleanupStack::PushL( uidArray );
    uidArray->AppendL( aUid );
    // entry view surely exists when status is 'available':
    iNormalDb->EntryView()->DeleteL( *uidArray );
    CleanupStack::PopAndDestroy( uidArray );
    return KErrNone;    
    }        

// ----------------------------------------------------------------------------
// CMRUtilsCalDbMgr::DeleteEntryL
// ----------------------------------------------------------------------------
//          
TInt CMRUtilsCalDbMgr::DeleteEntryL( const TCalLocalUid& aLocalUid )
    {
    RArray<TCalLocalUid> localUidArray( 1 );
    CleanupClosePushL( localUidArray ); 
    localUidArray.AppendL( aLocalUid );
    TInt numSuccessfulEntry( 0 );
    iNormalDb->EntryView()->DeleteL( localUidArray, numSuccessfulEntry );
    TInt retVal( numSuccessfulEntry == 1 ? KErrNone : KErrGeneral );
    CleanupStack::PopAndDestroy(); // localUidArray
    return retVal;
    }
     
// ----------------------------------------------------------------------------
// CMRUtilsCalDbMgr::ResetTombsFileL
// ----------------------------------------------------------------------------
//          
void CMRUtilsCalDbMgr::ResetTombsFileL()
    {
    iTombsDb->ResetDbL();
    }
    
// ----------------------------------------------------------------------------
// CMRUtilsCalDbMgr::DeleteTombstonesL
// ----------------------------------------------------------------------------
//          
void CMRUtilsCalDbMgr::DeleteTombstonesL(
    const CalCommon::TCalTimeRange& aCalTimeRange )
    {
    __ASSERT_DEBUG( iCurrentAsyncOp == CMRUtils::ENoOperation,
                    Panic( EPanicAsyncOpAlreadyExists ) );
    iTombsDb->EntryView()->DeleteL( aCalTimeRange,
                                    CalCommon::EIncludeAppts,
                                    *this );
    iCurrentAsyncOp = CMRUtilsInternal::EDeleteTombstones;
    }

// ----------------------------------------------------------------------------
// CMRUtilsCalDbMgr::StoreEntryCondL
// aResurrect can only be ETrue if aCheckOnly is EFalse, i.e. when user is
// really trying to store something.
// ----------------------------------------------------------------------------
//          
MMRUtilsTombsExt::TMRUtilsDbResult CMRUtilsCalDbMgr::StoreEntryCondL(
    CCalEntry& aCalEntry,
    TBool aResurrect,
    TBool aCheckOnly )
    {    
    RCPointerArray<CCalEntry> tmpFetchArray;
    CleanupClosePushL( tmpFetchArray );
        
    MMRUtilsTombsExt::TMRUtilsDbResult entryStatus(
        MMRUtilsTombsExt::EUndefined );
    TInt index( -1 );
    
    // 1. Normal db part, if entry is found in there then tombstones aren't
    //    checked at all:
    
    if ( EntryExistsInDbL( aCalEntry, *iNormalDb, tmpFetchArray, index ) )
        { // Entry exists in normal db
        entryStatus = EvaluateExistingEntryL( aCalEntry,
                                              *( tmpFetchArray[index] ) );
        if ( entryStatus == MMRUtilsTombsExt::ECheckedValidUpdate &&
             !aCheckOnly ) 
            {
            StoreEntryL( aCalEntry );
            entryStatus = MMRUtilsTombsExt::EStoredUpdate;
            }
        else if ( entryStatus == MMRUtilsTombsExt::EErrorCancelled &&
                  aResurrect ) 
            {
            __ASSERT_DEBUG( !aCheckOnly, Panic( EPanicIllegalResurrect ) );
            StoreEntryL( aCalEntry );
            entryStatus = MMRUtilsTombsExt::EResurrectedCancelled;
            }            
        // entry can't be new if it exists in the db already:            
        __ASSERT_DEBUG( entryStatus != MMRUtilsTombsExt::ECheckedValidNew,
                        Panic( EPanicIllegalEntryStatus) );                        
        }
        
    // 2. Tombstone db part (omitted if tombs db unavailable):
        
    else if ( iTombsDb->DbStatus() == MMRUtilsCalDbObserver::EFinishedOk &&
              EntryExistsInDbL( aCalEntry, *iTombsDb, tmpFetchArray, index ) )
        { // Entry has been deleted and is a tombstone now
        entryStatus = EvaluateExistingEntryL( aCalEntry,
                                              *( tmpFetchArray[index] ) );
                                              
                                              
        if ( entryStatus == MMRUtilsTombsExt::ECheckedValidUpdate &&
             !aCheckOnly )
            { // Older entry exists in tombstone db, our policy is that
              // updates are still accepted
            if ( MREntryConsultant::IsModifyingEntryL( aCalEntry ) &&
                 !OriginatingExistInDbL( aCalEntry ) )
                { // originating not found in normal db, so modifying entry
                  // is "illegal", cannot be saved
                entryStatus = MMRUtilsTombsExt::EErrorRecurrence;
                }
            else
                {                
                StoreEntryL( aCalEntry );
                entryStatus = MMRUtilsTombsExt::EStoredUpdate;
                // Try to delete this particular tombstone, any further
                // tombstone cleanup is taken care by a separate class:         
                TRAP_IGNORE( iTombsDb->EntryView()->DeleteL( aCalEntry ) );
                }
            }
        else if ( entryStatus == MMRUtilsTombsExt::EErrorIdenticalExists )
            { // Identical entry exists in tombstone db
            if ( aResurrect )
                {
                __ASSERT_DEBUG( !aCheckOnly, Panic( EPanicIllegalResurrect ) );
                if ( MREntryConsultant::IsModifyingEntryL( aCalEntry ) &&
                     !OriginatingExistInDbL( aCalEntry ) )
                    { // originating not found in normal db, so modifying entry
                      // is "illegal", cannot be resurrected
                    entryStatus = MMRUtilsTombsExt::EErrorRecurrence;
                    }
                else
                    {                    
                    StoreEntryL( aCalEntry );
                    entryStatus = MMRUtilsTombsExt::EResurrectedDeleted;                                    
                    // Try to delete this particular tombstone, any further
                    // tombstone cleanup is taken care by a separate class:         
                    TRAP_IGNORE( iTombsDb->EntryView()->DeleteL( aCalEntry ) );
                    }
                }
            else
                {                                
                entryStatus = MMRUtilsTombsExt::EErrorHasBeenDeleted;
                }
            }
        // entry can't be new if it exists in the db already:            
        __ASSERT_DEBUG( entryStatus != MMRUtilsTombsExt::ECheckedValidNew,
                        Panic( EPanicIllegalEntryStatus) );                        
        }
        
    // 3. New entry in this phone:
        
    else
        { // Completely new entry (or tombstone has disappeared)
        entryStatus = EvaluateNewEntryL( aCalEntry );
        if ( entryStatus == MMRUtilsTombsExt::ECheckedValidNew && !aCheckOnly )
            {
            StoreEntryL( aCalEntry );
            entryStatus = MMRUtilsTombsExt::EStoredNew;
            }
        
        // entry can't be update if doesn't exist in the db already:        
        __ASSERT_DEBUG( entryStatus != MMRUtilsTombsExt::ECheckedValidUpdate,
                        Panic( EPanicIllegalEntryStatus) );         
        }
        
    CleanupStack::PopAndDestroy(); // tmpFetchArray
    return entryStatus;        
    }

// ----------------------------------------------------------------------------
// CMRUtilsCalDbMgr::DeleteEntryCondL
// ----------------------------------------------------------------------------
//          
void CMRUtilsCalDbMgr::DeleteEntryCondL( const TDesC8& aUid )
    {
    RCPointerArray<CCalEntry> tmpFetchArray;
    CleanupClosePushL( tmpFetchArray );
    
    // 1. Delete all found entries from normal db:
    
    iNormalDb->EntryView()->FetchL( aUid, tmpFetchArray );
    DeleteEntryL( aUid );

    // 2. Add them to tombstone db, if this fails then no tombstone gets
    // created, but that isn't a major problem:
            
    if ( iTombsDb->DbStatus() == MMRUtilsCalDbObserver::EFinishedError )
        { // if tombstone db isn't available then we omit it silently          
        return;
        }
            
    TInt count( tmpFetchArray.Count() );
    for ( TInt i( 0 ); i < count; ++i )
        {
// TODO: currently child entry uid cannot be read anymore after it has been
// deleted from the db, this might be a bug in CalInterimApi. Check later
// if it works, if doesn't we could e.g. make a copy of each entry before
// deleting it and delete and store one at a time, parent as the last one.
// See also DeleteEntryCondL( const CCalEntry& aCalEntry ) variant below.       
        CCalEntry* tombstone = MRHelpers::CopyEntryLC( *( tmpFetchArray[i] ),
                                                tmpFetchArray[i]->MethodL(),
                                                MRHelpers::ECopySkeleton );
        // store to iTombsDb, parent will be added first (it has index 0):
        // don't care if creating a tombstone would fail
        TRAP_IGNORE( StoreEntryL( *tombstone, EFalse ) );
        CleanupStack::PopAndDestroy( tombstone );        
        }
    
    CleanupStack::PopAndDestroy(); // tmpFetchArray
    }

// ----------------------------------------------------------------------------
// CMRUtilsCalDbMgr::DeleteEntryCondL
// ----------------------------------------------------------------------------
//          
void CMRUtilsCalDbMgr::DeleteEntryCondL( const TCalLocalUid& aLocalUid )
    {
    CCalEntry* entry = iNormalDb->EntryView()->FetchL( aLocalUid );
    if ( entry )
        {
        CleanupStack::PushL( entry );
        DeleteEntryCondL( *entry );
        CleanupStack::PopAndDestroy( entry );
        }
    }

// ----------------------------------------------------------------------------
// CMRUtilsCalDbMgr::DeleteEntryCondL
// ----------------------------------------------------------------------------
//          
void CMRUtilsCalDbMgr::DeleteEntryCondL( const CCalEntry& aCalEntry )
    {        
    if ( MREntryConsultant::IsModifyingEntryL( aCalEntry ) )
        { // Modifying entry
        
        // 1. Delete entry from normal db:

// TODO: see above the problem of storing children to tombs db        

        iNormalDb->EntryView()->DeleteL( aCalEntry );
        
        // 2. Add it to tombstone db, if this fails then no tombstone gets
        // created, but that isn't a major problem:
        
        if ( iTombsDb->DbStatus() == MMRUtilsCalDbObserver::EFinishedError )
            { // if tombstone db isn't available then we omit it silently          
            return;
            }
        
        // We must ensure that originating entry exists in tombstone db,
        // but if it exists already it mustn't be stored again since that
        // would automatically clean up any related modifying entries:        
        TInt dummyIndex( -1 ); // unused but required
        RCPointerArray<CCalEntry> tmpFetchArray;
        CleanupClosePushL( tmpFetchArray );        
        TBool origExistsInTombsDb = EntryExistsInDbL( aCalEntry.UidL(),
                                                      TCalTime(),
                                                      *iTombsDb,
                                                      tmpFetchArray,
                                                      dummyIndex );
        tmpFetchArray.ResetAndDestroy();
        if ( !origExistsInTombsDb )
            { // Copy originating entry from normal to tombs db:
            iNormalDb->EntryView()->FetchL( aCalEntry.UidL(), tmpFetchArray );          
            CCalEntry* orig = MRHelpers::CopyEntryLC( *( tmpFetchArray[0] ),
                                                tmpFetchArray[0]->MethodL(),
                                                MRHelpers::ECopySkeleton );
            tmpFetchArray.ResetAndDestroy();
            // to iTombsDb, don't care if creating a tombstone would fail
            TRAP_IGNORE( StoreEntryL( *orig, EFalse ) );
            CleanupStack::PopAndDestroy( orig );
            }            
        CleanupStack::PopAndDestroy(); // tmpFetchArray
            
        // Now store the modifying entry to tombstone db:            
        CCalEntry* modif = MRHelpers::CopyEntryLC( aCalEntry,
                                                   aCalEntry.MethodL(),
                                                   MRHelpers::ECopySkeleton );
        // to iTombsDb, don't care if creating a tombstone would fail           
        TRAP_IGNORE( StoreEntryL( *modif, EFalse ) );
        CleanupStack::PopAndDestroy( modif );                                
        }
        
    else
        { // Originating entry, this is the same case as deleting with GUID:
        DeleteEntryCondL( aCalEntry.UidL() );
        }            
    }

// ----------------------------------------------------------------------------
// CMRUtilsCalDbMgr::DeleteEntryCondL
// ----------------------------------------------------------------------------
//          
void CMRUtilsCalDbMgr::DeleteEntryCondL(
    const CalCommon::TCalTimeRange& aCalTimeRange )
    {
    __ASSERT_DEBUG( iCurrentAsyncOp == CMRUtils::ENoOperation,
                    Panic( EPanicAsyncOpAlreadyExists ) );    
    
    RCPointerArray<CCalEntry> tmpFetchArray;
    CleanupClosePushL( tmpFetchArray );       
    // skeleton copies are enough -> set 3rd argument to EFalse:
    iNormalDb->FetchWithRangeL( tmpFetchArray, aCalTimeRange, EFalse );
    iCmdIterator->StartCmdIterationL( tmpFetchArray, CMRUtils::EDeleteEntries );
    iCurrentAsyncOp = CMRUtils::EDeleteEntries;                                                 
    tmpFetchArray.Reset(); // ownership of entries was transferred
    CleanupStack::PopAndDestroy(); // tmpFetchArray
    }

// ----------------------------------------------------------------------------
// CMRUtilsCalDbMgr::EntryExistsInDbL
// ----------------------------------------------------------------------------
//   
TBool CMRUtilsCalDbMgr::EntryExistsInDbL(
    const TDesC8& aUid,
    const TCalTime& aRecurrenceId,
    const CMRUtilsCalDbBase& aDb,
    RPointerArray<CCalEntry>& aCalEntryArray,
    TInt& aIndex ) const
    {
    TBool retVal( EFalse );
    aIndex = -1;
    
    aDb.EntryView()->FetchL( aUid, aCalEntryArray );
    TInt count( aCalEntryArray.Count() );
    for ( TInt i( 0 ); i < count; ++i )
        {
        const CCalEntry& dbEntry( *( aCalEntryArray[i] ) );
        if ( aRecurrenceId.TimeUtcL() == dbEntry.RecurrenceIdL().TimeUtcL() )
            { // Entry was found in the db,
              // it may be either originating or modifying entry
            retVal = ETrue;
            aIndex = i;
            }
        }

    return retVal;
    }

// ----------------------------------------------------------------------------
// CMRUtilsCalDbMgr::EntryExistsInDbL
// ----------------------------------------------------------------------------
//   
inline TBool CMRUtilsCalDbMgr::EntryExistsInDbL(
    const CCalEntry& aEntry,
    const CMRUtilsCalDbBase& aDb,
    RPointerArray<CCalEntry>& aCalEntryArray,
    TInt& aIndex ) const
    {
    return EntryExistsInDbL( aEntry.UidL(),
                             aEntry.RecurrenceIdL(),
                             aDb, 
                             aCalEntryArray,
                             aIndex );
    }

// ----------------------------------------------------------------------------
// CMRUtilsCalDbMgr::EvaluateExistingEntryL
// ----------------------------------------------------------------------------
//                                       
MMRUtilsTombsExt::TMRUtilsDbResult CMRUtilsCalDbMgr::EvaluateExistingEntryL(
    const CCalEntry& aEntry,
    const CCalEntry& aDbEntry ) const
    {
    __ASSERT_DEBUG( aEntry.UidL() == aEntry.UidL(),
                    Panic( EPanicUnexpectedUidValue ) );
    MMRUtilsTombsExt::TMRUtilsDbResult retVal( MMRUtilsTombsExt::EUndefined );
    
    TInt seq( aEntry.SequenceNumberL() );
    TInt dbSeq( aDbEntry.SequenceNumberL() );
    TTime stamp = aEntry.DTStampL().TimeUtcL();
    TTime dbStamp = aDbEntry.DTStampL().TimeUtcL();
    
    #ifdef ICALUI_ENABLE_LOGGING
    
        LOG("CMRUtilsCalDbMgr::EvaluateExistingEntryL");
        LOG1( "CMRUtilsCalDbMgr::EvaluateExistingEntryL, new entry seq no: %d", seq );
        TBuf<KLogTimeFormatLength> timeStamp;
    	TDateTime dateTime;
    	dateTime = stamp.DateTime();
    	timeStamp.Format( KLogTimeFormat,
                          dateTime.Hour(),
                          dateTime.Minute(),
                          dateTime.Second(),
                          dateTime.MicroSecond() );
        LOG1("CMRUtilsCalDbMgr::EvaluateExistingEntryL, new entry DTStamp: %S", &timeStamp );
        LOG1( "CMRUtilsCalDbMgr::EvaluateExistingEntryL, existing entry seq no: %d", dbSeq );
        dateTime = dbStamp.DateTime();
    	timeStamp.Format( KLogTimeFormat,
                          dateTime.Hour(),
                          dateTime.Minute(),
                          dateTime.Second(),
                          dateTime.MicroSecond() );
        LOG1( "CMRUtilsCalDbMgr::EvaluateExistingEntryL, existing entry DTStamp: %S", &timeStamp );
    
    #endif // ICALUI_ENABLE_LOGGING    
        
    if ( ( seq >= dbSeq ) && ( stamp >= dbStamp ) )
        { 
        if ( ( seq == dbSeq ) && ( stamp == dbStamp ) )
            {
            retVal = MMRUtilsTombsExt::EErrorIdenticalExists;
            }
            
        else if ( aDbEntry.StatusL() == CCalEntry::ECancelled )
            {
            retVal = MMRUtilsTombsExt::EErrorCancelled;
            }
        else
            {                
            retVal = MMRUtilsTombsExt::ECheckedValidUpdate;
            }
        }
    else
        {
        retVal = MMRUtilsTombsExt::EErrorObsolete;
        }
        
    return retVal;
    }

// ----------------------------------------------------------------------------
// CMRUtilsCalDbMgr::EvaluateNewEntryL
// This method is only used with iNormalDb
// ----------------------------------------------------------------------------
//
MMRUtilsTombsExt::TMRUtilsDbResult CMRUtilsCalDbMgr::EvaluateNewEntryL(
    const CCalEntry& aEntry ) const
    {
    MMRUtilsTombsExt::TMRUtilsDbResult retVal( MMRUtilsTombsExt::EUndefined );
        
    if ( MREntryConsultant::IsModifyingEntryL( aEntry ) &&
         !IsValidNewModL( aEntry ) )
        { // recurrence id does not match to any instance of an entry
        retVal = MMRUtilsTombsExt::EErrorRecurrence;
        }
    else
        {
        retVal = MMRUtilsTombsExt::ECheckedValidNew;
        }
        
    return retVal;        
    }
    
// ----------------------------------------------------------------------------
// CMRUtilsCalDbMgr::EvaluateNewEntryL
// This method is only used with iNormalDb
// ----------------------------------------------------------------------------
//
TBool CMRUtilsCalDbMgr::IsValidNewModL( const CCalEntry& aModEntry ) const
    {
    TBool retVal( EFalse );    
    RCPointerArray<CCalInstance> instances; // takes ownership
    CleanupClosePushL( instances );
    TCalTime recurrenceId( aModEntry.RecurrenceIdL() );
    CalCommon::TCalTimeRange range( recurrenceId, recurrenceId );
    
    iNormalDb->InstanceView()->FindInstanceL( instances, 
                                              CalCommon::EIncludeAppts, 
					   			              range );
    TInt count( instances.Count() );
    for ( TInt i( 0 ); i < count; ++i )
        {
        
        #ifdef ICALUI_ENABLE_LOGGING
        
            LOG("CMRUtilsCalDbMgr::IsValidNewModL");        
        	TDateTime dateTime;
            TBuf<KLogTimeFormatLength> timeStamp;        	                    
            
        	dateTime = recurrenceId.TimeUtcL().DateTime();            
        	timeStamp.Format( KLogTimeFormat,
                              dateTime.Hour(),
                              dateTime.Minute(),
                              dateTime.Second(),
                              dateTime.MicroSecond() );            
            LOG1( "CMRUtilsCalDbMgr::IsValidNewModL, mod entry uid: %S",
                  &( aModEntry.UidL() ) );                
            LOG1( "CMRUtilsCalDbMgr::IsValidNewModL, mod entry recurrence: %S",
                  &timeStamp );
            
        	dateTime = instances[i]->StartTimeL().TimeUtcL().DateTime();            
        	timeStamp.Format( KLogTimeFormat,
                              dateTime.Hour(),
                              dateTime.Minute(),
                              dateTime.Second(),
                              dateTime.MicroSecond() ); 
            LOG1( "CMRUtilsCalDbMgr::IsValidNewModL, instance uid: %S",
                  &( instances[i]->Entry().UidL() ) );
            LOG1( "CMRUtilsCalDbMgr::IsValidNewModL,instance recurrence: %S",
                  &timeStamp );
        
        #endif // ICALUI_ENABLE_LOGGING    
                
        // When creating a new modifying entry, recurrence id must match
        // to an existing instance start time (later on start time may get
        // modified whereas recurrence id remains unchanged).
        if ( instances[i]->Entry().UidL() == aModEntry.UidL() &&
             instances[i]->StartTimeL().TimeUtcL() == recurrenceId.TimeUtcL() )
            {
            retVal = ETrue;
            break; // match found, ready to return
            }
        }
    CleanupStack::PopAndDestroy(); // instances, delete array items
    return retVal;
    }


TBool CMRUtilsCalDbMgr::OriginatingExistInDbL( const CCalEntry& aModEntry )
    {
    TInt retVal( EFalse );
    CCalEntry* dbEntry = FetchEntryL( aModEntry.UidL(), TCalTime() );
    if ( dbEntry )
        {
        delete dbEntry;
        retVal = ETrue;
        }    
    return retVal;    
    }
    
// End of file
