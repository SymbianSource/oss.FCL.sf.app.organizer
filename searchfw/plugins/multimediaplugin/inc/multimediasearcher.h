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
* Description:   ECom search interface definition
*
*/





#ifndef C_CMULTIMEDIASEARCHER_H
#define C_CMULTIMEDIASEARCHER_H

//SYSTEM INCLUDES
#include <e32std.h>
#include <badesca.h>
#include <searchcontentsearcher.h>
#include <searchcondition.h>
#include <MCLFOperationObserver.h>
#include <MCLFContentListingEngine.h>
#include <MCLFItemListModel.h>

//FORWARD DECLARATION
class CSearchTextSearcher;
class MSearchPluginObserver;
class MSearchPluginObserver;
class MCLFOperationObserver;
class MCLFItemListModel;
class MCLFContentListingEngine;


/**
 *  This class is the searcher class for the multimedia items (music/videos/images)
 *
 *  This class searches the multimedia  using the Content listing framework 
 *  of the mediagallery2
 *
 *  @lib multimediasearchplugin.lib
 */
class CMultimediaSearcher : public CActive, 
                            public MSearchContentSearcher, 
                            public MCLFOperationObserver
        
{
	public:
	
	/**
	* 1st phase constructor 
	* 

	*
	* @param aContentIdArray - content id
	* @param aCondition - condition
	* @param aPluginId - Implementation id
	* @return returns pointer to the constructed object of type CContactsSearcher
	*/
	static CMultimediaSearcher* NewL( const RArray<TUid>& aContentIdArray, 
                                const CSearchCondition& aCondition, 
                                const TUid& aPluginId,
                                MSearchPluginObserver& aObserver  );
   
    /**
    * Destructor
    */
    virtual ~CMultimediaSearcher();
    
    	
    /**
	* Sets the value of iCurrentContentClassUid
	*
	*
	* @param aSearchClassUid
	*/   
    void SetContentClassUid(const TUid aSearchClassUid);
    
    /**
	* Sets the value of iCurrentMediaTypeToBeUsed
	*
	*
	* @param aMediaType - Type of media i.e music/image/video
	*/  
    void SetMediaType(TCLFMediaType aMediaType);  
    
    /**
	* Gets  the complete file path corresponding to the index in the iCompleteFilePathArray
	* 
	*
	* @param aIndex - Index in the iCompleteFilePathArray
	*/  
    HBufC* GetFilePathArrayIndexValue(TInt aIndex);
    
    public: //from base class MSearchContentSearcher    
    /**
    * From MSearchContentSearcher
    * Releses the object.
    *
    */
    void Destroy();
    
    
    /**
     * From MSearchContentSearcher
     * Starts the search. Progress of the search is notified through aObserver. 
     * Notice that this call must be asynchronous.
     *
     * @param aObserver Observer for search progress.
     */
     void SearchL();
    
     /**
     * From MSearchContentSearcher 
     * Function returning ETrue when a search is ongoing.  EFalse otherwise.
     * 
     * @return ETrue when a search is ongoing.  EFalse otherwise.
     */
     TBool IsSearching();
    
     /**
     * From MSearchContentSearcher 
     * Cancels the ongoing search. This call must complete synchronously and no calls for
     * observers must be made after this call.
     *
     */
     void CancelSearch();
    
    /**
     * From MSearchContentSearcher 
     * Gets the results indicated by the given document ids. Asynchronous. Results will be given
     * through callback in MSearchPlugInObserver given in Search request.
     * 
     * @param aResults Results of the search process. Ownership transfers to caller.
     */
     void GetResultsL( const RPointerArray<CSearchDocumentId>& aDocumentIdArray );
    
    /**
     * From MSearchContentSearcher 
     * Cancels the result retrieval process.
     *
     */
     void CancelResultsRetrieve();
    
    /**
     * From MSearchContentSearcher 
     * Gets the current search progress. 
     *
     * @param aContentClassId On return contains the content class id, which is currently searched.
     * @param aCurrentDocument On return contains the index document that is currently being searched.
     * @param aTotalDocuments On return contains the total count of documents to be searched.
     */         
     void GetSearchProgressL( TUid& aContentClassId, TInt& aCurrentDocument, TInt& aTotalDocuments );


    /**
    * From MSearchContentSearcher , through MSearchTextSearcherObserver
    * Called when all search criteria are met.
    *
    * @param aCharPos The character position of the keyword match within the original text.
    */
    void HitL( TInt aCharPos );
    
    
    HBufC8* LaunchInfoL( const CSearchDocumentId& aDocumentID );

 public:  // from base class MCLFOperationObserver
    
    /**
    * Method to get list model operation events. This method is
    * called when an event is received.
    * @param aOperationEvent Operation event code of the event
    * @param aError System wide error code if the operation did not
    *        succeed.
    */
    void HandleOperationEventL( TCLFOperationEvent aOperationEvent,
                                            TInt aError ) ;
   
 protected: // From base class CActive
    
    /**
    * From CActive
    * Implements cancellation of an outstanding request.
	*
	* This function is called as part of the active object's Cancel().
	*/
	void DoCancel() ;
    
   /**
    * From CActive
	* Handles an active object's request completion event.
	*
	* The function is called by the active scheduler when a request
	* completion event occurs, i.e. after the active scheduler's
	* WaitForAnyRequest() function completes.
	*
	* Before calling this active object's RunL() function, the active scheduler 
	* has:
    * 	
	* 1. decided that this is the highest priority active object with
	*   a completed request
	*
    * 2. marked this active object's request as complete (i.e. the request is no 
	*   longer outstanding)
	*
	* RunL() runs under a trap harness in the active scheduler. If it leaves,
	* then the active scheduler calls RunError() to handle the leave.
	*
	* Note that once the active scheduler's Start() function has been called, 
	* all user code is run under one of the program's active object's RunL() or 
	* RunError() functions.
	*
	*/
	void RunL();
    
    	
   /**
	* From CActive 
	* If the RunL function leaves,
	* then the active scheduler calls RunError() to handle the leave.
	*
	*/
	TInt RunError( TInt aError );
	
	
   
   
private:    

    /**
    * Constructor
    *
    *
    * @param aPluginId
    */
     CMultimediaSearcher( const TUid& aPluginId );
    
    
    /** 
    * 2nd phase constructor
    *
    * @param aContentIdArray
    * @param aCondition
    */
    void ConstructL( const RArray<TUid>& aContentIdArray, 
		const CSearchCondition& aCondition ,MSearchPluginObserver& aObserver); 

    /**
    * Notifies the observer about the search complete
    *
    *
    */
    void ReportFinishedL();
    
    /**
    * Cleans up the memory held
    *
    *
    */
    void CleanUp();
        	
	/**
	* Does actual search
	*
    *
	*/
	TBool DoActualSearchL();
	
    /**
	* Searches Music,Video and Image files from the Media Gallery
	*
    *
	*/   
	void SearchMediaL();

	/**
	* Creates Search Data for Videos for one Media Gallery item
	*
    *
    * @param aItem - the multimedia item
	*/            
	TBool FetchSearchDataForVideosL(const MCLFItem& aItem);
    
    /**
	* Creates Search Data for Image Files for one Media Gallery item
	*
    *
    * @param aItem - the multimedia item
	*/   
    TBool FetchSearchDataForImagesL(const MCLFItem& aItem);
    
    /**
	* Creates Search Data for Music files for one Media Gallery item
	*
    * @param aItem - the multimedia item
	*/   
    TBool FetchSearchDataForMusicL(const MCLFItem& aItem);


    
private:
 /**
     * Text searcher
     * Own.  
     */
     CSearchTextSearcher* iTextSearcher;
    
    /**
     * Search Plugin Observer
     * Own.  
     */
     MSearchPluginObserver*      iObserver;
    
    
    /**
     * The plugin id 
     */
    TUid iPluginId;
  	
  	/**
     * Number of multimedia items  searched 
     */
    TInt iTotalNumOfItemsSearched;
    
    /**
     * Total number of successfull hits
     */
    TInt iTotalHits;
    
  	/**
     * The title and snippet buffer 
     */
    HBufC* iCurrentTitle;
    HBufC8* iCurrentSnippet;

    
    /**
     * Flags for  multimedia operations
     */
    TBool iAllMultimediaFilesSearched;
    TBool iSearchAlreadyStarted;
   	TBool iRefreshStarted;
    TBool iRefreshDone;
    TBool iCancelSearch;
    
    /**
     * The content listing framework engine
     * Own.  
     */

    /**
    * some data required for local use
    */
    RFs ifSession;
    TUid iCurrentContentClassUid;
    TCLFMediaType iCurrentMediaTypeToBeUsed;
	MCLFContentListingEngine* iCLFengine ;
	HBufC* iCompletefilePath;
	
	
	/**
    * The  caption for "Kilobyte", read from the resource file
    * Own
    */	
	HBufC8*	 iSizeKiloBytes;
    
	/**
    * The  caption for "Bytes", read from the resource file
    * Own
    */	
    HBufC8*  iSizeBytes;
	
	/**
     * The content listing framework modes
     * Own.  
     */
    MCLFItemListModel* iCLFmodel;
    
    /**
     * The mime type array used for searching multimedia item from CLF
     * Own.  
     */
    CDesCArray* iMimeTypeArray;

   /**
     * The data to be searched
     * Own.  
     */
    HBufC* iDataToSearch;
        
   /**
     * The array containing the complete path of the multimedia item
     * Own.  
     */
    RPointerArray<HBufC> iCompleteFilePathArray;
    
    /**
     * The heavy results array
      
     */
    RPointerArray<CSearchResult> iHeavyResultsArray;
};

#endif //C_CMULTIMEDIASEARCHER_H




