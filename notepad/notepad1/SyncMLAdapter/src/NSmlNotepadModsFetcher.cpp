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
* Description:  DS notepad mods fetcher
*
*/


// INCLUDE FILES

#include <s32strm.h>
#include <nsmlsnapshotitem.h>
#include <nsmlchangefinder.h>

#include "NSmlNotepadModsFetcher.h"
#include "nsmlnotepadDatabase.h"

#include "nsmlnotepadlogging.h"

const TInt KNullId=-1;

// ------------------------------------------------------------------------------------------------
// CNSmlNotepadModsFetcher::NewL
// ------------------------------------------------------------------------------------------------
//
CNSmlNotepadModsFetcher* CNSmlNotepadModsFetcher::NewL( TBool& aSnapshotRegistered, 
														CNSmlNotepadDatabase* aNpdDb, 
														TKeyArrayFix& aKey, 
														CNSmlChangeFinder& aChangeFinder ) 
	{
	_NOTEPAD_DBG_FILE("CNSmlNotepadModsFetcher::NewL(): begin");
	CNSmlNotepadModsFetcher* self = NewLC(aSnapshotRegistered,aNpdDb,aKey,aChangeFinder);
	CleanupStack::Pop(self);
	_NOTEPAD_DBG_FILE("CNSmlNotepadModsFetcher::NewL(): end");
	return self;
	}

// ------------------------------------------------------------------------------------------------
// CNSmlNotepadModsFetcher::NewLC
// ------------------------------------------------------------------------------------------------
//
CNSmlNotepadModsFetcher* CNSmlNotepadModsFetcher::NewLC( TBool& aSnapshotRegistered, 
														CNSmlNotepadDatabase* aNpdDb, 
														TKeyArrayFix& aKey, 
														CNSmlChangeFinder& aChangeFinder )
	{
	_NOTEPAD_DBG_FILE("CNSmlNotepadModsFetcher::NewLC(): begin");
	CNSmlNotepadModsFetcher* self = new (ELeave) CNSmlNotepadModsFetcher(aSnapshotRegistered,aNpdDb,aKey,aChangeFinder);
	CleanupStack::PushL( self );
	self->ConstructL();
	_NOTEPAD_DBG_FILE("CNSmlNotepadDataProvider::NewLC(): end");
	return self;
	}	
// ------------------------------------------------------------------------------------------------
// CNSmlNotepadModsFetcher::CNSmlNotepadModsFetcher
// ------------------------------------------------------------------------------------------------
//
CNSmlNotepadModsFetcher::CNSmlNotepadModsFetcher( TBool& aSnapshotRegistered, 
												CNSmlNotepadDatabase* aNpdDb, 
												TKeyArrayFix& aKey, 
												CNSmlChangeFinder& aChangeFinder ) :
	CActive( EPriorityLow ), iSnapshotRegistered( aSnapshotRegistered ), iNpdDb( aNpdDb ), iKey( aKey ), iChangeFinder( aChangeFinder )
	{
	_NOTEPAD_DBG_FILE("CNSmlNotepadModsFetcher::CNSmlNotepadModsFetcher(): begin");
	CActiveScheduler::Add( this );
	iBeginning = ETrue;
	_NOTEPAD_DBG_FILE("CNSmlNotepadModsFetcher::CNSmlNotepadModsFetcher(): end");
	}
// ------------------------------------------------------------------------------------------------
// CNSmlNotepadModsFetcher::ConstructL
// ------------------------------------------------------------------------------------------------
//
void CNSmlNotepadModsFetcher::ConstructL()
	{
	_NOTEPAD_DBG_FILE("CNSmlNotepadModsFetcher::ConstructL: begin");
	iSnapshot = new (ELeave) CArrayFixSeg<TNSmlSnapshotItem>(KNSmlSnapshotSmallSize);
	_NOTEPAD_DBG_FILE("CNSmlNotepadModsFetcher::ConstructL: end");
	}
// ------------------------------------------------------------------------------------------------
// CNSmlNotepadModsFetcher::~CNSmlNotepadModsFetcher
// ------------------------------------------------------------------------------------------------
//
CNSmlNotepadModsFetcher::~CNSmlNotepadModsFetcher()
	{
	_NOTEPAD_DBG_FILE("CNSmlNotepadModsFetcher::~CNSmlNotepadModsFetcher(): begin");
	if(IsActive())
		Cancel();
	if(iSnapshot)
	{
		iSnapshot->Reset();
		delete iSnapshot;
		iSnapshot=NULL;	
	}
	_NOTEPAD_DBG_FILE("CNSmlNotepadModsFetcher::~CNSmlNotepadModsFetcher(): end");
	}
	
