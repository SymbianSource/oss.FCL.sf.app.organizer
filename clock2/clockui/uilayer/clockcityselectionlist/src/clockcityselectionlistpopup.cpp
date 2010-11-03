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
* Description:   The source file of the CClockCitySelectionListPopup class.
*
*/

// System includes
#include <aknsfld.h>
#include <eikbtgpc.h>

// User includes
#include "clockcityselectionlistpopup.h"
#include "clock_debug.h"

// ---------------------------------------------------------
// CClockCitySelectionListPopup::NewL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
CClockCitySelectionListPopup* CClockCitySelectionListPopup::NewL( CAknSinglePopupMenuStyleListBox* aListBox,
                                                                  TInt aCbaResource,
                                                                  AknPopupLayouts::TAknPopupLayouts aType )
    {
    __PRINTS( "CClockCitySelectionListImpl::NewL - Entry" );
    
    CClockCitySelectionListPopup* self = new ( ELeave ) CClockCitySelectionListPopup();        
    CleanupStack::PushL( self );
    
    self->ConstructL( aListBox, aCbaResource, aType );
    CleanupStack::Pop();
    
    __PRINTS( "CClockCitySelectionListImpl::NewL - Exit" );
    
    return self;
    }

// ---------------------------------------------------------
// CClockCitySelectionListPopup::~CClockCitySelectionListPopup
// rest of the details are commented in the header
// ---------------------------------------------------------
//
CClockCitySelectionListPopup::~CClockCitySelectionListPopup()
    {
    __PRINTS( "CClockCitySelectionListImpl::~CClockCitySelectionListPopup - Entry" );
    
    // No implementation yet
    
    __PRINTS( "CClockCitySelectionListImpl::~CClockCitySelectionListPopup - Exit" );
    }

// ---------------------------------------------------------
// CClockCitySelectionListPopup::ActivateL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClockCitySelectionListPopup::ActivateL()
    {
    __PRINTS( "CClockCitySelectionListImpl::ActivateL - Entry" );
    
    // Call Base class ActivateL()
    CCoeControl::ActivateL(); 
    // Add the listbox item change observer
    ListBox()->AddItemChangeObserverL( this );
    
    __PRINTS( "CClockCitySelectionListImpl::ActivateL - Exit" );
    }

// ---------------------------------------------------------
// CClockCitySelectionListPopup::ListBoxItemsChanged
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClockCitySelectionListPopup::ListBoxItemsChanged( CEikListBox* aListBox )
    {
    __PRINTS( "CClockCitySelectionListImpl::ListBoxItemsChanged - Entry" );
    
    // Get the CBA button group container
    CEikButtonGroupContainer* cbaContainer( ButtonGroupContainer() );
    
    if( !aListBox->Model()->NumberOfItems() )
        {
        cbaContainer->MakeCommandVisible( EAknSoftkeySelect, EFalse );
        cbaContainer->MakeCommandVisibleByPosition( CEikButtonGroupContainer::EMiddleSoftkeyPosition, EFalse );
        }
        
    else if( ( !cbaContainer->IsCommandVisible( EAknSoftkeySelect ) ) && 
             ( !cbaContainer->IsCommandVisibleByPosition( CEikButtonGroupContainer::EMiddleSoftkeyPosition ) ) ) 
        {
        // Enable the 'Select' button if disabled
        cbaContainer->MakeCommandVisible( EAknSoftkeySelect, ETrue );
        cbaContainer->MakeCommandVisibleByPosition( CEikButtonGroupContainer::EMiddleSoftkeyPosition, ETrue );              
        }
    
    __PRINTS( "CClockCitySelectionListImpl::ListBoxItemsChanged - Exit" );
    }

// ---------------------------------------------------------
// CClockCitySelectionListPopup::ConstructL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
 void CClockCitySelectionListPopup::ConstructL( CAknSinglePopupMenuStyleListBox* aListBox,
                                                TInt aCbaResource,
                                                AknPopupLayouts::TAknPopupLayouts aType )
	{
	__PRINTS( "CClockCitySelectionListImpl::ConstructL - Entry" );
	
	CAknPopupList::ConstructL( aListBox, aCbaResource, aType );
	
	__PRINTS( "CClockCitySelectionListImpl::ConstructL - Exit" );
	}

 // ---------------------------------------------------------
 // CClockCitySelectionListPopup::CClockCitySelectionListPopup
 // rest of the details are commented in the header
 // ---------------------------------------------------------
 //
 CClockCitySelectionListPopup::CClockCitySelectionListPopup()
     {
     __PRINTS( "CClockCitySelectionListImpl::CClockCitySelectionListPopup - Entry" );
     
     // No implementation yet
     
     __PRINTS( "CClockCitySelectionListImpl::CClockCitySelectionListPopup - Exit" );
     }

// End of File
