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
* Description:    This class acts as the container for CSrchUiSearchView. It also handles the user inputs.  
*
*/





#ifndef C_SRCHUI_MAIN_CONTAINER_H
#define C_SRCHUI_MAIN_CONTAINER_H

#include <coecntrl.h>
#include <aknlists.h>
#include <aknsfld.h> 
#include <e32std.h>

class CSrchUiSearchView;
class CSearchAppUi;
class CSrchUiModel;
class CSearchDocument;
class MAknsControlContext; // For skins support.
class CSrchUiGeneral;
class CAknNavigationDecorator;


/**
* Container class for CSrchUiSearchView.
*/
NONSHARABLE_CLASS( CSrchUiServiceContainer ) : public CCoeControl, 
		public MEikListBoxObserver,
		public MEikEdwinObserver
    {

public: // Constructors and destructor
  
    /** 
     * Performs the first phase of two phase construction.
     *
     * @param	aView   Reference to CSrchUiSearchView.
     * @param aGenSrchUi pointer to genaral search
     */
     CSrchUiServiceContainer( CSrchUiSearchView& aView, CSrchUiGeneral* aGenSrchUi );

    /**
     * Performs the second phase construction.
     *
     * @param aRect 
     */
     void ConstructL( const TRect& aRect );
        
    /**
     * Destructor
     *
     */
     ~CSrchUiServiceContainer();

    /**
     * Starts off the search.
     *
     */
     void DoSearchL();
    	
    /**
     * Clears the text of the search field
     *
     */
	   void ClearTextL();
		

		// Functions from base classes

    /**
     * From CCoeControl.
     * Called by the framework when a key event needs to be processed.
     *
     * @param aKeyEvent Key event
     * @param aType Key code
     */
     TKeyResponse OfferKeyEventL( const TKeyEvent& aKeyEvent, TEventCode aType );
     
     void UpDateCbaL();
     

    /**
     * From CCoeControl.
     * Called by the framework when the view size is changed
     *
     */
     void SizeChanged();

    /**
     * From CCoeControl.
     * Gets the number of controls contained in a compound control.
     */
     TInt CountComponentControls() const;

    /**
     * From CCoeControl.
     * Gets the specified component of a compound control.
     *
     * @param aIndex Index of the component
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
     * From MEikListBoxObserver
     * Called by the framework when a list box event needs to be processed.
     *
     * @param aListBox List box
     * @param aEventType Event type
     */
     void HandleListBoxEventL( CEikListBox* aListBox, 
     						TListBoxEvent aEventType );
    
    /**
     * Marks/UnMarks the check boxes for the items in the list box
     *
     */
     void HandleItemMarkUnMarkL();
    
    /**
     * Getter for index of the highlighted item in list box
     *
     */
     TInt CurrentItemIndex();
     
    /**
     * Handles changes to the navigation pane text.
     *
     */
     void UpdateNaviPaneL();
     
    /**
    *  get called when the focus is changed
    *  @param aDrawNow value passed to it by SetFocus().
    */
     #ifdef RD_INTELLIGENT_TEXT_INPUT   
     void FocusChanged(TDrawNow aDrawNow);
     #endif
     
	/**
	*  get event when iSearchFieldBox content changed
    *  @param aEdwin  The originating edwin. 
    *  @param aEventType  The event being reported. 
	*/
     void HandleEdwinEventL( CEikEdwin* aEdwin, TEdwinEvent aEventType );
private:	

    /**
     * Constructs and prepares the list box control.
     */
     void SetupListBoxL();
    	
    /**
     * Constructs and prepares the find box control.
     *
     */
     void SetupFindBoxL();
    
    /**
     * Updates the title pane
     *
     */
     void UpdateTitlePaneL();
    		
    /** 
     * Handles the selection of the currently active item.
     *
     */
     void HandleItemSelectionL();
     
     /** 
	 * Handles the selection of the currently active item.
	 *when aallSelected is true,all is marked.
	 */
     void HandleItemSelectionL(TBool aallSelected);

    /**
     * Returns an array of icons for use in the listbox.
     */		
     CArrayPtr<CGulIcon>* CSrchUiServiceContainer::ListBoxIconsArrayL();
    
    /**
     * Updates the "All in device" selection.
     * When one of the selections is taken off, the "All in device" selection 
     * must also be taken away.
     * When all selections are selected one by one, the "All in device" 
     *  selection must also be selected.
     *
     * @param aIncludedContentTypes 
     */
     void CSrchUiServiceContainer::UpdateAllInDeviceSelection( 
     				RArray<TBool>& aIncludedContentTypes );

    /**
     *
     * Handles a change to the application's resources which are shared 
     * across the environment.
     *
     * @param    aType   The type of resources that have changed. 
     */
     void HandleResourceChangeL( TInt aType );

  private:    // Data

    /**
     * Reference to the view.
     */
     CSrchUiSearchView&				    iView;
    	
    /**
     *  List box control.
     * 
     * Own
     */
     CAknSingleGraphicStyleListBox*	iListBox;
    
    /**
     * the search field
     *
     * Own
     */
     CAknSearchField* iSearchFieldBox; 
 
    /**
     *  Buffer for getting the text out of the CAknSearchField
     *
     * Own
     */
     HBufC* iSearchText;
        
    /**
     *  Modifiable pointer descriptor to iSearchText
     */
     TPtr iSearchTextPtr;

    
    /**
     *  Model data for the listbox.
     *
     * Own
     */
     CSrchUiModel*	iModel;
    
    /**
     *  Pointer to general search ui
     * 
     * Not Own
     */
     CSrchUiGeneral* iGenSrchUi;
    
    /**
     *  Pointer to navigation decorator
     *
     * Own
     */
     CAknNavigationDecorator* iNaviDecorator;
    
		/**
     *  icon for the search field
     * 
     * Not Own
     */
      CGulIcon*  iIcon;   
     //For touch
     TBool                               iTouchFlag;  
		};

#endif // C_SRCHUI_MAIN_CONTAINER_H


