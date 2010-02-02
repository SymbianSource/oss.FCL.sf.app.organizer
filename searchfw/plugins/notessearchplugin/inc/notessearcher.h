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




#ifndef      C_CNOTESSEARCHER_H
#define      C_CNOTESSEARCHER_H

//System Includes
#include <e32std.h>
#include <d32dbms.h>
#include <e32cmn.h> 
#include <badesca.h> 
 
//Search FW Common includes
#include <searchcontent.h>
#include <searchcontentsearcher.h>


//Forwared declarations
class CSearchCondition;
class CSearchResult;  
class CSearchTextSearcher;
class MSearchPluginObserver;
class CNotesSearchPluginUtils;

/**
 *  This class is the searcher class for the notes
 *
 *  This class searches the phonebook notes
 *
 *  @lib notessearchplugin.lib
 */

class CNotesSearcher : public CActive , 
                       public MSearchContentSearcher
	{

public: // Constructors and destructor
        /**
        * 
        * First phase construction
        * 
        * @return constructed CNotesSearchPlugin object.
        */
      	static CNotesSearcher*  NewL(const RArray<TUid>& aContentIdArray, 
                                         	  const CSearchCondition& aCondition, 
                                			  const TUid& aPluginId,
                                			  MSearchPluginObserver& aObserver );
      	
        /**
        * Destructor.
        * 
        */
        virtual ~CNotesSearcher();
        
        /**
        * Check if the entry exists in notes
        *
        */
        TBool CNotesSearcher::CheckIfEntryExistsL (TInt aNoteKey);
        /**
        * Cleanup internal resource
        *
        */
        void  CleanUp();

        

        
public:// from MSearchContentSearcher interface 
    
		/**
		* Search FW calls this function when it no longer needs this searcher.
		*
		*/
		void Destroy();


		/**
		* Starts the search. Progress of the search is notified through aObserver. Notice that
		* this call must be asynchronous.
		*
		*/
		void SearchL( void);

		/**
		* Function returning ETrue when a search is ongoing.  EFalse otherwise.
		* 
		* @return ETrue when a search is ongoing.  EFalse otherwise.
		*/
		TBool IsSearching();

		/**
		* Cancels the ongoing search. This call must complete synchronously and no calls for
		* observers must be made after this call.
		*  
		*/
		void CancelSearch();

		/**
		* Gets the results got so far. A call to this function must flush the 
		* plugins internal buffer so that results returned by this call are not
		* given twice. 
		* This is called in case the plugin fails to provide results to the framework
		* through MSearchPlugInObserver in the time defined by the engine.
		*
		* @param aResults Results of the search process. Ownership transfers to caller.
		*/
		void GetResultsL( const RPointerArray<CSearchDocumentId>& aDocumentIdArray );

	
		/**
		* Gets the current search progress. Typically this is called by the framework 
		* in case the plugin fails to provide results through MSearchPlugInObserver in the 
		* time defined by the framework.
		*
		* @param aContentClassId The content class, whose search progress is requested. If NULL, the function should return the 
		*                        total search progress.
		* @param aCurrentDocument The index document that is currently being searched (from the set of documents belonging to given
		*                         content class).
		* @param aTotalDocuments The total count of documents to be searched (from the set of documents belonging to given
		*                         content class).
		*/         
		void GetSearchProgressL( TUid& aContentClassId, TInt& aCurrentDocument, TInt& aTotalDocuments );
		
		/**
        * Cancels the result retrieval process.
        *
        */
		void CancelResultsRetrieve();
		
protected: //from the base class CActive 
        
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
    * From CActive.
    *
    * Implements cancellation of an outstanding request.
    */
    void DoCancel();
        
    /**
	* From CActive 
	* If the RunL function leaves,
	* then the active scheduler calls RunError() to handle the leave.
	*
	*/
    TInt RunError( TInt aError );

        		
private:
    /**
    * Does some initialization for database
	*/  
    void    InitDbL();
        
	/**
    * Does the search of the note.  Called from RunL.  In one RunL call
    *  a limited number of note items are searched.
    *
    * @return	ETrue if more note items need to be searched. EFalse otherwise.
	*/
	TBool   DoActualSearchL();
		
    /**
    * from MSearchTextSearcherObserver
    * @param aKeywordCharPos keyword position     
    * 
    * @return void 	
	*/
    void HitL( TInt aKeywordCharPos ); 
        
    HBufC8* LaunchInfoL( const CSearchDocumentId& aDocumentID );
    /**
    * Gets the content and date by key value
   	*
   	* @param    aKey
   	* @param    aContentDes
   	* @param    aTime
   	* @return void 	
	*/
    void  GetContentAndDateByKeyL(const TInt& aKey,HBufC*& aContentDes,TTime& aTime);
        
private:

    /**
    * Constructor
    *
    *
    * @param aPluginId
    */
    CNotesSearcher( const TUid&  aPluginId); 
     
    /** 
    * 2nd phase constructor
    *
    * @param aContentIdArray
    * @param aCondition
    */
    void  ConstructL( const RArray<TUid>& aContentIdArray,
                      const CSearchCondition& aCondition,
                      MSearchPluginObserver& aObserver); 		
        
       
    
    RPointerArray<CSearchLightResult> iSearchLightResult;
    RPointerArray<CSearchResult> iHeavyResultsArray;        
        
protected:

        enum TNotepadColumnType
        {
        ENotepadKey = 1, // this must be 1 (DBMS/SQL restriction)
        ENotepadUpdateTime,
        ENotepadMemo,
        ENotepadColCountPlusOne // sentinel
        };
            
private: //Data members

     	/**
    * The notes plugin id
    */
	const TUid                      iNotesPluginId;
        
    
    /**
    * Represents the DB status
    */      
    TBool                           iSDbmsUp;
    
    /**
    * Represents a session with the DBMS server
    */  
    RDbs                            iDbSession;  
    
    /**
    * provides an interface for creating and opening a database identified by name and format.
    */  
    
    RDbNamedDatabase                iNamedDataBase; // 
    
    /**
    * Provides a view to a table. The view depends on the SQL query used for building the view.
    */  
    RDbView                         iDbView;        
    
    /**
    * Counter used when reporting the status of the search. 
    */
    TInt            iStatusItemCounter;
       
    /**
    * Total number of items to be searched, used when reporting the status 
    * of the search.Set to KErrUnknown when the total number of items 
    * is not known in advance..
    */
    TInt                             iTotalNumOfItems;
	    

	
	/**
    * The array containg the key word hit position
    * Own
    */  
	RArray<TInt> iKeyHitPos;
	    
	    
    /**
    * The plugin observer 
    */
    MSearchPluginObserver*          iObserver;
      
    /**
    * The  Text searcher 
    * Own
    */
    CSearchTextSearcher*             iTextSearcher; //own
	    

	/**
    * The notes content
    * Own
    */  
	HBufC16*                         iContent;

    /**
    * The date format,read from the resource file
    * Own
    */	
    HBufC16*  iDateFormat;
    };


#endif  //C_CNOTESSEARCHER_H


