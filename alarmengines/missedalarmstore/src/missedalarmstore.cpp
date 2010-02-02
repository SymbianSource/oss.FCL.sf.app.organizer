/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   missed alarm store class description
*
*/



//Include files

#include "missedalarmstore.h"
#include "missedalarm.h"
#include "missedalarmstoreconstants.h"
#include "alarmenginesdebug.h"

// ----------------------------------------------------------------------------
// CMissedAlarmStore::NewLc
// 1st phase of construction
// ----------------------------------------------------------------------------
//
EXPORT_C CMissedAlarmStore* CMissedAlarmStore::NewLC()
    {
    TRACE_ENTRY_POINT;
    
    CMissedAlarmStore* self = new (ELeave) CMissedAlarmStore();
    CleanupStack::PushL(self);
    
    TRACE_EXIT_POINT;
    return self;
    }

// ----------------------------------------------------------------------------
// CMissedAlarmStore::NewL
// 1st phase of construction
// ----------------------------------------------------------------------------
//
EXPORT_C CMissedAlarmStore* CMissedAlarmStore::NewL(CRepository& aRepository)
    {
    TRACE_ENTRY_POINT;
    
    CMissedAlarmStore* self=new (ELeave) CMissedAlarmStore(aRepository);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    
    TRACE_EXIT_POINT;
    return self;
    }

// ----------------------------------------------------------------------------
// CMissedAlarmStore::~CMissedAlarmStore
// Class destructor.Frees all resources used by this class
// ----------------------------------------------------------------------------
CMissedAlarmStore::~CMissedAlarmStore()
	{
	TRACE_ENTRY_POINT;
	
	if(iRepository)
	    {
	    delete iRepository;
	    iRepository = NULL;
	    }
	
	TRACE_EXIT_POINT;
	}

