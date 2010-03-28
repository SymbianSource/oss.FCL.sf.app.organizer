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
* Description:     Acts as a container for the CSrchUiMainView view. 
*
*/






#ifndef C_SRCHUI_SERVICE_CONTAINER_H
#define C_SRCHUI_SERVICE_CONTAINER_H

//System Includes
#include <coecntrl.h>
#include <aknlists.h>
#include <aknsfld.h> 
#include <e32std.h>
#include <aknnavide.h>

//Forward Declarations
class CSrchUiMainView;
class MAknsControlContext; // For skins support.
class CSrchUiGeneral;

_LIT(KListTextFormat, "%d\t%S\t%S");

// CLASS DECLARATION

/**
* Container class for CSrchUiSearchView.
* @lib srchuigenui.lib
*/
NONSHARABLE_CLASS( CSrchUiMainContainer ) : public CCoeControl, public MEikListBoxObserver
    {
  public: // Constructors and destructor

    /** 
    * Performs the first phase of two phase construction.
    *
    * @param	aView   Reference to CSrchUiMainView.
    * @param    aGenSrchUi   Search UI.
    */
    CSrchUiMainContainer( CSrchUiMainView& aView,CSrchUiGeneral* aGenSrchUi  );

    /**
    * Performs the second phase construction.
    * @param    aRect   Dimensions of the view.
    */
    void ConstructL( const TRect& aRect );
        
    /**
    * Destructor.
    */
    ~CSrchUiMainContainer();

  public:	// New functions

    /**
    * Starts off the search.
    */
    void DoSearchL();
    	
	/**
    * Getter for index of the highlighted item in list box.
	* @return	Current Item Index.
    */
	TInt CurrentItemIndex();

    /**
    * Handles changes to the navigation pane text.
    */
    void UpdateNaviPaneL();

  public:    // Functions from base classes

    /**
    * From CCoeControl.
    * Called by the framework when a key event needs to be processed.
    * @param    aKeyEvent   Key event.
    * @param    aType   Type of the event.
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
    */
    void SizeChanged();

    /**
    * From CCoeControl.
    * Gets the number of controls contained in a compound control.
	* @return	Count of the controls.
    */
    TInt CountComponentControls() const;

    /**
    * From CCoeControl.
    * Gets the specified component of a compound control.
    * @param    aIndex   Index of the component of a compound control.
	* @return	component of a compound control.
    */
    CCoeControl* ComponentControl( TInt aIndex ) const;

    /**
    * From CCoeControl
    *
    * Handles a change to the application's resources which are shared across the environment.
    *
    * @param    aType   The type of resources that have changed. 
    */
    void HandleResourceChange( TInt aType );

    /**
    *
    * Handles a change to the application's resources which are shared across the environment.
    *
    * @param    aType   The type of resources that have changed. 
    */
    void HandleResourceChangeL( TInt aType );

    /**
    * From MEikListBoxObserver
    * Called by the framework when a list box event needs to be processed.
    * @param    aListBox   The originating list box. 
    * @param    aEventType   A code for the event
    */
    void HandleListBoxEventL( CEikListBox* aListBox, TListBoxEvent aEventType );

  private:	// New functions

    /**
    * Constructs and prepares the list box control.
    */
    void SetupListBoxL();
    	
    /** 
    * Handles the selection of the currently active item.
    */
    void HandleItemSelectionL();

    /**
    * Returns an array of icons for use in the listbox.
	* @return	an array of icons.
    */		
    CArrayPtr<CGulIcon>* ListBoxIconsArrayL();
    	
    /** 
    * Constructs the item text array.
    */
    void ConstructListTextL();
    
    /** 
    * Update the title pane.
    */
    void UpdateTitlePaneL();

  private:    // Data

    /* Reference to the view.
    */
    CSrchUiMainView&					  iView;
    
    /* Pointer to the general search ui.
    * not own.
    */
    CSrchUiGeneral* iGenSrchUi;
    	
    /* List box control.
    * own.
    */
    CAknDoubleGraphicStyleListBox*				  iListBox;
    
    /* Pointer to the navigation control container needed for TAB groups.
    * not own
    */
	CAknNavigationControlContainer*  iNaviPane;
	
	/* Title Text.
	* own.
    */
	HBufC* iTitleText;
	
	/* Navigation Lable.
	* own.
    */
    CAknNavigationDecorator*              iNaviDecorator;
    
    /* The Item Text Array.
    * own.
    */
    CDesCArrayFlat                        iArray;
    
    /* Buffer for getting the text out of the CAknSearchField.
    * own.
    */	
    HBufC*                                iSearchText;
        
    /* Modifiable pointer descriptor to iSearchText.
    * not own.
    */
    TPtr                                  iSearchTextPtr;
    //Touch
    TBool                               iTouchFlag; 
   	};

#endif // C_SRCHUI_SERVICE_CONTAINER_H

// End of File
