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
* Description:     This class is responsible for displaying all the available services on the device. It also contains a  search field through which the search 
*
*/





#ifndef C_SRCHUI_SERVICE_VIEW_H
#define C_SRCHUI_SERVICE_VIEW_H

//System Includes
#include <aknview.h>

//User Includes
#include "srchuigeneral.h"

//Forward Declarations
class CSrchUiMainContainer;
class CSearchAppUi;
class CGeneralSrchUi;

// CLASS DECLARATION

/**
* View class for the main view.
*
* @lib srchuigenui.lib
*/
NONSHARABLE_CLASS( CSrchUiMainView ) : public CAknView
    {
	public: // Constructors and destructor
		/** 
		* Symbian OS 2 phased constructor.
		*
    	* @param    aServiceInfo   UI plug-ins service info.
    	* @param    aGeneralsrchUi   Search UI.
		* @return	A pointer to the created instance of CSrchUiSearchView.
		*/
		static CSrchUiMainView* NewL(RPointerArray<TServiceInfo>& aServiceInfo, CSrchUiGeneral* aGeneralsrchUi);

		/** 
		* Symbian OS 2 phased constructor.
		*
    	* @param    aServiceInfo   Search UI.
    	* @param    ageneralSrchUi   Search UI.
		* @return	A pointer to the created instance of CSrchUiSearchView.
		*/
		static CSrchUiMainView* NewLC(RPointerArray<TServiceInfo>& aServiceInfo,CSrchUiGeneral* ageneralSrchUi);

	    /**
	    * Destructor.
	    */
		virtual ~CSrchUiMainView();

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
	    * @param    aIndex     The index of list box to be restored.
		*/
		void SetContainerListBoxIndex( TInt aIndex );
		
		/**
		* Getter for iContainerListBoxIndex.
    	*
		* Used by the container so that the list box index can be restored when the view is
		* is reactivated.
	    * Returns the index of ContainerListBox.
		*/
		TInt ContainerListBoxIndex();
   	
    	/**
    	* returns a pointer to the container.
    	*/
    	CCoeControl* Container();
    			
		/**
		* Getter for UI plug-ins info
	    * @param    aInfo : array of UI plug-ins service info.
		*/
		void GetData(RPointerArray<TServiceInfo>& aInfo); 
    			
	public: // Functions from base classes
        /**
	    * From CAknView.
	    * Returns the unique id of the view.
	    */
    	TUid Id() const;
        
	    /**
	    * From CAknView.
	    *
	    * Handles the commands that are passed from softkey generated commands and
	    * pop up's.
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
	    * Performs the first phase of two phase construction.
	    */
	  	CSrchUiMainView();

	    /**
	    * Performs the second phase construction.
	    * @param    aServiceInfo	UI Plug-ins Service info.
	    * @param    aGenSrchUi     	Search UI.
	    */
	    void ConstructL(RPointerArray<TServiceInfo>& aServiceInfo,CSrchUiGeneral* aGenSrchUi);
   
  	private:    // Data

	    /* Pointer to the container.
		* not own
	    */
	    CSrchUiMainContainer*			iContainer;

	    /* Flag indicating whether the container is added to the control stack.
	    */
	    TBool                       iContainerOnStack;

		/* Current item index from the listbox.
		// When the previous result view is activated, it's 
		// good to restore the previous listbox
		// item index
	    */
		TInt                        iContainerListBoxIndex;

		/* UI Plug-ins Service info.
		* not own.
	    */
		RPointerArray<TServiceInfo> iServiceInfo;
			
		/* Pointer to general search ui
		* not own.
	    */
		CSrchUiGeneral* iGenSrchUi;
    };

#endif // C_SRCHUI_SERVICE_VIEW_H

// End of File