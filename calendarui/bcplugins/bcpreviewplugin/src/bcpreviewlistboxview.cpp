/*
* Copyright (c) 2010-2010 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Class implementing the RECAL customisation interface.
* 
*/

#include <w32std.h>
#include <eiklbi.h> 
#include <AknsUtils.h>
#include <AknsDrawUtils.h>

#include "bcpreviewlistboxview.h"
#include "bcpreviewdefines.h"

// ----------------------------------------------------
// CBCPreviewListboxView::CBCPreviewListboxView
// ----------------------------------------------------
//
CBCPreviewListboxView::CBCPreviewListboxView()
	{
	TRACE_ENTRY_POINT;
	TRACE_EXIT_POINT;
	}

// ----------------------------------------------------
// CBCPreviewListboxView::~CBCPreviewListboxView
// ----------------------------------------------------
//
CBCPreviewListboxView::~CBCPreviewListboxView()
	{
	TRACE_ENTRY_POINT;
	TRACE_EXIT_POINT;
	}

// ----------------------------------------------------
// CBCPreviewListboxView::VScroll
// ----------------------------------------------------
//
void CBCPreviewListboxView::VScroll(TInt aVScrollAmount) 
	{
	TRACE_ENTRY_POINT;
	
	if (RedrawDisabled())
	    {
	    TRACE_EXIT_POINT;
		return;
	    }
	if (aVScrollAmount == 0)
	    {
	    TRACE_EXIT_POINT;
		return;
	    }
	
	TInt oldVerticalOffset = iVerticalOffset;
	iVerticalOffset -= aVScrollAmount;
	iVerticalTopOffset += aVScrollAmount;
	TInt actualScrollAmount = iVerticalOffset - oldVerticalOffset;
	iWin->Scroll(iViewRect, TPoint(0, -actualScrollAmount));
    
	iItemDrawer->Gc()->Clear( iViewRect );
	
	TInt itemHeight = iItemDrawer->ItemCellSize().iHeight;	
	TInt topItemIndex = TInt( Abs( iVerticalTopOffset ) / itemHeight );
	
	iWin->Invalidate(iViewRect);
	iWin->BeginRedraw(iViewRect);
	
	if (iPreviousItemTopIndex != topItemIndex)
		{
		if ( aVScrollAmount > 0 )	
			{
			iVerticalOffset = 0;
			}
		if ( aVScrollAmount < 0 )
			{
			iVerticalOffset -= itemHeight;
			}
		SetTopItemIndex( topItemIndex );
		iPreviousItemTopIndex = topItemIndex;
		Draw(&iViewRect);
		}
	
	iWin->EndRedraw();
	
	TRACE_EXIT_POINT;
	}

// ----------------------------------------------------
// CBCPreviewListboxView::ResetVScroll
// ----------------------------------------------------
//
void CBCPreviewListboxView::ResetVScroll()
	{
	iVerticalTopOffset = 0;
	iVerticalOffset = 0;
	iPreviousItemTopIndex = 0;
	
	SetCurrentItemIndex(0);
	SetTopItemIndex(0);
	}

// ----------------------------------------------------
// CBCPreviewListboxView::VerticalTopOffse
// ----------------------------------------------------
//
TInt CBCPreviewListboxView::VerticalTopOffset()
	{
	return iVerticalTopOffset;
	}

// ----------------------------------------------------
// CBCPreviewListboxView::NumberOfItemsThatFitInRect
// ----------------------------------------------------
//
TInt CBCPreviewListboxView::NumberOfItemsThatFitInRect(const TRect& aRect) const
	{
	if (iItemHeight == 0)
	    {
		return 0;
	    }
    TInt items = ( aRect.Height() - iVerticalOffset ) / iItemHeight;
	if ( ( iVerticalOffset != 0 ) || 
			( (aRect.Height() - iVerticalOffset) % iItemHeight > 0 ) ) items++;

	const_cast<TInt&>(iBottomItemIndex) = Max(0, iTopItemIndex + items - 1);
	
	return items;
	}

// ----------------------------------------------------
// CBCPreviewListboxView::SetBottomItemIndex
// ----------------------------------------------------
//
void CBCPreviewListboxView::SetBottomItemIndex(TInt aBottomItemIndex )
	{
	iBottomItemIndex = aBottomItemIndex; 
	}

// ----------------------------------------------------
// CBCPreviewListboxView::DrawEmptyList
// ----------------------------------------------------
//
void CBCPreviewListboxView::DrawEmptyList(const TRect &aClientRect) const
	{
    //custom color disabled, due to error in avkon listboxview
    //(if skins disabled, text is always black)
    
    //MAknsSkinInstance* skin = AknsUtils::SkinInstance();
	// text color
	//TRgb textColor(KRgbBlack);
	//TInt error = AknsUtils::GetCachedColor( skin, textColor, 
	//										KAknsIIDQsnTextColors,
	//										EAknsCIQsnTextColorsCG22);
	//if (!error)
	//	{
	//	// force our own text color
	//	CBCPreviewListboxView* ptr = 
	//		        const_cast<CBCPreviewListboxView*>( this );
	//
	//	ptr->SetTextColor( textColor );
	//	}

	// Disable AVKON skinning so that our own color definitions apply
	//TBool skinEnabled = AknsUtils::AvkonSkinEnabled();
	// Safe to ignore error
	//TRAP_IGNORE( AknsUtils::SetAvkonSkinEnabledL( EFalse ) ); 
	
	CFormattedCellListBoxView::DrawEmptyList( aClientRect );

	// Safe to ignore errors
    //TRAP_IGNORE( AknsUtils::SetAvkonSkinEnabledL( skinEnabled ) );
	}

// End of File
