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
* Description:  Customized drawing of CBCPreviewListBoxData.
*/

//  INCLUDE FILES
#include "bcpreviewlistboxitemdrawer.h"
#include "bcpreviewlistbox.h"

#include <layoutmetadata.cdl.h>
#include <aknlayoutscalable_apps.cdl.h>


namespace
	{
	const TInt KScrollBarWidth = 25; 
	}

// ================= MEMBER FUNCTIONS =======================

// C++ constructor can NOT contain any code, that
// might leave.
//
CBCPreviewListBoxItemDrawer::CBCPreviewListBoxItemDrawer( MTextListBoxModel* aTextListBoxModel,
                                                        const CFont* aFont,
                                                        CFormattedCellListBoxData* aFormattedCellData,
                                                        CBCPreviewListbox* aListBox )
    : CFormattedCellListBoxItemDrawer(aTextListBoxModel, aFont, aFormattedCellData ),
      iListBox( aListBox )
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    }

// Destructor
CBCPreviewListBoxItemDrawer::~CBCPreviewListBoxItemDrawer()
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------
// CBCPreviewListBoxItemDrawer::DrawItemText
// ---------------------------------------------------------
//
void CBCPreviewListBoxItemDrawer::DrawItemText ( TInt aItemIndex, // Item index
			const TRect& aItemTextRect,    	// Item rect
			TBool aItemIsCurrent,          	// Current item flag
			TBool aViewIsEmphasized,   		// Ignored
			TBool aItemIsSelected) const 	// Item highlighted flag
    {
    TRACE_ENTRY_POINT;
    
    TBool mirrored ( AknLayoutUtils::LayoutMirrored() );
    TAknLayoutRect stripeLayoutRect;
    // get stripe rect
    stripeLayoutRect.LayoutRect( aItemTextRect,
    		AknLayoutScalable_Apps::list_single_fp_cale_pane_g3(Layout_Meta_Data::IsLandscapeOrientation()).LayoutLine() );

    TRect itemRect ( aItemTextRect );
    TInt stripeWidth = stripeLayoutRect.Rect().Width();
    
    
    // should we draw stripe
    TRgb stripeColor = 0;             
    TBool colorDefined = iListBox->GetCurCalenColor( aItemIndex, stripeColor );
        
    if ( colorDefined )
    	{
    	// resize item's rect
		if ( mirrored )
			{
			itemRect.Move ( -stripeWidth, 0 );
			itemRect.iTl.iX += stripeWidth;
			}
		else
			{
			itemRect.Move ( stripeWidth, 0 );
			itemRect.iBr.iX -= stripeWidth;
			}
    	}

    UseSkinColors();
    // Disable AVKON skinning so that our own color definitions apply
    TBool skinEnabled = AknsUtils::AvkonSkinEnabled();
    
    // Safe to ignore error
    TRAP_IGNORE( AknsUtils::SetAvkonSkinEnabledL( EFalse ) ); 
    
	// draw item
	CFormattedCellListBoxItemDrawer::DrawItemText(aItemIndex, itemRect,
			aItemIsCurrent, aViewIsEmphasized, aItemIsSelected);
	
	// Safe to ignore error
	TRAP_IGNORE( AknsUtils::SetAvkonSkinEnabledL( skinEnabled ) );		


	//don't draw the stripe when there are more than one to-do events.
	if( colorDefined )    
    	{
		CCalEntry::TStatus status = iListBox->GetEventStatus( aItemIndex );
		DrawItemStripe( stripeLayoutRect.Rect(), stripeColor, status );
    	}
    
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------
// CBCPreviewListBoxItemDrawer::SetItemCellSize
// ---------------------------------------------------------
//
void CBCPreviewListBoxItemDrawer::SetItemCellSize(const TSize& aSizeInPixels)
	{
	TRACE_ENTRY_POINT;
	
	TSize cellSize ( aSizeInPixels );
	// cell can be wider when no scrollbar
	if  ( iListBox->ScrollBarFrame()->VScrollBarVisibility() == CEikScrollBarFrame::EOff  &&
			// because of different way of drawing lisbox in mirrored layout, this automatic 
			// resizing of item width when no scrollbar is supported only in western layout 
			!AknLayoutUtils::LayoutMirrored() )
		{
		cellSize.iWidth += KScrollBarWidth;
		CFormattedCellListBoxItemDrawer::SetItemCellSize ( cellSize );
		}
	else
		{
		CFormattedCellListBoxItemDrawer::SetItemCellSize ( cellSize );
		}
	
    TRACE_EXIT_POINT;
	}


// ---------------------------------------------------------
// CBCPreviewListBoxItemDrawer::SetSkinColors
// ---------------------------------------------------------
//
void CBCPreviewListBoxItemDrawer::UseSkinColors() const
	{
	// force our own text colors to item drawer
	CBCPreviewListBoxItemDrawer* ptr = 
			        const_cast<CBCPreviewListBoxItemDrawer*>( this );
	
	ptr->iTextColor = iTColor;
	ptr->iHighlightedTextColor = iHTColor;		
	}


// ---------------------------------------------------------
// CBCPreviewListBoxItemDrawer::DrawItemStripe
// ---------------------------------------------------------
//
void CBCPreviewListBoxItemDrawer::DrawItemStripe( const TRect& aRect,
			TRgb aColor, 
			CCalEntry::TStatus aStatus ) const
	{
	TInt stripeWidth( aRect.Width() );
	CGraphicsContext::TBrushStyle brushStyle = CGraphicsContext::ESolidBrush;
	TBool drawLines (EFalse);
	
	switch ( aStatus )
		{
		case CCalEntry::ECancelled:
			{
			// Stripe's rect is not filled, no diagonal lines
			brushStyle = CGraphicsContext::ENullBrush;
			break;
			}
		case CCalEntry::ETentative:
			{
			// Stripe's rect is not filled and diagonal
			// lines are drawn
			brushStyle = CGraphicsContext::ENullBrush;
			drawLines = ETrue;
			break;
			}
		default:
			{
			// Stripe's rect is solid, no diagonal lines
			break;
			}
		}
   	
	iGc->SetBrushStyle( brushStyle );
   	iGc->SetBrushColor( aColor );
   	iGc->SetPenColor( aColor );
   	iGc->DrawRect( aRect );
   	
   	if ( drawLines )
   		{
		TPoint rP( aRect.iTl );
		rP.iX += stripeWidth;
		
		TPoint lP( aRect.iTl );
		lP.iY += stripeWidth;
		TSize size (1,2);
		iGc->SetPenSize( size );
						
		while( lP.iY < aRect.iBr.iY )
			{
			iGc->DrawLine( lP, rP );
			lP.iY += stripeWidth;
			rP.iY += stripeWidth;
			}
   		}
   	
   	iGc->Reset();
   	
	}

// End of File
