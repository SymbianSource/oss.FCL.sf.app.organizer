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
* Description:   Implements CGsBaseView for Calendar GS plugin view.
*
*/



#ifndef GS_CALENDARGSPLUGIN_H
#define GS_CALENDARGSPLUGIN_H

// INCLUDES
#include <gsbaseview.h>

// FORWARD DECLARATIONS
class CCalenGSPluginContainer;
class CAknViewAppUi;
class CCalenSettingsContainer;

// CLASS DECLARATION

/**
 *  CCalenGSPlugin is Calendar's General Settings view class.
 */
class CCalenGSPlugin : public CGSBaseView
    {
    public:  // Construction and destruction
        /**
         * Two phased constructor
         * @param aInitParams initialisation parameters
         * @return a new fully constructed CCalenGSPlugin object.
         */
        static CCalenGSPlugin* NewL( TAny* aInitParams );
            
        /**
         * Destructor.
         */
        ~CCalenGSPlugin();       

    public:  // Functions from base classes
        /**
         * From CAknView, returns the views id.
         * @return id of the view.
         */       
        TUid Id() const;

        /**
         * From MEikCommandObserver, handles the menu based commands.
         * @param aCommand identifies the given command.
         */
        void HandleCommandL( TInt aCommand );

        /**
         * Updates specific value to container 
         * @param aListItem item index to update
         * @param aNewValue new value for the setting
         */
        void UpdateListBoxL( const TInt& aListItem, TInt aNewValue );

     public:  // From CGSPluginInterface 
        /**
         * Method for getting caption of this plugin. This should be the
         * localized name of the settings view to be shown in parent view.
         * @param aCaption pointer to Caption variable
         * @see CGSBaseView
         */
        void GetCaptionL( TDes& aCaption ) const;

        /**
         * Creates a new icon of desired type. Ownership of the created icon
         * is transferred to the caller.
         *
         * Icon type UIDs (use these defined constants):
         * KGSIconTypeLbxItem  -   ListBox item icon.
         * KGSIconTypeTab      -   Tab icon.
         *
         * @param aIconType UID Icon type UID of the icon to be created.
         * @return Pointer of the icon. NOTE: Ownership of this icon is
         *         transferred to the caller.
         * @see CGSBaseView
         */
        CGulIcon* CreateIconL( const TUid aIconType );
        
        /**
         * From CEikAppUi
         * Handles changes in the client rect.
         * @see CGSBaseView
         */
        void HandleClientRectChange();

        /**
         * Get CalenGSPlugin's container.
         * @return a new CCalenGSPluginContainer object.
         * @see CGSBaseView
         */
        CCalenGSPluginContainer* Container();
        
    protected:  // Construction and destruction
        /**
         * C++ default constructor.
         */
        CCalenGSPlugin();

        /**
         * Symbian OS default constructor.
         */
        void ConstructL();

    protected:  // Functions from base classes
        /**
         * Handles OK key press.
         * @see CGSBaseView
         */
        void HandleListBoxSelectionL();
        
        /**
         * Creates a new container.
         */
        void NewContainerL();
        
    protected: // Data        
        RConeResourceLoader iResourceLoader;
        
        // Buffered caption of the plugin. Owned
        HBufC* iCaption; 
        TInt iResourceFileOffset;
    };

#endif // GS_CALENDARGSPLUGIN_H
