/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Draws recurrence/alarm icons
*
*/



//debug
#include "calendarui_debug.h"

// User includes
#include "calenicondrawer.h"

#include <calenservices.h>
#include <CalenUid.h> 

// System includes
#include <fbs.h>
#include <eikapp.h>

// Layout includes
#include <aknlayoutscalable_apps.cdl.h>

#include <AknsConstants.h>
#include <AknsUtils.h>
#include <gulicon.h>
#include <avkon.mbg>
#include <gdi.h>
#include <fbs.h>

#include <AknIconArray.h>

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CCalenIconDrawer::NewL
// Two-phased constructor.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CCalenIconDrawer* CCalenIconDrawer::NewL( MCalenServices& aServices )
    {
    TRACE_ENTRY_POINT;

    CCalenIconDrawer* self = new (ELeave) CCalenIconDrawer( aServices );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    TRACE_EXIT_POINT;
    return self;
    }

// ---------------------------------------------------------------------------
// CCalenIconDrawer::ConstructL
// Second phase construction.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CCalenIconDrawer::ConstructL()
    {
    TRACE_ENTRY_POINT;

    // Create icons used by the viewer
    iIconIndices.AppendL( MCalenServices::ECalenAlarmIcon );
    iIconIndices.AppendL( MCalenServices::ECalenRepeatIcon );
    iIconIndices.AppendL( MCalenServices::ECalenRepeatExceptionIcon );
    iIconIndices.AppendL( MCalenServices::ECalenNotePriorityLow );
    iIconIndices.AppendL( MCalenServices::ECalenNotePriorityHigh );
    iIconIndices.AppendL( MCalenServices::ECalenMapIcon );

    iIconArray = CreateIconsL( iIconIndices );
    isMapIconAdded = EFalse;
    //SetIconSizesFromLayout();

    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------------------------
// CCalenIconDrawer::SetIconSizesFromLayout
// Calculate icon size from layout
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CCalenIconDrawer::SetIconSizesFromLayout(TInt aNumOfLinesBefLocField)
    {
    TRACE_ENTRY_POINT;

    TSize main_pane_size;
    AknLayoutUtils::LayoutMetricsSize (AknLayoutUtils::EMainPane, main_pane_size);
    TRect main_pane(main_pane_size);

    TAknLayoutRect main_cale_event_viewer_pane;
    main_cale_event_viewer_pane.LayoutRect(main_pane, AknLayoutScalable_Apps::main_cale_event_viewer_pane().LayoutLine() );

    TAknLayoutRect listscroll_cale_event_viewer_pane;
    listscroll_cale_event_viewer_pane.LayoutRect(main_cale_event_viewer_pane.Rect(), AknLayoutScalable_Apps::listscroll_cale_event_viewer_pane().LayoutLine() );

    TAknLayoutRect list_cale_ev2_pane;
    list_cale_ev2_pane.LayoutRect(listscroll_cale_event_viewer_pane.Rect(), AknLayoutScalable_Apps::list_cale_ev2_pane().LayoutLine() );

    TAknLayoutRect icon_layout_rect;
    icon_layout_rect.LayoutRect( list_cale_ev2_pane.Rect(), AknLayoutScalable_Apps::field_cale_ev2_pane_g1(1).LayoutLine() );
    iFirstIconRect = icon_layout_rect.Rect();

    icon_layout_rect.LayoutRect( list_cale_ev2_pane.Rect(), AknLayoutScalable_Apps::field_cale_ev2_pane_g2(1).LayoutLine() );
    iSecondIconRect = icon_layout_rect.Rect();

    icon_layout_rect.LayoutRect( list_cale_ev2_pane.Rect(), AknLayoutScalable_Apps::field_cale_ev2_pane_g3(2).LayoutLine() );
    iThirdIconRect = icon_layout_rect.Rect();
    
    // Adjust the map icon size
    if(isMapIconAdded)
        {
        TInt mapIconIndex = IconIndex(MCalenServices::ECalenMapIcon);
        TAknLayoutRect field_cale_ev2_pane;
        field_cale_ev2_pane.LayoutRect(list_cale_ev2_pane.Rect(), AknLayoutScalable_Apps::field_cale_ev2_pane(aNumOfLinesBefLocField, 0, 0).LayoutLine() );
            
        icon_layout_rect.LayoutRect( field_cale_ev2_pane.Rect(), AknLayoutScalable_Apps::field_cale_ev2_pane_g4(0).LayoutLine() );
        SetMapIconSize(icon_layout_rect.Rect());
        }
    AknIconUtils::SetSize( iIconArray->At(iFirstIconIndex)->Bitmap(), iFirstIconRect.Size() );
    AknIconUtils::SetSize( iIconArray->At(iSecondIconIndex)->Bitmap(), iSecondIconRect.Size() );
    AknIconUtils::SetSize( iIconArray->At(iThirdIconIndex)->Bitmap(), iThirdIconRect.Size() );

    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------------------------
// CCalenIconDrawer::SetMapIconSize
// Fuction to set map icon size
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CCalenIconDrawer::SetMapIconSize(TRect aIconRect)
	{
	TRACE_ENTRY_POINT;

	if(iIconIndices[iFirstIconIndex] == MCalenServices::ECalenMapIcon)
		{
		iFirstIconRect = aIconRect;
		iFirstIconRect.iBr.iX = iFirstIconRect.iTl.iX + 30;
		iFirstIconRect.iBr.iY = iFirstIconRect.iTl.iY + 30;	
		}
	else if(iIconIndices[iSecondIconIndex] == MCalenServices::ECalenMapIcon)
		{
		iSecondIconRect = aIconRect;
		iSecondIconRect.iBr.iX = iSecondIconRect.iTl.iX + 30;
		iSecondIconRect.iBr.iY = iSecondIconRect.iTl.iY + 30;	
		}
	else if(iIconIndices[iThirdIconIndex] == MCalenServices::ECalenMapIcon)
		{
		iThirdIconRect = aIconRect;
		iThirdIconRect.iBr.iX = iThirdIconRect.iTl.iX + 30;
		iThirdIconRect.iBr.iY = iThirdIconRect.iTl.iY + 30;	
		}
		
	TRACE_EXIT_POINT;	
	}
// ---------------------------------------------------------------------------
// CCalenIconDrawer::CCalenIconDrawer
// C++ constructor can NOT contain any code, that might leave.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
CCalenIconDrawer::CCalenIconDrawer( MCalenServices& aServices )
    : iServices( aServices )
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenIconDrawer::~CCalenIconDrawer
// Destructor.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CCalenIconDrawer::~CCalenIconDrawer()
    {
    TRACE_ENTRY_POINT;

    delete iIconArray;
    iIconIndices.Close();

    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenIconDrawer::Draw
// Draw icon function
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCalenIconDrawer::Draw(CGraphicsContext& aGc, const TPoint& /*aTopLeft*/,
                            const TRect& /*aClipRect*/, MGraphicsDeviceMap* /*aMap*/) const
    {
    TRACE_ENTRY_POINT;

    aGc.Reset();
    switch (iIconCount)
        {
        case EThreeIcons:
            aGc.DrawBitmapMasked(iThirdIconRect, iIconArray->At(iThirdIconIndex)->Bitmap(), iThirdIconRect.Size(), iIconArray->At(iThirdIconIndex)->Mask(), ETrue);
            //lint -fallthrough
        case ETwoIcons: /* fall through... */
            aGc.DrawBitmapMasked(iSecondIconRect, iIconArray->At(iSecondIconIndex)->Bitmap(), iSecondIconRect.Size(), iIconArray->At(iSecondIconIndex)->Mask(), ETrue);
            //lint -fallthrough
        case EOneIcon: /* fall through... */
            aGc.DrawBitmapMasked(iFirstIconRect, iIconArray->At(iFirstIconIndex)->Bitmap(), iFirstIconRect.Size(), iIconArray->At(iFirstIconIndex)->Mask(), ETrue);
            //lint -fallthrough
        case ENoIcons: /* fall through... */
        default:
            break;
        }

    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenIconDrawer::ExternalizeL
// Pure virtual from CPicture, intentionally empty.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCalenIconDrawer::ExternalizeL(RWriteStream& /*aStream*/) const
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenIconDrawer::GetOriginalSizeInTwips
// Convert size to twips
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCalenIconDrawer::GetOriginalSizeInTwips( TSize& /*aSize*/ ) const
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenIconDrawer::HandleResourceChange
// Handle resource change. e.g. screen resolution change
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCalenIconDrawer::HandleResourceChange()
    {
    TRACE_ENTRY_POINT;

    SetIconSizesFromLayout();

    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenIconDrawer::AddIconL
// Store icon index for reserved icon position
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCalenIconDrawer::AddIconL( MCalenServices::TCalenIcons aIconIndex)
    {
    TRACE_ENTRY_POINT;
    if(aIconIndex == MCalenServices::ECalenMapIcon)
        {
        isMapIconAdded = ETrue;
        }
    switch (iIconCount)
        {
        case ENoIcons:
            iFirstIconIndex = IconIndex( aIconIndex );
            User::LeaveIfError( iFirstIconIndex == KErrNotFound );
            iIconCount = EOneIcon;
            break;

        case EOneIcon:
            iSecondIconIndex = IconIndex( aIconIndex );
            User::LeaveIfError( iSecondIconIndex == KErrNotFound );
            iIconCount = ETwoIcons;
            break;

        case ETwoIcons:
            iThirdIconIndex = IconIndex( aIconIndex );
            User::LeaveIfError( iThirdIconIndex == KErrNotFound );
            iIconCount = EThreeIcons;
            break;

        case EThreeIcons:
            /* fall through... */
            User::Leave(KErrNotSupported);
            break;
        }

    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------
// CCalenIconDrawer::WidthInPixels
// Count width of all icons in pixel
// (other items were commented in a header).
// ---------------------------------------------------------
//
TInt CCalenIconDrawer::WidthInPixels()
    {
    TRACE_ENTRY_POINT;

    TInt widthInPixels = 0;
    if (iIconCount)
        {
        // All icons are the same size
        widthInPixels = iIconCount * iFirstIconRect.Size().iWidth;
        }

    TRACE_EXIT_POINT;
    return widthInPixels;
    }

// ----------------------------------------------------------------------------
// CCalenIconDrawer::CreateIconsL
// Create array of icons according to icon indices array given
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
CAknIconArray* CCalenIconDrawer::CreateIconsL( const RArray<MCalenServices::TCalenIcons>& aIndexArray )
    {
    TRACE_ENTRY_POINT;

    const TInt iconCount( aIndexArray.Count() );
    CAknIconArray* icons = new(ELeave) CAknIconArray( iconCount );
    CleanupStack::PushL( icons );
    icons->SetReserveL( iconCount );

    for( TInt i=0; i<iconCount; ++i )
        {
        icons->AppendL( iServices.GetIconL( static_cast<MCalenServices::TCalenIcons>(aIndexArray[i] ), KCalenMissedEventViewUidValue )); 
        }
    CleanupStack::Pop( icons );

    TRACE_EXIT_POINT;
    return icons;
    }

// ----------------------------------------------------------------------------
// CCalenContainer::IconIndex
// Get icon index of the icon array
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
TInt CCalenIconDrawer::IconIndex( MCalenServices::TCalenIcons aType ) const
    {
    TRACE_ENTRY_POINT;

    TInt index( iIconIndices.Find( aType ) );
   // User::LeaveIfError( index == KErrNotFound );

    TRACE_EXIT_POINT;
    return index;
    }

//  End of File
