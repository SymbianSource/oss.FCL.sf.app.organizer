/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Description:   Class implementing the RECAL customisation interface.
*
*/
#ifndef BCPREVIEWPLUGIN_H
#define BCPREVIEWPLUGIN_H

// INCLUDES
#include <calencustomisation.h>
#include "bcpreviewpanecontainer.h"

// FORWARD DECLARATIONS
class MCalenServices;


// CLASS DECLARATIONS
/**
 * This class is used for creating implement of view through ECom framework.
 */
class CBCPreViewPlugin : public CCalenCustomisation,
                         public MCalenNotificationHandler
    {
public: // C'tor and d'tor
    /**
    * Static constructor.
    * @param aServices
    * @return A new instance of MCalenServices
    */
    static CBCPreViewPlugin* NewL( MCalenServices* aServices );

    /**
    * Destructor
    */
    ~CBCPreViewPlugin();

public: // From base class
    /**
    * Get views and add them into the array.
    * @param aCustomViewArray the array which be used to store views.
    */
    void GetCustomViewsL(  RPointerArray<CCalenView>& aCustomViewArray );

    /**
    * Get settings in the custom plugin.
    * @param aCustomSettingArray the array which be used to store setting value.
    */
    void GetCustomSettingsL( RPointerArray<CAknSettingItem>& aCustomSettingArray );

    /**
    * Get the handler of infobar control.
    */
    CCoeControl* InfobarL( const TRect& aRect );

    /**
     * Gets text from plugin to be shown at top of calendar views
     *
     * @return text to be shown into infobar
     */
    const TDesC& InfobarL();

    MCalenPreview* CustomPreviewPaneL( TRect& aRect );

    /**
    * Get the handler of Preview Pane.
    */
    CCoeControl* PreviewPaneL( TRect& aRect );

    /**
    * Get the command handler.
    */
    MCalenCommandHandler* CommandHandlerL(TInt aCommand);

    /**
    * Remove specific view from the cycle of views.
    */
    void RemoveViewsFromCycle( RArray<TInt>& aViews );

    /**
    * Custom menu items in specific menu pane.
    */
    TBool CustomiseMenuPaneL( TInt aResourceId, CEikMenuPane* aMenuPane );

    /**
     * Checks whether the plugin can be enabled or disabled from calendar settings
     *
     *
     * @return ETrue if plugin can be enabled/disabled from calendar settings.
     *         EFalse if plugin cannot be enabled/disabled  from calendar settings.
     *
     */
    TBool CanBeEnabledDisabled();

    /**
     * Allows extending this API without breaking BC.
     *
     * @param aExtensionUid specifies
     * @return extension of the requested type
     */
    TAny* CalenCustomisationExtensionL( TUid aExtensionUid );
    
public: // From notifcationhandler
    void HandleNotification( const TCalenNotification aNotification );

private: // Implementation
    enum TMenuPluginSettingIndex
		{
    	EBCDisplayPreviewPaneItemId = 1
		};
    
    /**
     * C++ constructor
     */
    CBCPreViewPlugin( MCalenServices* aServices );

    /**
     * constructor
     */
    void ConstructL();
    
    /**
     * Add binary type (true/false) setting item
     * 
     * @param aBuffer reference to an externally owned boolen setting item
     * @param aId resource id for the associated values and texts
     * @param aTitleResource resource id for the setting title
     * @param aSettingPageResource resource id for the setting page
     * @param aAssociatedResource 
     * @param aOrdinal an ordinal number
     */
    CAknSettingItem* AddBinaryItemL( TBool& aBuffer,
									 TInt aId,
									 TInt aTitleResource,
									 TInt aSettingPageResource,
									 TInt aAssociatedResource,
									 TInt aOrdinal );
    /**
	 * This will enable or disable previewpane depending on whether aEnabled is 
	 * set to 1 or 0.
	 * @param aEnabled sets to 1 means preview pane is enable,
	 *                 sets to 0 means preview pane is disabled. 
	 */
	void SetPreviewPaneEnabledL(const TInt& aEnabled);
	/**
	 * Gets a value from cenrep indicating that preview pane is enabled or not.
	 * @return 1 when preview pane is enabled otherwise 0. 
	 */
	TInt GetPreviewPaneEnabledL();

private: // Data
    /// Ref: Control enviroment 
    CCoeEnv* iCoeEnv;
    /// Ref: Services interface for Calendar services
    MCalenServices* iServices;
    /// Own: Indicates whether to display preview pane
    TBool iDisplayPreviewPane;
    /// Own: Resource file offset
    TInt iResourceOffset;
    };

#endif // BCPREVIEWPLUGIN_H

// End of file
