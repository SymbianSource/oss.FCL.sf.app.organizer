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
* Description:  CCalenWeekListbox is the class that customizes drawing
 *                part of CEikFormattedCellListBox.
 *
*/

//debug
#include "calendarui_debug.h"

//  INCLUDE FILES
#include "calenweeklistbox.h"

#include "calendrawutils.h"
#include <calenagendautils.h>
#include "calenweekcallback.h"
#include "calenweekcontainer.h"
#include "calenweekdata.h"
#include "calenweeklistboxdata.h"
#include "calenweeklistboxitemdrawer.h"
#include "calenweeklistboxview.h"
#include "calenweeklistboxlayout.h"

#include <AknLayoutFont.h>
#include <aknlists.h>
#include <AknIconUtils.h>
#include <AknsControlContext.h>
#include <AknsDrawUtils.h>
#include <AknUtils.h>
#include <gulicon.h>
#include <calenviewutils.h>
#include <aknlayoutscalable_apps.cdl.h>

// ======== LOCAL FUNCTIONS ========

// ---------------------------------------------------------
// SetupWeekHourCellL
// Sets subcell properties for week hour cell
// ---------------------------------------------------------
//
static void SetupWeekHourCellL( CFormattedCellListBoxItemDrawer* aItemDrawer, TWeekListBoxLayout& aWeekLayout)
    {
    TRACE_ENTRY_POINT;
    
    TInt KSubCell = 0; // hour cell is first subcell 

    TAknLayoutText hourTextLayout = aWeekLayout.HourTextLayout();
    TRect subCellRect = aWeekLayout.HourSubCellRect_in_ListItemRect();
    // We use text rect from layout as subcell rect, so margins are zero! 
    TMargins margins = { 0, 0, 0, 0 };

    const CFont* font = hourTextLayout.Font();
    // Baseline calculation idea is copied fromTAknLayoutText::LayoutText 
    const CAknLayoutFont* layoutFont = CAknLayoutFont::AsCAknLayoutFontOrNull( font );
    TInt baseline = 0;
    if ( layoutFont )
        {
        baseline = layoutFont->TextPaneTopToBaseline();
        }
    else
        {
        baseline = font->AscentInPixels();
        }

    // Set subcell properties
    aItemDrawer->FormattedCellData()->SetSubCellFontL(       KSubCell, font );
    aItemDrawer->FormattedCellData()->SetSubCellMarginsL(    KSubCell, margins );
    aItemDrawer->FormattedCellData()->SetSubCellAlignmentL(  KSubCell, hourTextLayout.Align() );
    aItemDrawer->FormattedCellData()->SetSubCellPositionL(   KSubCell, subCellRect.iTl );
    aItemDrawer->FormattedCellData()->SetSubCellSizeL(       KSubCell, subCellRect.Size() );
    aItemDrawer->FormattedCellData()->SetSubCellBaselinePosL(KSubCell, baseline );

    // Set colors
    TRgb textColor = hourTextLayout.Color();

    AknsUtils::GetCachedColor(AknsUtils::SkinInstance(), textColor,
                              KAknsIIDQsnTextColors,
                              EAknsCIQsnTextColorsCG23);

    CFormattedCellListBoxData::TColors colors;

    //FIXME: this is hack for extended skinning. It seems to be difficult to pass and use
    // correct colors in LBItemDrawer and LBData. 
    aItemDrawer->SetTextColor( textColor ); 

    colors.iText = textColor;
    colors.iBack = aItemDrawer->BackColor();
    colors.iHighlightedText = aItemDrawer->HighlightedTextColor();
    colors.iHighlightedBack = aItemDrawer->HighlightedBackColor();
    aItemDrawer->FormattedCellData()->SetSubCellColorsL( KSubCell, colors );
    
    TRACE_EXIT_POINT;
    }


// ---------------------------------------------------------
// SetupWeekGridCellL
// Sets subcell properties for week grid cells 
// ---------------------------------------------------------
//
static void SetupWeekGridCellL(CFormattedCellListBoxData* aListboxData,
                               TInt aSubcellIndex, // Subcell index
                               TRect aSubcellRect // subcell rect relative to list item
    )
    {
    TRACE_ENTRY_POINT;
    
    // As week grid subcell doesn't have margins, it is easy to setup 
    TMargins margins = { 0, 0, 0, 0 };
    aListboxData->SetSubCellMarginsL(  aSubcellIndex, margins); 
    aListboxData->SetGraphicsSubCellL( aSubcellIndex, ETrue); 
    aListboxData->SetSubCellPositionL( aSubcellIndex, aSubcellRect.iTl);
    aListboxData->SetSubCellSizeL(     aSubcellIndex, aSubcellRect.Size());
    aListboxData->SetSubCellAlignmentL(aSubcellIndex, CGraphicsContext::ECenter);
    
    TRACE_EXIT_POINT;
    }


// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code, that
// might leave.
//
CCalenWeekListbox::CCalenWeekListbox(CCalenWeekContainer& aWeekContainer)
    : iWeekContainer( aWeekContainer )
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    }

// Symbian OS default constructor can leave.
void CCalenWeekListbox::ConstructL(
    const CCoeControl* aParent    
    )
    {
    TRACE_ENTRY_POINT;
    
    CEikFormattedCellListBox::ConstructL(aParent, 0);
    CCalenWeekLBData* data =
        static_cast<CCalenWeekLBData*>( ItemDrawer()->FormattedCellData() );
    data->SetHighlightAnimBackgroundDrawer( static_cast<CCalenWeekLBView*>(iView) );
    
    TRACE_EXIT_POINT;
    }


// Destructor
CCalenWeekListbox::~CCalenWeekListbox()
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------
// CCalenWeekListbox::SetCursorObserver
// Set cursor handling class
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCalenWeekListbox::SetCursorObserver
(MCalenWeekCursorObserver* aCursorObsever)  // A observer pointer
    {
    TRACE_ENTRY_POINT;
    
    iCursorObsever = aCursorObsever;
    
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------
// CCalenWeekListbox::SetStartDay
// Set the start of a week
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCalenWeekListbox::SetStartDay(TTime aStartDay)
    {
    TRACE_ENTRY_POINT;
    
    iStartDay = aStartDay;
    
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------
// CCalenWeekListbox::OfferKeyEventL
// handles key events.
// (other items were commented in a header).
// ---------------------------------------------------------
//
TKeyResponse CCalenWeekListbox::OfferKeyEventL(const TKeyEvent& aKeyEvent, TEventCode aType)
    {
    TRACE_ENTRY_POINT;
    
    TKeyResponse res(EKeyWasNotConsumed);
    TBool mirrored(AknLayoutUtils::LayoutMirrored());

    if (aType == EEventKey)
        {
        switch(aKeyEvent.iCode)
            {
            case EKeyLeftArrow:
                if (mirrored)
                    {
                    HandleRightArrowKeyL(CListBoxView::ENoSelection);
                    }
                else
                    {
                    HandleLeftArrowKeyL(CListBoxView::ENoSelection);
                    }
                res = EKeyWasConsumed;
                break;
            case EKeyRightArrow:
                if (mirrored)
                    {
                    HandleLeftArrowKeyL(CListBoxView::ENoSelection);
                    }
                else
                    {
                    HandleRightArrowKeyL(CListBoxView::ENoSelection);
                    }
                res = EKeyWasConsumed;
                break; 
            case EKeyUpArrow:
#ifdef SUPPORT_NAVISCROLL_EVENTS
            case EKeyPrevious:
#endif
                res = CEikListBox::OfferKeyEventL(aKeyEvent, aType);
                if (iCursorObsever)
                    {
                    iCursorObsever->CursorMovedL(
                        MCalenWeekCursorObserver::ECalenUp);
                    }
                break;
            case EKeyDownArrow:
#ifdef SUPPORT_NAVISCROLL_EVENTS
            case EKeyNext:
#endif
                res = CEikListBox::OfferKeyEventL(aKeyEvent, aType);
                if (iCursorObsever)
                    {
                    iCursorObsever->CursorMovedL(
                        MCalenWeekCursorObserver::ECalenDown);
                    }
                break;
           default:
                res = CEikListBox::OfferKeyEventL(aKeyEvent, aType);
                break;
            }
        }
    
    TRACE_EXIT_POINT;
    return res;
    }

TTypeUid::Ptr CCalenWeekListbox::MopSupplyObject(TTypeUid aId)
    {
    TRACE_ENTRY_POINT;
    
    // FormattedCellListBox function skipped intentioanlly as it intercepts
    // background contexts (and we want to provide our own BG context).
    if( aId.iUid == MAknsControlContext::ETypeId )
        {
        MAknsControlContext* cc = AknsDrawUtils::ControlContextOfParent( this );
        if ( cc )
            {
            TRACE_EXIT_POINT;
            return MAknsControlContext::SupplyMopObject( aId, cc );
            }
        }
    
    TRACE_EXIT_POINT;
    return CCoeControl::MopSupplyObject( aId );
    }

// ---------------------------------------------------------
// CCalenWeekListbox::SetupLayoutL
// Set all cell data
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCalenWeekListbox::SetupLayoutL()
    {
    TRACE_ENTRY_POINT;
    
    TRect rect = iWeekContainer.ReducePreview( iWeekContainer.Rect() );
    TWeekListBoxLayout lay( iWeekContainer.UseInfobar(), 
                            iWeekContainer.UseToolbar(),
                            iWeekContainer.UsePreview(),
                            rect
                            );

    SetItemHeightL( lay.ListItemHeight() );

    // Hour subcell layout
    SetupWeekHourCellL( ItemDrawer(), lay );

    // Grid subcell layouts
    CFormattedCellListBoxData* listBoxData = ItemDrawer()->FormattedCellData();
    for (TInt loop(1); loop < KCalenDaysInWeek + 1; ++loop)
        {
        TRect gridCellRect = lay.GridSubCellRect_in_ListItemRect(loop - 1);
        SetupWeekGridCellL( listBoxData, loop, gridCellRect );
        }

    // Icon sizes
    TInt KFirst = 0; // just fetch layout for first item 
    TRect firstCellRect = lay.GridSubCellRect_in_ListItemRect(KFirst);
    TAknLayoutRect iconLayout;
    TInt layoutVariant = lay.LayoutVariantIndex(TWeekListBoxLayout::ECellCaleWeekPaneG2);
    iconLayout.LayoutRect( firstCellRect /*P:cell_cale_week_pane*/,
                           AknLayoutScalable_Apps::cell_cale_week_pane_g2(layoutVariant).LayoutLine() );
    CalenDrawUtils::SetIconSizes(
        iconLayout, ItemDrawer()->FormattedCellData()->IconArray());

   // Colors and misc stuff
    SetSubcellColorsL();

    CCalenWeekLBData* celData = static_cast<CCalenWeekLBData*>( ItemDrawer()->FormattedCellData() );
    celData->SetCelCount( KCalenDaysInWeek + 1 );
    celData->SetVariantIndex(layoutVariant);
    
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------
// CCalenWeekListbox::SetSubcellColorsL
// Set a Background color for all subcells
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCalenWeekListbox::SetSubcellColorsL() const
    {
    TRACE_ENTRY_POINT;
    
    // FIXME: investigate, if this is needed anymore at all. 
    for (TInt loop(0); loop < KCalenDaysInWeek; ++loop)
        {
        CFormattedCellListBoxItemDrawer* itemDrawer = ItemDrawer();
        CFormattedCellListBoxData::TColors colors;
        colors.iText = itemDrawer->TextColor();
        colors.iBack = itemDrawer->BackColor();
        colors.iHighlightedText = itemDrawer->HighlightedTextColor();
        colors.iHighlightedBack = itemDrawer->HighlightedBackColor();
        itemDrawer->FormattedCellData()->SetSubCellColorsL(loop + 1, colors);
        }
    
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------
// CCalenWeekListbox::CreateItemDrawerL
// Create CColumnListBoxData and CColumnListBoxItemDrawer.
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCalenWeekListbox::CreateItemDrawerL()
    {
    TRACE_ENTRY_POINT;
    
    CCalenWeekLBData* listboxData = CCalenWeekLBData::NewL();
    CleanupStack::PushL( listboxData );
    iItemDrawer = new(ELeave) CCalenWeekListBoxItemDrawer(
        Model(), 
        iEikonEnv->NormalFont(),
        listboxData);
    CleanupStack::Pop();    // listboxData
    
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------
// CCalenWeekListbox::MakeViewClassInstanceL
// Create ListBoxView class
// (other items were commented in a header).
// ---------------------------------------------------------
//
CListBoxView* CCalenWeekListbox::MakeViewClassInstanceL()
    {
    TRACE_ENTRY_POINT;
    
    TRACE_EXIT_POINT;
    return (new(ELeave) CCalenWeekLBView);
    }

// ---------------------------------------------------------
// CCalenWeekListbox::HandleLeftArrowKeyL
// Called when Left arrow key was pressed
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCalenWeekListbox::HandleLeftArrowKeyL(CListBoxView::TSelectionMode aSelectionMode)
    {
    TRACE_ENTRY_POINT;
    
    TKeyResponse res(EKeyWasNotConsumed);
    if (iCursorObsever)
        {
        res = iCursorObsever->CursorMovedL(MCalenWeekCursorObserver::ECalenLeft);
        }
    if (res == EKeyWasNotConsumed)
        {
        iView->MoveCursorL(CListBoxView::ECursorPreviousColumn, 
                           aSelectionMode);
        ClearMatchBuffer();
        }
    
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------
// CCalenWeekListbox::HandleRightArrowKeyL
// Called when Right arrow key was pressed
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCalenWeekListbox::HandleRightArrowKeyL(CListBoxView::TSelectionMode aSelectionMode)
    {
    TRACE_ENTRY_POINT;
    
    TKeyResponse res(EKeyWasNotConsumed);
    if (iCursorObsever)
        {
        res = iCursorObsever->CursorMovedL(MCalenWeekCursorObserver::ECalenRight);
        }
    if (res == EKeyWasNotConsumed)
        {
        iView->MoveCursorL(CListBoxView::ECursorNextColumn, aSelectionMode);
        ClearMatchBuffer();
        }
    
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------
// CCalenWeekListbox::SizeChanged
// Initilizes ListBox cell
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCalenWeekListbox::SizeChanged()
    {
    TRACE_ENTRY_POINT;
    
    AknListBoxLayouts::SetupStandardFormListbox( ItemDrawer() );

    PIM_TRAPD_HANDLE( SetupLayoutL() );

    CEikFormattedCellListBox::SizeChanged();
    
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------
// CCalenWeekListbox::SetBgContext
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCalenWeekListbox::SetBgContext(MAknsControlContext* aCc)
    {
    TRACE_ENTRY_POINT;
    
    iBgContext = aCc;
    
    TRACE_EXIT_POINT;
    }

TRect CCalenWeekListbox::LayoutRect() const
    {
    TRACE_ENTRY_POINT;
    
    TRect rect = iWeekContainer.ReducePreview( iWeekContainer.Rect() ); 
    TWeekListBoxLayout weekLayout(  iWeekContainer.UseInfobar(), 
                                    iWeekContainer.UseToolbar(), 
                                    iWeekContainer.UsePreview(),
                                    rect );
    
    TRACE_EXIT_POINT;
    return weekLayout.ListBoxRect();
    }


// ---------------------------------------------------------
// CCalenWeekListbox::IsValidPointer
// Checks if pointer event happened in valid area
// (other items were commented in a header).
// ---------------------------------------------------------
//
TBool CCalenWeekListbox::IsValidPointer(TPoint aPos)
    {
    TRACE_ENTRY_POINT;
    
    TWeekListBoxLayout lay( iWeekContainer.UseInfobar(), 
                            iWeekContainer.UseToolbar(),
                            iWeekContainer.UsePreview(),
                            // iWeekContainer.Rect()
                            iWeekContainer.ReducePreview(iWeekContainer.Rect()));
    
    TRACE_EXIT_POINT;
    return lay.WeekGridRect().Contains(aPos);
    }

// ---------------------------------------------------------
// CCalenWeekListbox::PointerEventColumn
// Calculates new column from pointer event (touch ui)
// (other items were commented in a header).
// ---------------------------------------------------------
//
TInt CCalenWeekListbox::PointerEventColumn(TPoint aPos)
    {
    TRACE_ENTRY_POINT;
    
    TRect rect = iWeekContainer.ReducePreview( iWeekContainer.Rect() );
    TWeekListBoxLayout lay( iWeekContainer.UseInfobar(), 
                            iWeekContainer.UseToolbar(),
                            iWeekContainer.UsePreview(),
                            rect );

    TInt cellWidth(lay.WeekGridRect().Width() / KCalenDaysInWeek);
    if((lay.WeekGridRect().Width() % KCalenDaysInWeek) > KCalenDaysInWeek/2)
        {
        ++cellWidth;
        }

    TRACE_EXIT_POINT;
    return  ((aPos.iX - lay.WeekGridRect().iTl.iX) / cellWidth);
    }

// ---------------------------------------------------------
// CCalenWeekListbox::HandlePointerEventL
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCalenWeekListbox::HandlePointerEventL(const TPointerEvent& aPointerEvent)
    {
    TRACE_ENTRY_POINT;
    
    if(AknLayoutUtils::PenEnabled())
        {
        TInt newColumn = PointerEventColumn(aPointerEvent.iPosition);
        TInt pointerIndex(-1);
        TBool isItem (View()->XYPosToItemIndex(aPointerEvent.iPosition, pointerIndex));
        
        if(!isItem) // to handle scroll if drag out side of listbox
            CEikListBox::HandlePointerEventL(aPointerEvent);
        else if(newColumn >= 0 && newColumn < KCalenDaysInWeek)
            {
            iCursorObsever->PointerMovedL(newColumn);
            CEikListBox::HandlePointerEventL(aPointerEvent);
            }
        }
    
    TRACE_EXIT_POINT;
    }


// End of File
