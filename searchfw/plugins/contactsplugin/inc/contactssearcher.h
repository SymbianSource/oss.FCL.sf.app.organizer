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




#ifndef C_CCONTACTSSEARCHER_H
#define C_CCONTACTSSEARCHER_H

//SYSTEM INCLUDES
#include <f32file.h>
#include <badesca.h>
#include <searchcontentsearcher.h>
#include <searchcondition.h>

#include <MVPbkContactStoreListObserver.h>
#include <MVPbkSingleContactOperationObserver.h>
#include <MVPbkContactViewObserver.h>

// FORWARD DECLARATIONS
class CSearchCondition;
class CVPbkContactManager;
class CVPbkContactStoreUriArray;
class CVPbkContactLinkArray;
class MVPbkContactOperationBase;
class MVPbkContactLinkArray;
class MVPbkBaseContact;
class CSearchFScanRegistryEntry;
class CSearchTextSearcher;
class MSearchPluginObserver;
class CVPbkFieldTypeRefsList;
class MVPbkContactViewBase;


/**
 *  This class is the searcher class for the contacts
 *
 *  This class searches the contacts using the virtual phonebook2 apis
 *
 *  @lib contactssearchplugin.lib
 */
class CContactsSearcher : public CActive, 
                          public MSearchContentSearcher, 
	                      public MVPbkContactStoreListObserver,
                          public MVPbkSingleContactOperationObserver,
                          public MVPbkContactViewObserver
{	
public:
	
	
   /**
    * 1st phase constructor 
    *

  	* @param aContentIdArray - content id
	* @param aCondition - condition
	* @param aPluginId - Implementation id
	* @return returns pointer to the constructed object of type CContactsSearcher
	*/
	static CContactsSearcher* NewL( const RArray<TUid>& aContentIdArray, 
                                const CSearchCondition& aCondition, 
                                const TUid& aPluginId,
                                MSearchPluginObserver& aObserver );
   /**
    * Destructor
    */
    virtual ~CContactsSearcher();
   
    /**
    * A function that returns the contact link at given index
    * of the iContactLinkArray
    *
    *
    * @param aIndex
    */
    MVPbkContactLink* GetContactLinkAtIndexLC(TInt aIndex);
    
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

    
public:
     // from base class MVPbkContactStoreListObserver
     
     /**
      * From MVPbkContactStoreListObserver  
      * Called when the opening process is complete, ie. all stores have been reported
      * either failed or successful open.
      *
      */
      void OpenComplete();  
      
     /**
      * From MVPbkContactStoreListObserver  
      * Called when a contact store is ready to use.
      */
      void StoreReady(MVPbkContactStore& aContactStore);

     /**
      * From MVPbkContactStoreListObserver  
      * Called when a contact store becomes unavailable.
      * Client may inspect the reason of the unavailability and decide whether or not
      * it will keep the store opened (ie. listen to the store events).
      *
      *
      * @param aContactStore The store that became unavailable.
      * @param aReason The reason why the store is unavailable.
      *                This is one of the system wide error codes.
      */
      void StoreUnavailable(MVPbkContactStore& aContactStore, TInt aReason);

     /**
      * From MVPbkContactStoreListObserver  
      * Called when changes occur in the contact store.
      * @see TVPbkContactStoreEvent
      * @param aContactStore The store the event occurred in.
      * @param aStoreEvent   Event that has occured.
      */
      void HandleStoreEventL(
                MVPbkContactStore& aContactStore, 
                TVPbkContactStoreEvent aStoreEvent);

    
public:
      // From base class MVPbkSingleContactOperationObserver
          
      /**
       * From MVPbkSingleContactOperationObserver
       * Called when operation is completed.
       *
       *
       * @param aOperation the completed operation.
       * @param aContact  the contact returned by the operation.
       *                  Client must take the ownership immediately.
       *
       *                  !!! NOTICE !!!
       *                  If you use Cleanupstack for MVPbkStoreContact
       *                  Use MVPbkStoreContact::PushL or
       *                  CleanupDeletePushL from e32base.h.
       *                  (Do Not Use CleanupStack::PushL(TAny*) because
       *                  then the virtual destructor of the M-class
       *                  won't be called when the object is deleted).
       */
       void VPbkSingleContactOperationComplete(
                MVPbkContactOperationBase& aOperation,
                MVPbkStoreContact* aContact );

      /**
       * From MVPbkSingleContactOperationObserver
       * Called if the operation fails.
       *
       * @param aOperation    the failed operation.
       * @param aError        error code of the failure.
       */
       void VPbkSingleContactOperationFailed(
                MVPbkContactOperationBase& aOperation, 
                TInt aError );
       
public:
       void ContactViewReadyL(
                          MVPbkContactViewBase& aView );
       //From MVPbkContactViewObserver
       /**
        * Called when a view is ready for use. 
        * 
        * This function may also be called if view is already ready. Then
        * it means that the view has been updated and the observer have
        * to take this into account.
        *
        * @param aView A contact view sending the event.
        */
       void ContactViewReady(
                      MVPbkContactViewBase& aView );

       /**
        * Called when a view is unavailable for a while. 
        * 
        * When the view is again available ContactViewReady will be called.
        * The contents of the view may change completely while it is
        * unavailable.
        *
        * @param aView A contact view sending the event.
        */
       void ContactViewUnavailable(
                      MVPbkContactViewBase& aView );
       
       /**
        * Called when a contact has been added to the view.
        *
        * @param aView A contact view sending the event.
        * @param aIndex An index of the contact in the view.
        * @param aContactLink A link to the added contact that is
        *                     valid only during the functio call.
        */
       void ContactAddedToView(
                      MVPbkContactViewBase& aView, 
                      TInt aIndex, 
                      const MVPbkContactLink& aContactLink );
       
       /**
        * Called when a contact has been removed from a view.
        *
        * @param aView A contact view sending the event.
        * @param aIndex An index of the removed contact.
        * @param aContactLink A link to the removed contact that is
        *                      valid only during this function call.
        */
       void ContactRemovedFromView(
                      MVPbkContactViewBase& aView, 
                      TInt aIndex, 
                      const MVPbkContactLink& aContactLink );
       
       /**
        * Called when an error occurs in the view.
        *
        * If client decides to destroy the view then it should do
        * it asynchronously because the view can access member data
        * after a call to this.
        *
        * @param aView A contact view sending the event.
        * @param aError An error code of the failure.
        * @param aErrorNotified  ETrue if the implementation has already 
        *                        notified user about the error using e.g
        *                        an ECOM plug-in, EFalse otherwise.
        */
       void ContactViewError(
                      MVPbkContactViewBase& aView, 
                      TInt aError, 
                      TBool aErrorNotified );
       
    
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
    CContactsSearcher( const TUid& aPluginId );
    
   /** 
    * 2nd phase constructor
    *
    * @param aContentIdArray
    * @param aCondition
    */
    void ConstructL( const RArray<TUid>& aContentIdArray, 
	                 const CSearchCondition& aCondition,
	                 MSearchPluginObserver& aObserver );
	
  /**
    * Notifies the observer about the search complete
    *
    */
    void ReportFinishedL();
    
    /**
    * Cleans up the memory held
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
    * Fetches the contact items from the database.
    *
    */
	void FetchContactItemsFromDbAndSearchL();
    
    /**
    * Handles the operations for a single contact after it is fetched
    *
	*
	* @param aContact - The contact from database 
    */
    void HandleRetrievedContactL(MVPbkStoreContact* aContact );
    
    /**
    * Fetches the data from a particular contact 
    *
    *	
	* @param aContact - The contact from database 
    * 
    */
    void  GetDataForSingleContactL ( MVPbkBaseContact& aContact );
    
    /**
    * Add the data from contact fields
    *
	*
	* @param aContact - The contact from database 
    * @param afieldtype- Field to be added 
    */
    void AddContactFieldsL(MVPbkBaseContact& aContact,TInt afieldtype);

   
   
private: // Data
    
    /**
     *PluginId of contacts plugin
     */ 
    TUid iPluginId;         
    
    /**
     * Number of contacts searched 
     */
    TInt iTotalNumOfContactsSearched; 
    
    /**
     *Total Number of matches
     */
    TInt iTotalHits;        
    /**
     * Flags for  store operations
     */
    TBool iStoreReadyForAccessing ;
    TBool iAtLeastOneStoreReady;
    TBool iAllContactsSearched;
    TBool iNoContactStoreAvailable;
    TBool iSearchStarted;
    TBool iSearchCancelled;
    
    
    
    /**
     * Text searcher
     * Own.  
     */
     CSearchTextSearcher* iTextSearcher; // Text searcher
    
    /**
     * Search Plugin Observer
     * Own.  
     */
     MSearchPluginObserver*      iObserver;
   	
   	
    /**
     * Contact Link array containing the contact links of all the contacts in the database
     * Own.  
     */
    CVPbkContactLinkArray* iAllContactItemsFromDb;
    
    /**
     * The contact manager for accessing the phone contacts
     * Own.  
     */
    CVPbkContactManager*  iContactManager;
    
    /**
     * The contact Uri Array 
     * Own.  
     */
     CVPbkContactStoreUriArray* iCurrentlyOpenStores;
    
     /**
     * The contact operation. Used during Find operation
     * Own.  
     */
     MVPbkContactOperationBase* iVPbkOperation;
    
    /**
     * The Field types to be searched.Used during Find operation
     * Own.  
     */
     CVPbkFieldTypeRefsList* iFieldTypeRefList ;
    
    /**
    * The contact data to be searched
    * Own.  
    */
    HBufC* iContactsDataTobeSearched ;	
    
    /**
    * The file session
    */
	RFs ifSession;
	

	/**
     * Title,snippet,current contact data buffers
     */
    HBufC* iCurrentTitle;
    HBufC8* iCurrentSnippet;
    HBufC* iCurrentContactDataString;
    HBufC* iBufferForQuery;
   /**
    * The contact link currently being accessed
    * Own
    */
    TInt iCurrentContactLinkIndex;
      
   /**
    * The array containing the heavy results data
    * Own
    */
    RPointerArray<CSearchResult> iHeavyResultsArray;
    
   /**
    * The search strings used for searching in the contacts database 
    * Own
    */
    CDesCArrayFlat* iSearchStrings;
    
    /**
    * The array containing the map data from iHeavyResultsArray to
    * iAllContactItemsFromDb
    * 
    */
    RArray<TInt>    iContactArrayMapper;

    /**
    * The array containing data for deleted contacts
    * 
    */
    RArray<TBool> iCotnactDeletedList;
    
    /**
    * The current database item in use
    * 
    */
    TBool iIsFilterView;
    
    MVPbkContactViewBase* iContactViewBase;
    MVPbkContactViewBase* iFilterView;
    TInt iCurrentDatatBaseItemIndex; 
};





#endif //C_CCONTACTSSEARCHER_H



