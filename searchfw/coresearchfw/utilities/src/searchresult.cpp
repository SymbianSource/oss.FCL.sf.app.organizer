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
* Description:   Class representing a search result with title, snippet
*
*/



#include <e32def.h>
#include <e32cmn.h>
#include <s32strm.h>

#include <searchdocumentid.h>
#include <searchlightresult.h>
#include <searchresult.h>

//------------------------------------------------------------------------
// 1st phase constructor
//------------------------------------------------------------------------
//
EXPORT_C CSearchResult* CSearchResult::NewL( CSearchLightResult* aLightResult )
    {
    CSearchResult* searchResult = new ( ELeave ) CSearchResult( aLightResult );
    return searchResult;
	}

//------------------------------------------------------------------------
// 1st phase constructor
//------------------------------------------------------------------------
//
EXPORT_C CSearchResult* CSearchResult::NewL( const CSearchResult& aSearchResult )
    {
    CSearchResult* searchResult = new ( ELeave ) CSearchResult;
    CleanupStack::PushL( searchResult );
    searchResult->ConstructL( aSearchResult );
    CleanupStack::Pop( searchResult );
    return searchResult;
    }
	
//------------------------------------------------------------------------
// 1st phase constructor
//------------------------------------------------------------------------
//
EXPORT_C CSearchResult* CSearchResult::NewL( RReadStream& aStream )
    {
    CSearchResult* searchResult = new ( ELeave ) CSearchResult;
    CleanupStack::PushL( searchResult );
    searchResult->ConstructL( aStream );
    CleanupStack::Pop( searchResult );
    return searchResult;	
    }
    
//------------------------------------------------------------------------
// Destructor
//------------------------------------------------------------------------
//
CSearchResult::~CSearchResult() 
    {
    if ( iLightResult ) 
        {
        delete iLightResult;
        iLightResult = NULL;
        }
    if ( iTitle ) 
        {
        delete iTitle;
        iTitle = NULL;
        }
    if ( iSnippet ) 
        {
        delete iSnippet;
        iSnippet = NULL;
        }
    if ( iSnippetFormat ) 
        {
        delete iSnippetFormat;
        iSnippetFormat = NULL;
        }
    }


//------------------------------------------------------------------------
// Externalizes class to stream
//------------------------------------------------------------------------
//
EXPORT_C void CSearchResult::ExternalizeL( RWriteStream& aStream ) const
    {
    if ( iTitle && iTitle->Size() ) 
        {
        aStream.WriteUint32L( iTitle->Size() );
        aStream << *iTitle; //Writing the size and then the title buf to stream
        }
    else 
        {
        aStream.WriteUint32L( 0 );
        }

    if ( iSnippet && iSnippet->Size() ) 
        {
        aStream.WriteUint32L( iSnippet->Size() );
        aStream << *iSnippet; 
        }
    else 
        {
        aStream.WriteUint32L( 0 );
        }
	
    if ( iSnippetFormat && iSnippetFormat->Size() ) 
        {
        aStream.WriteUint32L( iSnippetFormat->Size() );
        aStream << *iSnippetFormat; 
        }
    else 
        {
        aStream.WriteUint32L( 0 );
        }
    if ( iLightResult ) 
        {
        aStream.WriteUint32L( 1 );
        iLightResult->ExternalizeL( aStream ); //Calling the iLightResult
       // Externalize func.	
        }
    else 
        {
        aStream.WriteUint32L( 0 );
		}
	}

//------------------------------------------------------------------------
// Gets result
//------------------------------------------------------------------------
//
EXPORT_C const CSearchLightResult& CSearchResult::Result() const
    {
    return *iLightResult;	
    }

//------------------------------------------------------------------------
// Sets the title
//------------------------------------------------------------------------
//
EXPORT_C void CSearchResult::SetTitleL( const TDesC& aTitle )
    {
    if ( iTitle ) 
        {
        delete iTitle;
        iTitle = NULL;
        }
    if ( 0 < aTitle.Size() )
        {
        iTitle = aTitle.AllocL();
        }
    else 
    	{
    	_LIT( KDesc, " " );
    	iTitle = KDesc().AllocL();//.AllocL();
        }
    }

