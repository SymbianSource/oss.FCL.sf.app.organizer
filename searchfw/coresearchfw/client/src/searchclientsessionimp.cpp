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
* Description:   This file is source file for CSearchClientSessionImp
*
*/



// INCLUDE FILES
//#include <searchResultArray.h>

#include <s32mem.h>
#include <searchclientsession.h>
#include "searchclientsessionimp.h"


// ====================================== MEMBER FUNCTIONS =========================================

// -------------------------------------------------------------------------------------------------
// CSearchServer::NewL
// Symbian OS 2 phased constructor.
// -------------------------------------------------------------------------------------------------
//
CSearchClientSessionImp* CSearchClientSessionImp::NewL()
    {
    CSearchClientSessionImp* self = CSearchClientSessionImp::NewLC( );
    CleanupStack::Pop( self );
    return self;
    }

// -------------------------------------------------------------------------------------------------
// CSearchServer::NewLC
// Symbian OS 2 phased constructor.
// -------------------------------------------------------------------------------------------------
//
CSearchClientSessionImp* CSearchClientSessionImp::NewLC( )
    {
    CSearchClientSessionImp* self = new( ELeave ) CSearchClientSessionImp( );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// -------------------------------------------------------------------------------------------------
// CSearchClientSessionImp::~CSearchClientSessionImp
// Destructor.
// -------------------------------------------------------------------------------------------------
//
CSearchClientSessionImp::~CSearchClientSessionImp()
    {


        iLightResultArray.ResetAndDestroy();


        iheavyresult.ResetAndDestroy();


    Cancel( );
    iSearchClient.Close( );
    }

// -------------------------------------------------------------------------------------------------
// CSearchClientSessionImp::SearchL
// Other Items are commented in the header
// -------------------------------------------------------------------------------------------------
//
void CSearchClientSessionImp::SearchL( const CSearchQuery& aQuery,
                                       const TSearchThreshold& aThreshold,
                                       MSearchQueryObserver* aObserver
                                       )
    {
    //Set the Observer
    iObserver = aObserver;
    iMaxresults = aThreshold.iMaxResults;
	
	iLightResultArray.ResetAndDestroy();
	iheavyresult.ResetAndDestroy();
    //If no outstanding request
    if ( !IsActive() )
    {

    //Send query to client for search
    TRAPD( error, iSearchClient.SearchL( aQuery, iSearchFinished, aThreshold,  iStatus,
                                            iSearchCount, iSearchRes ) );
    if( error != KErrNone )
        {		
        Cancel();
        //Trap has happened
        iSearchClient.Close();
        }
    //Above is an Async call. making obj Active
    SetActive(); 
    }
    else
        {
        User::Leave(KErrInUse); 	
        }

    }

// -------------------------------------------------------------------------------------------------
// CSearchClientSessionImp::CancelSearch
// Other Items are commented in the header
// -------------------------------------------------------------------------------------------------
//
void CSearchClientSessionImp::CancelSearch()
    {
    if ( IsActive() )
        {
        iSearchClient.CancelSearch();
        }
    }

// -------------------------------------------------------------------------------------------------
// CSearchClientSessionImp::CancelSearch
// Other Items are commented in the header                                                                                              
// -------------------------------------------------------------------------------------------------
//
void CSearchClientSessionImp::CancelResult()
    {
    if ( IsActive( ) )
        {
        iSearchClient.CancelResult();
        }
    }
// -------------------------------------------------------------------------------------------------
// CSearchClientSessionImp::IsSearching
// Other Items are commented in the header
// -------------------------------------------------------------------------------------------------
//
TBool CSearchClientSessionImp::IsSearching()
    {
    return IsActive();
    }

// -------------------------------------------------------------------------------------------------
// CSearchClientSessionImp::SupportedContentTypes
// Other Items are commented in the header
// -------------------------------------------------------------------------------------------------
//
void CSearchClientSessionImp::GetSupportedContentL(
                                    RPointerArray<CSearchContent>& aContent,
                                    TUid aServiceId )
    {

    iSearchClient.GetSupportedContentL(aContent,aServiceId);
    }

// -------------------------------------------------------------------------------------------------
// CSearchClientSessionImp::GetSupportedServicesL
// Other Items are commented in the Header
// -------------------------------------------------------------------------------------------------
//
void CSearchClientSessionImp::GetSupportedServicesL( RArray<TUid>& aServiceIdArray )
    {
    TRAPD(error,iSearchClient.GetSupportedServicesL( aServiceIdArray ));
    if(error != KErrNone)
        {
        Cancel();
        iSearchClient.Close();
        }

    }

// -------------------------------------------------------------------------------------------------
// CSearchClientSessionImp::GetResultsL
// Other Items are commented in the Header
// -------------------------------------------------------------------------------------------------
//
void CSearchClientSessionImp::GetResultsL(const RPointerArray<CSearchDocumentId>& aResultIDArray,
                               MSearchResultRetrieveObserver* aObserver)
    {

    iRetrieveObserver = aObserver;
    
    //Above is an Async call. making obj Active 
    if(!IsActive())
    	{
        TRAPD(error, iSearchClient.GetResultsL( aResultIDArray, iSearchFinished, iStatus ) );
        SetActive(); 
        
	    if( error != KErrNone )
	        {
	        Cancel();
	        iSearchClient.Close();
	        }
	    }
    else
    	{
        iResultIDArray.Close();
        iGetRes = ETrue;    
        for(TInt i =0;i<aResultIDArray.Count(); i++)
        iResultIDArray.AppendL( aResultIDArray[i]);
    	}

    }

// -------------------------------------------------------------------------------------------------
// CSearchClientSessionImp::LaunchApplicationL
// Other Items are commented in the Header
// -------------------------------------------------------------------------------------------------
//
void CSearchClientSessionImp::LaunchApplicationL( const CSearchDocumentId& aResultId )
    {
    iSearchClient.LaunchApplicationL( aResultId );

    }

// -------------------------------------------------------------------------------------------------
// CSearchClientSessionImp::CSearchClientSessionImp
// Performs the first phase of two phase construction.
// -------------------------------------------------------------------------------------------------
//
CSearchClientSessionImp::CSearchClientSessionImp( )
                        :   CActive(CActive::EPriorityStandard ), 
    iSearchTotalResultsFound(0),
    iMaxresults(0),
    iSearchFinished(ENotFinished)
    {

    CActiveScheduler::Add( this );

    }


// -------------------------------------------------------------------------------------------------
// CSearchClientSessionImp::ConstructL
// Performs the second phase construction.
// -------------------------------------------------------------------------------------------------
//
void CSearchClientSessionImp::ConstructL()
    {
    User::LeaveIfError( iSearchClient.Connect() );
    iGetRes = EFalse;
    iSearchPending = ETrue;
    }

// -------------------------------------------------------------------------------------------------
// CSearchServerAsyncRequestHandler::ContinueSearchingL
// 
// -------------------------------------------------------------------------------------------------
//
void CSearchClientSessionImp::ContinueSearchingLightL()
    {
    iSearchClient.ContinueSearchingLightL( iSearchFinished, 
    iStatus,iSearchCount, iSearchRes );
    //Above is an Async call. making obj Active
    SetActive( );   
    }

// -------------------------------------------------------------------------------------------------
// CSearchClientSessionImp::RunL
// Handles an active object’s request completion event.
// -------------------------------------------------------------------------------------------------
//
void CSearchClientSessionImp::RunL()
    {
    switch ( iStatus.Int() )
        {
        case KErrNone:
            {
            TInt validity = iSearchFinished();
            TInt searchRes = iSearchRes();
            //see whether to continue or to stop the search
            if ( validity == ENotFinished ) 
                {

                RPointerArray<CSearchLightResult> lightResultArray;
                TInt count( 0 );
                count = iSearchCount();
                //increment res count 	
                iSearchTotalResultsFound += count;
                
                //if results found exceeds limits complete search
                if ( iSearchTotalResultsFound > iMaxresults )
                    {
                    //since coutn number of results are igonred.. 
                    //iSearchTotalResultsFound is decremented by count
                    iSearchTotalResultsFound -= count;
                
                    TInt searchRes = iSearchRes();
                    TRAPD(error2,iObserver->SearchCompletedL(searchRes, EFalse ));//Update to UI
                  	if( error2 != KErrNone )
					    {
						Cancel();
						iSearchClient.Close();
					    }                    
              
                    iSearchFinished = EFinished;	
                  	iGetRes = ETrue;
                    }
                 else 
                    {
                    //Get the light results
                	iSearchClient.GetLightResultsL(lightResultArray); 
                    //Storing light results to destroy at the end
                	for ( TInt i = 0 ; i < lightResultArray.Count() ; i++ )
	                    {
	                    iLightResultArray.Append(lightResultArray[i]);
	                    }
		            //send back light res
		            TRAPD(error1,iObserver->UpdateSearchProgressL(lightResultArray, searchRes, 
		                                                              iSearchTotalResultsFound ) );//Update results to UI
		            if ( error1 != KErrNone )
		                {
		                Cancel();
		                iSearchClient.Close();
		                }   
		            lightResultArray.Reset();
		            iSearchPending = ETrue;
                    }
                 	
                }

            // if search is over complete search
            else if ( validity == EFinished )
                {
                TInt validity = iSearchFinished();
                TInt searchRes = iSearchRes();
                TRAPD(error2,iObserver->SearchCompletedL( searchRes, ETrue ));//Update to UI
                iSearchTotalResultsFound = 0;	

                if ( error2 != KErrNone )
                    {
                    //Trap has happened
                    Cancel();
                    iSearchClient.Close();
                    }
                    iSearchPending = EFalse;
                }
            else //For heavy results
                {
                RPointerArray<CSearchResult> heavyresult;
                iSearchClient.GetHeavyResultsL(heavyresult); //Get the heavy results

                for (TInt i = 0 ; i < heavyresult.Count() ; i++ )
                    {
                    iheavyresult.Append(heavyresult[i]);
                    }

                TRAPD(error3,iRetrieveObserver->ResultsRetrieveCompleteL( heavyresult ));  //Update Heavy results to UI
                if( error3 != KErrNone )
				    {
					Cancel();
					iSearchClient.Close();
					}    
                heavyresult.Reset();
                heavyresult.Close();
                }

            }
            break;

        case KErrCancel:
            {
            //	 TInt i(0);
            }
            break;

        default:
            {
            //no code
            }
            break;
        }

    ////Continue the search if search has not been finished	
    if((iGetRes) && !IsActive())
    	{
        iSearchClient.GetResultsL( iResultIDArray, iSearchFinished, iStatus );
        SetActive( );
        iGetRes = EFalse;
    	}
    else if ((( iSearchFinished() == ENotFinished ) || iSearchPending) && (!IsActive()))
        {
        ContinueSearchingLightL(); 
        iSearchPending = EFalse;
        }

    }

// -------------------------------------------------------------------------------------------------
// CSearchClientSessionImp::DoCancel
// Implements cancellation of an outstanding request.
// -------------------------------------------------------------------------------------------------
//
void CSearchClientSessionImp::DoCancel()
    {
    iSearchClient.CancelSearch();
    }

// -------------------------------------------------------------------------------------------------
// CSearchClientSessionImp::RunError
// Handles a leave occurring in the request completion event handler RunL().
// -------------------------------------------------------------------------------------------------
//
TInt CSearchClientSessionImp::RunError( TInt /*aError*/ )
    {
    return KErrNone;
    }

// -------------------------------------------------------------------------------------------------
// CSearchClientSessionImp::GetLaunchInfoL
// Get the launch information from search server
// -------------------------------------------------------------------------------------------------
//
HBufC8* CSearchClientSessionImp::GetLaunchInfoL( const CSearchDocumentId& aDocumentID )
	{
	return iSearchClient.GetLaunchInfoL( aDocumentID );
	}
// End of File