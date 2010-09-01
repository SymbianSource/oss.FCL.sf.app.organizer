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
* Description:   This file is header file for SearchClientSessionImp
*
*/



#ifndef SEARCH_SERVER_IMP_HANDLER_H
#define SEARCH_SERVER_IMP_HANDLER_H

// INCLUDE FILES
#include <e32base.h>
#include "searchclient.h"
#include <searchsessionobservers.h>
#include <searchlightresult.h>
#include <searchcommon.h>

// FORWARD DECLARATIONS
class CSearchDocumentId;

// CLASS DECLARATION

/**
 * This class represents an implementation to the client side session to search framework.
 * This class is dervied from CActive and is used to impliment the session in  the \
 * client side 
 * @lib searchclient.lib
 *
 */
NONSHARABLE_CLASS(CSearchClientSessionImp) : public CActive
    {
public: // Constructors and destructor
    /**
     * Symbian OS 2 phased constructor.
     * @return	A pointer to the created instance of CSearchServer.
     */
    static CSearchClientSessionImp* NewL( );

    /**
     * Symbian OS 2 phased constructor.
     * @return	A pointer to the created instance of CSearchServer.
     */
    static CSearchClientSessionImp* NewLC( );

    /**
     * Destructor
     */
    virtual ~CSearchClientSessionImp();

    // New functions
    /**
     * Starts the search with the specified search criteria.
     * @param    aQuery  Search Query.
     */
    void SearchL(const CSearchQuery& aQuery,
                    const TSearchThreshold& aThreshold,
                    MSearchQueryObserver* aObserver);

    /**
     * Cancels the currently ongoing search.
     * @param    aClientId   Unique id of the client.
     */
    void CancelSearch( );

    /**
     * Cancels the currently ongoing search.
     * @param    aClientId   Unique id of the client.
     */
    void CancelResult( );

    /**
     * Function returning ETrue when a search is ongoing.
     * EFalse otherwise.
     * @return   ETrue when a search is ongoing.  EFalse otherwise.
     */
    TBool IsSearching();

    /**
     * Function returning a list of support content types that can be searched by the engine.
     * @return   The list of supported content types.
     * @Param : The content Resource
     * @param : The service ID
     */
    void GetSupportedContentL( RPointerArray<CSearchContent>& aContent,
                               TUid aServiceId );

    /**
     * Gets supported service ids by the search framework.
     * @param aContent On return contains the list of supported service ids.
     * 
     */
    void GetSupportedServicesL( RArray<TUid>& aServiceIdArray );

    /**
     * Gets the results. Asynchronous.
     *
     * @param aResultIDArray The array of result ids.
     * @param aObserver Pointer to the observer.
     * @return Request id. This id is used to cancel the request.
     */    
    void GetResultsL( const RPointerArray<CSearchDocumentId>& aResultIDArray,
                        MSearchResultRetrieveObserver* aObserver );

    /**
     * Launches a viewer/editor application for the given result id.
     *
     * @param aResultId The id of the given document.
     */    
    void LaunchApplicationL( const CSearchDocumentId& aResultId );
    /**
    * Get file launch information.
    */    
    HBufC8* GetLaunchInfoL( const CSearchDocumentId& aDocumentID );

private: // More constructors
    /**
     * Performs the first phase of two phase construction.
     */
    CSearchClientSessionImp( );

    /**
     * Performs the second phase construction.
     */
    void ConstructL();

    /**
     * Continues asking Light results
     */
    void CSearchClientSessionImp::ContinueSearchingLightL(); 	

    // Functions from base classes
    /**
     * From CActive.
     * Handles an active object’s request completion event.
     */
    void RunL();

    /**
     * From CActive.
     * Implements cancellation of an outstanding request.
     */
    void DoCancel();

    /**
     * From CActive.
     * Handles a leave occurring in the request completion event handler RunL().
     */
    TInt RunError( TInt aError );

    // Data

    /**
     * To keep the count of total res found
     */
    TInt iSearchTotalResultsFound;

    /**
     * Maxe res
     */
    TInt iMaxresults;

    /** 
     * Client side of the search server
     */
    RSearchClient   iSearchClient;

    /**
     * Set by the server when no more results
     * Validation
     */
    TPckgBuf< TInt >           iSearchFinished;//Validation

    /**
     * Set by the server 
     * Count of the results found
     */
    TPckgBuf< TInt >           iSearchCount;  

    /**
     * Set by the server
     * Count of the results found
     */
    TPckgBuf< TInt >           iSearchRes;   //Error Value

    /**
     * Light res Array
     */
    RPointerArray<CSearchLightResult> iLightResultArray;

    /**
     * Heavy res Array
     */
    RPointerArray<CSearchResult> iheavyresult;

    /**
     * Pointer to iObserver
     * Not own
     */
    MSearchQueryObserver*             iObserver;

    /**
     * Pointer to iRetrieveObserver
     * Not own
     */
    MSearchResultRetrieveObserver*	  iRetrieveObserver;	
    
    TBool iGetRes;
    
    TBool iSearchPending;
    
    RPointerArray<CSearchDocumentId> iResultIDArray;
    };

#endif		// SEARCH_SERVER_IMP_HANDLER_H

// End of File
