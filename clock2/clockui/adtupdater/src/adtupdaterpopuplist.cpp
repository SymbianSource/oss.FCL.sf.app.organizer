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
* Description:   This is the source file for the CAdtUpdaterPopupList class.
*
*/

// System Includes
#include <aknlists.h>

// User Includes
#include "adtupdaterpopuplist.h"
#include "clock_debug.h"

// Constants

// Literals

// -----------------------------------------------------
// CAdtUpdaterPopupList::CAdtUpdaterPopupList
// rest of the details are commented in the header 
// -----------------------------------------------------
//
CAdtUpdaterPopupList::CAdtUpdaterPopupList()
	{
	__PRINTS( "CAdtUpdaterPopupList::CAdtUpdaterPopupList - Entry" );
	
	// No implementation yet.
	
	__PRINTS( "CAdtUpdaterPopupList::CAdtUpdaterPopupList - Exit" );
	}
	
// -----------------------------------------------------
// CAdtUpdaterPopupList::ConstructL
// rest of the details are commented in the header 
// -----------------------------------------------------
//
void CAdtUpdaterPopupList::ConstructL( CAknSinglePopupMenuStyleListBox* aListBox, 
                        			   TInt aCbaResource,
                        			   AknPopupLayouts::TAknPopupLayouts aType )
	{
	__PRINTS( "CAdtUpdaterPopupList::ConstructL - Entry" );
	
	CAknPopupList::ConstructL( aListBox, aCbaResource, aType );
	
	__PRINTS( "CAdtUpdaterPopupList::ConstructL - Exit" );
	}
	
// -----------------------------------------------------
// CAdtUpdaterPopupList::~CAdtUpdaterPopupList
// rest of the details are commented in the header 
// -----------------------------------------------------
//   
CAdtUpdaterPopupList::~CAdtUpdaterPopupList()
	{
	__PRINTS( "CAdtUpdaterPopupList::~CAdtUpdaterPopupList - Entry" );
	
	// No implementation yet.
	
	__PRINTS( "CAdtUpdaterPopupList::~CAdtUpdaterPopupList - Exit" );
	}
	
// -----------------------------------------------------
// CAdtUpdaterPopupList::NewL
// rest of the details are commented in the header 
// -----------------------------------------------------
//
CAdtUpdaterPopupList* CAdtUpdaterPopupList::NewL( CAknSinglePopupMenuStyleListBox* aListBox,
												  TInt aCbaResource,
												  AknPopupLayouts::TAknPopupLayouts aType )
	{
	__PRINTS( "CAdtUpdaterPopupList::NewL - Entry" );
	
	CAdtUpdaterPopupList* self = new ( ELeave ) CAdtUpdaterPopupList;
	CleanupStack::PushL( self );
	
	self->ConstructL( aListBox, aCbaResource, aType );
	
	CleanupStack::Pop( self );
	
	__PRINTS( "CAdtUpdaterPopupList::NewL - Exit" );
	
	return self;
	}

// -----------------------------------------------------
// CAdtUpdaterPopupList::ActivateL
// rest of the details are commented in the header 
// -----------------------------------------------------
//                                    
void CAdtUpdaterPopupList::ActivateL()
	{
	__PRINTS( "CAdtUpdaterPopupList::ActivateL - Entry" );
	
	CCoeControl::ActivateL();
    // Add the listbox item change observer
    ListBox()->AddItemChangeObserverL( this );
    
    __PRINTS( "CAdtUpdaterPopupList::ActivateL - Exit" );
	}

// -----------------------------------------------------
// CAdtUpdaterPopupList::AdtU
// rest of the details are commented in the header 
// -----------------------------------------------------
//
void CAdtUpdaterPopupList::ListBoxItemsChanged( CEikListBox* aListBox )
	{
	__PRINTS( "CAdtUpdaterPopupList::ListBoxItemsChanged - Entry" );
	
	CEikButtonGroupContainer* cbaContainer = ButtonGroupContainer();
	
    // Check if there's no match of itmes
    if( !aListBox->Model()->NumberOfItems() )
        {
        // Disable the 'Select' button 
        cbaContainer->MakeCommandVisible( EAknSoftkeySelect, EFalse );
        // Disable the 'Middle softkey' button
        cbaContainer->MakeCommandVisibleByPosition( CEikButtonGroupContainer::EMiddleSoftkeyPosition,
        											EFalse );
        }
    // Check if 'Select' is disabled
    else if( !cbaContainer->IsCommandVisible( EAknSoftkeySelect ) ) 
        {
        // Enable the 'Select' button if disabled
        cbaContainer->MakeCommandVisible( EAknSoftkeySelect, ETrue );
        // Enable the 'Middle softkey' button if disabled
        cbaContainer->MakeCommandVisibleByPosition( CEikButtonGroupContainer::EMiddleSoftkeyPosition,
        											ETrue );
        }
    
    __PRINTS( "CAdtUpdaterPopupList::ListBoxItemsChanged - Exit" );
	}

//End of file
