/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: The model part of preivew pane plugin.
*
*/
// INCLUDE FILES
#include <eiklabel.h>
#include <AknUtils.h>
#include <layoutmetadata.cdl.h>
#include <aknlayoutscalable_apps.cdl.h>
#include <aknlayoutscalable_avkon.cdl.h>
#include <AknsBasicBackgroundControlContext.h>
#include <AknsDrawUtils.h>
#include <AknsControlContext.h>
#include <w32std.h>
#include <AknsFrameBackgroundControlContext.h>
#include <eikclbd.h>
#include <eikcolib.h>
#include <akntoolbar.h>
#include <calenservices.h>
#include <calencommands.hrh>

#include "bcpreviewpanecontainer.h"
#include "bcpreviewpanedata.h"
#include "bcpreviewlistbox.h"
#include "bcpreviewlistboxview.h"
#include "bcpreviewlistboxitemdrawer.h"
#include "bcpreviewlongtaphandler.h"

// The scrolling speed. This is the number of milliseconds to reserve 
// per pixel to be scrolled.
static const TInt KPreviewScrollMsecPerPixelInterval = 35000; // 35ms
// The delay from the start of auto scroll function.
static const TInt KPreviewScrollDelay = 500000; // 0.5s

// --------------------------------------------------------------------------
// CBCPreviewPaneContainer::NewL
// --------------------------------------------------------------------------
//
CBCPreviewPaneContainer* CBCPreviewPaneContainer::NewL(
                                        MCalenServices& aServices,
                                        const TRect& aRect )
    {
    TRACE_ENTRY_POINT;
    CBCPreviewPaneContainer* self =
        new( ELeave ) CBCPreviewPaneContainer( aServices, aRect );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    TRACE_EXIT_POINT;
    }

// --------------------------------------------------------------------------
// CBCPreviewPaneContainer::CBCPreviewPaneContainer
// --------------------------------------------------------------------------
//
CBCPreviewPaneContainer::CBCPreviewPaneContainer(
                                        MCalenServices& aServices,
                                        const TRect& aRect )
    : iPreviewRect( aRect ),
      iServices( aServices )
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    }

// --------------------------------------------------------------------------
// CBCPreviewPaneContainer::~CBCPreviewPaneContainer
// --------------------------------------------------------------------------
//
CBCPreviewPaneContainer::~CBCPreviewPaneContainer()
    {
    TRACE_ENTRY_POINT;

    iServices.CancelNotifications( this );
    
    ResetComponents();
    delete iPreviewData;
    delete iBgContext;
    delete iBackgroundSkinContext;
    delete iGestureControl;
	
	if ( iAutoScrollTimer )
		{
		// Calling Cancel without checking if the timer is active is safe
		iAutoScrollTimer->Cancel();
		}
	delete iAutoScrollTimer;
	
	TRACE_EXIT_POINT;
	}

// --------------------------------------------------------------------------
// CBCPreviewPaneContainer::ConstructL
// --------------------------------------------------------------------------
//
void CBCPreviewPaneContainer::ConstructL()
    {
    TRACE_ENTRY_POINT;
    CreateWindowL();


    iPreviewData = CBCPreviewPaneData::NewL( iServices);

    InitComponentArrayL();
    
    iBgContext = CAknsBasicBackgroundControlContext::NewL( KAknsIIDQsnBgAreaMain,
															iPreviewRect, ETrue );
    
    iBackgroundSkinContext = CAknsFrameBackgroundControlContext::NewL(
                    KAknsIIDQsnFrCale, TRect(0,0,0,0), TRect(0,0,0,0), EFalse );

    TSize screenSize (iCoeEnv->ScreenDevice()->SizeInPixels());
    iIsVGA = (screenSize.iWidth == 640 && screenSize.iHeight == 480);
    
    CreateListboxL();
    CreateLabelL();
    InitComponentsL();

    SetComponentsToInheritVisibility(ETrue);

    SetRect( iPreviewRect );
  
    iGestureControl = CGestureHelper::NewL( *this );
    
	// Scrolling starts from the top of the list to the bottom.
	iPreviewScrollDirection = EScrollDown;
	// Initialize the periodic timer.
	iAutoScrollTimer = CPeriodic::NewL(CActive::EPriorityIdle);
    RArray<TCalenNotification> notifications;
    CleanupClosePushL( notifications );
    notifications.AppendL(ECalenNotifyAppForegrounded);
    notifications.AppendL(ECalenNotifyAppBackgrounded);
    notifications.AppendL(ECalenNotifyResourceChanged);
    
    iServices.RegisterForNotificationsL(this, notifications);
	
    CleanupStack::PopAndDestroy(&notifications);
    ActivateL();
    TRACE_EXIT_POINT
    }

