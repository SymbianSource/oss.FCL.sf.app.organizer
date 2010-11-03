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
* Description:      main UI Class also Acts as the observer for results
*
*/






#ifndef C_GENRALSRCHUI_H
#define C_GENRALSRCHUI_H

#include <e32cons.h>
#include <eikenv.h>
#include <s32mem.h>
#include <searchsessionobservers.h>

#include "srchuimanager.h"

class CAknView;
class CSrchUiMainView;
class CCoeControl;
class CSrchUiManager;
class CSrchUiSearchView;
class CSrchUiResFolder;
class CSrchUiResFolderView;
class MSrchUiFolder;
struct TServiceInfo;

/**
 * This class is the main UI Class which creates ui manager. And this is 
 * also observer for the retrieved results
 *
 */
class CSrchUiGeneral : public CBase, public MSearchQueryObserver,
                       public MSearchResultRetrieveObserver,
                       public MSrchUiResultHandler
  	{
  	
public: 
	  /**
	   * Symbian OS 2 phased constructor.
	   *
	   * @param aAppUi view app ui
	   * @return A pointer to the created instance of CSrchUiGeneral.
	   */
	   IMPORT_C static CSrchUiGeneral* NewL( CAknViewAppUi* aAppUi );
	  
	  /**
	   * Symbian OS 2 phased constructor.
	   *
	   * @param aAppUi view app ui
	   * @param aReadStream stream from which to read 
	   * @return	A pointer to the created instance of CSrchUiGeneral.
	   */
	   IMPORT_C static CSrchUiGeneral* NewL( CAknViewAppUi* aAppUi, 
	 		RDesReadStream& aReadStream );
	  
	  /**
	   * Symbian OS 2 phased constructor.
	   *
	   * @param aAppUi view app ui
	   * @return	A pointer to the created instance of CSrchUiGeneral.
	   */
	   static CSrchUiGeneral* NewLC( CAknViewAppUi* aAppUi );
	  
	  /**
	   * Symbian OS 2 phased constructor.
	   *
	   * @param aAppUi view app ui
	   * @param aReadStream stream from which to read 
	   * @return	A pointer to the created instance of CSrchUiGeneral.
	   */
	   static CSrchUiGeneral* NewLC( CAknViewAppUi* aAppUi, 
	 	    RDesReadStream& aReadStream );
	  
	  /**
     * Destructor
     *
     */
	   ~CSrchUiGeneral();
		
		
		
	void SearchPartiallyCompletedL( TUid aPluginId );
	  /**
	   * Launches main view
	   *
	   */
	   void ConstructSearchMainViewL();
	  
	  /**
	   * Launches service specific main view
	   *
	   */
	   void ConstructServiceSpecificMainViewL( TUid aServiceId );
	  
	  /**
	   * Constructs tree structure of content classes and launches 
	   * result folder view
	   *
	   */
	   void ConstructResultFolderViewL( TUid /*aServiceUid*/ );
  
	  /**
     * Get the supported content classes by framework for the given service id
     *
     * @param aServiceId : The id of the service
     */
	   void GetSupportedContentClassesL( const TUid& aServiceId );
	  	
	  /**
     * Tests whether all content classes have been selected or not by the user
     *
     * @return	ETrue/EFalse
     */
	   TBool AllContentClassesSelected() const;
	
   	/**
     * Sets the flag to ETrue / EFalse
     *
     * @param aSelected : the flag to be set
     */
	   void SetAllContentClassesSelected( TBool aSelected );

    /**
     * Get the supported content classes
     *
     * @return	array of pointers to search content
     */
     RPointerArray<CSearchContent>& GetContentTypes();
    
    /**
     * Get the selected content types
     *
     */
     RArray<TBool>& SelectedContentTypes();
     
    /**
     * Checks whether the search application is launched from AIW Consumer
     *
     * @return ETrue if launched from AIW consumer else EFalse
     */
     TBool AiwFlag();
     
    /**
     * Triggers the search with Query string and the service uid
     *
     * @param aQueryString : query string
     * @param aServiceUid : selected service uid
     */
     void HandleSelectionL( TDesC8& aQueryString, TUid aServiceUid );
      
    /**
     * Get the result root folder
     *
     * @return MSrchUiFolder : pointer to root result folder
     */
     MSrchUiFolder* SrchUiResRoot();
      
    /**
     * Loads the result viewer plugin
     *
     * @param aSelectedFolderIdx selected folder index
     */
     void LoadResultViewerL( TInt aSelectedFolderIdx );
      
    /**
	   * Launches the previous view
	   *
	   */
     void ActivatePreviousViewL();
      
    /**
     * Get the Query string
     *
     * @return : reference to the query string
     */
     TDesC8& GetQueryString();
      
    /**
	   * Checks whether the search is ongoing
	   *
     * @return : ETrue if search is ongoing else EFalse
	   */
     TBool IsSearchOngoing();
   	  
     
    /**
	   * Launches the main view
	   *
	   */
     void GoToMainViewL();
     
    /**
	   * Symbian OS 2 phased constructor.
	   *
	   * @param aAppUi view app ui
	   * @return A pointer to the created instance of CSrchUiGeneral.
	   */
     IMPORT_C void SetSearchStringL( const TDesC& aString );
     
     /**
	   * Symbian OS 2 phased constructor.
	   *
	   * @param aAppUi view app ui
	   * @return A pointer to the created instance of CSrchUiGeneral.
	   */
	   IMPORT_C TDesC8& SearchStringL();
     
    /**
     * Sets the last visited view id
     *
     * @param aViewId : the view id of the previous view
     */
     void SetLastLaunchViewId( const TUid aViewId );
    
    //From base class MSearchQueryObserver
    
    /**
     * From MSearchResultRetrieveObserver
     *
     * Called when framework notifies about the search progress. The user of 
     * CSearchClientSession can define the time how often this will be called 
     * when requesting for search.
     *
     * @param aResultIdArray The array of results found between this call and 
     *                     last update.  Ownership of the items transferres.
     * @param aCurrentProgressValue The current progress of the search process.
     * @param aTotalProgressValue Final progress value of the search process.
     */  
     void UpdateSearchProgressL( RPointerArray<CSearchLightResult>& 
     						aResultArray, TInt aCurrentProgressValue, 
                              TInt aTotalProgressValue );
                                        
    /**
     * From MSearchResultRetrieveObserver
     *
     * Called when the search has been completed.
     *
     * @param aResult If the process was completed succesfully KErrNone, 
     *                otherwise an error code.
     * @param aNumberOfDocuments The number of found documents in the search.
     */
     void SearchCompletedL( TInt aSrchUiRes, TInt aNumberOfFoundDocuments );
     
     			
   
   	//From base class MSearchResultRetrieveObserver
   	
    /**
     * From MSearchResultRetrieveObserver
     *
     * Called when operation is complete.
     *
     * @param aResultArray The results returned by the operation. 
     *        Ownership of the items is transferred to the caller.
     */
     void ResultsRetrieveCompleteL( RPointerArray<CSearchResult>& 
    				aResultArray );

	//From MSrchUiResultHandler    				
	/**
     * From MSrchUiResultHandler
     *
     * Launches the application for given document id
     *
     * @param aResultId Document Id for which is to be launched
     */
    void LaunchApplicationL( const CSearchDocumentId& aResultId );    	
    
    /**
	 * From MSrchUiResultHandler
	 *
     * Launches the previous view
     */
     void LaunchPreviousViewL();
     
    /**
    * From MSrchUiResultHandler
	* Starts the new search operation
	*/
	void StartNewSearchL();
	
	/**
     * Cancels the search operation
     */
     TInt CancelSearch();
     
	  /**
     * Launches the search settings view.
     *
     */	
	 	  
	 void LaunchUiSettingsViewL();     
	 
	 /**
	  * Returns ETrue if the search is in progress
	  *
	  */
	 TBool IsSearching() ;
	
    				
	 /**
	 * Return the launch file info
	 *
	  */
	 HBufC8* GetLaunchInfoL( const CSearchDocumentId& aDocumentID );
private:

    /**
	   * Starts the new search operation
	   *
	   * C++ constructor - not exported
	   * implicitly called from NewLC()
	   *
	   */
	   CSrchUiGeneral( );
	
	  /**
	   * 2nd phase construction, called by NewLC()
	   *
	   */
	   void ConstructL( CAknViewAppUi* aAppUi ) ; // second-phase constructor
	  
	  /**
	   * 2nd phase construction, called by NewLC()
	   *
	   */
	   void ConstructL( CAknViewAppUi* aAppUi, RDesReadStream& aReadStream );
	  
	  /**
	   * Break the interested content classes into folders in case of context-sensitive
	   *
	   *
     * @param aUid The uid of the interested contest class
     *
	   */
	   void FindAndReplaceTheClassWithFoldersL( const TUid aUid,
	  		const TInt aFrom  );
	  		
	  /**
     * Reads the data from a Stream
     *
     * @param The Write Data Stream
     * @return None
     */    
     void InternalizeL( RDesReadStream& aStream );
     
     /**
     * Launches the view of the specified view id
     *
     * @param aNewViewId The id of the view to be launched
     */
     void LaunchViewL( TUid aNewViewId );
     
    /**
 	 * Loads the result viewer plugins
	 */
    void LoadUiResultViewerL();
    
    /**
     * Updates the Result View with the recently received heavy results
     *
     * @param aSearchFinished Indicates if searching is complete/not
     */
     void UpdateResultViewL(TBool aSearchFinished, TBool aRetrieveResultCompletes );

	 /**
     * Launches the result viewer plugins of the selected content class
     *
     * @param aSelectedFolderIdx The index of the selected content class
     * @param aSearchFinished Indicates if searching is complete/not
     */
     void LaunchResultViewL( TInt aSelectedFolderIdx, TBool aSearchFinished );
     
     /**
     * Intitalizes result viewer plugin
     *
     * @param aImpUid Implementation uid
     */
     CSrchUiResultViewerPlugin* InitializeUiResPlugInsL( TUid aImpUid );
     
     /**
     * Construct the tree structure of the selected content classes
     *
     * @param aSelectedContentTypes Selected Content classes
     */
     void FormResultFoldersL( RArray <TBool> aSelectedContentTypes );
    
    /**
     * Add the light results to a folder structure
     *
     * @param aResultArray RPointer array of light results
     */
     void AddResultsToFolderL( RPointerArray<CSearchLightResult> aResultArray );
    
    /**
     * Add the heavy results to a folder structure
     *
     * @param aResultArray RPointer array of heavy results
     */
     void AddHeavyResultsToFolderL( RPointerArray<CSearchResult> aResultArray );
    
    /**
     * Finds the class result folder 
     *
     * @param aContentid content id
     * @param aFolderId folder id
     * @return CSrchUiResFolder result folder
     */
     CSrchUiResFolder* FindClass( const TUid& aContentid, 
     		const TUid& aFolderId );
    
    /**
	 * Checks if a class is selected
	 *
	 * @param aFolderId folder id 
	 * @param aClassId class id to which class belongs
	 */		
	 TBool IsClassSelected( const TUid aFolderId, const TUid aClassId );
	 
	 /**
     * Constructs the result folder
     *
     * @param aContent content to be added
     * @param aParent where to be added
     */
     CSrchUiResFolder* ConstructResultFolderL( const CSearchContent *aContent, 
     		CSrchUiResFolder* aParent );
     		
	/**
     * Finds the class result folder 
     *
     * @param aClass class result folder
     * @param aContentid content id
     * @return CSrchUiResFolder result folder
     */
     CSrchUiResFolder* FindFolder( CSrchUiResFolder* aClass,
	  	const TUid& aContentid );     		
    
    /***
    * Clears gets heavy results corresponding to
    * light result
    *
    */
    void GetResultsFromManagerL() ;
	  
private:  //data
    
    /**
     * Pointer to the search main view
     *
     * Not Own
	   */
     CEikonEnv*  iSrchEikonEnv;
      
    
    /**
     * Pointer to the search main view
     *
     * Own
	   */
     CSrchUiMainView* iSearchMainView;
     
     /**
     * result folder constructed
     *
     * Own 
     */        
     CSrchUiResFolder* iSrchUiResRootFolder;
     
  
	  /**
     * Pointer to the search service view
     *
     * Own
	   */
	   CSrchUiSearchView* iSearchView;
	  

	  /**
     * Pointer to the result folder view
     *
     * Own
	   */
	   CSrchUiResFolderView* iResultFolderView;
	 
	  /**
     * Array of resource file ofsets
	   */
	   RArray<TInt> iResFileOffset;
	   
	  /**
     * Pointer to searchui manager
     *
     * Own
	   */
	   CSrchUiManager* iManager;
	  
    /**
     * Array of selected content classes
	   */
	   RArray <TBool> iSelectedContentTypes;
		
		/**
	   * ETrue if all content classes are selected. Else false
	   */
	   TBool iAllContentClassesSelected;
	 

	  /**
	   * Array of service plugins info
	   */
	   RPointerArray<TServiceInfo> iServiceInfoArray;
	  
		/**
	   * Array of CSearchContent of supported classes
	   */
	   RPointerArray<CSearchContent> iSupportedContentClasses;

	  /**
	   * The index of the folder selected in the result folder view
	   */
	   TInt iSelectedFolderIdx;
	  
	  /**
	   * Flag to indicate whether the search is ongoing or not
	   */
	   TBool iSearchOn;

    /**
	   * Pointer to CAknViewAppUi
	   */
	   CAknViewAppUi* iAppUi;

	  /**
	   * Number of results found
	   */
	   TInt iNumOfResultsFound;
	  	  
	  /**
	   * Flag to check whether the search application
	   *  is launched from AIW Consumer
	   */
	   TBool iAiwFlag;

	  /**
	   * Flag to check whether the search application is launched
	   * as embedded or stand alone
	   */
	   TInt32 iLaunchType;
	  
	  /**
	   * String to be searched
	   *
	   * Own
	   */
	   HBufC8* iSearchString;
	  
	  /**
	   * Array of context sensitive uids
	   */
	   RArray<TUid> iContextUids;

	  /**
	   * Number of content classes selected
	   * in the service specific main view
	   */
	   TInt iSelectedContentCount;
	   
	   /**
	   * Checks if Result viewer is launched
	   */
	   TBool iIsResultViewerLaunched;

	   /**
	   * Checks if heavy results are asked
	   */
	   TBool iGetResAgain;
	   
	   /**
	   * Checks if heavy results are retrieved
	   */
	   TBool iResRetriveComplete;
	   
	   /**
     	* Flag to store if new search is requested by result viewer plugin
     	*/
     	TBool iIsNewSearch;
     	
     	/**
     	* Array of views available
     	*/
     	RArray<TUid> iViewIds;
     	
     	/**
     	* Array of result viewer plugins available
     	*/
     	RPointerArray<CSrchUiResultViewerPlugin> iResultViewers;
     	
     	/**
     	* Uid of the last launched view
     	*/
     	TUid iLastLaunchViewId;
     	
     	/**
    	 * Keyword to be searched
     	*/
     	HBufC8* iQueryString;
     	
     	/**
     	* Light result count
     	*/
     	TInt iLightResultCount;
     
     	
     	
	  };
#endif //C_GENRALSRCHUI_H

