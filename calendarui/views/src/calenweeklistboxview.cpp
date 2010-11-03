/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   CCalenWeekLBView customized key action of
 *                CFormattedCellListBoxView, and customized scrolling
 *                for event indicator.
 *
*/


//debug
#include "calendarui_debug.h"

//  INCLUDE FILES
#include "calenweeklistboxview.h"
#include "calenweeklistboxdata.h"


#include <AknUtils.h>


//  LOCAL CONSTANTS AND MACROS
#define ITEM_EXISTS(x) (((x) > -1) && ((x) < iModel->NumberOfItems()))


// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code, that
// might leave.
//
CCalenWeekLBView::CCalenWeekLBView()
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    }

// Destructor
CCalenWeekLBView::~CCalenWeekLBView()
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------
// CCalenWeekLBView::SetColumn
// Sets current Column
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCalenWeekLBView::SetColumn( TInt aColumn, TBool aRedraw)
    {
    TRACE_ENTRY_POINT;
    
    iColumn = aColumn;
    CFormattedCellListBoxItemDrawer* drawer =
        static_cast<CFormattedCellListBoxItemDrawer*>( iItemDrawer );
    CCalenWeekLBData* data =
        static_cast<CCalenWeekLBData*>( drawer->FormattedCellData() );

    data->SetHilightedSubCell(iColumn);
    // Resize highlight size (which is used by highlight animation)
    TSize size(data->SubCellSize(iColumn + 1));
    data->SetItemCellSize( size );

    if (aRedraw && ItemIsVisible(iCurrentItemIndex))
        {
        HideMatcherCursor();

        TRect redrawRect(ItemPos(iCurrentItemIndex),
                         ItemSize(iCurrentItemIndex));
        iWin->Invalidate(redrawRect);
        iWin->BeginRedraw(redrawRect);
        DrawItem(iCurrentItemIndex);
        iWin->EndRedraw();
        DrawMatcherCursor();
        }
    
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------
// CCalenWeekLBView::Column
// Gets current Column
// (other items were commented in a header).
// ---------------------------------------------------------
//
TInt CCalenWeekLBView::Column() const
    {
    TRACE_ENTRY_POINT;
    
    TRACE_EXIT_POINT;
    return iColumn;
    }

// ---------------------------------------------------------
// CCalenWeekLBView::DrawHighlightAnimBackground
// Callback for drawing highlight background to highlight animation input
// layer.
// ---------------------------------------------------------
//
TBool CCalenWeekLBView::DrawHighlightAnimBackground( CFbsBitGc& aGc ) const
    {
    TRACE_ENTRY_POINT;
    
    CFormattedCellListBoxItemDrawer* drawer =
        static_cast<CFormattedCellListBoxItemDrawer*>( iItemDrawer );
    CCalenWeekLBData* data =
        static_cast<CCalenWeekLBData*>( drawer->FormattedCellData() );

    aGc.SetPenStyle(CGraphicsContext::ESolidPen);
    aGc.SetBrushStyle(CGraphicsContext::ENullBrush);

    TInt index = CurrentItemIndex();
    if( index < 0 )
        index = 0;

    TRect rect;
    data->CurrentSubCellPos( rect, ItemPos( index ) );

    MAknsSkinInstance* skin = AknsUtils::SkinInstance();
    MAknsControlContext* cc = NULL;

    if((cc = AknsDrawUtils::ControlContextOfParent( data->Control() )) == NULL)
        {
        cc = data->SkinBackgroundContext();
        }
    
    TRACE_EXIT_POINT;
    return AknsDrawUtils::DrawBackground( skin, cc, NULL, aGc, TPoint(0,0), rect, KAknsDrawParamNoClearUnderImage);
    }

// ---------------------------------------------------------
// CCalenWeekLBView::HorizontalMoveToItemL
// Calls form MoveCursorL().Left/Right arrow key handing function
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCalenWeekLBView::HorizontalMoveToItemL( TInt aTargetSubCellIndex, TSelectionMode aSelectionMode)
    {
    TRACE_ENTRY_POINT;
    
    TInt oldCurrentItemIndex(iColumn);
    iColumn = aTargetSubCellIndex;

    if (ColumnExist(iColumn) && !(iColumn == oldCurrentItemIndex))
        {
        SetColumn(iColumn, EFalse);

        CFormattedCellListBoxItemDrawer* drawer =
            static_cast<CFormattedCellListBoxItemDrawer*>( iItemDrawer );
        CCalenWeekLBData* data =
            static_cast<CCalenWeekLBData*>( drawer->FormattedCellData() );

        TPoint oldPos(data->SubCellPosition(oldCurrentItemIndex + 1));
        TPoint pos(data->SubCellPosition(iColumn + 1));
        TSize size(data->SubCellSize(iColumn + 1));
        TInt left(Min(oldPos.iX, pos.iX));
        TInt right(Max(oldPos.iX, pos.iX));
        TInt y(ItemPos(iCurrentItemIndex).iY);
        TRect redrawRect(left, y, right, y + size.iHeight);

        iGc->SetClippingRect(redrawRect);

        iItemDrawer->DrawItem(iCurrentItemIndex,
                              ItemPos(iCurrentItemIndex), ItemIsSelected(iCurrentItemIndex),
                              ETrue, (iFlags&EEmphasized) > 0, (iFlags&EDimmed) > 0 );

        iGc->CancelClippingRect();
        }
    UpdateSelectionL(aSelectionMode);
    
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------
// CCalenWeekLBView::ColumnExist
// Check column existence
// (other items were commented in a header).
// ---------------------------------------------------------
//
TBool CCalenWeekLBView::ColumnExist(TInt aColumn)  // Column index
    {
    TRACE_ENTRY_POINT;
    
    CFormattedCellListBoxItemDrawer* drawer =
        static_cast<CFormattedCellListBoxItemDrawer*>( iItemDrawer );
    CCalenWeekLBData* data =
        static_cast<CCalenWeekLBData*>( drawer->FormattedCellData() );

    if (aColumn >= 0 && aColumn < (data->CellCount() - 1))
        {
        TRACE_EXIT_POINT;
        return ETrue;
        }
    
    TRACE_EXIT_POINT;
    return EFalse;
    }

// ---------------------------------------------------------
// CCalenWeekLBView::MoveCursorL
// Handling cursor move
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCalenWeekLBView::MoveCursorL(TCursorMovement aCursorMovement, TSelectionMode aSelectionMode)
    {
    TRACE_ENTRY_POINT;
    
    switch (aCursorMovement)
        {
        case ECursorNextColumn:     // Right cursor was pressed
            HorizontalMoveToItemL(iColumn + 1, aSelectionMode);
            break;
        case ECursorPreviousColumn: // Left cursor was pressed
            HorizontalMoveToItemL(iColumn - 1, aSelectionMode);
            break;
        default:
            CFormattedCellListBoxView::MoveCursorL(
                aCursorMovement, aSelectionMode);
            break;
        }
    
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------
// CCalenWeekLBView::Draw
// Drawing all visible item
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCalenWeekLBView::Draw(const TRect* /*clipRect*/) const
    {
    TRACE_ENTRY_POINT;
    
    if(RedrawDisabled() || !IsVisible())
        {
        return;
        }

    if(iModel->NumberOfItems() == 0)
        {
        iItemDrawer->ClearRect(iViewRect);
        }
    else
        {
        TInt firstPotentialItemIndex(iTopItemIndex);
        TRect rect(iViewRect);
        TInt lastPotentialItemIndex( iTopItemIndex + NumberOfItemsThatFitInRect(rect) -1 );
        TInt i(firstPotentialItemIndex);

        for(; i <= lastPotentialItemIndex; ++i)
            {
            if( ITEM_EXISTS(i) )
                {
                DrawItem( i );
                }
            }
        }
    
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------
// CCalenWeekLBView::DrawItem
// Draw item
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCalenWeekLBView::DrawItem(TInt aItemIndex) const     // Item index
    {
    TRACE_ENTRY_POINT;
    
    if (RedrawDisabled() || !IsVisible())
        {
        return;
        }

    if ((ITEM_EXISTS(aItemIndex)) && ItemIsVisible(aItemIndex))
        {
        iGc->SetClippingRect(iViewRect);

        iItemDrawer->DrawItem(aItemIndex,
                              ItemPos(aItemIndex), ItemIsSelected(aItemIndex),
                              (aItemIndex == iCurrentItemIndex),
                              (iFlags&EEmphasized) > 0, (iFlags&EDimmed) > 0 );

        iGc->CancelClippingRect();
        }
    
    TRACE_EXIT_POINT;
    }


// ---------------------------------------------------------
// CCalenWeekLBView::PointerMoveToItemL
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCalenWeekLBView::PointerMoveToItemL(TInt aNewColumn)
    {
    TRACE_ENTRY_POINT;
    
    if(AknLayoutUtils::PenEnabled())
        {
        if(aNewColumn >= 0 && aNewColumn != iColumn)
            HorizontalMoveToItemL(aNewColumn, CListBoxView::ENoSelection);
        }
    
    TRACE_EXIT_POINT;
    }


// End of File
