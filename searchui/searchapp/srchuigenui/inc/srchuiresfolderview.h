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
* Description:     The grouped result view for all the searched contents. 
*
*/





#ifndef C_SEARCH_RESULT_FOLDER_VIEW_H
#define C_SEARCH_RESULT_FOLDER_VIEW_H

// System Includes
#include <aknview.h>

// User Includes
#include "srchuiresfoldercontainer.h"

// FORWARD DECLARATIONS
class CSrchUiResFolderContainer;
class CSearchAppUi;
class CSrchUiResFolder;
class CSrchUiGeneral;

// CLASS DECLARATION

/**
* View class for the multiple content type search results.
* @lib srchuigenui.lib
*/
NONSHARABLE_CLASS( CSrchUiResFolderView ) : public CAknView
    {
	public: // Constructors and destructor
		/** 
		* Symbian OS 2 phased constructor.
		*
	    * @param    aGeneralsrchUi     Search UI.
	    * @param    aFolders     Array of Folders.
		* @return	A pointer to the created instance of CSrchUiResFolderView
		*/
		static CSrchUiResFolderView* NewL(CSrchUiGeneral* aGeneralsrchUi, 
			RPointerArray<CSrchUiResFolder>& aFolders );

		/** 
		* Symbian OS 2 phased constructor.
		*
	    * @param    aGeneralsrchUi     Search UI.
	    * @param    aFolders     Array of Folders.
		* @return	A pointer to the created instance of CSrchUiResFolderView
		*/
		static CSrchUiResFolderView* NewLC(CSrchUiGeneral* aGeneralsrchUi, 
			RPointerArray<CSrchUiResFolder>& aFolders );
		
    	/**
    	* Destructor.
    	*/
		~CSrchUiResFolderView();

	public:	// New functions
    	/**
    	* Called from the application ui class when results were added.
    	* Informs the container about new results.
    	*/
		void ResultsAddedL();

    	/**
    	* Called from the application ui class when the previous results were deleted.
    	* Informs the container.
    	*/
		void ResultsClearedL();
	
	    /**
	    * Called from the application ui class after some search status change.
    	* Informs the container.
	    */
		void SearchStatusChangedL();

	    /**
	    * Called from the application ui class after one of the searches has finished.
	    * @param    aNumberOfFoundDocuments     Total number of teh found documents.
	    */
		void SearchFinishedL(TInt aNumberOfFoundDocuments);

        /**
        * Returns the pointer to opened folder
        *
		* @return a pointer to the opened folder
        */
        MSrchUiFolder* GetOpenFolder();

		/**
		* Setter for iContainerListBoxIndex.
        *
		* Used by the container so that the list box index can be restored when the view is
		* is reactivated without having new search results.
		*/
		void SetContainerListBoxIndex( TInt aIndex );
		
		/**
		* Getter for iContainerListBoxIndex.
        *
		* Used by the container so that the list box index can be restored when the view is
		* is reactivated without having new search results.
		*/
		TInt ContainerListBoxIndex();
		
		/* Handles the addition of an item to the model. 
		*/
		void HandleItemAdditionL();
		
		/* Updates the navigation Pane.
		*/		
		void UpdateNaviPaneL();
		
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
        * Handles the commands that are passed from softkey generated commands and
        * pop up's. 
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

    private:    // More constructors
    
    	/** 
    	* Performs the first phase of two phase construction. 
    	*/
  		CSrchUiResFolderView();

    	/**
    	* Performs the second phase construction. 
    	*/
        void ConstructL(CSrchUiGeneral* aGeneralsrchUi, 
        	RPointerArray<CSrchUiResFolder>& aFolders );
        
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

    private:    // Data
    
		/* Pointer to the container.
		* own.
		*/
		CSrchUiResFolderContainer*		iContainer;

        /* Flag indicating whether the container is added to the control stack.
		*/
        TBool							iContainerOnStack;
		
		/* Current item index from the listbox.
		*/
		TInt                        	iContainerListBoxIndex;
		
		/* Pointer to general search ui
		* not own.
		*/
		CSrchUiGeneral* iGeneralsrchUi;
		
		/* Array of pointers to folders
		* not own.
		*/
		RPointerArray<CSrchUiResFolder> iFolders;
		
		/* Counter for navigation pane text updation.
		*/ 
		TInt iCount_val;
		 
	    /**
        * The periodic timer used to control the animation
        * Owned by CGraphicsAppView object.
		* own.
        */
        CPeriodic* iPeriodicTimer;
        
        /* Number of results found.
        */
        TInt iNumberOfFoundDocuments;
        
        
    };

#endif	// C_SEARCH_RESULT_FOLDER_VIEW_H

// End of File


