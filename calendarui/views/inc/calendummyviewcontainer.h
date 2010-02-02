/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Missed alarm container class declaration
*
*/


#ifndef CALENDUMMYVIEWCONTAINER_H
#define CALENDUMMYVIEWCONTAINER_H

#include <coecntrl.h>

#include "calencontainer.h"
#include "calendummyview.h"

// FORWARD DECLARATIONS
class CCalenAppUi;
class CAknDoubleGraphicStyleListBox;
class CAknIconArray;

/**
 * Class declaration for Missed alarms container
 */
NONSHARABLE_CLASS(CCalenDummyViewContainer) : public CCalenContainer
    {
    public:
        
   	    /**
         * Two-phased constructor.
         * @param aRect of this container.
         * @param aView CCalenDummyView* that owns this class.
         * @return CCalenDummyViewContainer*
         */
        CCalenDummyViewContainer( CCalenNativeView* aView,
                              MCalenServices& aServices);

        /**
         * Destructor.
         */
        virtual ~CCalenDummyViewContainer();

    private: // Functions from base classes
        /**
         * From CCoeControl, child control was resized.
         */
        void SizeChanged();

        /**
         * From CCoeControl, return child control count.
         * @return Control count
         */
        TInt CountComponentControls() const;

        /**
         * From CCoeControl, return child control pointer.
         * @param aIndex Child control index
         * @return Child control pointer.
         */
        CCoeControl* ComponentControl(TInt aIndex) const;

        /**
         * From CCoeControl, process key event.
         * @param aKeyEvent The key event.
         * @param aType EEventKey | EEventKeyUp | EEventKeyDown.
         * @return EKeyWasNotConsumed : Key event was not consumed.
         */
        TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent,
                                    TEventCode aType);

        /**
         * From CCoeControl, process pointer event.
         */
        void HandlePointerEventL(const TPointerEvent& aPointerEvent);

        /**
         * From CCoeControl
         */  
        void Draw(const TRect& /*aRect*/) const;

        /**
         * Pass skin information if needed
         */
        TTypeUid::Ptr MopSupplyObject(TTypeUid aId);

        /**
         * From CCoeControl, Get help context.
         * @param aContext Help context
         */
        void GetHelpContext(TCoeHelpContext& aContext) const;
    
        /**
         * From CCoeControl Handles a resource relative event
         */
        void HandleResourceChange(TInt aType);  
        
        /**
	     * From CCalenContainer.
	     * This methods is called after calendar settings have changed.
	     * Implementation for month view without week numbers.
	     */
	    void UpdateSizeL();                              
        
    private:    // From CCalenContainer

        /**
         * From CCalenContainer Third phase constructor.
         * This function was called CCalenView::ConstructL().
         */
        void ConstructImplL();
        
        /**
         * From CCalenContainer.
         * This methods is called after calendar settings have changed.
         */
        void UpdateSize();
     	
        /**
         * Create icon index
         */
        void CreateIconIndicesL( RArray<MCalenServices::TCalenIcons>& aIndexArray );
        
        /**
         * This method is called after pointer event in Navi
         * decoration object.
         * @param aDirection right or left
         */
        void HandleNaviDecoratorEventL(TInt aEventID);
        
        /**
         * Handle long tap events
         */
        void HandleLongTapEventL( const TPoint& aPenEventLocation, 
                                  const TPoint& aPenEventScreenLocation );
    private:
        
    	CCalenDummyView* idummyView;
    	CAknsBasicBackgroundControlContext* iBgContext; // for skins support
    	CAknDoubleGraphicStyleListBox* iListBox;
    	CDesCArray* iListBoxItemArray;
    };

#endif // CALENDUMMYVIEWCONTAINER_H

// End of File