// --------------------------------------------------------------------------
// CBCPreviewPaneContainer::SizeChanged
// --------------------------------------------------------------------------
//
void CBCPreviewPaneContainer::SizeChanged()
    {
    TRACE_ENTRY_POINT;
    TRect rect( Rect() );
    if( iBgContext )
        {
        iBgContext->SetRect(rect);
        }
    
    TAknLayoutRect cale_bg_pane_center;
    cale_bg_pane_center.LayoutRect(rect, AknLayoutScalable_Apps::cale_bg_pane_g1().LayoutLine() );

    iBackgroundSkinContext->SetFrameRects(rect, cale_bg_pane_center.Rect() );
    iBackgroundSkinContext->SetParentContext( iBgContext );

    // Listbox layout
    TAknLayoutRect list_cale_pane;
    list_cale_pane.LayoutRect(rect, AknLayoutScalable_Apps::list_fp_cale_pane( Layout_Meta_Data::IsLandscapeOrientation()).LayoutLine() );
    
    if ( iLabelControl )
    	{	
		iLabelControl->SetRect( LabelRect() );
		
        TRect listRect = cale_bg_pane_center.Rect();
        listRect.iTl.iY += LabelRect().Height();
        iListbox->SetRect( listRect );
    	}
    else
    	{
		iListbox->SetRect( cale_bg_pane_center.Rect() );
    	}
   // This is workaround for small VGA screens.
   // Fonts need to be smaller to display more information. 
    if(iIsVGA)
    	{
    	OverrideListBoxFontsForSmallScreen();
    	}
    
    TRACE_EXIT_POINT
    }

// --------------------------------------------------------------------------
// CBCPreviewPaneContainer::HandleNotification
// --------------------------------------------------------------------------
//
void CBCPreviewPaneContainer::HandleNotification(
        const TCalenNotification aNotification )
    {
    TRACE_ENTRY_POINT;
    
    switch ( aNotification )
    	{
    	case ECalenNotifyAppBackgrounded:
    		StopAutoScroll();
    		break;
    		
    	case ECalenNotifyAppForegrounded:
    		StartAutoScroll();
    		break;
			
    	case ECalenNotifyResourceChanged:
    		HandleResourceChange( KEikDynamicLayoutVariantSwitch );
    		break;
    		
    	default:
    		break;
    	}
    
    TRACE_EXIT_POINT;
    }

// --------------------------------------------------------------------------
// CBCPreviewPaneContainer::ResetComponents
// --------------------------------------------------------------------------
//
void CBCPreviewPaneContainer::ResetComponents()
    {
    TRACE_ENTRY_POINT;
    if ( iLabelControl )
    	{
		Components().Remove ( iLabelControl );
	    iLabelControl = NULL;
    	}
    Components().ResetAndDestroy();
    iListbox = NULL;
    TRACE_EXIT_POINT
    }

// --------------------------------------------------------------------------
// CBCPreviewPaneContainer::InitComponentsL
// --------------------------------------------------------------------------
//
void CBCPreviewPaneContainer::InitComponentsL()
    {
    TRACE_ENTRY_POINT;
    Components().AppendLC( iListbox );
    CleanupStack::Pop( iListbox );
    TRACE_EXIT_POINT
    }


// --------------------------------------------------------------------------
// CBCPreviewPaneContainer::CreateListboxL
// --------------------------------------------------------------------------
//
void CBCPreviewPaneContainer::CreateListboxL()
    {
    TRACE_ENTRY_POINT;
    iListbox = CBCPreviewListbox::NewL( this, iServices, iPreviewData );
    iListbox->CreateScrollBarFrameL(ETrue);
    iListbox->ScrollBarFrame()->SetScrollBarVisibilityL(
			CEikScrollBarFrame::EOff, CEikScrollBarFrame::EOff);
    iListbox->InitPreviewListboxL(iIsVGA);
    iListbox->EnableStretching( EFalse );
	// Make listbox invisible to avoid flickering.
	// Visibility is restored in CBCPreviewListbox::UpdateListBoxL
    iListbox->MakeVisible( EFalse );
    TRACE_EXIT_POINT
    }

