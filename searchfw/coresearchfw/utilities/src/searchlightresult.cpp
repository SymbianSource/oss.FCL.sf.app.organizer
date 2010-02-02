/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Class for Light results
*
*/



#include <e32def.h>
#include <e32cmn.h>
#include <s32strm.h>

#include <searchdocumentid.h>
#include <searchlightresult.h>

//------------------------------------------------------------------------
// 1st phase constructor
// The document id will be owned by this class. So no attempt should be made 
// to free this aDocId by whoever is passing.
//------------------------------------------------------------------------
//
EXPORT_C CSearchLightResult* CSearchLightResult::NewL( CSearchDocumentId* aDocId ) 
    {
    CSearchLightResult* self = new ( ELeave ) CSearchLightResult( aDocId );
    return self;
    }
	
//------------------------------------------------------------------------
// 1st phase constructor 
//------------------------------------------------------------------------
//
EXPORT_C CSearchLightResult* CSearchLightResult::NewL( const CSearchDocumentId& aDocId ) 
    {
    CSearchLightResult* self = new ( ELeave ) CSearchLightResult();
    CleanupStack::PushL( self );
    self->ConstructL( aDocId );
    CleanupStack::Pop( self );	
    return self;
    }
	
//------------------------------------------------------------------------
// 1st phase constructor
//------------------------------------------------------------------------
//
EXPORT_C CSearchLightResult* CSearchLightResult::NewL( RReadStream& aStream ) 
	{
    CSearchLightResult* self = new ( ELeave ) CSearchLightResult;
    CleanupStack::PushL( self );
    self->ConstructL( aStream );
    CleanupStack::Pop( self );
    return self;
    }
	
//------------------------------------------------------------------------
// 1st phase constructor
//------------------------------------------------------------------------
//
EXPORT_C CSearchLightResult* CSearchLightResult::NewL( 
		const CSearchLightResult& aSearchLightResult ) 
    {
    CSearchLightResult* self = new ( ELeave ) CSearchLightResult;
    CleanupStack::PushL( self );
    self->ConstructL( aSearchLightResult );
    CleanupStack::Pop( self );
    return self;
    }


//------------------------------------------------------------------------
// destructor
//------------------------------------------------------------------------
//
CSearchLightResult::~CSearchLightResult() 
    {
    if( iDocId ) 
        {
	    delete iDocId;
	    iDocId = NULL;
	    }
    }
	
//------------------------------------------------------------------------
// Externalizes the CSearchDocumentId class to the stream
//------------------------------------------------------------------------
//
EXPORT_C void CSearchLightResult::ExternalizeL( RWriteStream& aStream ) const
    {
    aStream.WriteUint32L( iContentClassId.iUid );
    aStream.WriteUint32L( iContentFolderId.iUid );
    aStream.WriteUint32L( iServiceId.iUid );
    aStream.WriteUint32L( iSubContentId );
    if( iDocId ) 
	    {
	    aStream.WriteUint32L( 1 );
	    iDocId->ExternalizeL( aStream );	
        }
    else 
	    {
	    aStream.WriteUint32L( 0 );
	    }
    }

//------------------------------------------------------------------------
// gets documentid 
//------------------------------------------------------------------------
//
EXPORT_C const CSearchDocumentId& CSearchLightResult::DocumentId() const
	{
    return *( iDocId );
	}
	
//------------------------------------------------------------------------
// gets content classid
//------------------------------------------------------------------------
//
EXPORT_C TUid CSearchLightResult::ContentClassId() const
	{
    return iContentClassId;
	}
	
//------------------------------------------------------------------------
// gets content folderid
//------------------------------------------------------------------------
//
EXPORT_C TUid CSearchLightResult::ContentFolderId() const
	{
    return iContentFolderId;
	}

//------------------------------------------------------------------------
// gets service id
//------------------------------------------------------------------------
//
EXPORT_C TUid CSearchLightResult::ServiceId() const 
	{
    return iServiceId;
	}

//------------------------------------------------------------------------
// gets realm
//------------------------------------------------------------------------
//
EXPORT_C TInt32 CSearchLightResult::SubContentId() const 
	{
    return iSubContentId;
	}

//------------------------------------------------------------------------
// sets serviceid
//------------------------------------------------------------------------
//
EXPORT_C void CSearchLightResult::SetServiceId( TUid aServiceId )
	{
    iServiceId.iUid = aServiceId.iUid;
	}
	
//------------------------------------------------------------------------
// sets contentclassid
//------------------------------------------------------------------------
//
EXPORT_C void CSearchLightResult::SetContentClassId( TUid aContentId )
	{
    iContentClassId.iUid = aContentId.iUid;
	}
	
//------------------------------------------------------------------------
// sets contentfolderid
//------------------------------------------------------------------------
//
EXPORT_C void CSearchLightResult::SetContentFolderId( TUid aContentId )
	{
    iContentFolderId.iUid = aContentId.iUid;
	}

//------------------------------------------------------------------------
// sets realm
//------------------------------------------------------------------------
//
EXPORT_C void CSearchLightResult::SetSubContentId( TInt32 aSubContentId ) 
	{
    iSubContentId= aSubContentId;
	}
		

//------------------------------------------------------------------------
// constructor
//------------------------------------------------------------------------
//
CSearchLightResult::CSearchLightResult( CSearchDocumentId* aDocId ) 
	{
    iDocId = aDocId;
	}
//------------------------------------------------------------------------
// constructor
//------------------------------------------------------------------------
//
CSearchLightResult::CSearchLightResult() 
	{
	
	}

//------------------------------------------------------------------------
// 2nd phase constructor
//------------------------------------------------------------------------
//
void CSearchLightResult::ConstructL( const CSearchDocumentId& aDocId ) 
	{
    iDocId = CSearchDocumentId::NewL( aDocId );
	}
	
//------------------------------------------------------------------------
// 2nd phase constructor
//------------------------------------------------------------------------
//
void CSearchLightResult::ConstructL( const CSearchLightResult& aSearchResult )
	{
    iContentClassId.iUid = aSearchResult.ContentClassId().iUid;
    iContentFolderId.iUid = aSearchResult.ContentFolderId().iUid;		
    iServiceId.iUid = aSearchResult.ServiceId().iUid;
    iSubContentId = aSearchResult.SubContentId();
    iDocId = CSearchDocumentId::NewL( ( aSearchResult.DocumentId() ) );
	}

//------------------------------------------------------------------------
// 2nd phase constructor
//------------------------------------------------------------------------
//
void CSearchLightResult::ConstructL( RReadStream& aStream ) 
	{
    iContentClassId.iUid = aStream.ReadUint32L();
    iContentFolderId.iUid = aStream.ReadUint32L();
    iServiceId.iUid = aStream.ReadUint32L();
    iSubContentId = aStream.ReadUint32L();
    TInt size = aStream.ReadUint32L();
    if( 0 != size ) 
        {
        iDocId = CSearchDocumentId::NewL( aStream );
        }
    }
