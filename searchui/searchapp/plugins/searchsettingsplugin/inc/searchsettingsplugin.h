/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:      Search Settings Plugin header file.
*
*/






#ifndef C_SEARCHSETTINGSPLUGIN_H
#define C_SEARCHSETTINGSPLUGIN_H

// Includes
#include <GSParentPlugin.h>
#include <GSFWViewUIDs.h>
#include <eiklbo.h>
#include <coneresloader.h>
#include <aknview.h>
#include <eikclb.h>
#include <srchuisettingsplugininterface.h>

// Classes referenced
class CAknViewAppUi;
class RConeResourceLoader;
class CGSTabHelper;
class CSearchSettingsContainer;
class CSrchUiSettingsView;
class CGSPluginInterface;
class CGSMainView;

// Use this UID if plugin belongs to SearchSettings view:
const TUid KGSSearchSettingsPluginUid = { 0x10282E04 };

_LIT( KGSAppsPluginResourceFileName, "z:GSAppsPluginRsc.rsc");
_LIT( KGSAppsPluginIconDirAndName, "z:gsappsplugin.mbm"); // Use KDC_BITMAP_DIR
   

// CLASS DECLARATION

/**
*  Applications Plugin. Displays plugins owned by applications view.
*
* @lib SearchSettingsPlugin.lib
*/
class CSearchSettingsPlugin : public CGSParentPlugin, public MEikListBoxObserver
    {
    public: // Constructors and destructor

        /**
        * Symbian OS two-phased constructor.
    	* @param aInitParams Initialisation Parameter.
        * @return pointer of CSearchSettingsPlugin.
        */
        static CSearchSettingsPlugin* NewL( TAny* aInitParams );

        /**
        * Destructor.
        */
        ~CSearchSettingsPlugin();

    public: // From CAknView

        /**
        * See base class.
        * @return TUid of CSearchSettingsPlugin.
        */
        TUid Id() const;

        /**
        * See base class.
    	* @param aPrevViewId Previous View ID before activation.
    	* @param aCustomMessageId ID of the Custom Message.
    	* @param aCustomMessage Actual Custom Message.
        */
        void DoActivateL( const TVwsViewId& aPrevViewId,
                          TUid aCustomMessageId,
                          const TDesC8& aCustomMessage );

        /**
        * See base class.
        */
        void DoDeactivate();
              

    public: // From CGSParentPlugin

        /**
        * See base class.
        * @return TUid of GS App.
        */
        TUid UpperLevelViewUid();

        /**
        * See base class.
    	* @param aContext The control's help context.
        */
        void GetHelpContext( TCoeHelpContext& aContext );
        
		/**
		* Handles the commands given by the user.
		* @param aCommand ID of the command
		*/
		void HandleCommandL( TInt aCommand );

		/**
		* Handles the commands given with keys
		* @param aListbox is the listbox concerned.
		* @param aEventType is the corrent event that needs to be handled.
		*/
		void HandleListBoxEventL( CEikListBox* aListBox, 
		                          TListBoxEvent aEventType );        

    public: // From CGSPluginInterface

        /**
        * See base class.
		* @param aCaption Main caption of the search setting.
        */
        void GetCaptionL( TDes& aCaption ) const;

        /**
        * See base class.
		* @param aIconType Type of the Icon to create.
        */
        CGulIcon* CreateIconL( const TUid aIconType );

        /**
        * See base class.
        * @return category of the plugin.
        */
        TInt PluginProviderCategory() const;

        /**
        * Handles changes to the application when it switches to 
        * or from the foreground
		* @param aForeground ETrue to switch to the foreground. 
		* EFalse to switch from the foreground.
        */
		void HandleForegroundEventL( TBool aForeground );
				
    protected: // New

        /**
        * C++ default constructor.
        */
        CSearchSettingsPlugin();

        /**
        * Symbian OS default constructor.
        */
        void ConstructL();

	private:
		/**
		* Update ListBox after selection changed.
		*/
		void HandleListBoxSelectionL();


    private: // Data

	    /**
	    * Resource loader
	    */
        RConeResourceLoader iResources;

	    /**
	    * Previous View ID.
	    */        
        TVwsViewId iPrevViewId;
 
	    /**
	    * Container of the PlugIn.
        * Own.
	    */       
        CSearchSettingsContainer* iContainer;
       
    };


#endif // C_SEARCHSETTINGSPLUGIN_H
// End of File
