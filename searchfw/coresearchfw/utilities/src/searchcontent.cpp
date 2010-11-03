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
* Description:   Has Description of a content type
*
*/



#include <e32cmn.h>
#include <e32def.h>
#include <searchiconinfo.h>
#include <searchlightresult.h>
#include <searchcontent.h>

//------------------------------------------------------------------------
// 1st phase constructor
//------------------------------------------------------------------------
//
EXPORT_C CSearchContent* CSearchContent::NewL( TSearchContentType aType )
    {
    CSearchContent* self = new ( ELeave )CSearchContent( aType );
    return self;
    }

//------------------------------------------------------------------------
// 1st phase constructor
//------------------------------------------------------------------------
//
EXPORT_C CSearchContent* CSearchContent::NewL( RReadStream& aStream )
    {
    TSearchContentType type  = ( TSearchContentType )aStream.ReadUint32L();
    CSearchContent* self = new ( ELeave ) CSearchContent( type );
    CleanupStack::PushL( self );
    self->ConstructL( aStream );
    CleanupStack::Pop( self );
    return self;
    }

//------------------------------------------------------------------------
// destructor
//------------------------------------------------------------------------
//
CSearchContent::~CSearchContent()
    {
    if ( iCaption ) 
        {
        delete iCaption;
        iCaption = NULL;
        }
    if ( iIconInfo ) 
        {
        delete iIconInfo;
        iIconInfo = NULL;
        }
    if ( iSnippetFormat ) 
        {
        delete iSnippetFormat;
        iSnippetFormat = NULL;
        }
    iChildren.ResetAndDestroy();
    }


//------------------------------------------------------------------------
// writes the object to stream
//------------------------------------------------------------------------
//
EXPORT_C void CSearchContent::ExternalizeL( RWriteStream& aStream ) const
    {
    aStream.WriteUint32L( ( TUint32 ) iType );
    aStream.WriteUint32L( iContentId.iUid );
    aStream.WriteUint32L( iSubContentId ); 
    aStream.WriteUint32L( iPluginId.iUid );
	
    if ( iCaption ) 
        {
        TInt size = iCaption->Size();
        aStream.WriteUint32L( size );
        aStream << *iCaption;
        }
    else 
        {
        aStream.WriteUint32L( 0 );
        }
		
    if ( iIconInfo ) 
        {
        aStream.WriteUint32L(  1 );
        iIconInfo->ExternalizeL( aStream );
        }
    else 
        {
        aStream.WriteUint32L( 0 );
        }
    if ( iSnippetFormat ) 
        {
        TUint32 size = iSnippetFormat->Size();
        aStream.WriteUint32L( size );
        aStream << *iSnippetFormat;
        }
    else 
        {
        aStream.WriteUint32L( 0 );
        }
	
    TUint32 size = iChildren.Count();
    aStream.WriteUint32L( size );//write the count first
    for ( TUint i = 0; i < size; i++ ) 
        {	
        iChildren[i]->ExternalizeL( aStream );
        }
    }

//------------------------------------------------------------------------
// returns content type
//------------------------------------------------------------------------
//
EXPORT_C CSearchContent::TSearchContentType CSearchContent::ContentType() const
    {
    return iType;
    }

//------------------------------------------------------------------------
// sets content id
//------------------------------------------------------------------------
//
EXPORT_C void CSearchContent::SetContentId( TUid aContentId )
    {
    iContentId.iUid = aContentId.iUid;
    }

//------------------------------------------------------------------------
// returns content id
//------------------------------------------------------------------------
//
EXPORT_C TUid CSearchContent::ContentId() const
    {
    return iContentId;
    }

//------------------------------------------------------------------------
// sets subcontent id
//------------------------------------------------------------------------
//
EXPORT_C void CSearchContent::SetSubContentId( TInt32 aSubContentId )
    {
    iSubContentId = aSubContentId;
    }

//------------------------------------------------------------------------
// returns sub contentid
//------------------------------------------------------------------------
//
EXPORT_C TInt32 CSearchContent::SubContentId() const
    {
    return iSubContentId;
    }

//------------------------------------------------------------------------
// sets plugin id
//------------------------------------------------------------------------
//
EXPORT_C void CSearchContent::SetPluginId( TUid aPluginId )
    {
    iPluginId.iUid = aPluginId.iUid;
    }

//------------------------------------------------------------------------
// returns plugin id
//------------------------------------------------------------------------
//
EXPORT_C TUid CSearchContent::PluginId() const
    {
    return iPluginId;
    }

//------------------------------------------------------------------------
// sets caption
//------------------------------------------------------------------------
//
EXPORT_C void CSearchContent::SetCaptionL( const TDesC& aCaption )
    {
    if ( 0 < aCaption.Size() )
        {
        iCaption = aCaption.AllocL();	
        }
    }
    
 //------------------------------------------------------------------------
