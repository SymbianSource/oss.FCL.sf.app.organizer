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
* Description:     This class is responsible for the display of the results in snippets for a particular content class selected by the user. 
*
*/






#ifndef C_SRCHUI_RESULT_VIEW_H
#define C_SRCHUI_RESULT_VIEW_H

//System Includes
#include <eikenv.h>
#include <e32std.h> 
#include <aknview.h>
#include <DocumentHandler.h>
#include <AknServerApp.h>
#include <w32std.h>

// User Includes
#include "srchuiresultcontainer.h"
#include "srchuiresultviewerplugin.h"
#include "srchuiresulthandler.h"
#include "searchcommon.hrh"
// FORWARD DECLARATIONS
class CSrchUiResFolder;
class CSrchUiResultContainer;
class MCCAConnection;
class MCCAParameter;
// CLASS DECLARATION

/**
* View class for the single content type search results.
*
*/
class CSrchUiResultView : public   CSrchUiResultViewerPlugin, public MAknServerAppExitObserver
    {
	public: // Constructors and destructor
		/** 
		* Symbian OS 2 phased constructor.
		*
		* @return	A pointer to the created instance of CSrchUiResultView.
		*/
		static CSrchUiResultView* NewL();

		/** 
		* Symbian OS 2 phased constructor.
		*
		* @return	A pointer to the created instance of CSrchUiResultView.
		*/
		static CSrchUiResultView* NewLC();

    	/**
    	* Destructor.
		*
    	*/
		~CSrchUiResultView();
		/**
		 * Increments the selected folder Index
		 * Should be called from conatiner when tab is used 
		 * to change the folder
		 */
		 void IncrementFolderIndex();
		 
		 /**
		 * Increments the selected folder Index
		 * Should be called from conatiner when tab is used 
		 * to change the folder
		 */
		 void DecrementFolderIndex();

	public:	// New functions
    	/**
    	* Called from the application ui class when results were added.
    	* Informs the container about new results.
    	*
    	*/
		void ResultsAddedL();
		
    	/**
    	* Called from the application ui class when the previous
    	* results were deleted.
    	* Informs the container.
    	*/
		void ResultsClearedL();
	
	    /**
	    * Called from the application ui class after some search status change.
    	* Informs the container.
	    *
	    */
		void SearchStatusChangedL();
		
	    /**
	    * Called from the application ui class after the search has finished.
		*
	    */
		void SearchFinishedL();
	
		/**
		* Setter for iContainerListBoxIndex.
        *
		* Used by the container so that the list box index can be
		* restored when the view is
		*  is reactivated without having new search results.
		*/
		void SetContainerListBoxIndex( TInt aIndex );
		
		/**
		* Getter for iContainerListBoxIndex.
        *
		* Used by the container so that the list box index can be
		* restored when the view is reactivated without having new search results.
		*/
		TInt ContainerListBoxIndex();
		
		/**
		* Updates the command buttons for tab done in the container
		*
		* @param aResultCount - Result count for the opened folder
		*/
		void UpdateCommandButtonsL( TInt aResultCount ) ;

		/**
	    * Launch the file in searchui
	    */
		void LaunchApplicationL( const TDesC8& aLaunchInfo );
		
		/*
	    * Launch contact with new contact API in searchui
	    */
		void LaunchContactApplicationL( const TDesC8& aLaunchInfo );
    public:
    
        /**
        * Gets the supported format.
        *
        * @return the supported fromat.
        */   
        const TDesC& SupportedFormat();
        
        /**
        * This function is called when one of the results is selected. 
        *
        * @param aDocumentId The document id of the result.
        */
        void HandleSelectionL(
                        const CSearchDocumentId& aDocumentId  );
         
        /**
        * This function is called to get the opened folder 
        *
        * @return the opened folder
        */ 
        MSrchUiFolder* OpenFolder();
		
		/**
	    * Gets the query string
	    *
	    * @return TDesC8& The query string
	    */
	    TDesC8& GetQueryString();
		
	public: // from CAknView
	    /**
	    * From CAknView.
	    *
	    * Returns the unique id of the view.
	    */
        TUid Id() const;

        /**
        * From CAknView.
        *
        * Handles the commands that are passed from softkey
        * generated commands and pop up's.
        */
        void HandleCommandL( TInt aCommand );

        /**
        * From CAknView.
        *
        * Called by the framework when the view is activated.
        */
        void DoActivateL( const TVwsViewId& aPrevViewId, TUid aCustomMessageId,
                          const TDesC8& aCustomMessage );
        
        /**
        * From CAknView.
        *
        * Called by the framework when the view is de-activated.
        */
        void DoDeactivate();

		/**
		* From CAknView.
		*
		* Called by the framework for dynamically initializing the menu pane.
		*/
		void DynInitMenuPaneL( TInt aResourceId, CEikMenuPane* aMenuPane );
		
		/**
        * This function is called to launch the single result view
        *
        * @param iSrchUiResRootFolder : The root folder
        * @param aSelectedFolderIdx :   The index of the selected folder
        */
		void LaunchViewL(CSrchUiResFolder* iSrchUiResRootFolder,
		                    TInt aSelectedFolderIdx, const TDesC8& aQueryString,TBool aSearchFinished);
		
			/**
        * This function is called to update the single result view
        *
        * @param iSrchUiResRootFolder : The root folder
        * @param aSearchFinished :   Tells if search is complete or not
        */
		void UpdateViewL(CSrchUiResFolder* iSrchUiResRootFolder, TBool aSearchFinished, TBool aResultRetrieve );
		/**
        * This function is called to initialize the observer
        *
		* @param aResHandler : The observer
        */
		void CreateManagerObserver(MSrchUiResultHandler* aResHandler);
		
    private:    // More constructors
    	/** 
    	* Performs the first phase of two phase construction.
		*
    	*/
  		CSrchUiResultView();

    	/**
    	* Performs the second phase construction.
		*
    	*/
        void ConstructL();
        /**
        * PeriodL
        * Call back function for a periodic timer
        * @param aPtr a parameter passed to the timer when the timer is started.
        * @return Value TRUE indicates the callback should be done again. 
        */
        static TInt PeriodL( TAny* aPtr );
        
		/** 
    	* Updates the navigation with the text about the staus. 
    	*/
        void SetNaviTextL();
                
		/** 
    	* Starts the timer. 
    	*/
        void StartTimer();
        
        /* Updates the navigation Pane.
		*/		
		void UpdateNaviPaneL();
    
    private:    // Data
        /**
        * Pointer to Eikon's Environment 
        *
        * Not Own
        */
        CEikonEnv*              iSrchEikonEnv;
        
        /**
        * Array of resource ids
        *
        */
        RArray<TInt>            iResFileOffset;
    
        /**
        * Pointer to the container.
        *
        * Own
        */
        CSrchUiResultContainer* iContainer;
        
        /**
        * Pointer to the root folder
        *
        * Not Own
        */
        CSrchUiResFolder*       iSrchUiResRootFolder;
        
        /**
        * Flag indicating whether the container is added to the control stack.
        */
        TBool                   iContainerOnStack;
        
		/**
		* Pointer to the opened folder
		*
		* Not Own
		*/
		CSrchUiResFolder*       iOpenedFolder;
		
		/**
		* Current item index from the listbox.
		* When the previous result view is activated, 
		* it's good to restore the previous listbox
		* item index.
		*/
		TInt                    iContainerListBoxIndex;

		/**
		* Index of the opened folder
		*/
		TInt                    iSelectedFolderIdx;
		
		/**
		* Pointer to the observer of srch ui manager
		*
		* Not Own
		*/
		MSrchUiResultHandler*   iResHandler;
		
		/**
		* Query string
		*/
		HBufC8*                 iQueryString;
		/**
        * The periodic timer used to control the animation
        * Owned by CGraphicsAppView object.
		* own.
        */
        CPeriodic* iPeriodicTimer;
      
    /* Search finished or not
		*/   
		TBool iSearchFinished;
		
		/* Counter for navigation pane text updation.
		*/ 
		TInt iCount_val;
		
        /*
         * a file handler to open a specified file
         */
		CDocumentHandler *iHandler ;
		
		/*
		 * open a contact with new contact API
		 */		
		MCCAConnection* iConnection;
    };

#endif // C_SRCHUI_RESULT_VIEW_H

// End of File