//------------------------------------------------------------------------
// Gets the title
//------------------------------------------------------------------------
//
EXPORT_C const TDesC& CSearchResult::Title() const 
    {
    return *iTitle;
    }
	
//------------------------------------------------------------------------
// Sets snippet
//------------------------------------------------------------------------
//
EXPORT_C void CSearchResult::SetSnippetL( const TDesC8& aSnippet )
    {
    if ( iSnippet ) 
        {
        delete iSnippet;
        iSnippet = NULL;
        }
    if ( 0 < aSnippet.Size() ) 
        {
        iSnippet = aSnippet.AllocL();
        }
    }
	
//------------------------------------------------------------------------
// Gets snippet
//------------------------------------------------------------------------
//
EXPORT_C const TDesC8& CSearchResult::Snippet() const 
    {
    return *iSnippet;	
    }

//------------------------------------------------------------------------
// Sets snippet
//------------------------------------------------------------------------
//
EXPORT_C void CSearchResult::SetSnippetFormatL( const TDesC8& aSnippetFormat )
    {
    if ( iSnippetFormat ) 
        {
        delete iSnippetFormat;
        iSnippet = NULL;
        }
    if ( 0 < aSnippetFormat.Size() ) 
        {
        iSnippetFormat = aSnippetFormat.AllocL();
        }
    }


//------------------------------------------------------------------------
// Gets snippet
//------------------------------------------------------------------------
//
EXPORT_C const TDesC8& CSearchResult::SnippetFormat() const 
    {
    return *iSnippetFormat;	
    }

//------------------------------------------------------------------------
// constructor
//------------------------------------------------------------------------
//
CSearchResult::CSearchResult() 
    {
	
    }

//------------------------------------------------------------------------
// constructor
//------------------------------------------------------------------------
//
CSearchResult::CSearchResult( CSearchLightResult* aResult ) 
    {
    iLightResult = aResult;
    }	

//------------------------------------------------------------------------
// 2nd phase constructor
//------------------------------------------------------------------------
//
void CSearchResult::ConstructL( const CSearchResult& aSearchResult ) 
    {
    if ( 0 < aSearchResult.Title().Size() )
        {
        iTitle = aSearchResult.Title().AllocL();
        }
    if ( 0 < aSearchResult.Snippet().Size() ) 
        {
    		iSnippet = aSearchResult.Snippet().AllocL();
        }
    if ( 0 < aSearchResult.SnippetFormat().Size() ) 
        {
        iSnippetFormat = aSearchResult.SnippetFormat().AllocL();
        }
    iLightResult = CSearchLightResult::NewL( aSearchResult.Result() );
    }


//------------------------------------------------------------------------
// 2nd phase constructor
//------------------------------------------------------------------------
//
void CSearchResult::ConstructL( RReadStream& aStream ) 
    {
    TUint size = aStream.ReadUint32L(); // read size frm stream
    if ( 0 != size ) 
        {
        iTitle = HBufC::NewL( size ); // 
        TPtr ptr = iTitle->Des();
        aStream >> ptr;
        }
    size = aStream.ReadUint32L(); // read size frm stream
    if ( 0 != size ) 
        {
        iSnippet = HBufC8::NewL( size ); // 
        TPtr8 ptr1 = iSnippet->Des();
        aStream >> ptr1;
        }
	
    size = aStream.ReadUint32L(); // read size frm stream
    if ( 0 != size ) 
        {
        iSnippetFormat = HBufC8::NewL( size ); // 
        TPtr8 ptr2 = iSnippetFormat->Des();
        aStream >> ptr2;
        }
	
    size = aStream.ReadUint32L(); // read size frm stream 
    if ( 0 != size ) 
        {
        iLightResult = CSearchLightResult::NewL( aStream );	
        }
    }






