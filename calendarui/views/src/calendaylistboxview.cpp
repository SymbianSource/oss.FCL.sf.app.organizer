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
* Description:   ListBox View class. The class has a special drawing and
 *                special cusor moving.
 *
*/



//debug
#include "calendarui_debug.h"


// INCLUDE FILES
#include "calendaylistboxview.h"
#include "CalendarVariant.hrh"
#include "calendrawutils.h"

#include <AknUtils.h>
#include <AknsDrawUtils.h>
#include <AknsEffectAnim.h>
#include <eikfrlbd.h>

#include <aknlayoutscalable_apps.cdl.h>
#include <aknlayoutscalable_avkon.cdl.h>


const TInt KMaxLineInACell(4);

#define ITEM_EXISTS_ONCE(x) (((x) > -1) && ((x) < iModel->NumberOfItems()))

// ================= MEMBER FUNCTIONS =======================

CCalenDayListBoxView::CCalenDayListBoxView(CCoeControl* aParent)
    : iParent(aParent)
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    }

CCalenDayListBoxView::~CCalenDayListBoxView()
    {
    TRACE_ENTRY_POINT;
    
    delete iCellArray;
    
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------
// CCalenDayListBoxView::SetCellArray
// Sets a array for dividing a cell
// (other items were commented in a header).
// ----------------------------------------------------
//
void CCalenDayListBoxView::SetCellArray(CArrayFix<TInt>* aArray)
    {
    TRACE_ENTRY_POINT;
    
    delete iCellArray;
    iCellArray = aArray;
    
    TRACE_EXIT_POINT;
    }

TInt CCalenDayListBoxView::FirstLineOfCell(TInt aLine) const
    {
    TRACE_ENTRY_POINT;
    
    for (TInt i = aLine; i >= 0; i--)
        {
        if (IsFirstLineOfCell(i))
            {
            return i;
            }
        }
    
    TRACE_EXIT_POINT;
    return aLine; // FIXME, should not get here. 
    }

TInt CCalenDayListBoxView::LastLineOfCell(TInt aLine) const
    {
    TRACE_ENTRY_POINT;
    
    for (TInt i = aLine; i < iModel->NumberOfItems(); ++i)
        {
        if (IsLastLineOfCell(i))
            {
            return i;
            }
        }
    
    TRACE_EXIT_POINT;
    return aLine; // FIXME, should not get here
    }

// ----------------------------------------------------
// CCalenDayListBoxView::IsLastLineOfCell
//
// (other items were commented in a header).
// ----------------------------------------------------
//
TBool CCalenDayListBoxView::IsLastLineOfCell(TInt aLine) const
    {
    TRACE_ENTRY_POINT;
    
    TKeyArrayFix key(0, ECmpTInt32);
    TInt index(0);
    TBool retValue(EFalse);
    if (iCellArray->FindIsq(aLine, key, index) == 0)
        {
        retValue = ETrue;
        }
    
    TRACE_EXIT_POINT;
    return retValue;
    }

// ----------------------------------------------------
// CCalenDayListBoxView::IsFirstLineOfCell
//
// (other items were commented in a header).
// ----------------------------------------------------
//
TBool CCalenDayListBoxView::IsFirstLineOfCell(TInt aLine) const
    {
    TRACE_ENTRY_POINT;
    
    TKeyArrayFix key(0, ECmpTInt32);
    TInt index(0);
    TBool retValue(EFalse);
    if (aLine == 0)
        {
        retValue = ETrue;
        }
    else if (iCellArray->FindIsq(aLine -1, key, index) == 0)
        {
        retValue = ETrue;            
        }

    TRACE_EXIT_POINT;
    return retValue;
    }

// ----------------------------------------------------
// CCalenDayListBoxView::DrawCell
//
// (other items were commented in a header).
// ----------------------------------------------------
//
void CCalenDayListBoxView::DrawCell(TInt aIndex)
    {
    TRACE_ENTRY_POINT;
    
    TInt lastLine(LastLineOfCell(aIndex));
    for (TInt i(aIndex); i <= lastLine; ++i)
        {
        DrawItem(i);
        }
    
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------
// 
// ----------------------------------------------------
//
TRect CCalenDayListBoxView::DayViewItemRect(TInt aItemIndex) const
    {
    TRACE_ENTRY_POINT;

    TInt topLine(FirstLineOfCell(aItemIndex));
    TInt bottomLine(LastLineOfCell(aItemIndex));

    //topLine = topLine > iTopItemIndex ? topLine : iTopItemIndex;

    bottomLine = bottomLine > iBottomItemIndex ? iBottomItemIndex : bottomLine;
    TRect rectHighlight(ItemPos(topLine), ItemPos(bottomLine));
    rectHighlight.iBr += CListBoxView::ItemSize(bottomLine);
    
    TRACE_EXIT_POINT;
    return rectHighlight;
    }

// ----------------------------------------------------
// CCalenDayListBoxView::ItemSize
//
// (other items were commented in a header).
// ----------------------------------------------------
//    
TSize CCalenDayListBoxView::ItemSize(TInt aItemIndex) const
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    return DayViewItemRect(aItemIndex).Size();
    }

void CCalenDayListBoxView::DrawItem(TInt aItemIndex) const
    {
    TRACE_ENTRY_POINT;
    
    PIM_TRAPD_HANDLE(DrawItemL(aItemIndex));
    
    TRACE_ENTRY_POINT;
    }

void CCalenDayListBoxView::DrawItemL(TInt aItemIndex) const
    {
    TRACE_ENTRY_POINT;

    // FIXME: this function needs cleanup. 
    if(RedrawDisabled() || !IsVisible())
        {
        return;
        }

    if((ITEM_EXISTS_ONCE(aItemIndex)) && ItemIsVisible(aItemIndex))
        {
        TBool highlight(EFalse);
        if(aItemIndex >= iCurrentItemIndex && aItemIndex < iCurrentItemIndex + KMaxLineInACell)
            {
            TInt nextCell(LastLineOfCell(iCurrentItemIndex) + 1);

        if ( !( iItemDrawer->Flags()
                & CTextListItemDrawer::ESingleClickDisabledHighlight  )
                && aItemIndex < nextCell )
            {
            highlight = ETrue;
            }
            }
        TBool hlDone( EFalse );
        TBool animHLDone( EFalse );

        MAknsSkinInstance *skin = AknsUtils::SkinInstance();
        CFormattedCellListBoxData* data = static_cast<CFormattedCellListBoxItemDrawer*>(iItemDrawer)->FormattedCellData();

        // Skinned highlight
        if( ( highlight && IsFirstLineOfCell( aItemIndex ) ) ||(highlight && aItemIndex == iTopItemIndex))
            {
            MAknsControlContext *cc = AknsDrawUtils::ControlContext( iParent );

            if(!cc)
                {
                cc = static_cast<CFormattedCellListBoxItemDrawer*>(iItemDrawer)->FormattedCellData()->SkinBackgroundContext();
                }

            TRect dayViewItemRect( DayViewItemRect( aItemIndex ) );
            TAknLayoutRect list_highlight_pane_cp1;
            list_highlight_pane_cp1.LayoutRect( dayViewItemRect, AknLayoutScalable_Avkon::list_highlight_pane_cp1().LayoutLine() );

            if( data->HighlightAnim() && data )
                {
                data->SetItemCellSize( dayViewItemRect.Size() ); // ??? 
                data->AboutToDrawHighlightAnim();
                if( data->HighlightAnim() )
                    hlDone = data->HighlightAnim()->Render( *iGc, list_highlight_pane_cp1.Rect() );
                animHLDone = hlDone;
                }

            // Either animation does not exist or drawing it failed
            if( !hlDone )
                {
                if( cc )
                    {
                    AknsDrawUtils::Background( skin, cc, iParent, *iGc, dayViewItemRect); //??? Isn't it this drawn already?
                    }

                iGc->SetPenStyle(CGraphicsContext::ENullPen);

                // Skin Center layout 
                TAknLayoutRect list_highlight_pane_g1_cp1;
                list_highlight_pane_g1_cp1.LayoutRect( list_highlight_pane_cp1.Rect(), AknLayoutScalable_Avkon::list_highlight_pane_g1_cp1().LayoutLine() );

                hlDone = AknsDrawUtils::DrawFrame(skin, *iGc, 
                                                  list_highlight_pane_cp1.Rect(), // whole highlight rect
                                                  list_highlight_pane_g1_cp1.Rect(), // skin center rect
                                                  KAknsIIDQsnFrList, KAknsIIDQsnFrListCenter);
                }
            }

        TBool skinFlg(data->SkinEnabled());

        iGc->SetDrawMode(CGraphicsContext::EDrawModePEN);

        // FIXME: investigate if this is really needed
        if(highlight)
            {
            //iGc->SetDrawMode( CGraphicsContext::EDrawModeAND );
            data->SetSkinEnabledL( EFalse );
            }
        TRect rect( ItemPos( aItemIndex ), CListBoxView::ItemSize( aItemIndex ) );

        // Non-highlighted cell skin background
        if(!highlight && iParent)
            {
            MAknsControlContext *cc = AknsDrawUtils::ControlContext(iParent);
            if(!cc)
                {
                cc = static_cast<CFormattedCellListBoxItemDrawer*>(iItemDrawer)->FormattedCellData()->SkinBackgroundContext();
                }

            if(cc)
                {
                AknsDrawUtils::Background( skin, cc, iParent, *iGc, rect);
                }
            }

        // Text and graphic content 
        iItemDrawer->DrawItem(aItemIndex, ItemPos(aItemIndex), ItemIsSelected(aItemIndex), highlight, 0, 0 );

        // if the highlight is animated we need to redraw all lines of a multi-line item
        if( animHLDone )
            {
            TInt index( aItemIndex );
            while( !IsLastLineOfCell( index ) )
                {
                if( ItemIsVisible( ++index ) )
                    {
                    iItemDrawer->DrawItem( index, ItemPos( index ), ItemIsSelected( index ), highlight, 0, 0 );
                    }
                }
            }

        iGc->SetDrawMode( CGraphicsContext::EDrawModePEN );
        data->SetSkinEnabledL( skinFlg );

        // Line between cells
        if( IsLastLineOfCell(aItemIndex) && !highlight )
            {
            // Get line color 
            TRgb lineColor(0,0,0);
            AknsUtils::GetCachedColor(AknsUtils::SkinInstance(), lineColor,
                                      KAknsIIDQsnLineColors,
                                      EAknsCIQsnLineColorsCG2);
            iGc->SetPenColor( lineColor );
            iGc->SetBrushColor( lineColor );
            iGc->SetPenStyle(CGraphicsContext::ESolidPen);
            iGc->SetBrushStyle(CGraphicsContext::ENullBrush);

            // Draw line. There is graphic item qgn_graf_line_cale_day defined for line, but using it would require
            // caching it first. To be done later? 
    #ifndef RD_CALEN_MIDNIGHT_VISUALIZATION
            TInt variety_g2 = 0; // We don't care about variety, because separator line definition is same for all  
    #else
            TInt variety_g2(-1);    
            TInt countryCode( User::Language() );
            TLocale locale;
            locale.Refresh();
            TTimeFormat timeFormat( locale.TimeFormat() );

            if( timeFormat == ETime24 )
                {
                // Option 1: 24 hour        
                variety_g2 = 0;
                }
            // 12 hour
            else if( countryCode == ELangCzech )
                {
                // Option 3: Czech, 12 hour with Arrow
                variety_g2 = 8;
                }
            else
                {
                // Option 15: 12 hour, with Arrow
                variety_g2 = 7;
                }

     #endif // RD_CALEN_MIDNIGHT_VISUALIZATION 
                     
            TAknLayoutRect list_cale_time_pane_g2;
            list_cale_time_pane_g2.LayoutRect( rect, AknLayoutScalable_Apps::list_cale_time_pane_g2( variety_g2 ).LayoutLine() );
            iGc->DrawRect( list_cale_time_pane_g2.Rect() );
            }
        }
    
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------
// 
// ----------------------------------------------------
//
void CCalenDayListBoxView::ScrollToItemL(TInt aItemIndex, TSelectionMode aSelectionMode)
    {
    TRACE_ENTRY_POINT;
    
    TInt oldItemIndex = iCurrentItemIndex;
    TInt oldTopIndex = iTopItemIndex;

    // To prevent incorrect drawing and flickering, we just move focus
    // without updating the screen. Uikon list can't draw correct multirow 
    // focus for us
    // Updating is done below. 
    VerticalMoveToItemL(aItemIndex, aSelectionMode);

    //These two scrolls ensure that all lines of item are visible.
    //If list is scrolled, it is correctly updated and drawn by these methods
    ScrollToMakeItemVisible( FirstLineOfCell(aItemIndex) );
    ScrollToMakeItemVisible( LastLineOfCell(aItemIndex) );

    // If list was not scrolled, we update old focus cell and new focus cell. 
    if (oldTopIndex == iTopItemIndex)
        {
        if (oldItemIndex != iCurrentItemIndex) 
            {
                DrawCell(oldItemIndex);
                DrawCell(iCurrentItemIndex);
            }
        }
    
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------
// CCalenDayListBoxView::MoveCursorL
//
// (other items were commented in a header).
// ----------------------------------------------------
//
void CCalenDayListBoxView::MoveCursorL(
    TCursorMovement aCursorMovement, TSelectionMode aSelectionMode)
    {
    TRACE_ENTRY_POINT;

    switch (aCursorMovement)
        {
        case CListBoxView::ECursorPreviousItem:
            {
            TInt prevLine(iCurrentItemIndex - 1);
            if (prevLine >= 0)
                {
                TInt topLine = FirstLineOfCell(prevLine);
                ScrollToItemL(topLine, aSelectionMode);
                }
            break;
            }
        case CListBoxView::ECursorNextItem:
            {
            TInt nextCell = LastLineOfCell(iCurrentItemIndex) + 1;
            if (nextCell < iModel->NumberOfItems())
                {
                ScrollToItemL(nextCell, aSelectionMode);
                }
            break;
            }
        case CListBoxView::ECursorFirstItem:
            {
            TInt firstItem = 0;
            if ( firstItem != iCurrentItemIndex) 
            {
            	ScrollToItemL(firstItem, aSelectionMode);
            }
            break;
            }
        case CListBoxView::ECursorLastItem:
            {
            TInt lastLine = iModel->NumberOfItems() - 1;
            TInt lastItem = FirstLineOfCell(lastLine);
            if ( lastItem != iCurrentItemIndex) 
            {
            	ScrollToItemL(lastItem, aSelectionMode);	
            }
            break;
            }
        default:
            {
            // Other TCursorMovements are not implemented. 
            // Calling superclass's function would not work
            break;
            }
        }
    
    TRACE_EXIT_POINT;
    }


// ----------------------------------------------------
// CCalenDayListBoxView::MoveToItemL
// Move to new event (from touch)
// (other items were commented in a header).
// ----------------------------------------------------
//
void CCalenDayListBoxView::MoveToItemL(TInt aItemIndex)
    {
    TRACE_ENTRY_POINT;
    
    if(AknLayoutUtils::PenEnabled())
        {
        TInt oldItemIndex = FirstLineOfCell(iCurrentItemIndex);
        TInt oldTopIndex = iTopItemIndex;
        
        // To prevent incorrect drawing and flickering, we just move focus
        // without updating the screen. Uikon list can't draw correct multirow 
        // focus for us
        // Updating is done below. 
        SetDisableRedraw(ETrue);
        VerticalMoveToItemL(FirstLineOfCell(aItemIndex), CListBoxView::ENoSelection);
        SetDisableRedraw(EFalse);
        
        //These two scrolls ensure that all lines of item are visible.
        //If list is scrolled, it is correctly updated and drawn by these methods
        ScrollToMakeItemVisible( FirstLineOfCell(aItemIndex) );
        ScrollToMakeItemVisible( LastLineOfCell(aItemIndex) );
        
        // If list was not scrolled, we update old focus cell and new focus cell. 
        if (oldTopIndex == iTopItemIndex)
            {
            if (oldItemIndex != FirstLineOfCell(iCurrentItemIndex)) 
                {
                DrawCell(oldItemIndex);
                DrawCell(FirstLineOfCell(iCurrentItemIndex));
                }
            }
        }
    
    TRACE_EXIT_POINT;
    }


// End of File
