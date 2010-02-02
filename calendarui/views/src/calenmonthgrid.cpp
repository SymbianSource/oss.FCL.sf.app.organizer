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
* Description:   Grid of the MonthView.
 *
*/



//debug
#include "calendarui_debug.h"

// INCLUDE FILES
#include "calenmonthgrid.h"
#include "calenmonthcontainer.h"
#include "calenmonthcelllistboxdata.h"
#include "calenmonthcelllistboxitemdrawer.h"

#include <aknlayoutscalable_apps.cdl.h>

// New line color groups in enhanced skinning
static void DrawLAFLine(CWindowGc& aGc, const TAknLayoutRect& aArea,
                        const TAknsItemID& aSkinComponent, TInt aColorGroup)
    {
    TRACE_ENTRY_POINT;

    TRgb lineColor = aArea.Color();
    AknsUtils::GetCachedColor(AknsUtils::SkinInstance(), lineColor,
                              aSkinComponent, aColorGroup);
    aGc.SetBrushColor( lineColor );
    aGc.SetBrushStyle( CGraphicsContext::ESolidBrush );
    aGc.Clear( aArea.Rect() );

    TRACE_EXIT_POINT;
    }

static void DrawSecondaryLine(CWindowGc& aGc, const TAknLayoutRect& aArea)
    {
    TRACE_ENTRY_POINT;

    DrawLAFLine(aGc, aArea, KAknsIIDQsnLineColors, EAknsCIQsnLineColorsCG2);

    TRACE_EXIT_POINT;
    }

// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code, that
// might leave.
//
CCalenMonthGrid::CCalenMonthGrid
(TTime aFirstDayOfGrid, CCalenMonthContainer* aMonthCont)
    : iFirstDayOfGrid(aFirstDayOfGrid),iMonthContainer(aMonthCont)
    {
    TRACE_ENTRY_POINT;
    
    SetVerticalMargin(0);
    SetHorizontalMargin(0);
    
    TRACE_EXIT_POINT;
    }

// Destructor
CCalenMonthGrid::~CCalenMonthGrid()
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------
// CCalenMonthGrid::Draw
// Drawing month gird
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCalenMonthGrid::Draw(const TRect& aRect)const
    {
    TRACE_ENTRY_POINT;
    //const TBool useWeeks( UseWeeks() );
    CAknGrid::Draw( aRect );
    
    // For drawing Secondary grid lines
    DrawGridLines();

    TRACE_EXIT_POINT;
    
    }

// ---------------------------------------------------------
// CCalenMonthGrid::DrawGridLines
// Draws secondary lines of the grid
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCalenMonthGrid::DrawGridLines()const
    {
    TRACE_ENTRY_POINT;
        
    CWindowGc& gc = SystemGc();
    TRect parentRect = iMonthContainer->Rect();
    TRect main_pane(iMonthContainer->ReducePreview( parentRect ) );
        

    TAknLayoutRect main_cale_month_pane;
    TInt layoutVariant = iMonthContainer->LayoutVariantIndex(CCalenMonthContainer::EMainCaleMonthPane);
    main_cale_month_pane.LayoutRect( main_pane, AknLayoutScalable_Apps::main_cale_month_pane(layoutVariant).LayoutLine() );
    
    TAknLayoutRect cale_month_pane_g;

    // Get indexes for grid lines (cale_month_pane_g)
    TAknLayoutScalableTableLimits cale_month_pane_g_Limits = AknLayoutScalable_Apps::cale_month_pane_g_Limits();
    TInt index( cale_month_pane_g_Limits.FirstIndex() );
    TInt end(   cale_month_pane_g_Limits.LastIndex() );
    // First two are primary lines to separate heading and week number panes from grid
    // We draw them elsewhere
    const TInt firstGridLineIndex = 2;
    layoutVariant = iMonthContainer->LayoutVariantIndex(CCalenMonthContainer::ECaleMonthPaneG );
    for ( index+=firstGridLineIndex; index<=end; ++index )
        {
        cale_month_pane_g.LayoutRect( main_cale_month_pane.Rect(),
                AknLayoutScalable_Apps::cale_month_pane_g( index, layoutVariant ).LayoutLine() );
        DrawSecondaryLine( gc, cale_month_pane_g );
        }
        
    TRACE_EXIT_POINT;
    
    }


// ---------------------------------------------------------
// CCalenMonthGrid::FirstDayOfGrid
// Return first day of grid
// (other items were commented in a header).
// ---------------------------------------------------------
//
TTime CCalenMonthGrid::FirstDayOfGrid()
    {
    TRACE_ENTRY_POINT;
    
    TRACE_EXIT_POINT;
    return iFirstDayOfGrid;
    }

// ---------------------------------------------------------
// CCalenMonthGrid::SetFirstDayOfGrid
// Set argument aDay to first day of Grid
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCalenMonthGrid::SetFirstDayOfGrid(TTime aDay)
    {
    TRACE_ENTRY_POINT;
    
    iFirstDayOfGrid = aDay;
    
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------
// CCalenMonthGrid::CreateItemDrawerL
// Creates CFormattedCellListBoxItemDrawer,
// actually CCalenMonthCellListBoxItemDrawer.
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCalenMonthGrid::CreateItemDrawerL()
    {
    TRACE_ENTRY_POINT;
    
    CCalenMonthCellListBoxData* columnData = CCalenMonthCellListBoxData::NewL();
    CleanupStack::PushL( columnData );

    iItemDrawer = new(ELeave)
        CCalenMonthCellListBoxItemDrawer(Model(), this, iEikonEnv->NormalFont(), columnData);

    CleanupStack::Pop(); // columnData
    
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------
// 
// ---------------------------------------------------------
//
void CCalenMonthGrid::UpdateScrollBarsL()
    {
    TRACE_ENTRY_POINT;
    
    // Override default implementation and just turn scrollbars off
    // This is needed, because CAknGrid doesn't respect scrollbar
    // visibility settings, but turns them on e.g. in HandleResourceChange
    CEikScrollBarFrame* sbf = ScrollBarFrame();
    if ( sbf )
        {
        sbf->SetScrollBarVisibilityL(CEikScrollBarFrame::EOff, 
                                     CEikScrollBarFrame::EOff);
        }
    
    TRACE_EXIT_POINT;
    }
// End of File
