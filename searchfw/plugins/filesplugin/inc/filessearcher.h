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
* Description:   Plugin for file names search 
*
*/





#ifndef C_FILESSEARCHER_H
#define C_FILESSEARCHER_H

#include <e32std.h>
#include <searchcontentsearcher.h>
#include <searchcondition.h>

class CSearchTextSearcher;


/**
 * This is a class dervied from CActive which does Searching
 *
 * This class implements the asynchronous calls for searching file
 * names
 *
 */
class CFilesSearcher : public CActive, public MSearchContentSearcher
    {
    
public:
	
	/**
	 * 1st phase constructor 
	 *
 	 * @param aContentIdArray - content id
	 * @param aCondition - condition
	 * @param aPluginId - Implementation id
	 * @return returns pointer to the constructed object of type CFilesSearcher
	 */
	static CFilesSearcher* NewL( const RArray<TUid>& aContentIdArray, 
                                const CSearchCondition& aCondition, 
                                const TUid& aPluginId,
                                MSearchPluginObserver& aObserver );                              
    /**
     * Destructor
     */
    virtual ~CFilesSearcher();
    
    //From base class MSearchContentSearcher
    
    /**
     * From MSearchContentSearcher
     * This function should be called before calling delete on the object
   	 * This fucntion releases all the resources held.
     */  
     
   /**
	* Gets  the complete file path corresponding to the index in the iCompleteFilePathArray
	* 
	*
	* @param aIndex - Index in the iCompleteFilePathArray
	*/  
    HBufC* GetFilePathArrayIndexValue(TInt aIndex);
                     
    void Destroy();
    
    /**
     * From MSearchContentSearcher
     * Starts the searching 
     *
     * @param aObserver plugin observer
     */
    void SearchL();
    
    /**
     * From MSearchContentSearcher
     * Returns TRUE if search is in progress else returns FALSE
     */
    TBool IsSearching();
    
    /**
     * From MSearchContentSearcher
     * Cancels the on going search. Nothing happens 
     * if tried to cancel already canceled object..
     * 
     */
    void CancelSearch();
    
    
    /**
     * From MSearchContentSearcher
     * Gets the heavy results corresponding to the documentid array
     *
     * @param aDocumentIdArray
     */
    void GetResultsL( const RPointerArray<CSearchDocumentId>& aDocumentIdArray );
    
    /**
     * From MSearchContentSearcher
     * Cancels the result retrieval.
     *
     */
    void CancelResultsRetrieve();
    
    /**
     * From MSearchContentSearcher
     * gets the search progress
     *
     * @param aContentClassId
     * @param aCurrentDocument
     * @param aTotalDocuments
     */
    void GetSearchProgressL( TUid& aContentClassId, TInt& aCurrentDocument, TInt& aTotalDocuments );
    
    /**
     * From MSearchContentSearcher
     * Will be called by the text searcher  when the result matching search criteria
     * is found
     *
     * @param aResult
     */
    void HitL( TInt aResult );
    
    
    
    HBufC8* LaunchInfoL( const CSearchDocumentId& aDocumentID );

private:

    /**
     * Constructor
     *
     * @param aPluginId
     */
    CFilesSearcher( const TUid& aPluginId );
    
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
     */
    void ReportFinishedL();
    
    /**
     * Cleans up the memory held
     */
    void CleanUp();
    
    /**
     * This function returns TRUE if access to path 
     * is restricted as it is a system dir/file
     */
    TBool IsSystemPath( TDesC& aPath );
	
	/**
	 * Gets all the files and subfolders, and also does the search
	 *
	 * @param aFolder
	 */
	void GetFileNameArrayL(const TDesC& aFolder );
    
    /**
	 * Does actual search
	 */
	TBool DoActualSearchL();
	
	/**
     * Called by ECOM framework
	 */
	void DoCancel() ;
	
	//From Base Class CActive
	
	/**
	 * From CActive 
	 */
	void RunL();
	
	/**
	 * From CActive 
	 */
	TInt RunError();

private:
	/**
	 * To read "KB" from resource file
	 * Own
	 */
	HBufC* iSizeKB; 
	
	/**
     * To read "B" from resource file
	 * Own
	 */
	HBufC* iSizeB; 
	
	/**
     * To read "MB" from resource file
	 * Own
	 */
	HBufC* iSizeMB; 
	
	/**
	 * Current count of files searched
	 */
    TInt iCurrentItemCount;
    
    /**
	 * total number of files matching search criteria
	 */
    TInt iTotalHits;
    
    /**
	 * File session opened
	 */
    RFs iFs;
    
    /**
     * plugin Id
     */
    TUid iPluginId;
    
    /**
     * Textsearcher which searches a given keyword
     */
    CSearchTextSearcher* iTextSearcher;
    
    /**
     * Observer to notify found results
     */
    MSearchPluginObserver*      iObserver;
    
    /**
     * list of files to search
     */
    CDir* iFiles;   
    
    /**
     * folder under search
     */
	HBufC* iFolder; 
	
    /**
     * filename under search
     */
	HBufC* iFileName;  
	
	/**
     * Light results
     * Not own
     */
	RPointerArray<CSearchLightResult> iLightResultsArray;
	
	/**
     * List of available drives
     */
	TDriveList iDriveList;
	
	/**
     * Current drive under search
     */
	TInt iCurrentDriveIndex;
	
	/**
     * True if heavy results asked
     */
	TBool iHeavyResultsAsked;
	/**
     * The array containing the complete path of the multimedia item
     * Own.  
     */
    RPointerArray<HBufC> iCompleteFilePathArray;
	
	RPointerArray<CSearchResult> iHeavyResultsArray; 
	
    };

#endif //C_FILESSEARCHER_H