// --------------------------------------------------------------------------
// CBCPreviewPaneContainer::CreateLabelL
// --------------------------------------------------------------------------
//
void CBCPreviewPaneContainer::CreateLabelL()
	{
	TRACE_ENTRY_POINT;

	if ( iServices.Infobar() != KNullDesC )
    	{
		if ( iLabelControl )
    		{
			Components().Remove ( iLabelControl );
    		}
		TRect nullRect (0, 0, 0, 0);
		iLabelControl = iServices.Infobar( nullRect );
		if ( iLabelControl )
			{
			iLabelControl->SetContainerWindowL( *this );
			
			Components().AppendLC( iLabelControl );
			CleanupStack::Pop( iLabelControl );
			}
    	} 
	//fix for disabling lunar info in settings
	else if( iLabelControl )
	        {
	        Components().Remove ( iLabelControl );
	        iLabelControl = NULL;
	        }

    
	TRACE_EXIT_POINT
	}

// --------------------------------------------------------------------------
// CBCPreviewPaneContainer::HandleResourceChange
// --------------------------------------------------------------------------
//
void CBCPreviewPaneContainer::HandleResourceChange( TInt aType )
    {
    TRACE_ENTRY_POINT;

    if( aType == KEikDynamicLayoutVariantSwitch )
        {
		iPreviewRect = PreviewPaneRect();
		SetRect( iPreviewRect );

		iBgContext->SetRect( Rect() );
       }
    TRACE_EXIT_POINT
    }

// --------------------------------------------------------------------------
// CBCPreviewPaneContainer::Draw
// --------------------------------------------------------------------------
//
void CBCPreviewPaneContainer::Draw( const TRect& /*aRect*/ ) const
    {
    TRACE_ENTRY_POINT;
    CWindowGc& gc = SystemGc();

    MAknsControlContext* cc =AknsDrawUtils::ControlContext( this );

    if( cc )
        {
        TRect main_pane( Rect() );

        // To prevent "tearing" (tearing resembles flickering), we don't want to clear
        // area under bottom-most skin image (KAknsDrawParamNoClearUnderImage).
        // But if bottom-most skin is semitransparent, we need to draw it without alpha channel,
        // so that something else doesn't show through it (KAknsDrawParamBottomLevelRGBOnly).
        const TInt drawParams = KAknsDrawParamNoClearUnderImage | KAknsDrawParamBottomLevelRGBOnly;
        AknsDrawUtils::Background( AknsUtils::SkinInstance(),
                                   cc,
                                   NULL,
                                   gc,
                                   main_pane,
                                   drawParams);
        }
    TRACE_EXIT_POINT
    }

// --------------------------------------------------------------------------
// CBCPreviewPaneContainer::FocusChangedL
// --------------------------------------------------------------------------
//
void CBCPreviewPaneContainer::FocusChangedL( const TTime& /*aTime*/ )
    {
    TRACE_ENTRY_POINT;
    // Timed cell focused.
    DoFocusChangedL();
    TRACE_EXIT_POINT
    }

// --------------------------------------------------------------------------
// CBCPreviewPaneContainer::FocusChangedL
// --------------------------------------------------------------------------
//
void CBCPreviewPaneContainer::FocusChangedL( CCalInstance* /*aInstance*/ )
    {
    TRACE_ENTRY_POINT;
    // Non-timed cell focused.
    DoFocusChangedL();
    TRACE_EXIT_POINT
    }

// --------------------------------------------------------------------------
// CBCPreviewPaneContainer::StartAutoScroll
// --------------------------------------------------------------------------
//
void CBCPreviewPaneContainer::StartAutoScroll()
	{
	if (!iAutoScrollTimer->IsActive() && ScrollDistance() > 0)
		{
		iAutoScrollTimer->Start(
			KPreviewScrollDelay, KPreviewScrollMsecPerPixelInterval, 
			TCallBack(AutoScrollTimerCallBack, this));
		}
	}

