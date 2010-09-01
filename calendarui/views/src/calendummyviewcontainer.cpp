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
* Description:  Container for missed alarm view's controls.
 *
*/


// System Includes
#include <AknsBasicBackgroundControlContext.h>
#include <AknsDrawUtils.h>
#include <AknUtils.h>
#include <aknlayoutscalable_apps.cdl.h>
#include <aknlists.h>
#include <AknIconArray.h>

// User Includes
#include "calendarui_debug.h"
#include "calendummyviewcontainer.h"
#include "AknsControlContext.h"

// -----------------------------------------------------------------------------
// CCalenDummyViewContainer::CCalenDummyViewContainer
// Performs the first phase of two phase construction.
// -----------------------------------------------------------------------------
//
CCalenDummyViewContainer::CCalenDummyViewContainer(CCalenNativeView* aView,
                         MCalenServices& aServices) :
    CCalenContainer(aView, aServices)
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenDummyViewContainer::~CCalenDummyViewContainer
// Destructor
// -----------------------------------------------------------------------------
//
CCalenDummyViewContainer::~CCalenDummyViewContainer()
    {
    TRACE_ENTRY_POINT;
    delete iBgContext;
    if(iListBoxItemArray)
        {
        delete iListBoxItemArray;
        }
    if (iListBox)
        {
        delete iListBox;
        iListBox = NULL;
        }

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenDummyViewContainer::ConstructImplL
// Third phase constructor.
// This function was called CCalenNativeView::ConstructL().
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenDummyViewContainer::ConstructImplL()
    {
    TRACE_ENTRY_POINT;
    
    TRect main_pane(  );
    SetRect( Rect() );
    
    iListBoxItemArray = new(ELeave) CDesCArrayFlat(1);
    
    iBgContext = CAknsBasicBackgroundControlContext::NewL( KAknsIIDQsnBgAreaMain,
                                                           Rect(),
                                                           ETrue );
    iListBox = new( ELeave ) CAknDoubleGraphicStyleListBox;
    iListBox->ConstructL( this, 0);
    iListBox->SetContainerWindowL( *this);
    
    // set the model array
    iListBox->Model()->SetItemTextArray(iListBoxItemArray);

    // set icon array
    CAknIconArray* iconArray = CreateIconsL( iIconIndices );
    CleanupStack::PushL(iconArray);
    iListBox->ItemDrawer()->FormattedCellData()->SetIconArray(iconArray);
    CleanupStack::Pop();

    iListBox->Model()->SetOwnershipType(ELbmDoesNotOwnItemArray);
    iListBox->View()->SetListEmptyTextL( KNullDesC );

    
    AddToStackAndMakeVisibleL();
    
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenDummyViewContainer::UpdateSize
// Do layout changes after calendar settings are
// changed.  No necessary actions for this view.
// ----------------------------------------------------------------------------
//
void CCalenDummyViewContainer::UpdateSize()
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenDummyViewContainer::CreateIconIndicesL
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenDummyViewContainer::CreateIconIndicesL(
                               RArray<MCalenServices::TCalenIcons>&  aIndexArray)
    {
    TRACE_ENTRY_POINT;
    aIndexArray.Reset();
    aIndexArray.AppendL( MCalenServices::ECalenBirthdayIcon );
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenDummyViewContainer::GetHelpContext
// Get help context.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenDummyViewContainer::GetHelpContext(TCoeHelpContext& /*aContext*/) const
    {
    TRACE_ENTRY_POINT;

    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenDummyViewContainer::MopSupplyObject
// Pass the skin information
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TTypeUid::Ptr CCalenDummyViewContainer::MopSupplyObject(TTypeUid aId)
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;

    return MAknsControlContext::SupplyMopObject(aId, iBgContext);
    }

// -----------------------------------------------------------------------------
// CCalenDummyViewContainer::SizeChanged
// Called by the framework when the view size is changed.
// -----------------------------------------------------------------------------
//
void CCalenDummyViewContainer::SizeChanged()
    {
    TRACE_ENTRY_POINT;
    TRect main_pane( Rect() );
    CCalenContainer::SizeChanged( main_pane );
    iServices.IssueNotificationL(ECalenNotifyCheckPluginUnloading);
    if(iListBox)
        {
        iListBox->SetRect( main_pane );
        }
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenDummyViewContainer::CountComponentControls
// Gets the number of controls contained in a compound control.
// -----------------------------------------------------------------------------
//
TInt CCalenDummyViewContainer::CountComponentControls() const
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;

    return 1;
    }

// -----------------------------------------------------------------------------
// CCalenDummyViewContainer::ComponentControl
// Gets the specified component of a compound control.
// -----------------------------------------------------------------------------
//
CCoeControl* CCalenDummyViewContainer::ComponentControl(TInt /*aIndex*/) const
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    if(iListBox)
        {
        return iListBox;
        }
    else
        {
        return NULL;
        }
    }

// -----------------------------------------------------------------------------
// CCalenDummyViewContainer::OfferKeyEventL
// Called by the framework when a key event needs to be processed. 
// -----------------------------------------------------------------------------
//
TKeyResponse CCalenDummyViewContainer::OfferKeyEventL(
        const TKeyEvent& /*aKeyEvent*/, TEventCode /*aType*/)
    {
    TRACE_ENTRY_POINT;

    TKeyResponse ret(EKeyWasNotConsumed);
  
    TRACE_EXIT_POINT;
    return ret;
    }

// -----------------------------------------------------------------------------
// CCalenDummyViewContainer::HandleResourceChange
// Handles a change to the application's resources which are shared across the environment. 
// -----------------------------------------------------------------------------
//
void CCalenDummyViewContainer::HandleResourceChange(TInt aType)
    {
    TRACE_ENTRY_POINT;
    
    CCalenContainer::HandleResourceChange( aType );

    if ( aType == KAknsMessageSkinChange || aType == KEikDynamicLayoutVariantSwitch )
        {
        SizeChanged();
        }
    CCoeControl::HandleResourceChange( aType );

    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenDummyViewContainer::Draw
// Draws the control
// -----------------------------------------------------------------------------
//
void CCalenDummyViewContainer::Draw(const TRect& /*aRect*/) const
    {
    TRACE_ENTRY_POINT;
    
    CWindowGc& gc = SystemGc();

    TRect mainPane;
    AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EMainPane, mainPane );
    TRect bgContextRect( TPoint(0, 0), mainPane.Size() );
    iBgContext->SetRect( bgContextRect);
    iBgContext->SetParentPos(mainPane.iTl);
    AknsDrawUtils::Background( AknsUtils::SkinInstance(), iBgContext, this, gc, Rect());
    
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenDummyViewContainer::HandlePointerEventL
// Handles the pointer events in touch resolutions
// -----------------------------------------------------------------------------
//
void CCalenDummyViewContainer::HandlePointerEventL(
        const TPointerEvent& /*aPointerEvent*/)
    {
    TRACE_ENTRY_POINT;
 
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenDummyViewContainer::HandleNaviDecoratorEventL
// ?implementation_description
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenDummyViewContainer::HandleNaviDecoratorEventL( TInt /*aEventID*/ )
    {
    TRACE_ENTRY_POINT;
    
    TRACE_EXIT_POINT;   
    }

// ----------------------------------------------------------------------------
// CCalenDummyViewContainer::HandleLongTapEventL
// processing of a long tap event
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenDummyViewContainer::HandleLongTapEventL( const TPoint& /*aPenEventLocation*/,
                                              const TPoint& /*aPenEventScreenLocation*/ )
    {
    TRACE_ENTRY_POINT;

    TRACE_EXIT_POINT;
    }

// End of File
