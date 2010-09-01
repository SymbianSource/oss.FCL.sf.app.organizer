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
* Description:   The header file of the CClockCitySelectionListPopup class.
*
*/

#ifndef __CLOCK_CITY_SELECTION_LIST_POPUP_H__
#define __CLOCK_CITY_SELECTION_LIST_POPUP_H__

// System includes
#include <aknPopup.h>
#include <aknlists.h>
#include <eiklbx.h>

// Class declaration
/**
* @class CClockCitySelectionListPopup
* @brief implements a customized pop-up which disables LSK if no city / country matches.
* @dll clockcityselectionlist.dll
*/
NONSHARABLE_CLASS( CClockCitySelectionListPopup ) : public CAknPopupList, 
									                public MListBoxItemChangeObserver 
	{
	public:        // Constructor and destructor
	    
	    /**
        * @brief Two-phased constructor.
        * @param aListBox Pre-existing listbox-derived class.
        * @param aCbaResource Softkey pane to display while pop-up is active.
        * @return CClkDateTimeViewPopupListInher*
        */
        static CClockCitySelectionListPopup* NewL( CAknSinglePopupMenuStyleListBox* aListBox,
                                                   TInt aCbaResource,
                                                   AknPopupLayouts::TAknPopupLayouts aType );
	            
	    /**
        * @brief Default C++ destructor
        */ 
        ~CClockCitySelectionListPopup();

	public:        // From base classes
	    
        /**
        * @brief From CCoeControl, handle activation of control.
        */                     
        void ActivateL();
    
        /**
        * @brief From MListBoxItemChangeObserver, handles enable / disable LSK.
        * @param aListBox The listbox pointer.
        */                         
        void ListBoxItemsChanged( CEikListBox* aListBox );
        
    private:
    
        /**
        * @brief Default C++ constructor
        */
        CClockCitySelectionListPopup();
    
        /**
        * @brief Symbian 2nd phase constructor.
        * @param aListBox Pre-existing listbox-derived class
        * @param aCbaResource Softkey pane to display while pop-up is active
        */
        void ConstructL( CAknSinglePopupMenuStyleListBox* aListBox,
                         TInt aCbaResource,
                         AknPopupLayouts::TAknPopupLayouts aType );
    };

#endif          // __CLOCK_CITY_SELECTION_LIST_POPUP_H__

// End of file
