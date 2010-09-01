/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   This file is header file for seachserversession
*
*/



#ifndef C_SEARCH_SERVER_SESSION_H
#define C_SEARCH_SERVER_SESSION_H

// INCLUDE FILES
#include <e32base.h>



#include <searchquery.h>
#include <searchcscommon.h>
#include "searchmanagerobserver.h"
#include "searchplugininterface.h"


// FORWARD DECLARATIONS
class CSearchServer;
class CSearchResultArray;
class CSearchResult;
class CSearchEvent;
class CSearchSnippetInfo;
class CSearchSnippet;

// CLASS DECLARATION

/**
* Server session class of search service Client/Server.
* 
* This Class is a Server session created by server to 
* handle request made by the cleint.  
*/
class CSearchServerSession : public CSession2, public MSearchManagerObserver
	{
	public:	// Constructors and destructor
		/**
		* Symbian OS 2 phased constructor.
		*
		* @param	aServer						Pointer to the search server.
		* @param	aSearchPluginInterface		Pointer to the aSearchPluginInterface.
		* @return	A pointer to the created instance of CSearchServerSession.
		*/
		static CSearchServerSession* NewL( CSearchServer& aServer, CSearchPluginInterface * aSearchPluginInterface );

		/**
		* Symbian OS 2 phased constructor.
		* @param	aServer						Pointer to the search server.
		* @param	aSearchPluginInterface		Pointer to the aSearchPluginInterface.
		* @return	A pointer to the created instance of CSearchServerSession.
		*/
		static CSearchServerSession* NewLC( CSearchServer& aServer, CSearchPluginInterface * aSearchPluginInterface );
		
        /**
    	* Destructor
    	*/
    	~CSearchServerSession();
    
    	/**
    	* Called when results when the error ouures
       	*
    	* @param    aErrorCode      Error code.
      	*/
    	void ErrorHappened( const TInt& aErrorCode );
 
		// Functions from base classes
    	
    	/**
    	* From CSession2.
    	* Handles the servicing of a client request that has been passed to the server.
    	*/
		void ServiceL( const RMessage2& aMessage );
		
    	// Functions from MSearchManagerObserver
    	
    	/**
    	* Called when results are found.  The result array may contain 1
    	*  or more results.
    	* From MSearchManagerObserver
    	*
    	* @param    aClientId       Unique id of the client.
    	*
    	* @param    aResultArray    Array containing the results.
    	*/
        void ResultFoundL( RPointerArray<CSearchLightResult>& aResult, TInt aCurrentDocument,TInt aTotalDocuments );

    	/**
    	* Called when a plug-in has finished searching.
    	* From MSearchManagerObserver
    	*
    	* @param    aClientId       Unique id of the client.
    	*
    	* @param    aContentTypes   Array of content types where the plug-in was searching.
        */
        void SearchFinishedL( TInt aCurrentDocument,TInt aTotalDocuments);

		/**
    	* Called when operation is complete.
    	* From MSearchManagerObserver
	    * @param aResultArray The results returned by the operation. 
	    *        Ownership of the items is transferred to the caller.
	    */   
    	void ResultsRetrieveCompleteL( RPointerArray<CSearchResult>& aResultArray );
    	 /**
    	  * Get launch information.
    	  *
    	  */    	    	
    	void GetLaunchInfoL( const RMessage2& aMessage );

	private:	// More constructors
		/** 
		* Performs the first phase of two phase construction.
		* @param	aServer		Pointer to the search server.
		*/
		CSearchServerSession( CSearchServer& aServer );

    	/**
    	* Performs the second phase construction.
 		* @param	aSearchPluginInterface  Pointer to the aSearchPluginInterface.   	
    	*/
        void ConstructL( CSearchPluginInterface * aSearchPluginInterface );

		//New functions
		
		/**
		* Services a clients request.
		* @param	aMessage	 Object encapsulating the client's request.
		*/
		void DispatchMessageL( const RMessage2& aMessage );

		/**
		* Services a clients request GetServiceIdL.
		*
		* @param	aMessage	 Object encapsulating the client's request.
		*/
		void GetServiceIdL( const RMessage2& aMessage );

		/**
		* Services a clients request GetContentTypesL.
		* @param	aMessage	 Object encapsulating the client's request.
		*/
		void GetContentTypesL( const RMessage2& aMessage );

		/**
		* Services a clients request SetSearchQueryL.
		* @param	aMessage	 Object encapsulating the client's request.
		*/
		void SetSearchQueryL( const RMessage2& aMessage );

		/**
		* Services a clients request SearchL.
		* @param	aMessage	 Object encapsulating the client's request.
		*/
		void SearchL( const RMessage2& aMessage );
	
		/**
		* Services a clients request GetLightResultsL.
		* @param	aMessage	 Object encapsulating the client's request.
		*/

		void GetLightResultsL( const RMessage2& aMessage );
		/**
		* Services a clients request ContinueSearchL.
		* @param	aMessage	 Object encapsulating the client's request.
		*/
		void ContinueSearchL( const RMessage2& aMessage );

		/**
		* Services a clients request GetDocumentIDArrayL.
		* @param	aMessage	 Object encapsulating the client's request.
		*/
		void GetDocumentIDArrayL( const RMessage2& aMessage );

		/**
		* Services a clients request GetSearchResultsL.
		* @param	aMessage	 Object encapsulating the client's request.
		*/
		void GetSearchResultsL( const RMessage2& aMessage );
	
		/**
		* Services a clients request GetHeavyResultsL.
		* @param	aMessage	 Object encapsulating the client's request.
		*/
		void GetHeavyResultsL( const RMessage2& aMessage );

		/**
		* Services a clients request CancelSearchL.
		* @param	aMessage	 Object encapsulating the client's request.
		*/
		void CancelSearchL( const RMessage2& aMessage );

		/**
		* Services a clients request CancelResult.
		*
		* @param	aMessage	 Object encapsulating the client's request.
		*/
		void CancelResult( const RMessage2& aMessage );

		/**
		* Services a clients request LaunchApplicationL.
		* @param	aMessage	 Object encapsulating the client's request.
		*/
		void LaunchApplicationL( const RMessage2& aMessage );

		/**
		* Sending back the results to the client.
		* @param aSearchResult	 number of results.
		* 
		*/
		void SignalClientL(TInt aSearchResult, TInt aTotalDocument);
		
		/**
		* Sending back the results to the client.
		* @param	aResult	 light rsult.
		* @param	aCurrentDocument current document.	
		*/
		void SignalClientL( RPointerArray<CSearchLightResult>& aResult, TInt aCurrentDocument );
	
		/**
		* Sending back the search (finished/notfinished) status to client.
		* 
		*/
		void SignalClientL( );
		
		// New functions
		
		/**
		* Panics the client.
		* @param	aPanic	Panic code.	
		*/
		void PanicClient( TInt aPanic );

			
		// Data
		/**
		* The flag to indicate whether the result is completly over
		*/
		TInt iFullyFinishedBuffer;

		/**
		* Total Number of Currentdocuments.
		*/
		TInt iCurrentDocument;

		/**
		* Total Number of documents.  
		*/
		TInt iTotalDocuments;
		
		/**
		* Error code
		*/
		TInt iErrorBuffer;

		/**
		* Grnularity of the results to be sent
		*/
		TInt iGranularity;

		/**
		* Max results to be sent.
		*/
		TInt iMaxResults;

		/**
		* Time Interval
		*/
		TInt64 iInterval;

 		/**
		* Uid array for supported services
		*/
		RArray<TUid> iServiceIdArray;

		/**
		* Message received from the client
		*/
		RMessage2		    iMessage;	// Not owned

		/**
		* Check for SupportedContents have been asked
		*/
		TBool iGetContentTypes;

		/**
		* Check for ServiceIds have been asked
		*/
		TBool iGetServiceId;

		/**
		* Flag indicating whether a client request is outstanding
		*/
    TBool               iRequestOutstanding;

		/**
		* DocID array
		*/
        RPointerArray<CSearchDocumentId> idocIdArray;

		/**
		* Light result array
		*/
		RPointerArray<CSearchLightResult> iLightResult;
		
		/**
		* Light result array for destroying Light results at the end.
		*/
		RPointerArray<CSearchLightResult> iLightResultBackup;
		
		/**
		* Supported content array
		*/
		RPointerArray<CSearchContent> iSupportedContent;

		/**
		* Heavy results array
		*/
		RPointerArray<CSearchResult> iResultArray;

		/**
		* Reference to the owning server
		* Not own
		*/
    CSearchServer&      iSearchServer;

		/**
		* Pointer to the Manager
		* Own
		*/
    CSearchManager*     iSearchManager;

		/**
		* Pointer to the actual query that needs to be sent to the plugins
		* Own
		*/
		CSearchQuery* 		iSearchQuery;


	};


#endif  //SEARCH_SERVER_SESSION_H

// End of File