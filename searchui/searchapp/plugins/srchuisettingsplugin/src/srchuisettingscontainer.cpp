/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:      This class implements the function of common
*                "Search UI" settings dialog.
*
*/






// INCLUDE FILES

#include "srchuisettingscontainer.h"
#include "srchuisettingsview.h"
#include <layoutmetadata.cdl.h>
#include <AknLayoutScalable_Apps.cdl.h>
#include <searchsettingspluginrsc.rsg>

// ================= MEMBER FUNCTIONS =======================


// ---------------------------------------------------------
// CSrchUiSettingsContainer::NewL
// Two-phased constructor.
// ---------------------------------------------------------
//
CSrchUiSettingsContainer* 
    CSrchUiSettingsContainer::NewL(CSrchUiSettingsView* aView)
    {
    CSrchUiSettingsContainer* self = new(ELeave) CSrchUiSettingsContainer;
    CleanupStack::PushL(self);
    self->ConstructL(aView);
    CleanupStack::Pop(self);
    return self;
    }
// ---------------------------------------------------------
// CSrchUiSettingsContainer::~CSrchUiSettingsContainer
// Destructor.
// ---------------------------------------------------------
// 
CSrchUiSettingsContainer::~CSrchUiSettingsContainer()
    {
    delete iListBox;
    }

// ---------------------------------------------------------
// CSrchUiSettingsContainer::OfferKeyEventL
// Handle key event.
// ---------------------------------------------------------
//
TKeyResponse CSrchUiSettingsContainer::OfferKeyEventL(
    const TKeyEvent& aKeyEvent, TEventCode aType)
    {    
    TKeyResponse ret(EKeyWasNotConsumed);
    
    if (EEventKey == aType)
        {
        if (EKeyLeftArrow == aKeyEvent.iCode ||
            EKeyRightArrow == aKeyEvent.iCode)
            {
            if (iView->IsSearchInUse())
                {
                ret = EKeyWasConsumed;
                }
            }
        else if (iListBox)
            {
            if (EKeyUpArrow == aKeyEvent.iCode)
                {
                iView->DecCurrentItem();
                }
            else if (EKeyDownArrow == aKeyEvent.iCode)
                {
                iView->IncCurrentItem();
                }
            ret = iListBox->OfferKeyEventL(aKeyEvent, aType);
            }
        }
    return ret;
    }
    
// ---------------------------------------------------------
// CSrchUiSettingsContainer::ConstructL()
// Symbian OS default constructor can leave.
// ---------------------------------------------------------
// 
void CSrchUiSettingsContainer::ConstructL(CSrchUiSettingsView* aView)
    {
    CreateWindowL();
    iView = aView;
    }

// ---------------------------------------------------------
// CSrchUiSettingsContainer::CountComponentControls() const
// Returns the number of components.
// ---------------------------------------------------------
//
TInt CSrchUiSettingsContainer::CountComponentControls() const
    {
    return 1;
    }

// ---------------------------------------------------------
// CSrchUiSettingsContainer::ComponentControl(TInt anIndex) const
// Returns a pointer of component.
// ---------------------------------------------------------
//
CCoeControl* CSrchUiSettingsContainer::ComponentControl(TInt anIndex) const
    {
    switch (anIndex)
        {
        case 0:
            return iListBox;
        default:
            return NULL;
        }
    }
 
// ---------------------------------------------------------
// CSrchUiSettingsContainer::SizeChanged()
// Handles a chage of client area size.
// ---------------------------------------------------------
//
void CSrchUiSettingsContainer::SizeChanged()
    {
    if (iListBox)
        {
        iListBox->SetExtent(TPoint(0,0), Rect().Size());
        }
    }

// ---------------------------------------------------------
// CSrchUiSettingsContainer::FocusChanged
// Handles the focus changed on the container.
// ---------------------------------------------------------
//
void CSrchUiSettingsContainer::FocusChanged(TDrawNow /*aDrawNow*/)
	{
	 if( iListBox )
        {
        iListBox->SetFocus( IsFocused() );
        }
	}

// ---------------------------------------------------------
// CSrchUiSettingsContainer::HandleResourceChange
// ---------------------------------------------------------
//
void CSrchUiSettingsContainer::HandleResourceChange(TInt aType)
	{
	iListBox->HandleResourceChange(aType);
	CCoeControl::HandleResourceChange(aType);

     if (aType == KEikDynamicLayoutVariantSwitch)
        {
        TRect mainPaneRect;
        AknLayoutUtils::LayoutMetricsRect(AknLayoutUtils::EMainPane,mainPaneRect);
        SetRect(mainPaneRect);
		} 
		
	TRAP_IGNORE(iView->SetCbaButtonsL( R_QTN_SOFTKEYS_OPTION_OPEN_BACK ));
	}
//  End of File