// --------------------------------------------------------------------------
// CBCPreviewPaneContainer::StopAutoScroll
// --------------------------------------------------------------------------
//
void CBCPreviewPaneContainer::StopAutoScroll()
	{
	//It is okay to call Cancel without checking if the timer is active.
	iAutoScrollTimer->Cancel();
	iIsAutoScrollRunning = EFalse;
	}

// --------------------------------------------------------------------------
// CBCPreviewPaneContainer::IsAutoScrollRunning
// --------------------------------------------------------------------------
//
TBool CBCPreviewPaneContainer::IsAutoScrollRunning() const
	{
	return (iIsAutoScrollRunning && iAutoScrollTimer->IsActive());
	}

// --------------------------------------------------------------------------
// CBCPreviewPaneContainer::Hide
// --------------------------------------------------------------------------
//
void CBCPreviewPaneContainer::Hide()
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT
    }

// --------------------------------------------------------------------------
// CBCPreviewPaneContainer::MopSupplyObject
// --------------------------------------------------------------------------
//
TTypeUid::Ptr CBCPreviewPaneContainer::MopSupplyObject( TTypeUid aId )
    {
    TRACE_ENTRY_POINT;
    if(aId.iUid == MAknsControlContext::ETypeId )
    	{
    	MAknsControlContext* cc = iBackgroundSkinContext;
    	if ( !cc )
    		{
    		cc = iBgContext;
    		}

    	if ( cc )
    		{
    		TRACE_EXIT_POINT;
    		return MAknsControlContext::SupplyMopObject( aId, cc );
    		}
        }

    return CCoeControl::MopSupplyObject( aId );
    TRACE_EXIT_POINT;
    }


// --------------------------------------------------------------------------
// CBCPreviewPaneContainer::HandlePointerEventL
// --------------------------------------------------------------------------
//
void CBCPreviewPaneContainer::HandlePointerEventL(
        const TPointerEvent& aPointerEvent)
	{
    CPreviewLongTapHandler* longTapHandler = iListbox->LongTapHandler();
    if( longTapHandler )
        {
        if( !longTapHandler->MenuShown() )
            {
            iGestureControl->HandlePointerEventL( aPointerEvent );            
            }
        }
    else
        {
        iGestureControl->HandlePointerEventL( aPointerEvent );
        }

    if ( iLabelControl && iLabelControl->Rect().Contains( aPointerEvent.iPosition ))
        {
        iLabelControl->HandlePointerEventL(aPointerEvent);
        }
    else if ( !IsAutoScrollRunning() )
		{
		iListbox->HandlePointerEventL(aPointerEvent);
		}
	

	}

// --------------------------------------------------------------------------
// CBCPreviewPaneContainer::HandleGestureL
// --------------------------------------------------------------------------
//
void CBCPreviewPaneContainer::HandleGestureL( 
		const GestureHelper::MGestureEvent& aEvent )
	{
	TBool mirrored( AknLayoutUtils::LayoutMirrored() );
	
	switch( aEvent.Code( MGestureEvent::EAxisBoth ) )
		{
		case EGestureStart:
			{
			iGestureHandled = EFalse;
			break;
			}
		case EGestureSwipeRight:
			{		
			iServices.IssueCommandL( mirrored ? ECalenGotoNextDay 
                                        : ECalenGotoPrevDay );
			iGestureHandled = ETrue;
			break;
			}
		case EGestureSwipeLeft:
		    {
			iServices.IssueCommandL( mirrored ? ECalenGotoPrevDay
                                        : ECalenGotoNextDay );
			iGestureHandled = ETrue;
			break;
		
			}
		case EGestureTap:
			{
			if ( IsAutoScrollRunning() )
				{
				// User has touched preview pane so stop auto-scrolling and show 
				// scrollbars to enable kinetic scrolling.
				// IMPORTANT! In Avkon listbox implementation kinetic scrolling is disabled 
				// if scrollbar is not visible!
				StopAutoScroll();
				
				iListbox->ScrollBarFrame()->SetScrollBarVisibilityL(
							CEikScrollBarFrame::EOff, CEikScrollBarFrame::EOn);

				CBCPreviewListboxView* listboxView =
						(CBCPreviewListboxView*) iListbox->View();
				
				TInt topItemIndex = listboxView->TopItemIndex();
				TInt bottomItemIndex = listboxView->BottomItemIndex();
				TInt verticalOffset = listboxView->ItemOffsetInPixels();
				
				iListbox->SizeChanged();
				if(iIsVGA)
					{
					OverrideListBoxFontsForSmallScreen();
					}
				
				listboxView->SetTopItemIndex( topItemIndex );
				listboxView->SetBottomItemIndex( bottomItemIndex );
				listboxView->SetItemOffsetInPixels( verticalOffset );
				
				iListbox->UpdateScrollBarsL();
				iListbox->DrawDeferred();

				iGestureHandled = ETrue;
				}
			}
		default:
			break;	
		}
	
	iListbox->SetGestureHandled( iGestureHandled );
	}

