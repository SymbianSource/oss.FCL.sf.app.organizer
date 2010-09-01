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
* Description:   The header file of the CClockCitySelectionListImpl class.
*
*/

#ifndef __CLOCK_CITY_SELECTION_LIST_IMPL_H__
#define __CLOCK_CITY_SELECTION_LIST_IMPL_H__

// System includes
#include <aknselectionlist.h>

// User includes
#include "clockcityselectionlist.h"

// Forward Declarations
class CAknSingleHeadingStyleListBox;

// Class declaration
/**
* @class CClockCitySelectionListImpl
* @brief The city selection list implementation.
* @dll clockcityselectionlist.dll
*/
class CClockCitySelectionListImpl : public CAknSelectionListDialog,
                                    public MClockCitySelectionList
    {
    public:             // Constructor and destructor
        
        /**
        * @brief Default C++ constructor.
        */
        CClockCitySelectionListImpl();
        
        /**
        * @brief Default C++ destructor.
        */
        virtual ~CClockCitySelectionListImpl();
        
        /**
        * @brief Symbian second phase constructor
        * @param aCityInfo A pointer used to return selected city information.
        */
        void ConstructL( TCityInfo* aCityInfo );
        
    public:             // From base classes
        
        /**
        * @brief From MEikMenuObserver
        * @param aResourceId The ID of the resource which triggered the menu.
        * @param aMenuPane The menu pane which caused the change.
        */
        void DynInitMenuPaneL( TInt aResourceId, CEikMenuPane* aMenuPane );

        /**
        * @brief From CCoeControl. Handles the layout change events for dialog.
        * @param aChange The type of change.
        */
        void HandleResourceChange( TInt aChange );
        
        /**
		* @brief From MClkCitySelectionList. Starts the dialog for execution.
		*/
        TInt ExecuteLD();

    protected:          // From base classes
        
        /**
        * @brief Tells if its ok to exit.
        * @return ETrue if its ok to exit, EFalse otherwise
        */
        TBool OKToExit();

        /**
		* @brief From CAknSelectionListDialog. Pre-layout dialog initialisation.
		*/
		void PreLayoutDynInitL();

        /**
		* @brief From CAknSelectionListDialog. Pre-layout dialog initialization.
		* @param aCommandId The command to process.
		*/
		void ProcessCommandL( TInt aCommandId );

        /**
		* @brief From CAknSelectionListDialog. Handles the key events.
		* @param aKeyEvent The key event which was triggered.
		* @param aEventCode The event code.
		*/
        TKeyResponse OfferKeyEventL( const TKeyEvent& aKeyEvent, TEventCode aEventCode );
        
        /** 
		* @brief From CAknSelectionListDialog. Handles the exit of the dialog once a city has been selected.
		* @param aButtonId The button ID.
		*/
		TBool OkToExitL( TInt aButtonId );
		
        /** 
		* @brief From MEikListBoxObserver. Handles pointer events on the list items
		* @param aListBox The listbox which triggered the event.
		* @param aEventType The event type.
		*/
        void HandleListBoxEventL( CEikListBox* aListBox, TListBoxEvent aEventType );

    private:            // New functions

        /**
        * @brief Called when the user selects a city from the listbox.
        * @param aTryExit Tells if the dialog has to exit after selection.
        */
        void DoSelectCityL( TBool aTryExit = ETrue );

    private:            // Data
        
        /**
        * @var iInfo 
        * @brief Used to pass data of selected city to caller.
        */
		TCityInfo*                  iCityInfo;
		
        /**
        * @var iCityArray
        * @brief String array for storing the city & country names. Owned by list component.
        */
        CDesCArray*                 iCityArray;

        /**
        * @var iCityDialogOffset 
        * @brief Resource file offset
        */
        TInt                        iCityDialogOffset;

        /** 
        * @var iSelectionIndex
		* @brief The index of the city selected.
        */ 
        TInt                        iSelectionIndex;
		
		/** 
        * @var iCitySelected
		* @brief Set to ETrue if any city is selected already.
        */
		TBool						iCitySelected;
    };

#endif      // __CLOCK_CITY_SELECTION_LIST_IMPL_H__

// End of file
