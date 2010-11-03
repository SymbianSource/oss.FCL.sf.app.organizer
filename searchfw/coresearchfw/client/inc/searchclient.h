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
* Description:   Client for search service Client/Server.
*
*/



#ifndef R_SEARCH_CLIENT_H
#define R_SEARCH_CLIENT_H

// INCLUDE FILES

#include <searchcommon.h>
#include <searchresult.h>
#include <searchtextsnippet.h>
#include <searchdocumentid.h>
#include <searchcontent.h>
#include <searchcscommon.h>
//Forward declaration
class CSearchQuery;
class CSearchSnippet;
class CSearchSnippetInfo;
class CSearchContentResource;

// CLASS DECLARATION

/**
  * Client side class of search service Client/Server.
  *
  * This class is used to create communiatiob between
  * ipc boundry
  * @lib searchclient.lib
  */

NONSHARABLE_CLASS (RSearchClient) : public RSessionBase
    {
public:
    /**
     * Constructor
     */
    RSearchClient();

    // New functions
    /**
     * Connects to the search server.
     */
    TInt Connect();
    /**
     * Returns the  API version number of the server's client.
     * @return	Version information.
     */
    TVersion Version() const;

    /**
     * Starts the search with the specified search criteria.Async call
     * @param aQuery  Search Query.
     * @param aSearchFinished Flag for search status
     * @param aThreshold The threshold of the search process.
     * @param aStatus RequestStatus.
     * @param aSearchCount Total count
     * @param aSearchRes Result Count
     * @return Request id. This id is used to cancel the request
     */
    TInt SearchL( const CSearchQuery& aSearchQuery,
                TPckgBuf<TInt>& aSearchFinished, const TSearchThreshold& aThreshold,
                TRequestStatus& aStatus,TPckgBuf<TInt>& aSearchCount, TPckgBuf<TInt>& aSearchRes );

    /**
     * Continuse search if it has not yet completed
     * @param aSearchFinished Flag for search status
     * @param aStatus RequestStatus.
     * @param aSearchCount Result count
     * @param aSearchRes Result Count
     * @return returns back the result status
     */
    void ContinueSearchingLightL( TPckgBuf<TInt>& aSearchFinished, TRequestStatus& aStatus, 
                                    TPckgBuf<TInt>& aSearchCount, TPckgBuf<TInt>& aSearchRes );

    /**
     * Function returning a list of support content types that can be searched by the engine.
     * @Param : The content Resource
     * @param : On return contains the list of supported content
     */
    void GetSupportedContentL( RPointerArray<CSearchContent>& aContent,
                               TUid aServiceId );
    /**
     * Get light results from server
     * @Param aLightResult Light result
     */
    void GetLightResultsL(RPointerArray<CSearchLightResult>& aLightResult);

    /**
     * Gets supported service ids by the search framework.
     * @param aServiceIdArray On return contains the list of supported service ids.
     */
    void GetSupportedServicesL( RArray<TUid>& aServiceIdArray );

    /**
     * Sends IDs to server to get heavy res the results. Asynchronous.
     * @param aResultIDArray The array of result ids.
     * @param aSearchFinished Flag for search status.
     * @param aStatus RequestStatus
     * @return Request id. This id is used to cancel the request.
     */    
    TInt GetResultsL( const RPointerArray<CSearchDocumentId>& aResultIDArray,
                        TPckgBuf<TInt>& aSearchFinished , TRequestStatus& aStatus
    );

    /**
     * Gets heay resultd from server
     * @param aSearchHeavyRes On retutn gets heavy res.
     */
    void GetHeavyResultsL( RPointerArray<CSearchResult>& aSearchHeavyRes );

    /**
     * Launches a viewer/editor application for the given result id.
     *
     * @param aResultId The id of the given document.
     */    
    void LaunchApplicationL( const CSearchDocumentId& aResultId );

    /**
     * Cancels the currently ongoing search.
     */
    void CancelSearch();
    /**
     * Cancels the heavy results asked.
     */
    void CancelResult();
    /**
    * Get file launch information.
    */
    HBufC8* GetLaunchInfoL( const CSearchDocumentId& aDocumentID );
    };

#endif  // SEARCH_CLIENT_H

// End of File
