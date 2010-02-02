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
* Description:     This class contains all the controls needed to display the heavy results.
*
*/






#ifndef C_SRCHUI_RESULT_CONTAINER_H
#define C_SRCHUI_RESULT_CONTAINER_H

//System Includes
#include <eiklbo.h>
#include <aknlists.h> 
#include <aknnavide.h>

//User Includes
#include "srchuiresfolder.h"

//Forward Declarations
class CSrchUiResultView;
class CAknDoubleGraphicStyleListBox;
class CAknTabGroup;
class MSrchUiFolder;
class CSrchUiResultModel;
class CSearchDocumentId;
class CSrchUiManager;
class CAknNavigationControlContainer;

//Class Declaration
/**
* Container class for CSearchResultFolderView.
*
*/
class CSrchUiResultContainer : public CCoeControl
    {
	public:// DATA TYPES
		/**
		* Enumerate for indicating if an item in the listbox represents
		* a folder or a file.
		*
		*/
		enum TFolderListBoxItem
			{
			EListBoxFolder,
			EListBoxFile
			};

    public:
    	/** 
    	* Performs the first phase of two phase construction.
    	*
        * @param	aView   Reference to CSearchResultFolderView.
    	*/
		CSrchUiResultContainer( CSrchUiResultView& aView );

    	/**
    	* Performs the second phase construction
    	*
    	*/
        void ConstructL(const TRect& aRect,TInt aSelectedFolderIdx,
                                     CSrchUiResFolder* aSrchUiResRootFolder);

    	/**
    	* Destructor
    	*
    	*/
        ~CSrchUiResultContainer();

    public:	// New functions
		/** 
		* Handles the selection of the currently selected item.
		* When a folder is selected the subfolder will be opened and displayed.
		* When a "file" is selected the action is depending
		* on the implementation of the FileSelectedL() method
		*
		*/
        const CSearchDocumentId& HandleItemSelectionL();

		/** 
		* Folders and Files are stored into 1 array
		* for use with the listbox.  First the 
		* folders are listed and then files.
		* The function returns a folder or file index.
		* To know if the selected item was a folder or file
		* the GetListBoxType method can be used.
		*
		* Example: 3 Folders and 2 Files are being displayed.
		*
		* When the 2nd item is selected this function returns 1.
		* 1 meaning the 2nd folder item.
		*
		* When the 4th item is selected this function returns 0.
		* 0 meaning the 1st file item.
		*
		* @param	aIndex		Index in the listbox (folders/files)
		* @return	Folder index or File index
		*/
		TInt GetItemIndex( TInt aIndex );
		
		/** 
		* Getter for iOpenFolder.
		*
		* @return	A pointer to the currently opened folder.
		*/
		MSrchUiFolder* GetOpenFolder() ;
		
    	/**
    	* Handles changes to the title pane text.
    	*
    	*/
    	void UpdateTitlePaneL();

		/** 
		* Handles the changes needed to the Navigation Pane.
		*
		*/
		void UpdateNaviPaneL();
		
		/** 
		* Handles the changes needed to the Navigation Pane.
		*
		*/
		void UpdateNaviPaneL(const TDesC& aBuf );
		/** 
		* Will go 1 step higher in the directory structure.
		*
		*/
		TBool GoUpOneLevelL();
		
		/** 
		* @return	ETrue when the directory structure
		* is empty (no folders and no files), 
		*           EFalse otherwise.
		*/
		TBool IsEmpty();
		
		/** 
		* Sets the root folder for this directory structure.
		*
		* @param	aFolder		A pointer to the root folder.
		*/
		void SetRoot( MSrchUiFolder* aFolder );
		
        /**
        * Informs the control(listbox) that search results were added.
        *
        */
    	void ResultsAddedL();
    	
    	/**
    	* Informs the control(listbox) that the previous results were deleted
    	*
    	*/
    	void ResultsClearedL();
    	
        /**
        * Called from the application ui class after some search status change.
        *
        */
        void SearchStatusChangedL();
		
        /**
        * Informs the control(listbox) that one of the searches has finished.
        *
        */
        void SearchFinishedL();
	
		/**
		* Calls the HandleItemAdditionL method on the correct
		* listbox (single or double line).
		*
		*/
        void HandleItemAdditionL();
        
        /**
        * Asks the item index from the correct listbox (single or double line).
        *
        * @return   The current item index in the listbox
        */
        TInt CurrentItemIndex();

        /**
        * Calls the SetCurrentItemIndex method on the correct
        * listbox (single or double line).
        *
        * @param    The index for the new current item.
        */
        void SetCurrentItemIndex( TInt aIndex );
        
        /**
        * Calls the SetCurrentItemIndexAndDraw method on the correct listbox 
        *  (single or double line).
        *
        * @param    The index for the new current item
        */
        void SetCurrentItemIndexAndDrawL( TInt aIndex );
        
        /**
        * Get the icons of the opened folder
        *
        * @param folders : The opened folder
        * @return the array of icons used in the list box
        */
        CArrayPtr<CGulIcon>* ListBoxIconsArrayL(CSrchUiResFolder* folders);

        /**
        * checks whether there are any elements in the current folder selected
        *
        * @return ETrue if there are any elements in the selected
        * folder else return EFalse
        */
        TBool IsElementValid();
        
        /** 
        * Update the Navi Pane
		*/
		void UpdateNaviResultPaneL();
		
       /** 
			* Update the Navi Pane
		* @param aSrchUiResRootFolder : The opened folder
		* @param aSrchUiResRootFolder : Indicates whether search is completed or not
		*/ 
    void UpdateViewL(CSrchUiResFolder* aSrchUiResRootFolder, TBool aSearchFinished, TBool aResultRetrieve );
        
        
    public:    // Functions from base classes
    	/**
    	* From CCoeControl.
    	* Called by the framework when a key event needs to be processed.
    	*
    	*/
        TKeyResponse OfferKeyEventL( const TKeyEvent& aKeyEvent, TEventCode aType );
        
        
         /**
		* From CCoeControl.
		* Called by the framework when a pointer event needs to be processed.
		*
		* @param aPointerEvent Pointer event
		* 
		*/
		void HandlePointerEventL(const TPointerEvent &aPointerEvent);
		

    	/**
    	* From CCoeControl.
    	* Called by the framework when the view size is changed.
    	*
    	*/
        void SizeChanged();

    	/**
    	* From CCoeControl.
    	* Gets the number of controls contained in a compound control.
    	*
    	*/
        TInt CountComponentControls() const;

    	/**
    	* From CCoeControl.
    	* Gets the specified component of a compound control.
    	*
    	*/
        CCoeControl* ComponentControl( TInt aIndex ) const;

        /**
        * From CCoeControl
        *.
        * Handles a change to the application's resources which
        * are shared across the environment.
        *
        * @param    aType   The type of resources that have changed. 
        */
        void HandleResourceChange( TInt aType );
        
        /**
    	*
	    * Handles a change to the application's resources
	    * which are shared across the environment.
	    *
	    * @param    aType   The type of resources that have changed. 
	    */
        void HandleResourceChangeL( TInt aType );

	private:    // New functions

		/** 
		* Constructs and prepares the list box control.
		*
		*/
		void SetupListBoxL();
		
		
		/** 
		* Displays the error note when there are no
		* results in the opened folder
		*
		* @param searchFinished True if search finished, else false
		* @param aResultsRetrieved True if search results have been 
		* retrieved, else false
		*/
		void SetErrorNoteLL( TBool searchFinished, TBool 
							aResultsRetrieved );
		
    private:    // Data
    
        /**
        * Pointer to Eikon's Environment 
        *
        * Not Own
        */
        CEikonEnv*                           iSrchEikonEnv;
        
        /**
        * Reference to the view.
        */
		CSrchUiResultView&			         iView;

    	/**
    	* (Double line) list box control.
    	*
    	* Own
    	*/
		CAknDoubleGraphicStyleListBox*	     iDoubleLineListBox;

    	/**
    	* Model data for the listbox.
    	*
    	* Own
    	*/
	    CSrchUiResultModel*		             iModel;
	    
        /**
        * Pointer to the opened folder
        *
        * Not Own
        */
        CSrchUiResFolder*                    iSrchUiResRootFolder;

        /**
        * Pointer to the navigation control
        * container needed for TAB groups
        *
        * Not Own
        */
	    CAknNavigationControlContainer*      iNaviPane;
	    
        /**
        * Pointer to the TAB group
        *
        * Not Own
        */
        CAknTabGroup*                        iTabGroup;
        
        /**
        * Pointer to the Navigation decorator
        *
        * Own
        */
        CAknNavigationDecorator*             iNaviDecorator1;
        
        /**
        * Pointer to the Navigation decorator
        *
        * Own
        */
        CAknNavigationDecorator*             iNaviDecorator2;
        
        /**
        * Pointer to the Navigation decorator
        *
        * Own
        */
        CAknNavigationDecorator*             iNaviDecorator3;

        /**
        * Pointer to the currently open folder
        *
        * Not Own
        */
		CSrchUiResFolder*                    iOpenedFolder;

    	/**
    	* Name of the title in the title pane
    	*
    	* Own
    	*/
    	HBufC*                               iSearchResultsTitle;
    	/**
    	* Array of Icons
    	*
    	* Own
    	*/
    	CArrayPtr<CGulIcon>* iIcons;

    	/**
    	* Index to the selected list box item
    	*
    	*/
    	TInt                                 iSelectedFolderIdx;
    	
    	/**
    	* True if search is finished. This value is updated in the updateview
    	*
    	*/
    	TBool iSearchFinished;
    	//For Touch
    	TBool iTouchFlag;

    };
#endif	// C_SRCHUI_RESULT_CONTAINER_H
// End of File