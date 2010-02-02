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
* Description:     A container class for CSrchUiResFolderView. 
*
*/





#ifndef C_SEARCH_RESULT_FOLDER_CONTAINER_H
#define C_SEARCH_RESULT_FOLDER_CONTAINER_H

#include <eiklbo.h>
#include <aknnavide.h>

#include "srchuiresfolder.h"

class CSrchUiResFolderView;
class CAknDoubleGraphicStyleListBox;
class CAknSingleGraphicStyleListBox;
class CAknTabGroup;
class MSrchUiFolder;
class MFile;
class CSrchUiResFolder;
class CSrchUiResFolderModel;
class CAknNavigationControlContainer;
class CSrchUiGeneral;


/**
* Container class for CSrchUiResFolderView.
*
*/
NONSHARABLE_CLASS( CSrchUiResFolderContainer ) : public CCoeControl , 
												 public MEikListBoxObserver
    {

public:// DATA TYPES
		/**
		 * Enumerate for indicating if an item in the listbox represents a 
		 * folder or a file.
		 */
		 enum TFolderListBoxItem
				{
				EListBoxFolder,
				EListBoxFile
				};

    /** 
     * Performs the first phase of two phase construction.
     *
     * @param	aView   Reference to CSrchUiResFolderView.
     */
		 CSrchUiResFolderContainer( CSrchUiResFolderView& aView,
		 	 CSrchUiGeneral* aGeneralsrchUi );

    /**
     * Performs the second phase construction.
     *
     */
     void ConstructL( const TRect& aRect );

    /**
     * Destructor
     */
     virtual ~CSrchUiResFolderContainer();

		/** 
		 * Handles the selection of the currently selected item.
		 * When a folder is selected the subfolder will be opened and displayed.
		 * When a "file" is selected the action is depending on the implementation of
		 * the FileSelectedL() method.
		 *
		 */
     TInt HandleItemSelectionL();

		/** 
		 * Returns EListBoxFolder when the selected item is a folder item.
		 * Returns EListBoxFile when the selected item is a file item.
		 *
		 * @param	aIndex		Index in the listbox
		 *
		 * @return	EFolderListBoxItem
		 */
		 TInt GetListBoxType( TInt aIndex );
		
		/** 
		 * Folders and Files are stored into 1 array for use with the listbox.  First the 
		 * folders are listed and then files.
		 * The function returns a folder or file index.
		 * To know if the selected item was a folder or file the GetListBoxType method can be used.
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
		 *
		 * @return	Folder index or File index
		 */
		 TInt GetItemIndex( TInt aIndex );
		
		/** 
		 * Getter for iOpenFolder.
		 *
		 * @return	A pointer to the currently opened folder.
		 */
		 MSrchUiFolder* GetOpenFolder() const;
		
    /**
     * Handles changes to the title pane text.
     *
     */
     void UpdateTitlePaneL();

		/** 
		 * Handles the changes needed to the Navigation Pane.
		 *
		 */
		 void UpdateNaviPaneL( const TDesC& aBuf );
		
		/** 
		 * Will go 1 step higher in the directory structure.
		 *
		 */
		 TBool GoUpOneLevelL();
		
		/** 
		 * Checks if directory structure is empty
		 *
		 * @return	ETrue when the directory structure is empty (no folders and no files), 
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
     * Informs the control(listbox) that the previous results were deleted.
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
		 * Calls the HandleItemAdditionL method on the correct listbox 
		 * (single or double line).
		 *
		 */
     void HandleItemAdditionL();
        
    /**
     * Asks the item index from the correct listbox (single or double line).
     *
     * @return   The current item index in the listbox.
     */
     TInt CurrentItemIndex();

    /**
     * Calls the SetCurrentItemIndex method on the correct listbox 
     * (single or double line).
     *
     * @param    The index for the new current item.
     */
     void SetCurrentItemIndex( TInt aIndex );
        
    /**
     * Calls the SetCurrentItemIndexAndDraw method on the correct listbox 
     * (single or double line).
     *
     * @param    The index for the new current item.
     */
     void SetCurrentItemIndexAndDrawL( TInt aIndex );
        
    /**
     * Sets the error note
     *
     */
     void SetErrorNoteL();
        
    /**
     * Checks whether the selected folder has any elements
     *
     */
     TBool IsElementValid();

    // Functions from base class CCoeControl
    
    /**
     * From CCoeControl.
     *
     * Called by the framework when a key event needs to be processed.
     */
     TKeyResponse OfferKeyEventL( const TKeyEvent& aKeyEvent, TEventCode aType );
     
     /**
     * From MEikListBoxObserver
     * Called by the framework when a list box event needs to be processed.
     *
     * @param aListBox List box
     * @param aEventType Event type
     */
     void HandleListBoxEventL( CEikListBox* aListBox, 
     						TListBoxEvent aEventType );
     
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
     *
     * Called by the framework when the view size is changed.
     */
     void SizeChanged();

    /**
     * From CCoeControl.
     *
     * Gets the number of controls contained in a compound control.
     */
     TInt CountComponentControls() const;

    /**
     * From CCoeControl.
     *
     * Gets the specified component of a compound control.
     */
     CCoeControl* ComponentControl( TInt aIndex ) const;

    /**
     * From CCoeControl
     *
     * Handles a change to the application's resources which are shared 
     * across the environment.
     *
     * @param    aType   The type of resources that have changed. 
     */
     void HandleResourceChange( TInt aType );
     
	   /**
	    * From CCoeControl
	    *
	    * Handles a change to the application's resources which are shared 
	    * across the environment.
	    *
	    * @param    aType   The type of resources that have changed. 
	    */
	    void HandleResourceChangeL( TInt aType );

private:    // New functions
		/** 
		 * Handles the selection of the currently selected Folder.
		 * Called by HandleItemSelectionL().
		 * The result is that the folder will be opened and the view will be redrawed.
		 *
		 * @param	aFolder		A pointer to the currently selected folder.
		 */
		 void FolderSelectedL( MSrchUiFolder* aFolder );
		
		/** 
		 * Handles the selection of the currently selected File.
		 * Called by HandleItemSelectionL().
		 * Not implemented in this class.
		 *
		 * @param	aFile		A pointer to the currently selected file.
		 */
		 void FileSelected( MFile* aFile );
		
		/** 
		 * Constructs and prepares the list box control.
		 *
		 */
		 void SetupListBoxL();
		
		/**
     * Returns an array of icons for use in the listbox.
     *
     * @param folders 
     */
		 CArrayPtr<CGulIcon>* ListBoxIconsArrayL( RPointerArray<CSrchUiResFolder>& 
		 			folders );
		
private:    // Data
    // Reference to the view.
		CSrchUiResFolderView&			 iView;

    // (Single line) list box control.
		CAknSingleGraphicStyleListBox*	 iSingleLineListBox;
    	
    // Model data for the listbox.
	  CSrchUiResFolderModel*		     iModel;
	    
    // Pointer to the general search ui
    CSrchUiGeneral*                  iGeneralsrchUi;

    // Pointer to the navigation control container needed for TAB groups
	  CAknNavigationControlContainer*  iNaviPane; //Not owned
	    
    // Pointer to tab group
    CAknTabGroup*                    iTabGroup; //Not owned
        
    // Pointer to navigation decorator
    CAknNavigationDecorator*         iNaviDecorator;

    // Pointer to the currently open folder
		CSrchUiResFolder*                iOpenFolder;

    // Name of the title in the title pane
   	HBufC*                           iSrchUiResTitle;
    
    HBufC*                           iNaviPaneSearchStatus;
    //Touch
    TBool                               iTouchFlag; 

    };

#endif	// C_SEARCH_RESULT_FOLDER_CONTAINER_H

// End of File
