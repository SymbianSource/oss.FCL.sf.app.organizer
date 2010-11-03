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
* Description:   This class implements the function of common "Date & Time" settings dialog.
*
*/

// System includes
#include <e32base.h>
#include <csxhelp/clk.hlp.hrh>
#include <layoutmetadata.cdl.h>
#include <aknlayoutscalable_apps.cdl.h>

// User includes
#include "clkdatetimeviewcontainer.h"
#include "clock.h"
#include "clock.hrh"
#include "clkdatetimeview.hrh"
#include "clockappui.h"
#include "clkdatetimeview.h"

// Constants
const TInt KControlOne( 1 );

// ---------------------------------------------------------
// CClkDateTimeViewContainer::NewL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
CClkDateTimeViewContainer* CClkDateTimeViewContainer::NewL( CClkDateTimeView* aView )
    {
    CClkDateTimeViewContainer* self = new( ELeave ) CClkDateTimeViewContainer;
    CleanupStack::PushL( self );
    
    self->ConstructL( aView );
    
    CleanupStack::Pop( self );
    
    return self;
    }

// ---------------------------------------------------------
// CClkDateTimeViewContainer::~CClkDateTimeViewContainer
// rest of the details are commented in the header
// ---------------------------------------------------------
//
CClkDateTimeViewContainer::~CClkDateTimeViewContainer()
    {
    if( iListBox )
    	{
    	delete iListBox;
    	iListBox = NULL;
    	}
    }

// ---------------------------------------------------------
// CClkDateTimeViewContainer::OfferKeyEventL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
TKeyResponse CClkDateTimeViewContainer::OfferKeyEventL( const TKeyEvent& aKeyEvent, TEventCode aType )
    {
    TKeyResponse returnValue( EKeyWasNotConsumed );
    
    // We handle key events only for the event key.
    if( EEventKey == aType )
        {
        if( EStdKeyNo == aKeyEvent.iScanCode )
            {
            // This indicates that Call End key was pressed.
            // The application is hidden in the background.
            if( iView->IsClockInUse() )
                {
                // CClockAppUi should handle the command.
                iView->ClockApplicationUi()->HandleCommandL( EClkSettExitCmd );

                return EKeyWasConsumed;
                }
            }
        
        // For right arrow and left arrow, we don't need to do anything.
        if( EKeyLeftArrow == aKeyEvent.iCode || EKeyRightArrow == aKeyEvent.iCode )
            {
            if( iView->IsClockInUse() )
                {
                returnValue = EKeyWasConsumed;
                }
            }
        // For up/down arrow press, we update the list item focused.
        else if( iListBox )
            {
            if( EKeyUpArrow == aKeyEvent.iCode )
                {
                iView->DecCurrentItem();
                }
            else if( EKeyDownArrow == aKeyEvent.iCode )
                {
                iView->IncCurrentItem();
                }
            
            returnValue = iListBox->OfferKeyEventL( aKeyEvent, aType );
            }
        }
    return returnValue;
    }

// ---------------------------------------------------------
// CClkDateTimeViewContainer::GetHelpContext
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClkDateTimeViewContainer::GetHelpContext( TCoeHelpContext& aContext ) const
    {
    aContext.iMajor = KClockAppUid;
    aContext.iContext = KCLK_HLP_SETTINGS;
    }

// ---------------------------------------------------------
// CClkDateTimeViewContainer::CountComponentControls
// rest of the details are commented in the header
// ---------------------------------------------------------
//
TInt CClkDateTimeViewContainer::CountComponentControls() const
    {
    // We have only one control i.e the listbox.
    return KControlOne;
    }

// ---------------------------------------------------------
// CClkDateTimeViewContainer::ComponentControl
// rest of the details are commented in the header
// ---------------------------------------------------------
//
CCoeControl* CClkDateTimeViewContainer::ComponentControl( TInt aIndex ) const
    {
    switch( aIndex )
        {
        case 0:
			{
            return iListBox;
			}
        default:
			{
            return NULL;
			}
        }
    }

// ---------------------------------------------------------
// CClkDateTimeViewContainer::SizeChanged
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClkDateTimeViewContainer::SizeChanged()
    {
    if( iListBox )
        {
        iListBox->SetExtent( TPoint( FALSE, FALSE ), Rect().Size() );
        }
    }

// ---------------------------------------------------------
// CClkDateTimeViewContainer::FocusChanged
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClkDateTimeViewContainer::FocusChanged( TDrawNow /*aDrawNow*/ )
	{
	// If focus is changed, we allow the listbox to handle the same.
	if( iListBox )
		{
		iListBox->SetFocus( IsFocused() );
		}
	}

// ---------------------------------------------------------
// CClkDateTimeViewContainer::HandleResourceChange
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClkDateTimeViewContainer::HandleResourceChange( TInt aType )
	{
	// First the listbox and then the CCoeControl to handle the resource change.
	iListBox->HandleResourceChange( aType );
	CCoeControl::HandleResourceChange( aType );

	if( KEikDynamicLayoutVariantSwitch == aType )
		{
		TRect mainPaneRect;
		AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EMainPane, mainPaneRect );
		SetRect( mainPaneRect );
		} 
	}
	
// ---------------------------------------------------------
// CClkDateTimeViewContainer::HandlePointerEventL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClkDateTimeViewContainer::HandlePointerEventL( const TPointerEvent& aPointerEvent )
    {
    // Check if touch is enabled or not.
    if( !AknLayoutUtils::PenEnabled() )
        {
        return;
        }
    
    // Let the listbox handle the event.
    iListBox->HandlePointerEventL( aPointerEvent );
    }

// ---------------------------------------------------------
// CClkDateTimeViewContainer::ConstructL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClkDateTimeViewContainer::ConstructL( CClkDateTimeView* aView )
    {
    CreateWindowL();
    iView = aView;
    iTouchFlag = EFalse;
    }
   
// End of File
