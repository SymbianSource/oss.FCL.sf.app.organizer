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
* Description:   Holder for icon related information
*
*/



#include <searchiconinfo.h>

// --------------------------------------------------------------------------
//  CSearchIconInfo::NewL
//  Symbian C++ 2 phased constructor.
// --------------------------------------------------------------------------
//
EXPORT_C CSearchIconInfo* CSearchIconInfo::NewL()
    {
    CSearchIconInfo* self = new ( ELeave ) CSearchIconInfo;
    return self;
    }

// --------------------------------------------------------------------------
//  CSearchIconInfo::NewL
//  Symbian C++ 2 phased constructor.
// --------------------------------------------------------------------------
//
EXPORT_C CSearchIconInfo* CSearchIconInfo::NewL( RReadStream& aStream )  
    {   
    CSearchIconInfo* self = CSearchIconInfo::NewLC( aStream );
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
//  CSearchIconInfo::NewLC
//  Symbian C++ 2 phased constructor.
// --------------------------------------------------------------------------
//
EXPORT_C CSearchIconInfo* CSearchIconInfo::NewLC( RReadStream& aStream )
    {	
    CSearchIconInfo* self = new ( ELeave ) CSearchIconInfo;
    CleanupStack::PushL( self );
    self->ConstructL( aStream );
    return self;	
    }

// --------------------------------------------------------------------------
//  CSearchIconInfo::~CSearchIconInfo
//  C++ Destructor
// --------------------------------------------------------------------------
//
CSearchIconInfo::~CSearchIconInfo()
    {
    if ( iIconFileName )
        {
        delete iIconFileName;
        iIconFileName = NULL;
        }
    }

// --------------------------------------------------------------------------
//  CSearchIconInfo::ExternalizeL
//  Externalizing 
// --------------------------------------------------------------------------
//
EXPORT_C void CSearchIconInfo::ExternalizeL( RWriteStream& aStream ) const
    {
    TInt size( 0 );
    if ( iIconFileName ) 
        {
        size = iIconFileName->Size();
        aStream.WriteInt32L( size );
        aStream << (*(iIconFileName)); // writes iIconFileName in to stream.
        }
    else 
        {
        aStream.WriteInt32L( 0 );
        }		
    aStream.WriteInt32L( iIconIndex );
    aStream.WriteInt32L( iMaskIndex );
    aStream.WriteInt32L( iIsSkinnableIcon );
    aStream.WriteInt32L( iSkinId.iMajor );
    aStream.WriteInt32L( iSkinId.iMinor );
    aStream.WriteInt32L( iHasMask );
    }

// --------------------------------------------------------------------------
//  CSearchIconInfo::IsSkinnableIcon
//  Indicates if the icon is skinnable
// --------------------------------------------------------------------------
//
EXPORT_C TBool CSearchIconInfo::IsSkinnableIcon() const    
    {
    return iIsSkinnableIcon;
    }

// --------------------------------------------------------------------------
//  CSearchIconInfo::SetSkinId
//  Sets the skin id
// --------------------------------------------------------------------------
//
EXPORT_C void CSearchIconInfo::SetSkinId( const TAknsItemID& aSkinId )
    {
    iSkinId = aSkinId;
    }

// --------------------------------------------------------------------------
//  CSearchIconInfo::SkinId
//  Gets the skin id
// --------------------------------------------------------------------------
//
EXPORT_C const TAknsItemID& CSearchIconInfo::SkinId() const
    {
    return iSkinId;
    }

// --------------------------------------------------------------------------
//  CSearchIconInfo::HasIconMask
//  Indicates if the icon has a mask
// --------------------------------------------------------------------------
//
EXPORT_C TBool CSearchIconInfo::HasIconMask() const
    {
    return iHasMask;
    }

// --------------------------------------------------------------------------
//  CSearchIconInfo::SetIconFileL
//  Sets the icon file path
// --------------------------------------------------------------------------
//
EXPORT_C void CSearchIconInfo::SetIconFileL( const TDesC& aIconFileName )
    {
    if ( iIconFileName ) 
        {
        delete iIconFileName;
        iIconFileName = NULL;
        }
    if ( 0 < aIconFileName.Size() ) 
        {
    	iIconFileName = aIconFileName.AllocL();
        }
    }

// --------------------------------------------------------------------------
//  CSearchIconInfo::GetIconFileL
//  Gets the icon file path
// --------------------------------------------------------------------------
//
EXPORT_C const TDesC& CSearchIconInfo::GetIconFile() const
    {
    return *iIconFileName;
    }

// --------------------------------------------------------------------------
//  CSearchIconInfo::SetIconIndexL
//  Sets the icon index
// --------------------------------------------------------------------------
//
EXPORT_C void CSearchIconInfo::SetIconIndex( TInt aIconIndex )
    {
    iIconIndex = aIconIndex;
    }

// --------------------------------------------------------------------------
//  CSearchIconInfo::GetIconIndexL
//  Gets the icon index
// --------------------------------------------------------------------------
//
EXPORT_C TInt CSearchIconInfo::GetIconIndex() const
    {
    return iIconIndex;
    }

// --------------------------------------------------------------------------
//  CSearchIconInfo::SetIconMaskIndexL
//  Sets the mask index
// --------------------------------------------------------------------------
//
EXPORT_C void CSearchIconInfo::SetIconMaskIndex( TInt aIconIndex )
    {
    iIconIndex = aIconIndex;
    }

// --------------------------------------------------------------------------
//  CSearchIconInfo::GetIconMaskIndexL
//  Gets the mask index
// --------------------------------------------------------------------------
//
EXPORT_C TInt CSearchIconInfo::GetIconMaskIndex() const
    {
    return iIconIndex;
    }

// --------------------------------------------------------------------------
//  CSearchIconInfo::CSearchTextSnippet
//  C++ default constructor
// --------------------------------------------------------------------------
//
CSearchIconInfo::CSearchIconInfo()
    {
	
    }
// --------------------------------------------------------------------------
//  CSearchIconInfo::ConstructL
//  Performs the second phase construction
// --------------------------------------------------------------------------
//
void CSearchIconInfo::ConstructL(RReadStream& aStream)	
    {
    TInt size( 0 );
    size = aStream.ReadInt32L();
    if ( 0 != size ) 
        {
        iIconFileName = HBufC::NewL( size );
        TPtr ptr( iIconFileName->Des() ); 
        aStream>> ptr;            // Reads iIconFileName from Stream.
        }
		
    iIconIndex = aStream.ReadInt32L();
    iMaskIndex = aStream.ReadInt32L();
    iIsSkinnableIcon = aStream.ReadInt32L();
    iSkinId.iMajor = aStream.ReadInt32L();
    iSkinId.iMinor = aStream.ReadInt32L();
    iHasMask = aStream.ReadInt32L(); 
    }	


