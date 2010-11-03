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
* Description:   Class for document id which uinquely represents 
*				 a file
*
*/






#include <searchdocumentid.h>

//------------------------------------------------------------------------
// 1st phase constructor
//------------------------------------------------------------------------
//
EXPORT_C CSearchDocumentId* CSearchDocumentId::NewL( const TInt aDocumentId, 
		TUid aPluginId  ) 
    {
    CSearchDocumentId* self = new ( ELeave ) CSearchDocumentId( aPluginId );
    CleanupStack::PushL( self );
    self->ConstructL( aDocumentId );
    CleanupStack::Pop( self );
    return self;
    }
	
//------------------------------------------------------------------------
// 1st phase constructor
//------------------------------------------------------------------------
//
EXPORT_C CSearchDocumentId* CSearchDocumentId::NewL( 
										const CSearchDocumentId& aDocumentId )
    {
    CSearchDocumentId* self = new ( ELeave ) CSearchDocumentId(
                                  aDocumentId.PluginId() );
    CleanupStack::PushL( self );
    self->ConstructL( aDocumentId.RepositoryId(), aDocumentId.DocumentId() );
    CleanupStack::Pop( self );
    return self;
    }

//------------------------------------------------------------------------
// 1st phase constructor
//------------------------------------------------------------------------
//
EXPORT_C CSearchDocumentId* CSearchDocumentId::NewL( RReadStream& aStream )
    {
    TUid pluginUid;
    pluginUid.iUid = aStream.ReadInt32L(); //Reading pluginId
	
    CSearchDocumentId* self = new ( ELeave ) CSearchDocumentId( pluginUid );
    CleanupStack::PushL( self );
    self->ConstructL( aStream );
    CleanupStack::Pop( self );
    return self;
    }

//------------------------------------------------------------------------
// destructor
//------------------------------------------------------------------------
//
CSearchDocumentId::~CSearchDocumentId() 
    {
    
    }

//------------------------------------------------------------------------
// Externalizes the CSearchDocumentId class to the stream
//------------------------------------------------------------------------
//
EXPORT_C void CSearchDocumentId::ExternalizeL( RWriteStream& aStream ) const 
    {
    aStream.WriteInt32L( iPluginId.iUid ); //writing plugin id
    
    aStream.WriteInt32L( iRepositoryId );
    aStream.WriteInt32L( iDocumentId );
    
    }
	
//------------------------------------------------------------------------
// gets the plugin id
//------------------------------------------------------------------------
//
EXPORT_C TUid CSearchDocumentId::PluginId() const
    {
    return iPluginId;
    }

//------------------------------------------------------------------------
// gets repository id
//------------------------------------------------------------------------
//
EXPORT_C const TInt32 CSearchDocumentId::RepositoryId() const
    {
    return iRepositoryId ;
    }

//------------------------------------------------------------------------
// gets the document id
//------------------------------------------------------------------------
//
EXPORT_C const TInt CSearchDocumentId::DocumentId() const
    {
    return iDocumentId;
    }

//------------------------------------------------------------------------
// Sets repository id
//------------------------------------------------------------------------
//
EXPORT_C void CSearchDocumentId::SetRepositoryIdL( const TInt32 aRepositoryId )
    {
        iRepositoryId = aRepositoryId;
     
    }
    
//------------------------------------------------------------------------
// constructor
//------------------------------------------------------------------------
//
CSearchDocumentId::CSearchDocumentId( TUid aPluginId ) 
    {
    iPluginId.iUid = aPluginId.iUid;
    }

//------------------------------------------------------------------------
// 2nd phase constructor
//------------------------------------------------------------------------
//
void CSearchDocumentId::ConstructL( const TInt  aDocumentId )
    {
    iDocumentId = aDocumentId;
    }

//------------------------------------------------------------------------
// 2nd phase constructor
//------------------------------------------------------------------------
//
void CSearchDocumentId::ConstructL( const TInt32 aRepositoryId, const TInt aDocumentId )
    {
        iDocumentId = aDocumentId;
        iRepositoryId = aRepositoryId;
     
    }

//------------------------------------------------------------------------
// 2nd phase constructor
//------------------------------------------------------------------------
//
void CSearchDocumentId::ConstructL( RReadStream& aStream )
    {
      iRepositoryId = aStream.ReadInt32L();
    iDocumentId = aStream.ReadInt32L();
    }

