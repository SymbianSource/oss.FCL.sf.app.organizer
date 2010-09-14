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
* Description: 
*
*/

#include <calenlocationutil.h>
#include <mnproviderfinder.h>
#include <aknPopup.h>
#include <eikenv.h>
#include <aknlists.h>
#include <StringLoader.h>
#include <Calendar.rsg>
#include "calendarui_debug.h"

_LIT(KNokiaVendorName, "Nokia gate5 GmbH");
const TInt KArrayGranularity = 5;

// ----------------------------------------------------------------------------
// CCalenLocationUtil::ShowLocationReplaceNoticeL
// API to tell claendar modules if Nokia maps provider is available in the device or not
// (other items were commented in a header).
// ----------------------------------------------------------------------------
EXPORT_C TBool CCalenLocationUtil::IsMapProviderAvailableL()
	{
	TBool providerAvailable = EFalse;
	RPointerArray<CMnProvider> providers;
	const CMnProvider::TServices neededServices = CMnProvider::EServiceNavigation |
    											  CMnProvider::EServiceMapView ; 
    MnProviderFinder::FindProvidersL( providers, neededServices );

    TInt count = providers.Count();

    for(TInt i = 0; i < count; i++)
	    {
	    TPtrC vendorName;
        providers[i]->GetVendorName(vendorName);
        if(vendorName.Compare(KNokiaVendorName) == 0)
	        {
	        // Found provider for Nokia maps
	        providerAvailable = ETrue;
	        break;	
	        }
	    }
    providers.ResetAndDestroy();
    return providerAvailable;	
	}

// ----------------------------------------------------------------------------
// CCalenLocationUtil::ShowLocationReplaceNoticeL
// Utility function to display list query with three strings.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
EXPORT_C TInt CCalenLocationUtil::ShowLocationAppendOrReplaceL(RPointerArray<HBufC>& aStrings)
    {
    TRACE_ENTRY_POINT;
        
    // Create listbox and PUSH it.
    CAknSinglePopupMenuStyleListBox* list = new(ELeave) CAknSinglePopupMenuStyleListBox;
    CleanupStack::PushL(list);
    
    // Create popup list and PUSH it.
    CAknPopupList* popupList = CAknPopupList::NewL(list, R_AVKON_SOFTKEYS_OK_CANCEL,
                                                    AknPopupLayouts::EMenuWindow);
    CleanupStack::PushL(popupList);
    
    // initialize listbox.
    list->ConstructL(popupList, CEikListBox::ELeftDownInViewRect);
    list->CreateScrollBarFrameL(ETrue);
    list->ScrollBarFrame()->SetScrollBarVisibilityL(CEikScrollBarFrame::EOff,
                                                    CEikScrollBarFrame::EAuto);
    
    // Enable Marquee for the list
    list->ItemDrawer()->FormattedCellData()->EnableMarqueeL( ETrue );
    CDesCArrayFlat* listItems = new( ELeave ) CDesCArrayFlat( KArrayGranularity );
    
    // Add the three strings to listitems
    HBufC* temp = aStrings[0];
    listItems->AppendL(*temp);
    temp = aStrings[1];
    listItems->AppendL(*temp);
    temp = aStrings[2];
    listItems->AppendL(*temp);
    temp = NULL;
    
    CTextListBoxModel* model = list->Model();
    model->SetItemTextArray(listItems);
    model->SetOwnershipType(ELbmOwnsItemArray);
    
    // Set title
    HBufC* title = NULL;
    title = CCoeEnv::Static()->AllocReadResourceLC( 
                                            R_CALEN_QTN_UPDATE_LOCATION_TITLE );
    popupList->SetTitleL(*title);
    CleanupStack::PopAndDestroy(title);
    // Show popup list.
    TInt popupOk = popupList->ExecuteLD();
    CleanupStack::Pop(popupList); 
    if (popupOk)
       {
       TInt index = list->CurrentItemIndex();
       CleanupStack::PopAndDestroy(list);
       return index;
       }
    else
        {
        CleanupStack::PopAndDestroy(list);
        return KErrCancel;
        }
    
    TRACE_EXIT_POINT;   
    }

// End Of File

