/*
* Copyright (c) 2009-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Class implementing the RECAL customisation interface
*
*/


#ifndef CBCVIEWPLUGIN_H
#define CBCVIEWPLUGIN_H

// SYSTEM INCLUDES
#include <calencustomisation.h>

// FORWARD DECLARATIONS
class MCalenServices;
class CBCResourceManager;
class MCalenPreview;

// CLASS DECLARATIONS
/**
 * This class is used for creating implement of view through ECom framework.
 */
class CBCViewPlugin : public CCalenCustomisation
    {
    public: // C'tor and d'tor
        /**
        * Static constructor.
        * @param aServices 
        * @return A new instance of MCalenServices
        */        
        static CBCViewPlugin* NewL( MCalenServices* aServices );
        
        /**
        * Destructor
        */
        ~CBCViewPlugin();
        

        
    public: // Interface
    
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
        virtual const TDesC& InfobarL();
        
        /**
         * Gets the preview pane from the plugin to be shown at the 
         * calendar view. Offers the rectangle for the preview pane, 
         * which it can be used to draw the pane.
         * 
         * @param aRect The area available for the Preview Pane
         * @return Preview pane interface
         */
        MCalenPreview* CustomPreviewPaneL( TRect& aRect ); // from wk39 RECAL
        
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
        virtual TBool CanBeEnabledDisabled();
        
        /**
         * Allows extending this API without breaking BC.
         * 
         * @param aExtensionUid specifies
         * @return extension of the requested type
         */
        virtual TAny* CalenCustomisationExtensionL( TUid aExtensionUid );
        
    private: // Implementation
        /**
         * C++ constructor
         */
        CBCViewPlugin( MCalenServices* aServices );
        
        /**
         * constructor
         */
        void ConstructL();
        
    private: // Data
        /// Ref: Services interface for Calendar services
        MCalenServices* iServices;
        /// Own: Resource file offset
        TInt iResourceOffset;
    };
    
#endif // CBCVIEWPLUGIN_H

// End of file
