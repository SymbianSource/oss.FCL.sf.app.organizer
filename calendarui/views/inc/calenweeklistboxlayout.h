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



#ifndef CALENWEEKLISTBOXLAYOUT_H
#define CALENWEEKLISTBOXLAYOUT_H

#include <AknUtils.h>
#include <e32std.h>

#include "CalendarVariant.hrh"

/**
 * TWeekListBoxLayout provides helper functions to calculate correct layout values
 * for week view listbox.
 * 
 * Long description below is divided to two sections:
 * 1) Rationale: Why listbox layout calculations are needed?
 * 2) (Naming) conventions explained 
 *
 *
 * 1) Rationale: Why listbox layout calculations are needed
 *    -----------------------------------------------------
 *
 * Week view implementation differs from LAF specification. 
 * LAF specification defines week view grid and time pane in the left side of grid. 
 * But implementation uses listbox which combines both grid and time pane.
 * 
 * Implementation implements week view as specially customized Uikon listbox. Uikon listboxes have
 * list items (= list rows), which have several subcells (rectangle inside row).
 * For week view listbox, first column is for hour labels, and it can't be focused.
 * Next 7 columns are for day columns of scrollable grid. Each row is one list item. 
 *
 * Following images might help you to imagine the idea: 
 *
 * Week view: 
 *
 *         Mo  Tu  We  Th  Fr  Sa  Su
 *  -----------------------------------
 *   8:00 |___|___|___|___|___|___|___|
 *   9:00 |___|___|_#_|___|___|___|___|
 *  10:00 |___|___|_#_|___|_#_|___|___|
 *  11:00 |___|___|___|___|_#_|___|___|
 * ... 
 *
 * Listbox: used in implementation, not defined in LAF
 * 
 *   8:00 |___|___|___|___|___|___|___|
 *   9:00 |___|___|_#_|___|___|___|___|
 *  10:00 |___|___|_#_|___|_#_|___|___|
 *  11:00 |___|___|___|___|_#_|___|___|
 * ... 
 *
 * Time pane: defined in LAF
 *   8:00 
 *   9:00 
 *  10:00 
 *  11:00 
 * ...
 *
 * Week view grid: defined in LAF
 *
 *  |___|___|___|___|___|___|___|
 *  |___|___|_#_|___|___|___|___|
 *  |___|___|_#_|___|_#_|___|___|
 *  |___|___|___|___|_#_|___|___|
 * ...
 *
 *
 * 2) (Naming) conventions explained 
 *    ------------------------------
 * 
 * Following conventions are used:
 * - TAknLayoutRect/TAknLayoutText are written similar way as Avkon Layout system functions:
 *   lower case, with underscore _ separating words. 
 *   e.g. TAknLayotRect cale_week_time_pane 
 * 
 * - TRect types has _Rect ending 
 *   e.g. TRect main_pane_Rect 
 * 
 * - cached (member variables) have i_ in the beginning
 *   e.g TAknLayoutRect i_grid_cale_week_pane
 * 
 * - Layout values that can't be read from layout data but has to be calculated are always TRects. 
 *   They have Rect ending. They are written in CamelCase, e.g. ListBoxRect
 * 
 * - all layout rects are main pane relative unless otherwise noted. Especially data that is read through layout APIs 
 *   is always main pane relative because parents are passed to LayoutRect functions
 * 
 * - if layout rect is not main_pane relative, it is indicated by writing layout rect that it is relative to in the end
 *   of name: e.g. hourCellRect_in_ListItemRect 
 *   We only need ListItemRect relative stuff, because listbox subcell rects are given as list item relative. 
 *
 *  @lib Calendar.exe
 *  @since S60 v3.0
 */

NONSHARABLE_CLASS( TWeekListBoxLayout )
    {
public:
    /**
     * Constructor
     */ 
    TWeekListBoxLayout( TBool aUseExtraRowLayout, TBool aUseToolbarLayout
                        , TBool aUsePreview, TRect aMainPane );

    /**
     * List box rect relative to main pane.
     * This is used directly to set list box size in CCalenWeekCont.
     * 
     * Layout data defines both week view grid and time pane relative to listscroll_cale_week_pane
     * We calculate listbox size by combaining these two.
     */ 
    TRect ListBoxRect();

    /**
     * Return height of single line of list.
     */
    TInt ListItemHeight();

    /**
     * ListItemRect returns rect for list item line relative to main pane. 
     * This is used to make coordinate system conversions between main pane and list item,
     * because list's subcell rects are given relative to list item.
     */ 
    TRect ListItemRect();


    /**
     * WeekGridRect returns rect for list item line relative to main pane (with out time). 
     * This is used to make coordinate system conversions between main pane and list item,
     * because list's subcell rects are given relative to list item.
     */
    TRect WeekGridRect();


    /**
     * Equivalent to cale_week_time_pane_t1 
     */ 
    TAknLayoutText HourTextLayout();

    /** 
     * Layout rect of hour text subcell rect. Relative to list item rect
     * Used for SetSubCellSizeL. Margins are calculated based on this and actual text rect layout.
     */
    TRect HourSubCellRect_in_ListItemRect();

    /**
     * Layout rect for week grid cell relative to list item rect
     * Used directly for SetSubcellSize. Used also for icon layout for memo, todo and anniversary icons. 
     * 
     */
    TRect GridSubCellRect_in_ListItemRect(TInt aSubCellIndex);


    /**
     * @returns ETrue, if extra row layout has to be used
     * Extra row layout is used for regional calendar variants. 
     */
    TBool UseExtraRowLayout() const;

    TBool UseToolbarLayout() const;

    enum TWeekLayoutVariant
        {
        EListScrollCaleWeekPane,
        EPopupToolbarWindowCp01,
        EScrollPaneCp08,
        EBgCalePaneCp01,
        EGridCaleWeekPane,
        ECaleWeekDayHeadingPane,
        ECaleWeekTimePane,
        ECaleWeekTimePaneT1,
        EBgCaleSidePane,
        ECaleWeekScrollPaneG1,
        EBgCaleHeadingPane,
        EListScrollCaleWeekPaneT1,
        ECellCaleWeekPane,
        ECaleWeekDayHeadingPaneT1,
        ECellCaleWeekPaneG2
        };

    /**
     * Return currently used variant index to LAF
     */    
    TInt LayoutVariantIndex(TWeekLayoutVariant) const;

    /**
     * BC - Return currently used variant index to LAF
     */    
    TInt LayoutVariantIndexBc( TWeekLayoutVariant aLayout ) const;

private: 
    TAknLayoutRect i_grid_cale_week_pane;
    TAknLayoutRect i_listscroll_cale_week_pane;
    TBool iUseExtraRowLayout;
    TBool iUseToolbarLayout;
    TBool iUsePreviewPane;
    };

#endif // CALENWEEKLISTBOXLAYOUT_H
