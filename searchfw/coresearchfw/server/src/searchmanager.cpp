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
* Description:   This file is source file for searchmanager
*
*/



// INCLUDE FILES
#include <e32std.h>
#include <searchplugin.h>
#include <searchplugindefines.hrh>
#include <searchresult.h>
#include <searchtextsnippet.h>
#include "searchhandler.h"
#include "searchmanager.h"
#include "searchmanagerobserver.h"
#include "searchserverdefines.h"
#include "searchserversession.h"
#include "searchdocumentid.h"
#include "searchlightresult.h"


// FORWARD DECLARATIONS
class CSearchPlugin;

// -----------------------------------------------------------------------------
// CSearchManager::ManagerCallbackL
// Cancels the currently ongoing search for the specified client.
// -----------------------------------------------------------------------------
//
TInt ManagerCallbackL(TAny *ptr)
    {
    return(1);
    }

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CSearchManager::NewL
// Symbian OS 2 phased constructor.
// -----------------------------------------------------------------------------
//
CSearchManager* CSearchManager::NewL( MSearchManagerObserver& aObserver, CSearchPluginInterface * aSearchPluginInterface )//, RPointerArray<CSearchPlugin> * aSearch )
    {
    CSearchManager* self = CSearchManager::NewLC( aObserver, aSearchPluginInterface );//, aSearch );
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CSearchManager::NewLC
// Symbian OS 2 phased constructor.
// -----------------------------------------------------------------------------
//
CSearchManager* CSearchManager::NewLC( MSearchManagerObserver& aObserver, CSearchPluginInterface * aSearchPluginInterface )//, RPointerArray<CSearchPlugin> * aSearch )
    {
    CSearchManager* self = new( ELeave )CSearchManager( aObserver, aSearchPluginInterface );//, aSearch );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// -----------------------------------------------------------------------------
// CSearchManager::~CSearchManager
// Destructor.
// -----------------------------------------------------------------------------
//
CSearchManager::~CSearchManager()
    {
    iHandlers.ResetAndDestroy();

    iSearchPluginInterface = NULL;

    if ( iCallbacktimer )
        {
        delete iCallbacktimer;
        iCallbacktimer = NULL;
        }
        
        
    	iHeavyResultArray.Reset();
    	iLightResultHolder.Reset();
    	iLightResultBackup.Reset();
    	
    }

// ------------------------------------------------------------------------------------------------
// CSearchManager::GetSupportedServicesL
// Get supported service ids from all search plug-ins known by the ecom framework.
// -----------------------------------------------------------------------------
//
void CSearchManager::GetSupportedServicesL( RArray<TUid>& aServiceIdArray )
    {
    iSearchPluginInterface->GetSupportedServicesL( aServiceIdArray );
    }

// ------------------------------------------------------------------------------------------------
// CSearchManager::GetSupportedContentClassesL
// Get suported content classes from all search plug-ins known by the ecom framework.
// -----------------------------------------------------------------------------
//
void CSearchManager::GetSupportedContentClassesL( 
                                                 RPointerArray<CSearchContent>& aContent,
                                                 TUid aServiceId )
    {

    iSearchPluginInterface->GetSupportedContentClassesL( aContent,aServiceId  );
    }

// -----------------------------------------------------------------------------
// CSearchManager::CleanupResources
// Disconnects a client from the search manager.
// -----------------------------------------------------------------------------
//
void CSearchManager::CleanupResourcesL( )
    {
    //Before starting the new search clean up the resources
    iHandlers.ResetAndDestroy();
    //if heavy results asked dont destroy light results
    
    	iHeavyResultArray.Reset();
    	iLightResultHolder.Reset();
    	iLightResultBackup.Reset();
   
    if ( iCallbacktimer )
        {
        delete iCallbacktimer;
        iCallbacktimer = NULL;
        }
        //Reseting all the flags and counters for next search
    iNumResults =  0; 
    iNumHandlersSendingResult = 0;
    iNumHandlersCompleted = 0 ;
    iShouldResultBeSent = ETrue;
    iSearchCancelled = EFalse ;
    iSearchComp = EFalse;
    iComplete = ETrue;
    iIsHeavyResultsAsked = EFalse;
    }

// -----------------------------------------------------------------------------
// CSearchManager::SearchL
// Starts the search for the specficied client with the specified search
//  criteria.
// -----------------------------------------------------------------------------
//
void CSearchManager::SearchL( CSearchQuery* aSearchQuery, const TInt aGranularity, const TInt aMaxResults, const TInt64 aMaxTime )
    {
    //assigining granularity
    iGranularity = aGranularity;
    //assighning max results
    iMaxResult = aMaxResults;
    //creating conetnt searcher
    MSearchContentSearcher *iSearcher;
    //Reseting counts for safer side
    iNumHandlersSendingResult = 0;
    iNumHandlersCompleted = 0;
    iTotalDocuments = 0;
    iSearchCancelled = EFalse;
    iComplete = ETrue;
    iSearchComp = EFalse;
    //array of plugin pointers
    RPointerArray<CSearchPlugin> plugIns;
    //contentid array
    RArray<TUid> contentarray;
    //get the array of conetntids where search to be preformed in contentarray
    aSearchQuery->GetTargetContent(contentarray);

    //if there is no contentids complete the search
    if ( 0 == contentarray.Count() )
        {
        SearchIntCompletedL( KErrNotFound, iTotalDocuments );
        return;
        }

    //Get the supporting plugins
    iSearchPluginInterface->GetSupportingPlugInsL(plugIns,contentarray );

    RArray<TInt32> contentBool;

    for ( TInt i = 0; i < contentarray.Count() ; i++ )
        {
        RArray<TUid> tmpArray( sizeof(TUid), &(contentarray[i]),1 );
        TUid tmpUid;
        tmpUid.iUid = tmpArray[0].iUid;
        TBool contentBoolFlag = EFalse;
        //It keepes tarck of handlers created and avoids duplication of the same
        for( TInt m = 0; m < contentBool.Count(); m++ ) 
            {
            if( contentBool[m] == tmpUid.iUid ) 
                {
                //if the handler is created for particular contentUid
                //break this loop
                contentBoolFlag = ETrue;
                break;
                }

            }
        if ( contentBoolFlag ) 
            {
            //if habndler is already created skip this iteration
            continue;
            }

        //This loop is to create handlers for correspoding contentids
        for ( TInt id = 0; id < plugIns.Count(); id++ )
            {
            //create iSearcher
            iSearcher = plugIns[ id ]->ContentSearcherL( tmpArray,  aSearchQuery->Condition() , *this);
            if ( 0 != iSearcher )
                {
                //create handler
                CSearchHandler* handler = CSearchHandler::NewL( (MSearchPluginObserver*)this , (MSearchContentSearcher*)iSearcher );//, aGranularity );
                if( KErrNotFound == iHandlers.Find( handler ) ) 
                    {
                    //append created handler in iHandlers array 
                    iHandlers.AppendL( handler );
                    //append handlers in contentBool array for checking duplication
                    //of contentids
                    contentBool.AppendL( tmpUid.iUid );
                    break;	
                    }

                }


            }


        }
        contentBool.Close();

    //if no handlers complete the search
    if ( !iHandlers.Count())
        {
        SearchIntCompletedL( KErrNotFound, iTotalDocuments );
        return;
        }
    plugIns.Reset();
    contentarray.Reset();
    iMaxTime = aMaxTime;
    TRAPD( err, StartTimerL(aMaxTime) );
    if ( KErrNone != err )
        {

        }
    else
        {
        for ( TInt idx = 0; idx < iHandlers.Count(); idx++ )
            {
            //call iHandler 
            iHandlers[idx]->SearchL( );
            }
        }
    }

// -----------------------------------------------------------------------------
// CSearchManager::IsSearching
// Function returning ETrue when a search is ongoing for the specified
//  client.  EFalse otherwise.
// -----------------------------------------------------------------------------
//
TBool CSearchManager::IsSearching(  )
    {
    return(ETrue);
    }

// -----------------------------------------------------------------------------
// CSearchManager::Cancel
// Cancels the currently ongoing search for the specified client.
// -----------------------------------------------------------------------------
//
void CSearchManager::Cancel( )
    {
    for( TInt idx = 0; idx < iHandlers.Count(); idx++ )
        {
        iHandlers[idx]->Cancel( );
        }
    }

// -----------------------------------------------------------------------------
// CSearchManager::CancelResultsRetrieve
// Cancels the currently ongoing search for the specified client.
// -----------------------------------------------------------------------------
//
void CSearchManager::CancelResultsRetrieve( )
    {
    for( TInt idx = 0; idx < iHandlers.Count(); idx++ )
        {
        //call iHandler 
        iHandlers[idx]->CancelRetrieveResult( );
        }

    }

// -----------------------------------------------------------------------------
// CSearchManager::DisconnectClientL
// Disconnects a client from the search manager.
// -----------------------------------------------------------------------------
//
void CSearchManager::Close( )
    {
    //Destroy all handlers
    iHandlers.ResetAndDestroy();
    //Destroy all the results
    iLightResultHolder.Reset/*AndDestroy*/();
    //Destroy all the results
    iLightResultBackup.Reset/*AndDestroy*/();
    }

// -----------------------------------------------------------------------------
// CSearchManager::StartTimerL
// Cancels the currently ongoing search for the specified client.
// -----------------------------------------------------------------------------
//
void CSearchManager::StartTimerL(const TInt64 aMaxTime)
    {
    TCallBack iOperationCallback( &ManagerCallbackL, this );
    //create timer
    iCallbacktimer = CSearchManagerCallbackTimer::NewL( iOperationCallback ); 
    //start  the timer
    iCallbacktimer->StartTimer( aMaxTime );
    }

// -----------------------------------------------------------------------------
// CSearchManager::ForceResultToClient
// Called when results are forced to client if timer expires.
// -----------------------------------------------------------------------------
//
void CSearchManager::ForceResultToClientL()	
    {
    //if there are results in iLightResultBackup and there is an outstanding request
    if ( iLightResultBackup.Count()>0 && iShouldResultBeSent )
        {
        RPointerArray<CSearchLightResult> lightResultBackup;

        //append all the results from iLightResultBackup to lightResultBackup
        for (TInt i = 0; i < iGranularity; i++ )
            {
            lightResultBackup.AppendL( iLightResultBackup[i]);
            }

        //remove all the results from lightResultBackup
        for (TInt i = iGranularity-1; i >= 0; i-- )
            {
            iLightResultBackup.Remove( i );
            }
            iLightResultBackup.Compress();
            //send results back through observer
            iObserver.ResultFoundL( lightResultBackup, iCurrentDocument, iTotalDocuments );
            lightResultBackup.Reset();
        }
    //if there are results in iLightResultHolder and there is an outstanding request
    else if( iLightResultHolder.Count()>0 && iShouldResultBeSent )
        {
        //send results back through observer
        iObserver.ResultFoundL( iLightResultHolder, iCurrentDocument, iTotalDocuments );
        iLightResultHolder.Reset();
        }

    //Complete the search	
    SearchIntCompletedL( KErrNotFound, KErrNotFound );
    }

// -----------------------------------------------------------------------------
// CSearchManager::SetFlagL
// Sets the flag iShouldResultBeSent if there is an outstanding request
// -----------------------------------------------------------------------------
//
void CSearchManager::SetFlagL( TBool aFlagValue )
    {
    //set the flag if there is an outstanding request
    iShouldResultBeSent = aFlagValue;
    //Complete the search if, search is cancelled,if there is an outstanding req,
    //and if all the pluguins finished searching 
    if((iSearchCancelled && iShouldResultBeSent )|| (iShouldResultBeSent && iSearchComp ))
        {
        SearchIntCompletedL(  KErrNone, iTotalDocuments );
        }

    }

// -----------------------------------------------------------------------------
// CSearchManager::IsHeavyResultsAsked
// Return whether heavy results asked
// -----------------------------------------------------------------------------
//
TBool CSearchManager::IsHeavyResultsAsked()
    {
    return iIsHeavyResultsAsked;
    }

// -----------------------------------------------------------------------------
// CSearchManager::SearchCompleteAfterCancelL
// Completes the serch after cancelling the search
// -----------------------------------------------------------------------------
//
void CSearchManager::SearchCompleteAfterCancelL( )
    {
    if (iShouldResultBeSent)
        {
        //call the internal search complete   
        SearchIntCompletedL(  KErrNone, iTotalDocuments );
        iShouldResultBeSent = EFalse;
        }
    iSearchCancelled = ETrue;
    }

// -----------------------------------------------------------------------------
// CSearchManager::SearchIntCompletedL
// Internally called when a plug-in has finished searching.
// -----------------------------------------------------------------------------
//
void CSearchManager::SearchIntCompletedL( TInt aSearchResult, TInt aTotalNumberOfResults )
    {
    //if search is not cancelled
    if ( !iSearchCancelled )
        {

        //if there is an outstanding req and all the plugins finished seaching
        if ( iShouldResultBeSent && iSearchComp )
            {
            //if there is an outstanding req and there are results in iLightResultBackup
            if( iShouldResultBeSent && (iLightResultBackup.Count() > 0 ) && ( iHandlers.Count() ==  iNumHandlersCompleted ))
                {
                RPointerArray<CSearchLightResult> lightResultBackup;
                //to flush results from backup of count granularity
                for (TInt i = 0; i < iGranularity; i++ )
                    {
                    lightResultBackup.AppendL( iLightResultBackup[i]);
                    }
                for (TInt i = iGranularity-1; i >= 0; i-- )
                    {
                    iLightResultBackup.Remove( i );
                    }
                iLightResultBackup.Compress();
                //send results of count granulatiy
                iObserver.ResultFoundL( lightResultBackup, aSearchResult, aTotalNumberOfResults );
                lightResultBackup.Reset();
                }

            //if there is an outstanding req and there are results in iLightResultHolder
            else if ( iShouldResultBeSent && (iLightResultHolder.Count() > 0 ) && ( iHandlers.Count() ==  iNumHandlersCompleted  ))
                {
                //send the results
                iObserver.ResultFoundL( iLightResultHolder, aSearchResult, aTotalNumberOfResults );
                iLightResultHolder.Reset();
                }

            //if there is an outstanding req and seach has not yet over
            else if (iShouldResultBeSent && iComplete )
                {
                if ( iHandlers.Count() ==  iNumHandlersCompleted )
                    {
                    //Finsih the search
                    iObserver.SearchFinishedL(  aSearchResult, aTotalNumberOfResults );
                    iSearchComp = EFalse;
                    iComplete = EFalse;
                    }
                }
            }
        else
            {
            iSearchComp = ETrue;
            }
        }
    else
        {
        //finish the search
        iObserver.SearchFinishedL(  aSearchResult, aTotalNumberOfResults );

        }
    }



// -----------------------------------------------------------------------------
// CSearchManager::GetResultsL
// Called when result is found
// -----------------------------------------------------------------------------
//
void CSearchManager::GetResultsL( const RPointerArray<CSearchDocumentId>& aResultIDArray )
    {
        TInt idx( 0 );

        //ask heavy reults from each plugin
        for ( ; idx < iHandlers.Count(); idx++ )
            {
            iHandlers[idx]->GetResultsL( aResultIDArray );
            }
        iIsHeavyResultsAsked = ETrue;
    }

// -----------------------------------------------------------------------------
// CSearchManager::SendheavyResultL
// Called when stored heavy results to be sent
// -----------------------------------------------------------------------------
//
void CSearchManager::SendheavyResultL( )
    {
    //send stored heavy res
    RPointerArray <CSearchResult> tempHeavyResult;
    for( TInt count = 0; count < iHeavyResultArray.Count(); count++ )
        {
        tempHeavyResult.AppendL( iHeavyResultArray[ count ]);
        }
    iObserver.ResultsRetrieveCompleteL( tempHeavyResult  );
    tempHeavyResult.Reset();
    }

// -----------------------------------------------------------------------------
// CSearchManager::LaunchApplicationL
// Launch application of sepcified docId
// -----------------------------------------------------------------------------
//
void CSearchManager::LaunchApplicationL( const CSearchDocumentId& aDocumentId )
    {
        for ( TInt id = 0; id < iHandlers.Count(); id++ )
        {
        
        HBufC8* launchInfo = (iHandlers[id]->LaunchInfoL(aDocumentId));	
        if (launchInfo)
            {
        	iSearchPluginInterface->LaunchApplicationL(aDocumentId,launchInfo);
            }
             
             if(launchInfo)
             {
             	delete launchInfo;
             	launchInfo = NULL;
             }
        }

    }

// -----------------------------------------------------------------------------
// CSearchManager::GetLaunchInfoL
// Get the launch infor of the specified file
// -----------------------------------------------------------------------------
//
HBufC8* CSearchManager::GetLaunchInfoL( const CSearchDocumentId& aDocumentID )
	{
	for ( TInt id = 0; id < iHandlers.Count(); id++ )
	    {
	    HBufC8* launchInfo = (iHandlers[id]->LaunchInfoL( aDocumentID ));
	    if( launchInfo )
	    	{
	    	return launchInfo ;
	    	}
            }
        return NULL;
	}
// -----------------------------------------------------------------------------
// CSearchManager::ResultFoundL
// Called when results are found from pluins.
// -----------------------------------------------------------------------------
//
void CSearchManager::ResultFoundL( CSearchLightResult* aResult,
                                    TInt aCurrentDocument,
                                    TInt aTotalDocuments )
    {
    //if search has not been cancelled
    if ( !iSearchCancelled )
        {
        //add reults into iLightResultHolder
        iLightResultHolder.AppendL( aResult );
        //keep the count of current document
        iCurrentDocument = aCurrentDocument;
        //keep the count of total socument
        iTotalDocuments += aTotalDocuments;

        iNumResults++;
        //flush  results to backup from holder if results count is of granularity 
        if ( iGranularity <= iLightResultHolder.Count() )
            {
            for (TInt i = 0 ; i < iGranularity ; i++ )
                {
                iLightResultBackup.AppendL( iLightResultHolder[i] );
                }
            iLightResultHolder.Reset();

            }
        //if  there is an outstanding request
        if ( iShouldResultBeSent )
            {
            //if there are res in back up and count is of granularity flush the results
            //and remove them from backup  
            if ( 0 < iLightResultBackup.Count() && ( 0 == ( iLightResultBackup.Count() % iGranularity ) ) ) 
                {
                RPointerArray<CSearchLightResult> lightResultBackup;

                for (TInt i = 0; i < iGranularity; i++ )
                    {
                    lightResultBackup.AppendL( iLightResultBackup[i]);
                    }

                for (TInt i = iGranularity-1; i >= 0; i-- )
                    {
                    iLightResultBackup.Remove( i );
                    }
                iLightResultBackup.Compress();
                iObserver.ResultFoundL( lightResultBackup, aCurrentDocument, aTotalDocuments );
                lightResultBackup.Reset();
                lightResultBackup.Close();
                }
            else
                {
                //if results holder count is of granularity
                if ( ( 0 == ( iLightResultHolder.Count() % iGranularity ) ) )
                    {
                    //flush the results
                    iObserver.ResultFoundL( iLightResultHolder, aCurrentDocument, aTotalDocuments );
                    iLightResultHolder.Reset();	
                    }
                }
            }

        }
    else
        {
        //	SearchIntCompletedL( KErrCancel, iTotalDocuments );
        }

    }




// -----------------------------------------------------------------------------
// CSearchManager::SearchFinishedL
// Called when a plug-in has finished searching.
// -----------------------------------------------------------------------------
//
void CSearchManager::SearchCompletedL( TInt aSearchResult, TInt aTotalNumberOfResults )
    {
    //count number of handlers finished count
    iNumHandlersCompleted++;

    if ( iHandlers.Count() ==  iNumHandlersCompleted )
        {
        if ( iCallbacktimer )
            {
            delete iCallbacktimer;
            iCallbacktimer = NULL;
            }
        iSearchComp = ETrue;
        }

    //call search complete
    SearchIntCompletedL( aSearchResult, aTotalNumberOfResults );
    }

// -----------------------------------------------------------------------------
// CSearchManager::ResultsRetrieveCompleteL
// Called when a plug-in has finished retrieving results.
// -----------------------------------------------------------------------------
//
void CSearchManager::ResultsRetrieveCompleteL( RPointerArray<CSearchResult>& aResultArray )
    {
    iNumHandlersSendingResult++;
    RPointerArray <CSearchResult>  tempHeavyResult;
    for ( TInt count = 0; count < aResultArray.Count(); count++ )
        {


        iHeavyResultArray.AppendL( aResultArray[ count ]);
        tempHeavyResult.AppendL( aResultArray[ count ]);
        }
    //send heavy results
    iObserver.ResultsRetrieveCompleteL( tempHeavyResult );
    tempHeavyResult.Reset();
    tempHeavyResult.Close();

    }    

// -----------------------------------------------------------------------------
// CSearchManager::CSearchManager
// Performs the first phase of two phase construction.
// -----------------------------------------------------------------------------
//
void CSearchManager::SearchPluginCompletedL( TUid aUid )
{
	
	
}
// -----------------------------------------------------------------------------
// CSearchManager::CSearchManager
// Performs the first phase of two phase construction.
// -----------------------------------------------------------------------------
//
CSearchManager::CSearchManager( MSearchManagerObserver& aObserver, CSearchPluginInterface * aSearchPluginInterface )//, RPointerArray<CSearchPlugin> * aSearch )
    :iGranularity( KSearchResultGranularity ),   
    iMaxResult( 0), 
    iNumResults( 0 ), 
    iNumHandlersSendingResult( 0 ),
    iNumHandlersCompleted( 0 ),
    iComplete( ETrue),
    iIsHeavyResultsAsked( EFalse ),
    iShouldResultBeSent( ETrue ),
    iSearchCancelled( EFalse ),
    iSearchComp( EFalse),
    iObserver( aObserver )
    {
    //iServiceId.Uid(0);
    iSearchPluginInterface = aSearchPluginInterface;
    }

// -----------------------------------------------------------------------------
// CSearchManager::ConstructL
// ConstructL.
// -----------------------------------------------------------------------------
//	
void CSearchManager::ConstructL()
    {
    }
    

// End of File
