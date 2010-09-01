/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Layout calculations for week view listbox. 
*                Week view implementation differs from LAF specification model
*                and some calculations are needed.
*
*/


//debug
#include "calendarui_debug.h"

#include "calenweeklistboxlayout.h"

#include <aknlayoutscalable_apps.cdl.h>

#include "CalendarVariant.hrh"

#include <layoutmetadata.cdl.h>

// ======== LOCAL FUNCTIONS ========

// Coordinate system conversion functions 
// REMEMBER: in layout calculations we need changes of coordinate systems. These are easiest
//  to understand, when you think top-left corner of each rectangle as a vector. When you have two 
//  vectors P and R in one coordinate system , you can use subtraction P-R to calculate P as a vector
//  in coordinate system, where R is origo.
//
// E.g. if we have following rects: listbox_in_main_pane, and listscroll_pane_in_main_pane, you can
// calculate listbox_in_listscroll_pane = listbox_in_main_pane - listscroll_pane_in_main_pane.
// 
// Following two helper functions do this for you. 

// ---------------------------------------------------------------------------
// Returns rectangle aRectInOld in new coordinate system of aNewCoordSystem. 
// Both aRectInOld and aNewCoordSystemInOld has to be in same (old) coordinate system. 
//
// Example: If A is relative to X, and B is relative to X
// then ChangeOrigoToNew(A, B) gives A relative to B. 
// ---------------------------------------------------------------------------
static TRect ChangeOrigoToNew(TRect aRectInOld, TRect aNewCoordSystemInOld) 
    {
    TRACE_ENTRY_POINT;
    
    TPoint newOrigo = aRectInOld.iTl - aNewCoordSystemInOld.iTl;
    
    TRACE_EXIT_POINT;
    return TRect( newOrigo, aRectInOld.Size() );
    }

// ---------------------------------------------------------------------------
// Copied from AknUtils.cpp
// ---------------------------------------------------------------------------
TBool IsParentRelative(TInt aVal)
    {
    TRACE_ENTRY_POINT;
    
    TRACE_EXIT_POINT;
    return aVal > ELayoutP-ELayoutPRange && aVal <= ELayoutP;
    }

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TWeekListBoxLayout::TWeekListBoxLayout( TBool aUseExtraRowLayout, TBool aUseToolbarLayout
                                        , TBool aUsePreview, TRect aMainPane )
    : i_grid_cale_week_pane(), i_listscroll_cale_week_pane(), 
      iUseExtraRowLayout( aUseExtraRowLayout ), iUseToolbarLayout(aUseToolbarLayout )
      
    {
    TRACE_ENTRY_POINT;
    
    // Layout data items listscroll_cale_week_pane and grid_cale_week_pane are used lot, so we precalculate them
    TRect main_pane_Rect;

    if( aUsePreview )
        main_pane_Rect = aMainPane;
    else
        {
        TSize main_pane_Size;
        AknLayoutUtils::LayoutMetricsSize( AknLayoutUtils::EMainPane, main_pane_Size );
        main_pane_Rect = TRect( main_pane_Size );
        }

    TAknLayoutRect main_cale_week_pane;
    main_cale_week_pane.LayoutRect( main_pane_Rect, AknLayoutScalable_Apps::main_cale_week_pane().LayoutLine() );

    TInt layoutVariant = LayoutVariantIndex( EListScrollCaleWeekPane );
    i_listscroll_cale_week_pane.LayoutRect( 
        main_cale_week_pane.Rect(), 
        AknLayoutScalable_Apps::listscroll_cale_week_pane(layoutVariant).LayoutLine() );

    layoutVariant = LayoutVariantIndex( EGridCaleWeekPane );
    i_grid_cale_week_pane.LayoutRect( 
        i_listscroll_cale_week_pane.Rect(), 
        AknLayoutScalable_Apps::grid_cale_week_pane( layoutVariant ).LayoutLine() );
    
    TRACE_EXIT_POINT;
    }


