/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   This is the header file for the CClkDateTimeViewPluginInterface class.
*
*/



#ifndef __CLK_DATETIMEVIEW_PLUGIN_INTERFACE_H__
#define __CLK_DATETIMEVIEW_PLUGIN_INTERFACE_H__

// System includes
#include <gsplugininterface.h>

// User includes

// Forward declarations
class CClkDateTimeViewInher;

// Class declaration
/**
* @class CClkDateTimeViewPluginInterface
* @brief clkdatetimeview ecom plugin.
* @dll clkdatetimeviewplugin.dll 
*/
class CClkDateTimeViewPluginInterface : public CGSPluginInterface
    {    
	public:			// Constructor & destructor

	    /**
		* @breif A two-phased constructor. Creates an instance of datetimeview plugin.
		*		 Returns error code KErrNoteFound if implementation is not found.
		* @return CGSPluginInterface* Pointer to the constructed plugin object.
		*/
		static CGSPluginInterface* NewL();

		/**
		* @brief Destructor.
		*/
		~CClkDateTimeViewPluginInterface();

	public:			// Functions from base classes
				
	    /**
	    * @brief From CAknView. Returns the id of the view.
		* @return TUid The view id.
	    */
		TUid Id() const;
	    
	    /**
		* @brief From CGSPluginInterface. Returns the caption(text) to be displayed in the GS view.
		* @param aCaption A reference argument, gets filled with the caption(text).
		*/
	    void GetCaptionL( TDes& aCaption ) const;
		
		/**
		* @brief From CGSPluginInteface. Returns the icon to be displayed in the GS view.
		* @param aIconType The type of icon.
		* @param CGulIcon* Pointer to the icon created.
		*/
		CGulIcon* CreateIconL( const TUid aIconType );
	    
	    /**
		* @brief From CAknView. Handles various commands.
		* @param aCommandId The id of the command to be handled.
	    */
		void HandleCommandL( TInt aCommandId );

        /**
        * @brief From MEikMenuObserver. Changes the menu item dynamically.
		* @param aResourceId The id of the menu pane resource.
		* @param aMenuPane Pointer to the menu pane in context.
        */
        void DynInitMenuPaneL( TInt aResourceId, CEikMenuPane* aMenuPane );
        
        /**
        * @brief Handles the foreground events.
        * @param aForeground ETrue if app is gaining the focus EFalse if loosing focus.
        */
        void HandleForegroundEventL( TBool aForeground );

	protected:		// Functions from base classes
	    
	    /**
	    * @brief From CAknView. Activates this view.
		* @see CAknView.
	    */
		void DoActivateL( const TVwsViewId& aPrevViewId,
						  TUid aCustomMessageId,
						  const TDesC8& aCustomMessage );
		
		/**
	    * @brief From CAknView. Deactivates this view.
		* @see CAknView.
	    */
		void DoDeactivate();		

	private:		// New functions
	  
		/**
		* @brief Performs the 2nd phase of the two-phase construction.
        * @param aId Id of a view which is using the settings view. It is ignored when aClockUse is ETrue.
		* @param aClockUse ETrue if clock is using the plugin, EFalse otherwise.
        */
		void ConstructL( TUid aId, TBool aClockInUse );
		
		/**
		* @brief C++ default constructor.
		*/
	    CClkDateTimeViewPluginInterface();
		
	private:		// Data
	
		/**
		* @var iView
		* @brief Pointer to the datetimeview object.
		*/
		CClkDateTimeViewInher*			iView;	
		
		/**
		* @var iOffset
		* @brief Offset of the resource file in the control environment.
		*/
		TUint							iOffset;
	};

#endif			// __CLK_DATETIMEVIEW_PLUGIN_INTERFACE_H__

// End of file