// ----------------------------------------------------------------------------
// CMissedAlarmStore::CMissedAlarmStore
// Default constructor
// ----------------------------------------------------------------------------
//
CMissedAlarmStore::CMissedAlarmStore()
    {
    TRACE_ENTRY_POINT;
    
    TRAP_IGNORE(iRepository = CRepository::NewL(KUidRepository));
    
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CMissedAlarmStore::CMissedAlarmStore
// Default constructor
// ----------------------------------------------------------------------------
//
CMissedAlarmStore::CMissedAlarmStore(CRepository& aRepository)
    : iRepository(&aRepository)
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CMissedAlarmStore::ConstructL
// Second stage constructor
// ----------------------------------------------------------------------------
//
void CMissedAlarmStore::ConstructL()
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CMissedAlarmStore::AddL
// Add the list of CMissedAlarm in the Central Repository.
// ----------------------------------------------------------------------------
//
EXPORT_C void CMissedAlarmStore::AddL(RPointerArray<CMissedAlarm>& aMissedAlarmList)
	{
	TRACE_ENTRY_POINT;
	
	TUint count = aMissedAlarmList.Count();
	for( TUint i = 0; i < count; i++)
		{
		CMissedAlarm& missedalarm = *(aMissedAlarmList[i]);
		SetL( missedalarm.iLuid, missedalarm.iInstanceTime, missedalarm.iCalFileName );
		}
	
	TRACE_EXIT_POINT;
	}

// ----------------------------------------------------------------------------
// CMissedAlarmStore::GetL
// Gets the list of CMissedAlarm from the Central Repository.
// ----------------------------------------------------------------------------
//
EXPORT_C void CMissedAlarmStore::GetL(RPointerArray<CMissedAlarm> &aArray)
	{
	TRACE_ENTRY_POINT;
	
	RArray<TUint32> foundIds;
	TUint32 key;
	TInt err = 0;
	CMissedAlarm *missedalarm = NULL;
    TReal instanceTime;
    TInt luid;
    TBuf<256> calFileName;
	err = iRepository->FindL( KLuidPartialKey, KLuidMask, foundIds);
	
	TUint count = foundIds.Count();
	if( count && KErrNone == err)
	    {
	    for(TUint32 i =0; i < count; i++)
		    {
            key = foundIds[i];
            err = iRepository->Get( key, luid);
            if( KErrNone == err )
	            {
	            err = iRepository->Get( key | KTTimeShift, instanceTime );
	            if( KErrNone == err )
		            {
		            err = iRepository->Get( key | KCalFileNameShift, calFileName );
		            if( KErrNone == err )
		                {
                        missedalarm = CMissedAlarm::NewL( luid,
                                                        TTime(TInt64(instanceTime)), calFileName );
                        CleanupStack::PushL( missedalarm );
                        aArray.Append( missedalarm );
                        CleanupStack::Pop(missedalarm );
		                }
		            }
	            }
		    }
	    }
    foundIds.Reset();
    TRACE_EXIT_POINT;
	}

// ----------------------------------------------------------------------------
// CMissedAlarmStore::RemoveAllL
// Removes all the entries from the central repository.
// ----------------------------------------------------------------------------
//
EXPORT_C void CMissedAlarmStore::RemoveAllL()
	{
	TRACE_ENTRY_POINT;
	
	RArray<TUint32> foundIds;
	TUint32 key;
	TInt err = 0;
	err = iRepository->FindL( KLuidPartialKey, KLuidMask, foundIds);
	
	TUint count = foundIds.Count();
	if( count && KErrNone == err)
	    {
	    for(TInt i =0; i < count; i++)
		    {
            key = foundIds[i];
            iRepository->Delete(key);
            iRepository->Delete(key | KTTimeShift);
            iRepository->Delete(key | KCalFileNameShift);
		    }
	    }
    foundIds.Reset();
    
    TRACE_EXIT_POINT;
	}

// ----------------------------------------------------------------------------
// CMissedAlarmStore::RemoveL
// Removes an entry from the central repository.
// ----------------------------------------------------------------------------
//
EXPORT_C void CMissedAlarmStore::RemoveL( CMissedAlarm& aMissedAlarm )
	{
	TRACE_ENTRY_POINT;
	
	RArray<TUint32> foundIds;
	TUint32 key;
	TInt err = 0;
	err = iRepository->FindEqL( KLuidPartialKey, KLuidMask, 
	                            aMissedAlarm.iLuid, foundIds );

	TUint count = foundIds.Count();
	TBool remAllLUids = EFalse;
	if( count && KErrNone == err)
	    {
		TReal instTime;
		TReal aInstTime = TReal( (aMissedAlarm.iInstanceTime).Int64());
		TBuf<256> calFileName;
		for( TUint j = 0; j < count; j++ )
			{
			key = foundIds[j];
			err = iRepository->Get( key | KTTimeShift, instTime );

			if( remAllLUids || 
			    ( KErrNone == err && aInstTime == instTime ))
				{
				err = iRepository->Get( key | KCalFileNameShift, calFileName );
				if( KErrNone == err && !aMissedAlarm.iCalFileName.CompareF( calFileName ) )
				    {
	                DeleteEntryAndReArrangeL( key );
	                foundIds.Reset();
				    }
				return;
				}
			}
	    }
    foundIds.Reset();
    
    TRACE_EXIT_POINT;
	}

// ----------------------------------------------------------------------------
// CMissedAlarmStore::CountL
// Counts the number of entries in the central repository.
// ----------------------------------------------------------------------------
//
EXPORT_C void CMissedAlarmStore::CountL(TUint32 &aCount)
	{
	TRACE_ENTRY_POINT;
	
	RArray<TUint32> foundIds;
	TInt err = 0;
	err = iRepository->FindL( KLuidPartialKey, KLuidMask, foundIds);

	if( KErrAbort == err || KErrPermissionDenied == err )
		{
		User::Leave( err );
		}

	aCount = foundIds.Count();
    foundIds.Reset();
    
    TRACE_EXIT_POINT;
	}

// ----------------------------------------------------------------------------
// CMissedAlarmStore::SetL
// Add the single entry in the Central Repository.
// ----------------------------------------------------------------------------
//
void CMissedAlarmStore::SetL( const TInt aLuid, const TTime aInstanceTime, const TDesC& aCalFileName)
	{
	TRACE_ENTRY_POINT;
	
	TUint32 keyId;
	RArray<TUint32> foundIds;
    TInt err = 0;
	err = iRepository->FindEqL( KLuidPartialKey, KLuidMask, aLuid, foundIds );

	TUint32 count = foundIds.Count();
	TReal istTime;
	TBuf<256> calFileName;

	for( TUint32 j = 0; j < count; j++ )
		{
		TUint32 val = foundIds[j];
		err = iRepository->Get( foundIds[j] | KTTimeShift, istTime );
		if( KErrNone == err && TReal( aInstanceTime.Int64()) == istTime )
			{
			err = iRepository->Get( foundIds[j] | KCalFileNameShift, calFileName );
			if( KErrNone == err && !calFileName.CompareF( aCalFileName ) )
			    {
                //entryexists = ETrue;
                foundIds.Reset();
                return;
			    }
			}
		}
	foundIds.Reset();    
	
	CountL(count);
	TUint32 key = count;
	if( count >= KMaxKeys )
		{
		DeleteEntryAndReArrangeL(0);
		key = KMaxKeys - 1;
		}
		
    // Begin read write transaction
    err = iRepository->StartTransaction(CRepository::EReadWriteTransaction);
    
  	// perform some write operations
	err = iRepository->Create(key, aLuid);

	err = iRepository->Create(key | KTTimeShift, TReal( aInstanceTime.Int64()) );
	
	err = iRepository->Create(key | KCalFileNameShift, aCalFileName );
	
	err = iRepository->CommitTransaction( keyId );
	if (err != KErrNone)
		{
		iRepository->FailTransaction();
		}
	
	TRACE_EXIT_POINT;
	}

// ----------------------------------------------------------------------------
// CMissedAlarmStore::DeleteEntryAndReArrangeL
// Delete old entry and insert new entry when the missed alarms count is more
// than 10 entries
// ----------------------------------------------------------------------------
//
void CMissedAlarmStore::DeleteEntryAndReArrangeL(TInt aDeletedIndex)
	{
	TRACE_ENTRY_POINT;
	
    TInt err;
    TUint32 keyInfo;
    TUint32 count = 0;
    CountL(count);    
    err = iRepository->StartTransaction(CRepository::EReadWriteTransaction);
    iRepository->CleanupCancelTransactionPushL();
    TInt idx = 0;
    for( idx = aDeletedIndex; idx < KMaxKeys-1 && idx < count-1; idx++ )
	    {
	    // Move the key to the target position
	    iRepository->Delete( idx );
	    iRepository->Delete( idx | KTTimeShift );
	    iRepository->Delete( idx | KCalFileNameShift );
	    err = User::LeaveIfError(iRepository->Move(idx+1, idx, 0xFFFFFFFF, keyInfo));
	    err = User::LeaveIfError(iRepository->Move((idx+1) | KTTimeShift,
	                            idx | KTTimeShift, 0xFFFFFFFF, keyInfo));
	    err = User::LeaveIfError(iRepository->Move((idx+1) | KCalFileNameShift,
	                                    idx | KCalFileNameShift, 0xFFFFFFFF, keyInfo));
	    }
    iRepository->Delete( idx );
    iRepository->Delete( idx | KTTimeShift );
    iRepository->Delete( idx | KCalFileNameShift );
    CleanupStack::Pop(); // transaction
    err = iRepository->CommitTransaction(keyInfo);
    
    User::LeaveIfError(err);
    
    TRACE_EXIT_POINT;
	}

// End of file