// --------------------------------------------------------------------------
// CBCPreviewPaneContainer::DoPreviewAutoScroll
// --------------------------------------------------------------------------
//
void CBCPreviewPaneContainer::DoPreviewAutoScroll()
	{
	TRACE_ENTRY_POINT;
	
	iIsAutoScrollRunning = ETrue;

	TInt distance = ScrollDistance();
	if (distance > 0)
		{
		CBCPreviewListboxView* listboxView =
				(CBCPreviewListboxView*) iListbox->View();
	
		switch (iPreviewScrollDirection)
			{
			case EScrollDown:
				// Scroll list box vertically to the bottom of the listbox.
				listboxView->VScroll(1);
				if (listboxView->VerticalTopOffset() == distance)
					{
					iPreviewScrollDirection = EScrollUp;
					}
				break;

			case EScrollUp:
				// Scroll list box vertically to the top of the listbox.
				listboxView->VScroll(-1);
				if (listboxView->VerticalTopOffset() == 0)
					{
					iPreviewScrollDirection = EScrollDown;
					}
				break;

			default:
				break;
			}
		}

	TRACE_EXIT_POINT;
	}

// --------------------------------------------------------------------------
// CBCPreviewPaneContainer::AutoScrollTimerCallBack
// --------------------------------------------------------------------------
//
TInt CBCPreviewPaneContainer::AutoScrollTimerCallBack(TAny* aAny)
	{
	CBCPreviewPaneContainer* self = static_cast<CBCPreviewPaneContainer*>(aAny);
	// Do preview pane auto scroll.	 
	self->DoPreviewAutoScroll();

	return KErrNone;
	}

// --------------------------------------------------------------------------
// CBCPreviewPaneContainer::DoFocusChangedL
// --------------------------------------------------------------------------
//
void CBCPreviewPaneContainer::DoFocusChangedL()
	{
	TRACE_ENTRY_POINT;
	
	// When changing focus, autoscroll should be stopped, scrollbars should be 
	// disabled, listbox should be updated and after all autoscroll should be
	// again started from the top of the list.
	
	if(iListbox)
		{
		SetSkinColors();
		StopAutoScroll();
		
		// Disable scrollbars.
		if ( iListbox->ScrollBarFrame()->VerticalScrollBar()->IsVisible() )
			{
			iListbox->ScrollBarFrame()->SetScrollBarVisibilityL(
						CEikScrollBarFrame::EOff, CEikScrollBarFrame::EOff);
			iListbox->UpdateScrollBarsL();
			}
		
		((CBCPreviewListboxView*) iListbox->View())->ResetVScroll();
		iPreviewScrollDirection = EScrollDown;
		iListbox->UpdateListBoxL();
		StartAutoScroll();
		}

	CreateLabelL();
	
	DrawDeferred();
	SizeChanged();
	TRACE_EXIT_POINT
	}

// --------------------------------------------------------------------------
// CBCPreviewPaneContainer::PreviewPaneRect
// --------------------------------------------------------------------------
//
TRect CBCPreviewPaneContainer::PreviewPaneRect()
	{
	// Get the screen rect
	TRect screenRect;
	AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EScreen, screenRect );

