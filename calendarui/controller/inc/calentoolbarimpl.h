/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Calendar Toolbar Implementation
 *
*/


#ifndef CALENTOOLBARIMPL_H
#define CALENTOOLBARIMPL_H

#include <akntoolbarobserver.h>                 // MAknToolbarObserver
#include <coecobs.h>                            // MCoeControlObserver
#include <calentoolbar.h>                       // MCalenToolbarObserver

//Forward declarations
class CAknAppUi;
class CAknButton;
class CAknToolbar;
class CCalenController;
class CGulIcon;
class CAknView;
class CCalenViewInfo;

NONSHARABLE_CLASS( CCalenToolbarImpl ) : public CBase, 
public MCalenToolbar,
public MAknToolbarObserver,
public MCoeControlObserver
    {
    public:

        /**
         * First phase construction
         * Creates a new CCalenToolbarImpl instance
         * @param aController Reference to the Calendar Controller
         */
        static CCalenToolbarImpl* NewL( CCalenController& aController );

        /**
         * Destructor
         * Frees all resources used by this class
         */
        ~CCalenToolbarImpl();

        /**
         * From MCalenToolbar
         * Check if the toolbar is currently visible
         * @return ETrue if the toolbar is currently shown, EFalse otherwise
         */
        TBool IsVisible();

        /**
         * From MCalenToolbar
         * Show or hide the toolbar.  Has no effect if the toolbar is
         * disabled
         * @param aVisibility ETrue to set visible, EFalse to set invisible
         */
        void SetToolbarVisibilityL( TBool aMakeVisible );

        /**
         * From MCalenToolbar
         * Update the toolbar using Calendar layout information
         */
        void UpdateToolbar();

        /**
         * From MCalenToolbar
         * Accessor for the concrete toolbar
         * @return Reference to the concrete toolbar object
         */
        CAknToolbar& Toolbar();

        /**
         * From MCalenToolbar.
         * Allows extending this API without breaking BC.
         * 
         * @param aExtensionUid specifies
         * @return extension of the requested type
         */
        TAny* CalenToolbarExtensionL( TUid aExtensionUid );

        /**
         * From MAknToolbarObserver
         * Called before toolbar is drawn.  Allows modification of toolbar
         * components based on current context
         * @param aResourceId The resource ID for particular toolbar
         * @param aToolbar The toolbar object pointer
         */
        void DynInitToolbarL( TInt aResourceId, CAknToolbar* aToolbar );

        /**
         * From MAknToolbarObserver
         * Handles toolbar events for a toolbar item
         * @param aCommand The command ID of the toolbar item
         */
        void OfferToolbarEventL( TInt aCommand );

        /**
         * From MCoeControlObserver
         * Handles an event from an observed control
         * @param aControl The control that sent the event
         * @param aEventType The event type
         */
        void HandleControlEventL( CCoeControl* aControl, TCoeEvent aEventType );

        /**
         * Update icon for "NextView" button
         * @param aIcon New icon for the nextview button
         */
        void SetNextViewIcon( CGulIcon* aIcon );

        /**
         * Populate the change view toolbar extension 
         * with available cycling views
         */
        void PopulateChangeViewToolbarExtensionL();

        /**
         * Resets calendar toolbar
         */
        void ResetCalendarToolbar();

        /**
         * For handling focus in toolbar extension
         */
        void SetToolbarExtensionFocus(TBool aFocus);

        /**
         * Reset toolbar extension commands 
         */
        void ResetToolbarExtensionCommandsL();
        
        inline TBool IsICalenToolBar()
            {
            return iCalenToolbar ? ETrue : EFalse;
            }
        
    private:

        /**
         * Private constructor
         * Creates a new CCalenToolbarImpl instance
         * @param aController Reference to the Calendar Controller
         */
        CCalenToolbarImpl( CCalenController& aController );

        /**
         * Second phase construction
         * Creates initial toolbar buttons
         */
        void ConstructL();

        /**
         * Create calendar toolbar buttons
         * @param aIcon Button Icon
         * @param aIconTextId Icon Text
         * @param aTooltipID Tooltip Text
         */ 
        CAknButton* CreateButtonL( CGulIcon* aIcon, 
                                   TInt aIconTextId,
                                   TInt aTooltipID );
        
        /**
         * Create calendar toolbar buttons
         * @param aIcon Button Icon
         * @param aIconTextId Icon Text
         * @param aTooltipID Tooltip Text
         */        
        CAknButton* CreateButtonForCustomViewsL(CGulIcon* aIcon,
                                                TInt aIconTextId,
                                                const TDesC &aIconText );

        /**
         * Add view toolbar items to the extendable toolbar as per 
         * view cycle positions
         * 
         * @param aIndex Index of the toolbar item
         * @aViewInformation reference to the CCalenViewInfo 
         */
        void AddItemsToExtendableToolbarL( TInt aIndex,
                                           CCalenViewInfo& aViewInformation);
        
        /**
         * Get view position list based on the cycling positions
         * 
         * @param aCurrentViewUid UID of the current view
         * @param aViewPositionArray array of view positions as per view cycle  
         */
        void GetViewPositionList( TUid aCurrentViewUid,
                                  RArray<TUid>& aViewPositionArray,
                                  RPointerArray<CCalenViewInfo>& aViewInfoArray);
        
        /**
         * Get IconText and tooltip text resource ids for extendable toolbar
         * items  
         * 
         * @param aViewUid view uid of the view
         * @param aIconTextId resource id of icon text
         * @param aToolTipTextId resource id of tooltip text
         */
        void GetResourceIdForToolTip( TUid aViewUid, TInt& aIconTextId,
                                      TInt& aToolTipTextId);
        
    private: // Data.
        
        CAknAppUi* iAppUi;                  // Application UI. Not owned by this class
        CCalenController& iController;      // Calendar Controller reference.
        CAknToolbar* iCalenToolbar;         // calendar toolbar. Owned by the CCalenToolbarImpl
        TBool iStartUpToolbar;
        RArray<TInt> iExtensionToolbarCommands;
        CAknToolbar* iAppToolbar;           // Concrete toolbar. Owned by the appui.

    };

#endif // CALENTOOLBARIMPL_H

// End of File
