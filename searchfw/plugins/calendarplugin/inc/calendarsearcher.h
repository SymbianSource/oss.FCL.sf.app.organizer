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



#ifndef C_CALENDARSEARCHER_H
#define C_CALENDARSEARCHER_H

//System Includes 
#include <e32std.h>
#include <calsession.h>
#include <calentryview.h>
#include <calinstanceview.h>
#include <calprogresscallback.h> 

//Search FW includes
#include <searchcontentsearcher.h>
#include <searchcondition.h>

// FORWARD DECLARATIONS
class CSearchTextSearcher;
class CSearchDocumentId;
class MSearchPluginObserver;
class CCalDesC;
   
/**
* Active object handling the Calendar database search.
* Per each RunL call, a limited number of calendar days are searched.
*
* @lib calendarsearchplugin.lib
 */
class CCalendarSearcher : public CActive, 
                          public MSearchContentSearcher,
                          public MCalProgressCallBack
    
    {    
           
    public:
                               
         /**
         * Defines the state of handler
         */
         enum TCalendarState
         {
            EIdle = 0,
            ECreatingCalendarEntryView,
            ECreatingCalendarInstanceView,
            ESearching
         };
         
        static CCalendarSearcher* CCalendarSearcher::NewL( 
                                const RArray<TUid>& aContentIdArray, 
                                const CSearchCondition& aCondition, 
                                const TUid& aPluginId,
                                MSearchPluginObserver& aObserver );                                
                                
        /**
        * Standard Destructor.
        *
        */
        virtual ~CCalendarSearcher();        
        
        /**
        * Checck if the Entry exists in calander
        *
        * @param   aId  the Local Uid to be searched
        */
          
        TBool GetIfEntryExistsL (TCalLocalUid aId);
                              
        /**
        * Starts the search process.
        *
        * @param    aCriteria   Search criteria for the search.
        */
        void StartSearchL();
        
    public: //from base class MSearchContentSearcher

        /**
        * From MSearchContentSearcher
        * Search FW calls this function when it no longer needs this searcher.
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
        
         /*
         * Gets the current search progress. Typically this is called by the framework 
         * in case the plugin fails to provide results through MSearchPluginObserver in the 
         * time defined by the framework. 
         *
         * @param aContentClassId On return contains the content class id, which is currently searched.
         * @param aCurrentDocument On return contains the index document that is currently being searched.
         * @param aTotalDocuments On return contains the total count of documents to be searched.
         */         
         void GetSearchProgressL( TUid& aContentClassId, TInt& aCurrentDocument, TInt& aTotalDocuments );

        /**
        * From MSearchContentSearcher , through MSearchTextSearcherObserver
        * Is called by MSearchTextSearcherObserver when Keyword is found automatically.
        * @param aCharPos The character position of the keyword match within the original text.
        *
        */
        void HitL( TInt aCharPos );
        
		/**
        * Gets the information required for launching.
        * @param aDocumentID Document ID to launch.
        *
        */
        HBufC8* LaunchInfoL( const CSearchDocumentId& aDocumentID );
        
        /**
        * Initializes the session with Calendar Server.
        */
        void InitializationL();
              
        /**
        * Is called when classes related to Calendar connection are finished
        * initializing. This is called after CCalInstanceView::NewL and
        * CCalEntryView::NewL. When initialization is complete starts the search 
        * automatically.
        * @param aError
        */
        void Completed(TInt aError);
        
        /**
        * Is called when contentsearcher finishes it's operation or it is unable to
        * search further.
        *
        * @param aTotalNumOfItems
        * @param aSearchResult
        */
        void ReportFinishedL( TInt aTotalNumOfItems, TInt aSearchResult = 0 );        

    protected:  // New functions
                            
        /** 
        * Performs the second phase construction.
        *
        * @param    aContentTypeUid
        */
        void ConstructL( const RArray<TUid>& aContentIdArray, 
                         const CSearchCondition& aCondition,
                         MSearchPluginObserver& aObserver );
    
        /** 
        * Performs the first phase of two phase construction.
        *
        *
        * @param    aPluginId   Plug-in's ID.
        */
        CCalendarSearcher::CCalendarSearcher( const TUid& aPluginId );
             
     private:   // new functions
        /** 
        * Handles the cleanup operation of this class.
        */
        void Cleanup();
        
        /**
        * Does the text search of the agenda instance.  Called from RunL. In one RunL call
        *  a limited number of agenda days/instances are searched.
        *
        * @return   ETrue       When more agenda days with instances are available.
        *                        EFalse otherwise.
        */
        TBool DoActualSearchL();
        
        /** 
        * Fetches all the Calendar Entries.
        */
        TBool FetchAllCalendarIdsL();
        
    private: // from CActive
     /**
        * From CActive.
        *
        * Handles an active object’s request completion event.
        */
        void RunL();
        
        /**
        * From CActive.
        *
        * Implements cancellation of an outstanding request.
        */
        void DoCancel();
        
        /**
        * From CActive.
        *
        * Handles a leave occurring in the request completion event handler RunL().
        */
        TInt RunError(TInt aError); 
  
                                            
    private: // from MCalProgressCallBack
    
        /* Not in use */
        /**
        * From MCalProgressCallBack.
        *
        * progress of the search.
        */
        void Progress(TInt /*aPercentageCompleted*/){};
        
        /* Not in use */
        /**
        * From MCalProgressCallBack.
        *
        * progress notification.
        */
        /* Not in use */
        TBool NotifyProgress(){ return 0;};

    private:  //Data
        
        /**
         * Search Plugin Observer
         * Own.  
         */
        MSearchPluginObserver*      iObserver;
        
        /**
         * Text searcher
         * Own.  
         */
        CSearchTextSearcher*        iTextSearcher;
     
        /**
         * Current document which is currently Searched. 
         */    
        TInt                        iCurrentDocumentSearching;
        
        /**
         * Total number of documents Searched. 
         */
        TInt                        iTotalNumOfDocumentsSearched;
        
        /**
         * A handle to the calendar file.iSession.
         * Own.
         */
        CCalSession*                iSession;
        
        /**
         * View to the session for accessing calendar entries.
         * Own.
         */   
        CCalEntryView*              iEntryView; 
        
        /**
         * An instance of a calendar entry.
         * Own.
         */ 
        CCalInstanceView*           iInstanceView;
        
		/**
         * Array of UIDs that contain all items in the calendar DB.
		 * own
		 */
	    RArray<TCalLocalUid>			iCalLocalUidArray;
	            
        /**
         * Entry filter used in the entry view and instance view.
         * defines the rules according to which the instances are filtered.
         */
        CalCommon::TCalViewFilter   iFilter;
        
        /**
         * An instance of a calendar entry.
         * Not Own.
         */ 
        CCalEntry*                  iEntry;
        
        /**
         * State of the calendar initialization.
         */ 
        TCalendarState              iCalendarState;
        
        /**
         * PlugIn Uid.
         */ 
        const TUid                  iPluginId;
        
        /**
         * Position of the first Keyword hit for preparing snippets.
         */ 
        TInt                        isFirstKeyWordHit;
        
        /**
         * Flag of parameter setting status.
         */ 
        TBool                       iSetParamsDone;
        
        /**
         * Error Code of the last operation
         */ 
        TInt                        iErrorCode;
        
        /**
         * Flag to report that searching operation has finished.
         */ 
        TBool                       iNeedToCallReportFinishedL;
        
        /**
         * Array of CCalDesC, a mapper class.
         * Own.
         */ 
        RPointerArray<CCalDesC>     iCalDesCArray;
        
        /**
         * Checks wheather Search is cancelled or not.
         */ 
        TBool                       iSearchNotCancelled;
          
        /**
         * Array of heavy Results.
         * Own.
         */ 
         RPointerArray<CSearchResult> iHeavyResultsArray; 
         
        /**
         * File session.
         */ 
        RFs iFsSession;
        
        /**
         * Buffer to read snippet format of Date from resource file.
         * Own.
         */ 
        HBufC8* iReadDateBuffer;
        
        /**
         * Buffer to read snippet format of time from resource file.
         * Own.
         */ 
        HBufC8* iReadTimeBuffer;
    };

#endif      // C_CALENDARSEARCHER_H

// End of File