// ---------------------------------------------------------------------------
// Week view grid rect relative to main pane.
// Used to handle touch events in week view
// 
// ---------------------------------------------------------------------------
TRect TWeekListBoxLayout::WeekGridRect()
    {
    TRACE_ENTRY_POINT;
    
    TRACE_EXIT_POINT;
    return i_grid_cale_week_pane.Rect();
    }


// ---------------------------------------------------------------------------
// List box rect relative to main pane.
// This is used directly to set list box size in CCalenWeekCont.
// 
// Layout data defines both week view grid and time pane relative to listscroll_cale_week_pane
// We calculate listbox size by combaining these two.
// ---------------------------------------------------------------------------
TRect TWeekListBoxLayout::ListBoxRect()
    {
    TRACE_ENTRY_POINT;
    
    TAknLayoutRect cale_week_time_pane;
    TInt layoutVariant = LayoutVariantIndex(ECaleWeekTimePane);
    cale_week_time_pane.LayoutRect( i_listscroll_cale_week_pane.Rect(), 
                                    AknLayoutScalable_Apps::cale_week_time_pane( layoutVariant ).LayoutLine() );

    // listbox = grid + time pane:
    TRect listBoxRect = i_grid_cale_week_pane.Rect();
    listBoxRect.BoundingRect( cale_week_time_pane.Rect() );
    
    TRACE_EXIT_POINT;
    return listBoxRect;
    }

// ---------------------------------------------------------------------------
// Return height of single line of list.
//
TInt TWeekListBoxLayout::ListItemHeight()
    {
    TRACE_ENTRY_POINT;
    
    // Calculate height between second and first row
    TAknLayoutRect first_row;
    const TInt layoutVariant = LayoutVariantIndex(ECellCaleWeekPane);
    first_row.LayoutRect( 
        i_grid_cale_week_pane.Rect(), 
        AknLayoutScalable_Apps::cell_cale_week_pane( 0, 0, layoutVariant ).LayoutLine() );

    TAknLayoutRect second_row;
    second_row.LayoutRect( 
        i_grid_cale_week_pane.Rect(), 
        AknLayoutScalable_Apps::cell_cale_week_pane( 0, 1, layoutVariant ).LayoutLine() );

    TInt height = second_row.Rect().iTl.iY - first_row.Rect().iTl.iY;

    // We assume that ListBoxRect is exactly 8 times list item height
    //ASSERT( 8*height == ListBoxRect().Height() );
    
    TRACE_EXIT_POINT;
    return height;
    }

// ---------------------------------------------------------------------------
// ListItemRect returns rect for list item line relative to main pane. 
// This is used to make coordinate system conversions between main pane and list item,
// because list's subcell rects are given relative to list item.
// ---------------------------------------------------------------------------
TRect TWeekListBoxLayout::ListItemRect()
    {
    TRACE_ENTRY_POINT;
    
    // list item rect is same as listbox, except height is reduced
    TRect result = ListBoxRect();
    result.SetHeight( ListItemHeight() );
    
    TRACE_EXIT_POINT;
    return result;
    }

// ---------------------------------------------------------------------------
// Equivalent to cale_week_time_pane_t1 
// --------------------------------------------------------------------------- 
TAknLayoutText TWeekListBoxLayout::HourTextLayout()
    {
    TRACE_ENTRY_POINT;

    TInt layoutVariant = LayoutVariantIndex(ECaleWeekTimePane);
    TAknLayoutRect cale_week_time_pane;
    cale_week_time_pane.LayoutRect( i_listscroll_cale_week_pane.Rect(), 
                                    AknLayoutScalable_Apps::cale_week_time_pane( layoutVariant ).LayoutLine() );

    TAknLayoutText cale_week_time_pane_t1;
    layoutVariant = LayoutVariantIndex(ECaleWeekTimePaneT1);
    cale_week_time_pane_t1.LayoutText( 
        cale_week_time_pane.Rect(),
        AknLayoutScalable_Apps::cale_week_time_pane_t1( layoutVariant ).LayoutLine() );

    
    TRACE_EXIT_POINT;
    return cale_week_time_pane_t1;
    }

