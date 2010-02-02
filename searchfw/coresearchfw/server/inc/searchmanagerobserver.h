/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Observer for results
*
*/



#ifndef SEARCH_MANAGER_OBSERVER_H
#define SEARCH_MANAGER_OBSERVER_H

// INCLUDES
// FORWARD DECLARATIONS
class CSearchResult;
class CSearchManager;
class CSearchLightResult;
// CLASS DECLARATION

/**
* Observer class for the search manager/service.
*/
class MSearchManagerObserver
	{
    public: // New functions
    	/**
    	* Called when results are found.  The result array may contain 1
    	*  or more results.
    	* @param    aClientId       Unique id of the client.
    	*
    	* @param    aResultArray    Array containing the results.
    	*/
    	virtual void ResultFoundL( RPointerArray<CSearchLightResult>& aResult,
    							   TInt aCurrentDocument,
                       			   TInt aTotalDocuments) = 0;

    	/**
    	* Called when a plug-in has finished searching.
    	* @param    aClientId       Unique id of the client.
    	*
    	* @param    aContentTypes   Array of content types where the plug-in was searching.
        */
    	virtual void SearchFinishedL( TInt aSearchResult, TInt aTotalNumberOfResults  ) = 0;
    	
    	
		/**
    	* Called when operation is complete.
    	*
    	* @param aResultArray The results returned by the operation. 
    	*        Ownership of the items is transferred to the caller.
    	*/   
    	virtual void ResultsRetrieveCompleteL( RPointerArray<CSearchResult>& aResultArray ) =0;
	};

#endif  // SEARCH_MANAGER_OBSERVER_H

// End of File

    	                            
