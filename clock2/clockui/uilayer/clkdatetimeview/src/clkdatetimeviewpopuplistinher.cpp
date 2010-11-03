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
* Description:   This is the source file for the CClkDateTimeViewPopupListInher class.
*
*/

// System includes
#include <aknsfld.h>
#include <eikbtgpc.h>

// User includes
#include "clkdatetimeviewpopuplistinher.h"

// Constants

// Literals

// ---------------------------------------------------------
// CClkDateTimeViewPopupListInher::NewL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
CClkDateTimeViewPopupListInher* CClkDateTimeViewPopupListInher::NewL( CAknSinglePopupMenuStyleListBox* aListBox,
                                                                      TInt aCbaResource,
                                                                      AknPopupLayouts::TAknPopupLayouts aType )
	{
	CClkDateTimeViewPopupListInher* self = new( ELeave ) CClkDateTimeViewPopupListInher();        
    CleanupStack::PushL( self );
    
    self->ConstructL( aListBox, aCbaResource, aType );
    
    CleanupStack::Pop( self );
    return self;
	}

// ---------------------------------------------------------
// CClkDateTimeViewPopupListInher::~CClkDateTimeViewPopupListInher
// rest of the details are commented in the header
// ---------------------------------------------------------
//
CClkDateTimeViewPopupListInher::~CClkDateTimeViewPopupListInher()
    {
    // No implementation yet.
    }    

// ---------------------------------------------------------
// CClkDateTimeViewPopupListInher::ActivateL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClkDateTimeViewPopupListInher::ActivateL()
	{
	// Call Base class ActivateL().
	CCoeControl::ActivateL(); 
	// Add the listbox item change observer.
	ListBox()->AddItemChangeObserverL( this ); 
 	}

// ---------------------------------------------------------
// CClkDateTimeViewPopupListInher::ListBoxItemsChanged
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClkDateTimeViewPopupListInher::ListBoxItemsChanged( CEikListBox* aListBox )
 	{
 	// Get the CBA button group container.
	CEikButtonGroupContainer* cbaContainer = ButtonGroupContainer();
	
	// Check if there's no match of countries.
	if( !aListBox->Model()->NumberOfItems() )
		{
		// Disable the 'Select' button.
		cbaContainer->MakeCommandVisible( EAknSoftkeySelect, EFalse );
		cbaContainer->MakeCommandVisibleByPosition(	CEikButtonGroupContainer::EMiddleSoftkeyPosition, EFalse );
		}
		
	// Check if 'Select' is disabled.
	else if( !cbaContainer->IsCommandVisible( EAknSoftkeySelect ) &&
	         !cbaContainer->IsCommandVisibleByPosition( CEikButtonGroupContainer::EMiddleSoftkeyPosition ) ) 
		{
		// Enable the 'Select' button if disabled.
		cbaContainer->MakeCommandVisible( EAknSoftkeySelect, ETrue );
		cbaContainer->MakeCommandVisibleByPosition(	CEikButtonGroupContainer::EMiddleSoftkeyPosition, ETrue );				
		}
 	}

// ---------------------------------------------------------
// CClkDateTimeViewPopupListInher::CClkDateTimeViewPopupListInher
// rest of the details are commented in the header
// ---------------------------------------------------------
//
CClkDateTimeViewPopupListInher::CClkDateTimeViewPopupListInher()
    {
    // No implementation yet.
    }

// ---------------------------------------------------------
// CClkDateTimeViewPopupListInher::ConstructL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClkDateTimeViewPopupListInher::ConstructL( CAknSinglePopupMenuStyleListBox* aListBox,	
                                                 TInt aCbaResource,
                                                 AknPopupLayouts::TAknPopupLayouts aType )
	{
	CAknPopupList::ConstructL( aListBox, aCbaResource,aType );
	}

// End of File
