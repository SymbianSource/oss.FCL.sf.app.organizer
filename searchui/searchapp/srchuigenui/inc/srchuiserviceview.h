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
* Description:    This class is responsible for the display of all the supported contents available for the search operation. It also contains a 
* search field through which the search can be triggered.  
*
*/






#ifndef C_SRCHUI_MAIN_VIEW_H
#define C_SRCHUI_MAIN_VIEW_H

//System Includes
#include <aknview.h>
//#include "SrchUiManager.h"

// FORWARD DECLARATIONS
class CSrchUiServiceContainer;
class CSearchAppUi;
class CSrchUiGeneral;

// CLASS DECLARATION
/**
* View class for the main view.
*
* @lib srchuigenui.lib
*/
class CSrchUiSearchView : public CAknView
    {
	public: // Constructors and destructor
		/** 
		* Symbian OS 2 phased constructor.
		*
    	* @param    aGenSrchUi   Search UI.
		* @return	A pointer to the created instance of CSrchUiSearchView.
		*/
		static CSrchUiSearchView* NewL(CSrchUiGeneral* aGenSrchUi);

		/** 
		* Symbian OS 2 phased constructor.
		*
    	* @param    aGenSrchUi   Search UI.
		* @return	A pointer to the created instance of CSrchUiSearchView.
		*/
		static CSrchUiSearchView* NewLC(CSrchUiGeneral* aGenSrchUi);

	    /**
	    * Destructor.
	    */
		virtual ~CSrchUiSearchView();

	 public: // New functions
	  
	    /**
	    * Changes the CBA buttons.
	    *
	    * @param    aResourceId     Resource of the new CBA buttons.
	    */
	    void SetCbaButtonsL( TInt aResourceId );

		/**
		* Setter for iContainerListBoxIndex.
        *
		* Used by the container so that the list box index can be restored when the view is
		*  is reactivated.
		*
	    * @param    aIndex     The index of list box to be restored.
		*/
		void SetContainerListBoxIndex( TInt aIndex );
		
		/**
		* Getter for iContainerListBoxIndex.
        *
		* Used by the container so that the list box index can be restored when the view is
		* is reactivated.
		*
	    * Returns the index of ContainerListBox.
		*/
		TInt ContainerListBoxIndex();
			
	 public: // Functions from base classes
	    /**
	    * From CAknView.
	    *
	    * Returns the unique id of the view.
	    */
    	TUid Id() const;
        
	    /**
	    * From CAknView.
	    *
	    * Handles the commands generated from softkey and pop up's.
	    * 
	    * @param    aCommand     command to be handled.
	    */
	    void HandleCommandL( TInt aCommand );

	    /**
	    * From CAknView.
	    *
	    * Called by the framework when the view is activated.
		*
	    * @param    aPrevViewId      	Specifies the view previously active.
	    * @param    aCustomMessageId    Specifies the message type.
	    * @param    aCustomMessage     	The activation message.
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
		* @param    aResourceId  ID of the Menu resource.
	    * @param    aMenuPane    The in-memory representation of the menu pane.
		*/
		void DynInitMenuPaneL( TInt aResourceId, CEikMenuPane* aMenuPane );

	 private:    // More constructors
	    /** 
	    * standard constructor.
	    */
	  	CSrchUiSearchView();

	    /**
	    * Performs the second phase construction.
	    */
	    void ConstructL(CSrchUiGeneral* aGenSrchUi);

	 private:    // Data

		/** 
	    * Pointer to the container.
	    * own.
	    */
	    CSrchUiServiceContainer*			iContainer;

		/** 
	    * Flag indicating whether the container is added to the control stack.
	    */
	    TBool                       iContainerOnStack;
	        
		/** 
	    * Current item index from the listbox.
	    */
		TInt                        iContainerListBoxIndex;
			
	    /** 
	    * Pointer to general search ui.	    
	    * not own.
	    */
	    CSrchUiGeneral* iGenSrchUi;
	    TBool iUpdataCba;
	    
	    TBool iCancel;
    };

#endif // C_SRCHUI_MAIN_VIEW_H

// End of File