// sets caption
//------------------------------------------------------------------------
//
EXPORT_C void CSearchContent::SetShortCaptionL( const TDesC& aShortCaption )
    {
    if ( 0 < aShortCaption.Size() )
        {
        iShortCaption = aShortCaption.AllocL();	
        }
    }   
    

//------------------------------------------------------------------------
// Returns caption
//------------------------------------------------------------------------
//
EXPORT_C const TDesC& CSearchContent::Caption() const
    {
    return *iCaption;
    }

//------------------------------------------------------------------------
// sets icon info
//------------------------------------------------------------------------
//
EXPORT_C void CSearchContent::SetIconInfo( CSearchIconInfo* aIconInfo )
    {
    iIconInfo = aIconInfo;
    }

//------------------------------------------------------------------------
// returns iconinfo
//------------------------------------------------------------------------
//
EXPORT_C const CSearchIconInfo* CSearchContent::IconInfo() const
    {
    return iIconInfo;
    }

//------------------------------------------------------------------------
// sets snippet format
//------------------------------------------------------------------------
//
EXPORT_C void CSearchContent::SetSnippetFormatL( const TDesC8& aSnippetFormat )
    {
    if ( 0 < aSnippetFormat.Size() ) 
        {
        iSnippetFormat = aSnippetFormat.AllocL() ;	
        }
    }

//------------------------------------------------------------------------
// returns snippet format
//------------------------------------------------------------------------
//
EXPORT_C const TDesC8& CSearchContent::SnippetFormat() const
    {
    return *iSnippetFormat;
    }


//------------------------------------------------------------------------
// adds a child 
//------------------------------------------------------------------------
//
EXPORT_C void CSearchContent::AddChildL( CSearchContent* aContent )
    {
    if ( ETypeContentFolder == iType && aContent->ContentType() == ETypeContentClass ) 
        {
        User::Leave( KErrArgument );
        }
    iChildren.Append( aContent );
    }

//------------------------------------------------------------------------
// gets children
//------------------------------------------------------------------------
//
EXPORT_C void CSearchContent::GetChildrenL( 
   		RPointerArray<const CSearchContent>& aContentArray ) const
    {
    TInt count = 0;
    count = iChildren.Count();
    for ( TInt i = 0; i < count; i++ )
        {
        aContentArray.Append( iChildren[i] );
        }
    }

//------------------------------------------------------------------------
// finds a child by contentid
//------------------------------------------------------------------------
//
EXPORT_C const CSearchContent* CSearchContent::FindChildL( TUid aContentId ) const
    {
    TInt count = 0;
    CSearchContent* searchContent;
    count = iChildren.Count();
	
    for ( TInt i = 0; i < count; i++ ) 
        {
        searchContent = iChildren[i];
        if ( searchContent->ContentId().iUid == aContentId.iUid ) 
            {
            return searchContent;
            }
        }
	
    return NULL;
    }

//------------------------------------------------------------------------
// finds a child by sub contentid
//------------------------------------------------------------------------
//
EXPORT_C const CSearchContent* CSearchContent::FindSubContentL( 
   		const CSearchLightResult& aResult ) const
    {
    TInt count = 0;
    CSearchContent* searchContent;
    count = iChildren.Count();
	
    for ( TInt i = 0; i < count; i++ ) 
        {
        searchContent = iChildren[i];
        if ( searchContent->SubContentId() == aResult.SubContentId() ) 
            {
            return searchContent;
            }
        }
	
    return NULL;		
    }

//------------------------------------------------------------------------
// constructor
//------------------------------------------------------------------------
//
CSearchContent::CSearchContent( TSearchContentType aType )
    {
    iType = aType;
    }

//------------------------------------------------------------------------
// 2nd phase constructor
//------------------------------------------------------------------------
//
void CSearchContent::ConstructL( RReadStream& aStream )
    {
    iContentId.iUid = aStream.ReadUint32L();
    iSubContentId = aStream.ReadUint32L();
    iPluginId.iUid = aStream.ReadUint32L();
	
    TInt size = aStream.ReadUint32L();
    if ( 0 != size ) 
        {
        iCaption = HBufC::NewL( size );
        TPtr ptr ( iCaption->Des() );
        aStream >> ptr;
        }
	
    size = aStream.ReadUint32L(); 
    if ( 0 != size ) 
        {
        iIconInfo = CSearchIconInfo::NewL( aStream );
        }
		
    size = aStream.ReadUint32L();
    if ( 0 != size ) 
        {
        iSnippetFormat = HBufC8::NewL( size );
        TPtr8 ptr1 ( iSnippetFormat->Des() );
        aStream >> ptr1;
        }
	
    size = aStream.ReadUint32L();
			
    CSearchContent* tmpContent;
    for ( TInt i = 0; i < size; i++ )
        {
        tmpContent = CSearchContent::NewL( aStream );
        iChildren.Append( tmpContent );
        }
    }