//	   // Get the application window rect
//	   TAknLayoutRect application_window;
//	   application_window.LayoutRect(
//	       screenRect,
//	       AknLayoutScalable_Apps::application_window().LayoutLine() );

	// Get the preview pane rect
	TAknLayoutRect popup_fixed_preview_cale_window;
	popup_fixed_preview_cale_window.LayoutRect(
	   screenRect,
	   AknLayoutScalable_Apps::popup_fixed_preview_cale_window(
	   Layout_Meta_Data::IsLandscapeOrientation()).LayoutLine() );
	
	
	return (popup_fixed_preview_cale_window.Rect());
	}

// --------------------------------------------------------------------------
// CBCPreviewPaneContainer::LabelRect
// --------------------------------------------------------------------------
//
TRect CBCPreviewPaneContainer::LabelRect()
	{
	const TInt KLabelHeightFactor = 7;
    TRect rect( Rect() );
    TAknLayoutRect cale_bg_pane_center;
    cale_bg_pane_center.LayoutRect(rect, AknLayoutScalable_Apps::cale_bg_pane_g1().LayoutLine() );
    
	rect = cale_bg_pane_center.Rect();
    rect.SetHeight( rect.Height() / KLabelHeightFactor );
	
    return rect;
	}


// --------------------------------------------------------------------------
// CBCPreviewPaneContainer::ScrollDistance
// --------------------------------------------------------------------------
//
TInt CBCPreviewPaneContainer::ScrollDistance() const
	{
	CBCPreviewListboxView* listboxView =
				(CBCPreviewListboxView*) iListbox->View();
	TInt numOfItems = iListbox->Model()->NumberOfItems();
	
	// Calculate the total height of the content
	TInt contentHeight = iListbox->ItemHeight() * numOfItems;
	TInt viewportHeight = listboxView->ViewRect().Height();

	// Calculate how much content is clipped outside of the viewport.
	// This is the distance that the viewport origin will move during 
	// scrolling.
	return contentHeight - viewportHeight;
	}


// ---------------------------------------------------------
// CBCPreviewPaneContainer::SetSkinColors
// ---------------------------------------------------------
//
void CBCPreviewPaneContainer::SetSkinColors()
	{
	//Instead of default lisbox text colors
	//EAknsCIQsnTextColorsCG22 and EAknsCIQsnTextColorsCG10
	//are used as a text colors. The same IDs are used for text's in ReCal's views
	
	// force our own text colors to item drawer
	CBCPreviewListBoxItemDrawer* drawer =
			static_cast<CBCPreviewListBoxItemDrawer*>( iListbox->ItemDrawer() );
	
	
	MAknsSkinInstance* skin = AknsUtils::SkinInstance();
	// text color
	TRgb textColor(KRgbBlack);
	TInt error = AknsUtils::GetCachedColor( skin, textColor, 
											KAknsIIDQsnTextColors,
											EAknsCIQsnTextColorsCG22);
	if(!error)
		{
		drawer->SetSkinColors( textColor, KRgbWhite );
		}
	
	// highlighted text color
	TRgb hTextColor(KRgbWhite);
	error = AknsUtils::GetCachedColor( skin, hTextColor, 
											 KAknsIIDQsnTextColors,
											 EAknsCIQsnTextColorsCG10);
	if(!error)
		{
		drawer->SetSkinColors( textColor, hTextColor );
		}
	}

// ---------------------------------------------------------
// CBCPreviewPaneContainer::OverrideListBoxFontsForSmallScreen
// ---------------------------------------------------------
//
void CBCPreviewPaneContainer::OverrideListBoxFontsForSmallScreen()
	{
    const CFont* pFont  = AknLayoutUtils::FontFromId(EAknLogicalFontPrimarySmallFont);        
  	
    if(pFont)
    	{
		TFontSpec fontSpec = pFont->FontSpecInTwips();
		fontSpec.iHeight = 90;
		
		CFont* fontNew(NULL);
		iCoeEnv->ScreenDevice()->GetNearestFontInTwips( ( CFont*&)fontNew, fontSpec );
		
		CFormattedCellListBoxData *pLBData(NULL);
		pLBData = iListbox->ItemDrawer()->ColumnData();
		
		if(pLBData)
			{
			pLBData->SetSubCellFontL(1,fontNew);
			pLBData->SetSubCellFontL(2,fontNew);
			}
    	}
	}
//End of file