// ------------------------------------------------------------------------------------------------
// CNSmlNotepadModsFetcher::FetchModificationsL
// ------------------------------------------------------------------------------------------------
//
TInt CNSmlNotepadModsFetcher::FetchModificationsL( TRequestStatus& aStatus )
	{
	_NOTEPAD_DBG_FILE("CNSmlNotepadModsFetcher::FetchModificationsL(): begin");
	iCallerStatus = &aStatus;
	*iCallerStatus = KRequestPending;
	if(!iSnapshot)
	{
		iSnapshot = new (ELeave) CArrayFixSeg<TNSmlSnapshotItem>( KNSmlSnapshotSize );
	}
	SetActive();
	TRequestStatus* status = &iStatus;
	User::RequestComplete( status, KErrNone );
	_NOTEPAD_DBG_FILE("CNSmlNotepadModsFetcher::FetchModificationsL(): end");
	return KErrNone;
	}

// ------------------------------------------------------------------------------------------------
// CNSmlNotepadModsFetcher::Reset()
// ------------------------------------------------------------------------------------------------
//	
void CNSmlNotepadModsFetcher::Reset()
{
	iBeginning = ETrue;
}
	

// ------------------------------------------------------------------------------------------------
// CNSmlNotepadModsFetcher::FetchNextChunkL
// ------------------------------------------------------------------------------------------------	
//
void CNSmlNotepadModsFetcher::FetchNextChunkL()
	{
	_NOTEPAD_DBG_FILE("CNSmlNotepadModsFetcher::RunL(): end");
	if ( iSnapshotRegistered )
	{
	
		_NOTEPAD_DBG_FILE("CNSmlNotepadModsFetcher::FetchNextChunkL: if snapshotregistered");
		iEnd = ETrue;
	}
		
	else
	{
		iEnd = EFalse;
		TInt id;
		
		if ( iBeginning )
		{
			
				_NOTEPAD_DBG_FILE("CNSmlNotepadModsFetcher::FetchNextChunkL: callinf firstl");
			id = iNpdDb->FirstL();
			iBeginning = EFalse;
		}
		else
		{
			
				_NOTEPAD_DBG_FILE("CNSmlNotepadModsFetcher::FetchNextChunkL: calling nextl");
			id = iNpdDb->NextL();
		}
		if ( id == KNullId )
		{
			
				_NOTEPAD_DBG_FILE("CNSmlNotepadModsFetcher::FetchNextChunkL: invalid id");
			iEnd = ETrue;
		}
		else
		{
			
				_NOTEPAD_DBG_FILE("CNSmlNotepadModsFetcher::FetchNextChunkL: reading item from db");
			CNpdItem* item = CNpdItem::NewLC();
			TInt err;
			TRAP(err,iNpdDb->GetNoteL( id,*item ));
			if( err == KErrNone )
			{
			
				_NOTEPAD_DBG_FILE("CNSmlNotepadModsFetcher::FetchNextChunkL: item read successful");
				TNSmlSnapshotItem snap( item->Key() );
				snap.SetLastChangedDate( item->LastModified() );
				iSnapshot->InsertIsqL( snap, iKey );
				iCurrentSnapshotCounter++;
				if ( iCurrentSnapshotCounter > KNSmlSnapshotItemCount )
				{
					iSnapshot->Compress();
					iCurrentSnapshotCounter = 0;
				}
			}
			CleanupStack::PopAndDestroy(item); // item
		}
		
		if ( iEnd  )
		{
			
			_NOTEPAD_DBG_FILE("CNSmlNotepadModsFetcher::FetchNextChunkL: end item");
			iChangeFinder.SetNewSnapshot( iSnapshot ); // changefinder takes ownership
			iSnapshot = NULL;
			iSnapshotRegistered = ETrue;
		}
	}
	if ( iEnd )
		{
		User::RequestComplete( iCallerStatus, KErrNone ); // CNSmlContactsModsFetcher
		}
		_NOTEPAD_DBG_FILE("CNSmlNotepadModsFetcher::FetchNextChunkL: end");
	}
// ------------------------------------------------------------------------------------------------
// CNSmlNotepadModsFetcher::RunL
// ------------------------------------------------------------------------------------------------	
//
void CNSmlNotepadModsFetcher::RunL()
	{
	_NOTEPAD_DBG_FILE("CNSmlNotepadModsFetcher::RunL(): begin");
	FetchNextChunkL();
	if ( !iEnd )
		{
		SetActive();
		TRequestStatus* status = &iStatus;
		User::RequestComplete( status, KErrNone );
		}
	_NOTEPAD_DBG_FILE("CNSmlNotepadModsFetcher::RunL(): end");
	}

// ------------------------------------------------------------------------------------------------
// CNSmlNotepadModsFetcher::DoCancel
// ------------------------------------------------------------------------------------------------	
//
void CNSmlNotepadModsFetcher::DoCancel()
	{
	_NOTEPAD_DBG_FILE("CNSmlNotepadModsFetcher::DoCancel(): begin");
	User::RequestComplete( iCallerStatus, KErrCancel );
	iEnd = ETrue;
	_NOTEPAD_DBG_FILE("CNSmlNotepadModsFetcher::DoCancel(): end");
	}

// End of File  