// ---------------------------------------------------------------------------
// Layout rect of hour text subcell rect. Relative to list item rect
// Used for SetSubCellSizeL. Margins are calculated based on this and actual text rect layout.
// ---------------------------------------------------------------------------
TRect TWeekListBoxLayout::HourSubCellRect_in_ListItemRect()
    {
    TRACE_ENTRY_POINT;
    
    TAknLayoutText cale_week_time_pane_t1 = HourTextLayout();
    TRect hourCellRect_in_ListItem = 
        ChangeOrigoToNew( cale_week_time_pane_t1.TextRect(),
                          ListItemRect() );
    
    TRACE_EXIT_POINT;
    return hourCellRect_in_ListItem;
    }

// ---------------------------------------------------------------------------
// Layout rect for week grid cell relative to list item rect
// Used directly for SetSubcellSize. Used also for icon layout for memo, todo and anniversary icons. 
// ---------------------------------------------------------------------------

TRect TWeekListBoxLayout::GridSubCellRect_in_ListItemRect(TInt aSubCellIndex)
    {
    TRACE_ENTRY_POINT;
    const TInt layoutVariant = LayoutVariantIndex(ECellCaleWeekPane);
    // Subcell is fetched for first row
    TAknLayoutRect cell_cale_week_pane;
    cell_cale_week_pane.LayoutRect( i_grid_cale_week_pane.Rect(), 
                                    AknLayoutScalable_Apps::cell_cale_week_pane(aSubCellIndex, 0, layoutVariant).LayoutLine() );

    TRect cell_cale_week_pane_in_ListItemRect = ChangeOrigoToNew(cell_cale_week_pane.Rect(),
                                                                 ListItemRect());
    
    TRACE_EXIT_POINT;
    return cell_cale_week_pane_in_ListItemRect;
    }

// ---------------------------------------------------------
// 
// ---------------------------------------------------------
//
TBool TWeekListBoxLayout::UseExtraRowLayout() const
    {
    TRACE_ENTRY_POINT;
    
    TRACE_EXIT_POINT;
    return iUseExtraRowLayout;
    }

TBool TWeekListBoxLayout::UseToolbarLayout() const
    {
    TRACE_ENTRY_POINT;
    
    TRACE_EXIT_POINT;
    return iUseToolbarLayout;
    }

// -----------------------------------------------------------------------------
// ?classname::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt TWeekListBoxLayout::LayoutVariantIndex(TWeekLayoutVariant aLayout) const
    {
    TInt layoutVariant(0);
    TRACE_ENTRY_POINT;

    switch (aLayout)
        {
        case EListScrollCaleWeekPane:
            {
            if(UseToolbarLayout())
                layoutVariant = 1;
            else
                layoutVariant = 0;
            break;
            }
        case EPopupToolbarWindowCp01:
            {
            if(UseToolbarLayout())
                layoutVariant = 0;
            else
                __ASSERT_DEBUG(0, User::Invariant());
            break;
            }
        case EScrollPaneCp08:
        case EBgCalePaneCp01:
        case EGridCaleWeekPane:

        case ECaleWeekScrollPaneG1:
        case EBgCaleHeadingPane:

        case ECellCaleWeekPane:

        case ECaleWeekDayHeadingPane:
        case ECaleWeekDayHeadingPaneT1:

        case ECaleWeekTimePane:
        case ECaleWeekTimePaneT1:
        case EBgCaleSidePane:

        case ECellCaleWeekPaneG2:
 
            {
            if(UseToolbarLayout() && UseExtraRowLayout())
                layoutVariant = 3;
            else if(UseToolbarLayout())
                layoutVariant = 2;
            else if(UseExtraRowLayout())
                layoutVariant = 1;
            else
                layoutVariant = 0;
            break;
            }
        case EListScrollCaleWeekPaneT1:
            {
            if(UseToolbarLayout() && UseExtraRowLayout())
                layoutVariant = 1;
            else if(UseExtraRowLayout())
                layoutVariant = 0;
            else
                __ASSERT_DEBUG(0, User::Invariant());
            break;
            }
        }
    TRACE_EXIT_POINT;
    return layoutVariant;
    }

// End of File
