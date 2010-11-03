/*
* Copyright (c) 2006-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Search manager
*
*/



#ifndef C_SEARCH_MANAGER_H
#define C_SEARCH_MANAGER_H

// INCLUDES
#include <e32base.h>
#include "searchpluginobserver.h"
#include "searchmanagercallbacktimer.h"
#include "searchcontent.h"
#include "searchquery.h"
#include "searchplugininterface.h"
#include "searchlightresult.h"

// FORWARD DECLARATIONS
class CSearchRequirement;
class CSearchResultArray;
class CSearchSession;
class MSearchManagerObserver;
class CSearchResultHandler;
class CDesC16Array;
class CDesC8Array;
class CSearchContentTypeArray;
class CSearch;
class CSearchContentResource;
class CSearchDocumentId;
class CSearchSnippetInfo;
class CSearchSnippet;
class CSearchPlugin;
class CSearchHandler;
class MContentSearcher;
class CSearchLightResult;



/**
* This function is for the timer. Once timer finshes it's interval it sends 
* informs manager through this function. Basically a call back function
* @param aptr Any potinter
*
* @return Integer value
*/
TInt ManagerCallbackL(TAny *aptr);

// CLASS DECLARATION	

/**
* Class managing/hiding the ecom plug-in handling of the search plug-in engine 
*  implementations.
* 
* Per each connected search client, one search session is constructed that will
* be the owner of those plug-in implementations.
*/
NONSHARABLE_CLASS (CSearchManager) : public CBase, public MSearchPluginObserver//HandlerObserver
	{
    public: // Constructor and destructor
		/** 
		* Symbian OS 2 phased constructor.
		*
		*
		* @param    aObserver   Search manager observer.
		*
		* @return	A pointer to the created instance of CSearchManager.
		*/
    	static CSearchManager* NewL( MSearchManagerObserver& aObserver, CSearchPluginInterface * aSearchPluginInterface );//, 
    															 //RPointerArray<CSearchPlugin> * aSearch );

		/** 
		* Symbian OS 2 phased constructor.
		*
		*
		* @param    aObserver   Search manager observer.
		*
		* @return	A pointer to the created instance of CSearchManager.
		*/
    	static CSearchManager* NewLC( MSearchManagerObserver& aObserver, CSearchPluginInterface * aSearchPluginInterface );//, 
    																//RPointerArray<CSearchPlugin> * aSearch );

    	/**
    	* Destructor.
    	*
    	*
    	*/
    	virtual ~CSearchManager();

        // New functions
      
    	
    	/**
    	* Gets supported service ids by the search framework.
   	 	*
   	 	*
    	* @param aContent On return contains the list of supported service ids.
    	*/   
    	void GetSupportedServicesL( RArray<TUid>& aServiceIdArray );
    
    	/**
    	* Gets content Types supported by plugins
    	*
    	*
    	* @param    aSupportedContentClasses   Array of ContentResources.
    	*
    	*/
       	void GetSupportedContentClassesL( RPointerArray<CSearchContent>& aContent,
                                        TUid aServiceId );
    	/**
    	* Cleans up any resource left from previous search
    	*
    	*
    	*/
       	void CleanupResourcesL();
   
     	/**
    	* Starts the search for the specficied client with the specified search
    	*  criteria.
    	*
    	* 
    	* @param    aClientId   Unique id of the client.
    	*
    	* @param    aCriteriaArray  Search criteria array.
    	*/
    	void SearchL( CSearchQuery* aSearchQuery, const TInt aGranularity, const TInt aMaxResults, const TInt64 aMaxTime );

    	/**
    	* Function returning ETrue when a search is ongoing for the specified
    	*  client.  EFalse otherwise.
    	*
        *
    	* @return   ETrue when a search is ongoing.  EFalse otherwise.
    	*/
    	TBool IsSearching( );

    	/**
    	* Cancels the currently ongoing search for the specified client.
    	*
    	*
    	*/
    	void Cancel( );
    
    	/**
    	* Cancels the result retrieval process.
    	*
    	*
     	*/
    	void CancelResultsRetrieve() ;
 
    	/**
    	* Close the manager session, removes all resources.
    	* 
    	*
    	*/
    	void Close();	
		
		/**
		* Starts timer. Installs callback.
		*
		*
		*/
		void StartTimerL( const TInt64 aMaxTime );

		/**
		* Forces light results to client in case ResultFoundl in not hit
		* 
		*
		*/
		void ForceResultToClientL();
		
		/**
		* Set by serversession to tell if results should be sent or not
		*
		*
		* @param aFlagValue if there is an outstanding request
		*/
		void SetFlagL( TBool aFlagValue );

		/**
		* Set by serversession to tell if results should be sent or not
		* 
		*
		* @return returns True/False whether heavy results has been asked 
		*/
		TBool IsHeavyResultsAsked ();

		/**
		* Complete the search after Cancel
		*
		*
		*/
		void SearchCompleteAfterCancelL( );
		
		/**
    	* This function is internal to manager. It is used to send back the results
    	* to the cleant session.
    	*
    	*
    	* @param    aClientId       Unique id of the client.
        *    	
    	* @param    aContentTypes   Array of content types where the plug-in was searching.
    	*/
		void SearchIntCompletedL( TInt aSearchResult, TInt aTotalNumberOfResults );
		
		/**
	    * Gets the results from all the plugins through handlers. Asynchronous.
	    *
        *
	    * @param aResultIDArray The array of result ids.
	    *
	    * @param aObserver Pointer to the observer.
	    *
	    * @return Request id. This id is used to cancel the request.
	    */    
	    void GetResultsL( const RPointerArray<CSearchDocumentId>& aResultIDArray );

		/**
	    * This function is called when heavy results are found. It inturn sennds 
	    * results back to through observer
	    *
	    */    
	    void SendheavyResultL( );
	    
	    /**
	    * Launches a viewer/editor application for the given result id.
	    *
	    *
	    * @param aResultId The id of the given document.
	    */    
	    void LaunchApplicationL( const CSearchDocumentId& aResultId );
	    /**
	    * Get launch information.
	    *
	    */    	    
	    HBufC8* GetLaunchInfoL( const CSearchDocumentId& aDocumentID );
   	
    	// Functions from base classe
        /**
        * From MSearchPluginObserver.
        *
    	* Called when results are found.  The result array may contain 1
    	*  or more results.
    	*
    	*
    	* @param    aClientId       Unique id of the client.
    	*
    	* @param    aResultArray    Array containing the results.
    	*/
		 
         void ResultFoundL( CSearchLightResult* aResult,                                
                               			TInt aCurrentDocument,
                               			TInt aTotalDocuments );
        /**
        * From MSearchPluginObserver.
        *
    	* Called when a plug-in has finished searching.
    	*
    	*
    	* @param    aClientId       Unique id of the client.
        *    	
    	* @param    aContentTypes   Array of content types where the plug-in was searching.
    	*/
    	void SearchCompletedL( TInt aSearchResult, TInt aTotalNumberOfResults );
    	
    	/**
	    * This fucntion is called when the search of particular plugin 
	    * is complete. 
	    * From MSearchPluginObserver
	    * @param aUid - 
	    */
	    void SearchPluginCompletedL( TUid aUid ) ;
	    
	    /**
	    * From MSearchPluginObserver.
	    * Called when operation is complete.
	    *
	    *
	    * @param aResultArray The results returned by the operation. 
	    *        Ownership of the items is transferred to the caller.
	    */   
	    void ResultsRetrieveCompleteL( RPointerArray<CSearchResult>& aResultArray );
         	
        
    private:    // More constructors
    	/** 
        * Performs the first phase of two phase construction.
        *
        *
        * @param    aObserver   Search manager observer.
        *
        * @param  	aSearchPluginInterface plugin interface
    	*/
    	CSearchManager( MSearchManagerObserver& aObserver, CSearchPluginInterface * aSearchPluginInterface );//, RPointerArray<CSearchPlugin> * aSearch );

    	/** 
        * Performs the second phase of two phase construction.
        *
        *
    	*/
		void ConstructL();
    	
   		// Data
	    /**
	    * To store the Granularity 
	    */
		TInt iGranularity;
	    /**
	    * To store the maxnumber of results 
	    */
		TInt iMaxResult;

	    /**
	    * To store the total number of results got from plugins 
	    */
		TInt iNumResults;

	    /**
	    * To keep the count of number of habdlers sending back the 
	    * heavy results
	    */
		TInt iNumHandlersSendingResult;

	    /**
	    * To keep the count of number of handlers finshing the search
	    */
		TInt iNumHandlersCompleted;

	    /**
	    * To keep the count of number of current document
	    */
		TInt iCurrentDocument;
		
	    /**
	    * To keep the count of number of total document
	    */
		TInt iTotalDocuments;

	    /**
	    * For the timer if no response from any of the handlers
	    */
		TInt64 iMaxTime;


		/**
	    * This flag is used to verify wheteher all the results in the 
	    * back up array have been flushed or not
	    */
		TBool iBackupFlag;

		/**
	    * This flag is used to verify wheteher all the results in the 
	    * lightresult array have been flushed or not
	    */
		TBool iLightFlag;

		/**
	    * This flag is used to send the search finished only once for
	    * the particular search
	    */
		TBool iComplete;

		/**
	    * This flag is used to veryfy whether heavy results have been
	    * asked for the particular search
	    */
		TBool iIsHeavyResultsAsked;

		/**
	    * This flag is used to veryfy whether there is an outstanding
	    * request 
	    */
		TBool iShouldResultBeSent;
		
		/**
	    * This flag is used to veryfy whether heavy search has been
	    * cancelled
	    */
		TBool iSearchCancelled;

	    /**
	    * This flag is used to verify whether all the plugins finished 
	    * searching 
	    */
		TBool iSearchComp;

		/**
	    * This array is store the light results 
	    */
   		RPointerArray<CSearchLightResult>		iLightResultHolder;

   		/**
	    * This array is store the light results of count = granularity 
	    */
   		RPointerArray<CSearchLightResult> iLightResultBackup;
   		
   		/**
	    * This array is store the heavy results of 
	    */
   		RPointerArray<CSearchResult> iHeavyResultArray;

   		/**
	    * This array is store the handlers created 
	    */
    	RPointerArray<CSearchHandler>	iHandlers;
  		/**
	    * Pointer to the plugin inertface
	    * Not Own
	    */
    	CSearchPluginInterface* iSearchPluginInterface;
    	
 		/**
	    * manager observer
	    * Not Own
	    */
    	MSearchManagerObserver&		iObserver;
 
 		/**
	    * Pointer to the call back timer
	    * Own
	    */
      CSearchManagerCallbackTimer * iCallbacktimer;
	};

#endif  // __SEARCH_MANAGER_H__

// End of File

