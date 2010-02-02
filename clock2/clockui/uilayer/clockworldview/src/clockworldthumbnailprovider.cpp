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
* Description:   This is the source file for the CClockWorldThumbnailProvider class.
*
*/

// System includes
#include <aknnotewrappers.h>
#include <thumbnailmanager.h>
#include <thumbnailobjectsource.h>
#include <thumbnaildata.h> 

// User includes.
#include "clockworldarray.h"
#include "clockworldthumbnailprovider.h"
#include "clock_debug.h"

// Literals

//constants 
const TDisplayMode KThumbnailColors = EColor64K;

// ---------------------------------------------------------
// CClockWorldThumbnailProvider::NewL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
CClockWorldThumbnailProvider* CClockWorldThumbnailProvider::NewL( CClockWorldArray* aWorldArray )
    {
    __PRINTS( "CClockWorldThumbnailProvider::NewL - Entry" );  
    
    CClockWorldThumbnailProvider* self = new( ELeave ) CClockWorldThumbnailProvider();
    CleanupStack::PushL( self );
    
    self->ConstructL( aWorldArray );
    CleanupStack::Pop( self );
    
    __PRINTS( "CClockWorldThumbnailProvider::NewL - Exit" );
    
    return self;
    }

// ---------------------------------------------------------
// CClockWorldThumbnailProvider::NewL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
CClockWorldThumbnailProvider::~CClockWorldThumbnailProvider()
	{
    __PRINTS( "CClockWorldThumbnailProvider::~CClockWorldThumbnailProvider - Entry" );

	if( iThumbnailManager )
	    {
	    delete iThumbnailManager;
	    iThumbnailManager = NULL;
	    }

	__PRINTS( "CClockWorldThumbnailProvider::~CClockWorldThumbnailProvider - Exit" );
	}

// ---------------------------------------------------------
// CClockWorldThumbnailProvider::GetThumbnailL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
TInt CClockWorldThumbnailProvider::GetThumbnailL( const TDesC& aFileName )
    {
    __PRINTS( "CClockWorldThumbnailProvider::GetThumbnailL - Entry" );
     
    TInt thumbnailRequestId( KErrNone );
     
    // Create CThumbnailObjectSource instance which stores source file related data.
    CThumbnailObjectSource* source = CThumbnailObjectSource::NewLC( aFileName, KErrNone );
     
    // Request the thumbnail for the given image file from CThumbnailManager api.
    // CThumbnailManager fetches the thumbnail if it already exists from the ThumbnailServer.
    // Else it creates a new thumbnail for the specified file.    
    thumbnailRequestId = iThumbnailManager->GetThumbnailL( *source );    
       
    CleanupStack::PopAndDestroy( source );
     
    __PRINTS( "CClockWorldThumbnailProvider::GetThumbnailL - Exit" );
     
    return thumbnailRequestId;
    }

// ---------------------------------------------------------
// CClockWorldThumbnailProvider::DeleteThumbnailL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClockWorldThumbnailProvider::DeleteThumbnailL( const TDesC& aFileName )
    {
    __PRINTS( "CClockWorldThumbnailProvider::DeleteThumbnailL - Entry" );
     
    // Create CThumbnailObjectSource instance which stores source file related data.
    CThumbnailObjectSource* source = CThumbnailObjectSource::NewLC( aFileName, KErrNone );
     
    // Request to delete thumbnail for the given image file.
    iThumbnailManager->DeleteThumbnails( *source );    
       
    CleanupStack::PopAndDestroy( source );
     
    __PRINTS( "CClockWorldThumbnailProvider::DeleteThumbnailL - Exit" );     
    }

// ---------------------------------------------------------
// CClockWorldThumbnailProvider::ThumbnailReady
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClockWorldThumbnailProvider::ThumbnailReady( TInt aError,
                                                   MThumbnailData& aThumbnail,
                                                   TThumbnailRequestId aId )
    {
    __PRINTS( "CClockWorldThumbnailProvider::ThumbnailReady - Entry" );
       
    CFbsBitmap* bitmap = NULL;
    if ( !aError )
         {
         // Claim ownership of the bitmap instance.
         bitmap = aThumbnail.DetachBitmap();
         
         // Set the thumbnail for the listitem having the specified thumbnailRequestId.
         iWorldArray->SetThumbnail( bitmap, aId );
         }
    else
         {
         // An error occurred while getting the thumbnail.       
         }
       
    __PRINTS( "CClockWorldThumbnailProvider::ThumbnailReady - Exit" );
    }

// ---------------------------------------------------------
// CClockWorldThumbnailProvider::ThumbnailPreviewReady
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClockWorldThumbnailProvider::ThumbnailPreviewReady( MThumbnailData& /*aThumbnail*/,
                                                          TThumbnailRequestId /*aId*/ )   
    {
    __PRINTS( "CClockWorldThumbnailProvider::ThumbnailPreviewReady - Entry" );
    
    // No implementation yet. 
    
    __PRINTS( "CClockWorldThumbnailProvider::ThumbnailPreviewReady - Exit" );
    }     

// ---------------------------------------------------------
// CClockWorldThumbnailProvider::CClockWorldThumbnailProvider
// rest of the details are commented in the header
// ---------------------------------------------------------
//
CClockWorldThumbnailProvider::CClockWorldThumbnailProvider()
    {
    __PRINTS( "CClockWorldThumbnailProvider::CClockWorldThumbnailProvider - Exit" );
    
    // No implementation yet.
    
    __PRINTS( "CClockWorldThumbnailProvider::CClockWorldThumbnailProvider - Exit" );
    }

// ---------------------------------------------------------
// CClockWorldThumbnailProvider::ConstructL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClockWorldThumbnailProvider::ConstructL( CClockWorldArray* aWorldArray )
    {
    __PRINTS( "CClockWorldThumbnailProvider::ConstructL - Entry" );
    
    iWorldArray = aWorldArray;
    
    iThumbnailManager = CThumbnailManager::NewL( *this  );
    
    // Set colour for thumbnail generation.
    iThumbnailManager->SetDisplayModeL( KThumbnailColors );
    
    // Specify that thumbnail generated will be displayed for a list.
    iThumbnailManager->SetThumbnailSizeL( EImageListThumbnailSize );
    
    // Set flags for thumbnail generation.
    iThumbnailManager->SetFlagsL( CThumbnailManager::ECropToAspectRatio );
    
    __PRINTS( "CClockWorldThumbnailProvider::ConstructL - Exit" );
    }
 
// End of file
