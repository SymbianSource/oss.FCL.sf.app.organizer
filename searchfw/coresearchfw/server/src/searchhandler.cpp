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
* Description:   This is Source file for searchandler  
*
*/


//SYSTEM INCLUDES 
#include <e32std.h>
#include <e32base.h>
#include <e32def.h>
#include <s32strm.h>

//  USER INCLUDES
#include "searchtextsnippet.h"
#include "searchlightresult.h"
#include "searchresult.h"
#include "searchcontentsearcher.h"
#include "searchpluginobserver.h"
#include "searchhandler.h"


// -------------------------------------------------------------------------------------------------
//  CSearchHandler::NewL
//  Symbian OS 1 phase constructor.
// -------------------------------------------------------------------------------------------------

CSearchHandler* CSearchHandler::NewL( MSearchPluginObserver* aManager, 
                                        MSearchContentSearcher* aContentSearcher )
    {
    CSearchHandler* handler = new ( ELeave ) CSearchHandler( );
    CleanupStack::PushL( handler );
    handler->ConstructL( aManager, aContentSearcher );
    CleanupStack::Pop();
    return handler;
    }

// -------------------------------------------------------------------------------------------------
//  CSearchHandler::~CSearchHandler
//  Destructor
// -------------------------------------------------------------------------------------------------
CSearchHandler::~CSearchHandler()
    {
    if( iSearcher )
        {
        //
        iSearcher->Destroy();
        //delete iSearcher;
        //iSearcher = NULL;	
        }
    }

// -------------------------------------------------------------------------------------------------
//  CSearchHandler::Free
//  Frees the resource
// -------------------------------------------------------------------------------------------------
void CSearchHandler::Free()
    {
    iSearcher->Destroy();
    }

// -------------------------------------------------------------------------------------------------
//  CSearchHandler::Cancel
//  Cancels the search
// -------------------------------------------------------------------------------------------------
void CSearchHandler::Cancel() 
    {
    iSearcher->CancelSearch();
    }

// -------------------------------------------------------------------------------------------------
//  CSearchHandler::CancelRetrieveResult
//  Cancels the search result retrieval
// -------------------------------------------------------------------------------------------------
void CSearchHandler::CancelRetrieveResult() 
    {
    iSearcher->CancelResultsRetrieve();
    }

// -------------------------------------------------------------------------------------------------
//  CSearchHandler::SearchL
//  Starts the search
// -------------------------------------------------------------------------------------------------
void CSearchHandler::SearchL() 
    {
    iSearcher->SearchL();
    }

// -------------------------------------------------------------------------------------------------
//  CSearchHandler::GetResultsL
//  Gets the Result
// -------------------------------------------------------------------------------------------------
void CSearchHandler::GetResultsL( const RPointerArray<CSearchDocumentId>& aDocumentIdArray )
    {
    iSearcher->GetResultsL(  aDocumentIdArray );

    }

// -------------------------------------------------------------------------------------------------
//  CSearchHandler::ResultFoundL
//  Returns the Results found
// -------------------------------------------------------------------------------------------------

void CSearchHandler::ResultFoundL( CSearchLightResult* aResult,                                
                                   TInt aCurrentDocument,
                                   TInt aTotalDocuments )
    {
    iManagerObserver->ResultFoundL( aResult, aCurrentDocument, aTotalDocuments );
    }


// -------------------------------------------------------------------------------------------------
//  CSearchHandler::SearchCompletedL
//  Notifies the completion of search
// -------------------------------------------------------------------------------------------------
void CSearchHandler::SearchCompletedL( TInt aSearchResult, TInt aTotalNumberOfResults ) 
    {
    iManagerObserver->SearchCompletedL( aSearchResult, aTotalNumberOfResults );

    }


// -------------------------------------------------------------------------------------------------
//  CSearchHandler::ResultsRetrieveCompleteL
//  Retrieved results passed to manager
// -------------------------------------------------------------------------------------------------
void CSearchHandler::ResultsRetrieveCompleteL( RPointerArray<CSearchResult>& aResultArray )
    {
    iManagerObserver->ResultsRetrieveCompleteL( aResultArray );
    }


// -------------------------------------------------------------------------------------------------
//Return the launch info corresponding to this document id
// -------------------------------------------------------------------------------------------------

HBufC8* CSearchHandler::LaunchInfoL( const CSearchDocumentId& aDocumentID ) 
{
	return iSearcher->LaunchInfoL( aDocumentID );
}


  void CSearchHandler::SearchPluginCompletedL( TUid aUid )
  {
  	
  	
  	
  }
// -------------------------------------------------------------------------------------------------
//  CSearchHandler::CSearchHandler
//  parameterized constructor.
// -------------------------------------------------------------------------------------------------
CSearchHandler::CSearchHandler( )
    {

    }

// -------------------------------------------------------------------------------------------------
//  CSearchHandler::ConstructL
//  Symbian OS 2 phase constructor.
// -------------------------------------------------------------------------------------------------
void CSearchHandler::ConstructL( MSearchPluginObserver* aManager , 
                                MSearchContentSearcher* aContentSearcher ) 
    {
    iManagerObserver = aManager;
    iSearcher = aContentSearcher;
    }







//End Of File
