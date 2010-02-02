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
* Description:   Manages preview pane, info bar and toolbar for calendar views
 *
*/



#ifndef CALENCONTAINERLAYOUTMANAGER_H
#define CALENCONTAINERLAYOUTMANAGER_H

//  INCLUDES
#include <calenservices.h>              // Calendar services
#include "calencontainer.h"             // CCalenContainer

//  FORWARD DECLARATIONS
class CAknToolbar;

/**
 *  CCalenNativeViewExtension
 *  This class decides if a calendar view should additionally display a 
 *  preview pane or an info bar
 */
NONSHARABLE_CLASS( CCalenContainerLayoutManager ) : public CBase
    {
    public:

        enum TCalenContainerLayout
            {
            ECalenContainerToolbar,
            ECalenContainerInfobar,
            ECalenContainerPreview
            };
    
        static CCalenContainerLayoutManager* NewL( 
                            MCalenServices& aServices, CCalenContainer& aContainer );
                            
        ~CCalenContainerLayoutManager();

    public:
        TBitFlags LayoutFlags();
        CCoeControl* ControlOrNull();
        void GetLayoutAndExtensionL();
        CAknToolbar* ToolbarOrNull();
        void SetToolbarVisibility( TBool aVisible );
        
        void HandleResourceChange( TInt aType );
        
    private:
        CCalenContainerLayoutManager( MCalenServices& aServices, CCalenContainer& aContainer );
        void ConstructL();       
        
        void CheckPreviewL();
        void CheckInfobarL();
        void CheckToolbarL();
        
    private:
        CCalenContainer& iContainer;        // Calendar Container
        MCalenServices& iServices;          // Calendar Services
        CCoeControl* iInfobar;              // Info bar, owned by a plugin
        CAknToolbar* iToolbar;              // Toolbar, owned by the appui
        TBitFlags iLayoutFlags;             // Bitfield for current layout
    };

#endif // CALENCONTAINERLAYOUTMANAGER_H


// End of file
