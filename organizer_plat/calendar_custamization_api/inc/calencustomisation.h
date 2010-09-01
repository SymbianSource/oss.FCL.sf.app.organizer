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
* Description:  Interface for Calendar customisation plugins
*
*/

#ifndef CALENCUSTOMISATION_H
#define CALENCUSTOMISATION_H

// System includes
#include <e32base.h>
#include <ecom/ecom.h>

// Constants
const TUid KCalenCustomisationInterfaceUid = { 0x1028336F };

// Forward declarations
class CCalenView;
class MCalenServices;
class MCalenCommandHandler;
class CAknSettingItem;
class CCoeControl;
class TRect;
class CEikMenuPane;
class MCalenPreview;
	
/**
 * @class CCalenCustomisation 
 * @brief Base class for creating customised calendar plugins.
 */
class CCalenCustomisation : public CBase
    {
    public:
        
		/**
		 * @brief Creates the implementation with UID aUid
		 * 
		 * @param aUid Interface uid
		 * @param aServices Services API which are exposed to plugins.
		 * @return CCalenCustomisation Pointer to created plugin
		 */
        static CCalenCustomisation* CreateImplementationL( TUid aUid, MCalenServices* aServices );
        
		/**
		 * @brief Default C++ destructor
		 */        
        virtual ~CCalenCustomisation();

    public:
        
		/**
		 * @brief Gets the list of views provided/supported by the plugin in aCustomViewArray
		 * 
		 * @param aCustomViewArray The array containing all the views supplied by the plugin, of type CCalenView
		 */
        virtual void GetCustomViewsL(  RPointerArray<CCalenView>& aCustomViewArray ) =0;
        
		/**
		 * @brief Gets the list of settings provided/supported by the plugin in aCustomSettingArray
		 *
		 * @param aCustomSettingArray The array containing all the setting items supplied by the plugin, of type CAknSettingItem
		 */
        virtual void GetCustomSettingsL( RPointerArray<CAknSettingItem>& aCustomSettingArray ) =0;
        
		/**
		 * @brief Gets the information bar which is to be shown at the top of the calendar views from the plugin
		 * 
		 * @param aRect The size of the information bar bar provided to plugin
		 * @return CCoeControl The actual information bar control
		 */
        virtual CCoeControl* InfobarL( const TRect& aRect ) =0;
        
		/**
		 * @brief Gets the text to be shown inside the information bar from the plugin
		 * 
		 * @return TDesC Text to be shown in the information bar
		 */
        virtual const TDesC& InfobarL() = 0;
        
        /**
         * Gets the preview pane from the plugin to be shown at the 
         * calendar view. Offers the rectangle for the preview pane, 
         * which it can be used to draw the pane.
         * 
         * @param aRect The area available for the Preview Pane
         * @return Preview pane interface
         */
        virtual MCalenPreview* CustomPreviewPaneL( TRect& aRect ) =0;
        
        /**
         * Gets preview pane from plugin to be shown at bottom of calendar views
         * 
         * @param aRect The size of preview pane provided to plugin.
         * @return The control of preview pane.
         */
        virtual CCoeControl* PreviewPaneL( TRect& aRect ) =0;
        
		/**
		 * @brief Gets the command handler for aCommand. Plugins can choose to handle the command 
		 * or ignore it.
		 * 
		 * @param aCommand The command id for which the command handler is needed
		 * @return MCalenCommandHandler The command handler to handle aCommand
		 */
        virtual MCalenCommandHandler* CommandHandlerL( TInt aCommand ) = 0;
        
		/**
		 * @brief Gets the list of view ids to be removed from view cycle
		 * 
		 * @param aViews The array of view ids to be removed from view cycle
		 */
        virtual void RemoveViewsFromCycle( RArray<TInt>& aViews ) = 0;

		/**
		 * @brief Adds a menuitem to the menupane specified by aMenuPane
		 * 
		 * @param aResourceId The resource id of the menu pane
		 * @param aMenuPane The menupane to which menuitem is added
		 */
        virtual TBool CustomiseMenuPaneL( TInt aResourceId, CEikMenuPane* aMenuPane ) = 0;
        
		/**
		 * @brief Checks whether the plugin can be enabled or disabled from calendar settings
		 * 
		 * @return ETrue if plugin can be enabled/disabled from calendar settings, EFalse otherwise
		 */
        virtual TBool CanBeEnabledDisabled() = 0;
        
		/**
		 * @brief Allows extending this API without breaking BC.
		 * 
		 * @param aExtensionUid Specifies the UID of the extenstion
		 * @return TAny Extension of the requested type
		 */
        virtual TAny* CalenCustomisationExtensionL( TUid aExtensionUid ) = 0;

    private:

		/**
		 * @var iDtor_ID_Key
		 * @brief Instance identifier key. When instance of an implementation is
		 * created by ECOM framework, the framework will assign a UID for
		 * it. The UID is used in the destructor to notify framework that
		 * this instance is being destroyed and resources can be released.
		 * Plugin implementations should leave this untouched, as the inline
		 * destructor and constructor method take care of ECom releasing.
		 */
        TUid	iDtor_ID_Key;
    };

#include "calencustomisation.inl"

#endif  // CALENCUSTOMISATION_H

// End of file
