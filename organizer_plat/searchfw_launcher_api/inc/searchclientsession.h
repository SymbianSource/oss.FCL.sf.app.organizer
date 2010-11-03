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
* Description:  Client side session 
*
*/


#ifndef C_SEARCHCLIENTSESSION_H
#define C_SEARCHCLIENTSESSION_H

// INCLUDE FILES
#include <e32base.h>
#include <e32std.h>
#include <searchsessionobservers.h>
#include <searchcommon.h>
#include <searchresult.h>
#include <searchcontent.h>
#include <searchdocumentid.h>
#include <searchquery.h>
#include <searchcscommon.h>

// FORWARD DECLARATIONS
class CSearchClientSessionImp;


// CLASS DECLARATION
/**
  * This class represents a client side session to search framework.
  * This class is used by the Search UI.
  *
  * @lib    searchclient.lib
  */

NONSHARABLE_CLASS( CSearchClientSession ) :public CBase
    {
public:
    /**
     * Symbian OS 2 phased constructor.
     * @return - A pointer to the created instance of CSearchClientSession
     */
    IMPORT_C static CSearchClientSession* NewL();

    /**
     * Destructor
     */
    virtual ~CSearchClientSession();

    /**
     * Starts the search process. This call is asynchronous and progress is notified
     * through the observer.
     *
     *
     * @param aQuery - The search query.
     * @param aObserver - The search observer.
     * @param aThreshold - The threshold of the search (Example: maxResults, maxSearchInterval etc).
     */
    IMPORT_C void SearchL( const CSearchQuery& aQuery,
                           MSearchQueryObserver& aObserver,
                           const TSearchThreshold& aThreshold );

    IMPORT_C void CancelSearch( ) ;

    /**
     * Function returning a list of support content types that can be searched by the engine.
     *
     * @Param : aContent - On return, contains the array of supported content types. 
     * @param : aServiceId - The Type of service (Example:InDeviceSearch, ProximitySearch etc). 
     */
    IMPORT_C void GetSupportedContentL( RPointerArray<CSearchContent>& aContent,
                                        TUid aServiceId );

    /**
     * Gets supported service ids by the search framework.
     *
     * @param aContent - On return, contains the list of supported service ids.
     * 
     */
    IMPORT_C void GetSupportedServicesL( RArray<TUid>& aServiceIdArray );

    /**
     * Gets the search heavy results. 
     *
     * @param aResultIDArray - The array of document ids.
     * @param aObserver - Pointer to the observer through which the results are returned.
     * @return Request id -  This id is used to cancel the request.
     */    
    IMPORT_C void GetResultsL( const RPointerArray<CSearchDocumentId>& aResultIDArray,
                                     MSearchResultRetrieveObserver& aObserver );
    IMPORT_C void CancelGetResults( ) ;
    /**
     * Launches a viewer/editor application for the given document id.
     *
     * @param aResultId - The id of the given document to be launched.
     */    
    IMPORT_C  void LaunchApplicationL( const CSearchDocumentId& aResultId );

    /**
     * Get launch infor of a file to be opened.
     *
     */   
    IMPORT_C  HBufC8* GetLaunchInfoL( const CSearchDocumentId& aDocumentID );
private:

    /**
     * Constructor.
     */
    CSearchClientSession();

    /**
     * 2nd phase constructor.
     */
    void ConstructL();

    // Data 
    /**
     * Pointer to the session imp
     * Own
     */
    CSearchClientSessionImp* iSessionImp;
    };


#endif		// SEARCHCLIENTSESSION_H

// End of File
