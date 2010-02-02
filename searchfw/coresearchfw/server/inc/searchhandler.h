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
* Description:   This is Source file for searchandler   
*
*/



#ifndef C_SEARCHHANDLER_H
#define C_SEARCHHANDLER_H


#include <searchpluginobserver.h>

class CSearchResult;
class CSearchLightResult;
class CSearchDocumentId;
class CSearchSnippetInfo;
class CSearchSnippet;
class MSearchPluginObserver;
class MSearchContentSearcher;

// CLASS DECLARATION	

/**
* Class is used to communcate between manager and searcher
* This class is used to get update of the search from plugin.
*/

NONSHARABLE_CLASS ( CSearchHandler ) : public CBase, 
						public MSearchPluginObserver
{
	
public:
	/**
	* Creating the handler - 1st phase construction
	* @param aManager - Observer or a call back provider when the results 
	* 	are found
	* @param aContentSearcher - ContentSearcher with which this handler 
	* 	wil be communicating with
	*/
	static CSearchHandler* NewL( MSearchPluginObserver* aManager, 
		MSearchContentSearcher* aContentSearcher );
		
	/**
	* Destructor
	*/
	virtual ~CSearchHandler();
	
	/**
	* Free the resources held by handler
	*/
	void Free();
	
	/**
	* Cancel the ongoing search
	*/
	void Cancel();
	
	/**
	* Cancel result retrieve
	*/
	void CancelRetrieveResult();
		
	/**
	* This function starts the search process
	*/	
	void SearchL( );
	
	/**
	* Gets the Results - Called by the manager after a time interval when 
	* granularity of results could not be found yet
	* @param aResults - In which Results should be written to
	*/
	void GetResultsL( const RPointerArray<CSearchDocumentId>& aDocumentIdArray ); 

	//From base class MSearchPluginObserver
    /**
    * Called when a is result is found.
    * From MSearchPluginObserver
    * @param aResult Pointer to the found result. Ownership is transferred.
    * @param aCurrentDocument The index of document that is currently being searched.
    * @param aTotalDocuments The total count of documents to be searched (from all content).
    */
    void ResultFoundL( CSearchLightResult* aResult,                                
                               TInt aCurrentDocument,
                               TInt aTotalDocuments );

	/**
	* This fucntion is called by the contentSearcher when the searching is
	* completed. 
	* From MSearchPluginObserver
	* @param aSearchResult - Result of the Seacrh KErrNone if successful.
	* @param aTotalNumberOfResults - Total number of documents found
	*/
	
	void SearchCompletedL( TInt aSearchResult, TInt aTotalNumberOfResults );
	
	/**
    * Called when operation is complete.
    * From MSearchPluginObserver
    * @param aResultArray The results returned by the operation. 
    *        Ownership of the items is transferred to the caller.
    */   
    void ResultsRetrieveCompleteL( RPointerArray<CSearchResult>& aResultArray );


	/**
    * Called when search is complete.
    * From MSearchPluginObserver
    * @param aUid  The Uid of the plugin which has completed the search.. 
    */ 
    void SearchPluginCompletedL( TUid aUid );
    
   /**
    * Returns the information required for launching the item corresponding to the document id
    * for this searcher.
    *
    * @param aExtensionUid Uid of the extension.
    */    
    HBufC8* LaunchInfoL( const CSearchDocumentId& aDocumentID );
    

private:
	
	/**
	* parameterized constructor
	*/
	CSearchHandler();

	/**
	* 2nd phase constructor
	* @param aManager - Manager observer whose resultfoundl to be 
	* 	called when results are found
	* @param aContentSearcher - pointer to content searcher which does the actual 
	*	search
	*/
	void ConstructL( MSearchPluginObserver* aManager , 
		MSearchContentSearcher* aContentSearcher ) ;
    
    //Data
    /**
    * content searcher
    * Own
    */
	MSearchContentSearcher* iSearcher; //content searcher - owned

    /**
    * Manager observer
    * Not Own
    */
	MSearchPluginObserver *iManagerObserver; //Manager observer 
	
};

#endif 

//End of file