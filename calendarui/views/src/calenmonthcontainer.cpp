/*
* Copyright (c) 2002-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   For MonthView of the Calendar application.
 *
*/


//debug
#include "calendarui_debug.h"

// INCLUDE FILES
#include "calenmonthcontainer.h"
#include "calencontroller.h"
#include "calencontainerlayoutmanager.h"

#include <calenconstants.h>
#include <calendateutils.h>
#include "calendrawutils.h"
#include "calenmonthconst.h"
#include "calenmonthgrid.h"
#include "calenmonthview.h"
#include "calenpreview.h"
#include <calenagendautils.h>
#include "CalenUid.h"
#include "CleanupResetAndDestroy.h"
#include "CalenUid.h"
#include "calendar.hrh"
#include <calendar.mbg>
#include <Calendar.rsg>
#include <csxhelp/cale.hlp.hrh>
#include <calencontext.h>
#include <aknlists.h>
#include <akntoolbar.h>
#include <gulicon.h>
#include <AknIconArray.h>
#include <AknsFrameBackgroundControlContext.h>
#include <calinstance.h>
#include <calinstanceview.h>
#include <calenservices.h>
#include <calenviewutils.h>


// Layouts
#include <aknlayoutscalable_apps.cdl.h>


// LOCAL CONSTANTS AND MACROS
_LIT(KMonthCellFormat, "%F%*D\t");



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

// New line color groups in enhanced skinning
static void DrawPrimaryLine(CWindowGc& aGc, const TAknLayoutRect& aArea)
    {
    TRACE_ENTRY_POINT;

    DrawLAFLine(aGc, aArea, KAknsIIDQsnLineColors, EAknsCIQsnLineColorsCG1);

    TRACE_EXIT_POINT;
    }

// ================= MEMBER FUNCTIONS =======================

// ----------------------------------------------------------------------------
// CCalenMonthContainer::CCalenMonthContainer
// C++ default constructor can NOT contain any code, that might leave.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
CCalenMonthContainer::CCalenMonthContainer( CCalenNativeView* aView,
                                            TTime& aDate,
                                            MCalenServices& aServices )
    : CCalenContainer( aView, aServices ),
      iDate( aDate )
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenMonthContainer::~CCalenMonthContainer
// Destructor
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
CCalenMonthContainer::~CCalenMonthContainer()
    {
    TRACE_ENTRY_POINT;

    delete iGrid;
    delete iMonthDataArray;
    delete iBackgroundSkinContext;
    if( iNativePreview && iPreview )
        {
        delete iPreview;
        iPreview = NULL;
        }

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenMonthContainer::ChangesMidnightCrossover
// Called when Midnight cross over
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenMonthContainer::ChangesMidnightCrossover()
    {
    TRACE_ENTRY_POINT;
	
    TTime baseTime( TInt64(0) );
    TTime today = CalenDateUtils::Today();
    TTimeIntervalDays daysToToday( today.DaysFrom( baseTime ) );
    TTimeIntervalDays daysToGridStart( iFirstDayOfGrid.DaysFrom( baseTime ) );
    TInt dataIndex( daysToToday.Int() -daysToGridStart.Int() );
    TInt listBoxIndex( 0 );

    if( dataIndex >= 0 && dataIndex < KNumberOfRows * KCalenDaysInWeek )
        {
        listBoxIndex = static_cast<CAknGridView*>( iGrid->View() )->ListBoxIndex( dataIndex );
        iGrid->View()->DrawItem( listBoxIndex );
        }

    dataIndex--;

    if( dataIndex >= 0 && dataIndex < KNumberOfRows * KCalenDaysInWeek )
        {
        listBoxIndex = static_cast<CAknGridView*>( iGrid->View() )->ListBoxIndex( dataIndex );
        iGrid->View()->DrawItem( listBoxIndex );
        }

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenMonthContainer::SetDataL
// Set data from beginning.
// ----------------------------------------------------------------------------
//
void CCalenMonthContainer::InitDataForMonthL()
    {
    TRACE_ENTRY_POINT;
    
	SetActiveDayL( CCalenContainer::DateFromContextL( iServices.Context() ) );
    iView->BeginRepopulationL();
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenMonthContainer::SetActiveDayL
// Set iDate to argument's aDay,
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenMonthContainer::SetActiveDayL(TTime aDay)
    {
    TRACE_ENTRY_POINT;
	
	// test
	TDateTime aDayTime = aDay.DateTime();
    iDate = aDay;
    TLocale locale;

    // day number of iDate in month
    TInt dayNumberOfCurrentDay( iDate.DayNoInMonth() );
    // 1th of month of iDate
    TTime firstDay( iDate - TTimeIntervalDays(dayNumberOfCurrentDay) );
    TInt firstDayNoInWeek(firstDay.DayNoInWeek() - locale.StartOfWeek());
    if (firstDayNoInWeek < 0)
        {
        firstDayNoInWeek += KCalenDaysInWeek;
        }

    iFirstDayOfGrid = firstDay - TTimeIntervalDays(firstDayNoInWeek);
    
        //test
	TDateTime firstDayOfGrid =  iFirstDayOfGrid.DateTime();   
    
    iGrid->SetFirstDayOfGrid(iFirstDayOfGrid);

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenMonthContainer::DayOfIndex
// Return a day from Grid index
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
TTime CCalenMonthContainer::DayOfIndex(TInt aIndex)
    {
    TRACE_ENTRY_POINT;

    TRACE_EXIT_POINT;
    return iFirstDayOfGrid + TTimeIntervalDays(aIndex);
    }

// ----------------------------------------------------------------------------
// CCalenMonthContainer::SetCursorToActiveDay
// Sets cursor to Active day
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenMonthContainer::SetCursorToActiveDay()
    {
    TRACE_ENTRY_POINT;

    TTimeIntervalDays intervalDays( iDate.DaysFrom( iFirstDayOfGrid ) );
    TInt index = static_cast<CAknGridView*>( iGrid->View() )->ListBoxIndex( intervalDays.Int() );
    iGrid->SetCurrentItemIndex(index);

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenMonthContainer::ApplySelGridGraphicStyleL
// Month grid cells setup method
// (other items were commented in a header).
// ----------------------------------------------------------------------------
// FIXME: this should be renamed
void CCalenMonthContainer::ApplySelGridGraphicStyleL(CAknGrid& aListBox)
    {
    TRACE_ENTRY_POINT;

    // Setup subcells of grid.
    //
    // Subcell index | type      | purpose
    // 0             | text cell | day number
    // 1             | graphics  | triangle icon
    const TInt KDayNumberSubcell = 0;
    const TInt KTriangleIconSubcell = 1;

    TRect main_pane( ReducePreview( Rect() ) );

    TAknLayoutRect main_cale_month_pane;
    TInt layoutVariant = LayoutVariantIndex( EMainCaleMonthPane );
    main_cale_month_pane.LayoutRect( main_pane, AknLayoutScalable_Apps::main_cale_month_pane(layoutVariant).LayoutLine() );

    // Layout variant index to Table 4.38 Main cale month pane contents.
    TAknLayoutRect grid_cale_month_pane, cell_cale_month_pane;
    layoutVariant = LayoutVariantIndex( EGridCaleMonthPane );
    grid_cale_month_pane.LayoutRect( main_cale_month_pane.Rect(), AknLayoutScalable_Apps::grid_cale_month_pane( layoutVariant  ).LayoutLine() );

    TRect tmpRect = grid_cale_month_pane.Rect();
    //tmpRect.iTl.iY -= 10;
    layoutVariant = LayoutVariantIndex( ECellCaleMonthPane );
    cell_cale_month_pane.LayoutRect( tmpRect /*grid_cale_month_pane.Rect()*/, AknLayoutScalable_Apps::cell_cale_month_pane( layoutVariant, 0, 0 ).LayoutLine() );

    // cellRect for AknListBoxLayouts functions has to be _inside_ cell rect,
    // so we take only size to make origin to be (0,0)
    TRect cellRect(cell_cale_month_pane.Rect().Size());
    layoutVariant = LayoutVariantIndex( ECellCaleMonthPaneT1 );

    // Setup day number text cell
    AknListBoxLayouts::SetupFormTextCell( aListBox,
                                          aListBox.ItemDrawer(),
                                          KDayNumberSubcell,
                                          AknLayoutScalable_Apps::cell_cale_month_pane_t1( layoutVariant ).LayoutLine(),
                                          cellRect.iTl,
                                          cellRect.iBr );

    // Setup triangle icons
    TAknLayoutRect cell_cale_month_pane_g1;
    layoutVariant = LayoutVariantIndex( ECellCaleMonthPaneG1 );
    // Note: we pass cellRect as parent, not cell_cale_month_pane.Rect(), because AknListbox subcell layouting expect cell origin to be 0,0
    cell_cale_month_pane_g1.LayoutRect( cellRect, AknLayoutScalable_Apps::cell_cale_month_pane_g1(layoutVariant).LayoutLine() );  // <CDL LAYOUT>
    TRect triangleRect = cell_cale_month_pane_g1.Rect();

    SetupSelgridFormBitmapCellL( aListBox, KTriangleIconSubcell, triangleRect.iTl, triangleRect.Size() );
    CalenDrawUtils::SetIconSizes( cell_cale_month_pane_g1, iGrid->ItemDrawer()->FormattedCellData()->IconArray() );

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenMonthContainer::ChangeMonthL
// Change Active Day and Data.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenMonthContainer::ChangeMonthL()
    {
    TRACE_ENTRY_POINT;

    // Call diable redraw right before view repopulation starts to 
    // prevent flashing focus on cell
    iGrid->View()->SetDisableRedraw( ETrue );
    iView->BeginRepopulationL();
    // FIXME: Examine if SetCursorToActiveDay is needed for all cases, where
    // InitDataForMonthL or SetActiveDayL is called
	// Hide/unhide "today" toolbar item based on the focused day
    UpdateTodayToolbarItemL();

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenMonthContainer::SetupSelgridFormBitmapCellL
// Icon column setting for CAknGrid
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenMonthContainer::SetupSelgridFormBitmapCellL( CAknGrid& aListBox,
                                                        TInt aIndex,
                                                        TPoint aP1,
                                                        TSize aSize )
    {
    TRACE_ENTRY_POINT;

    CFormattedCellListBoxData* cellData =
        aListBox.ItemDrawer()->FormattedCellData();

    cellData->SetGraphicsSubCellL(aIndex,ETrue);
    cellData->SetSubCellPositionL(aIndex, aP1);
    cellData->SetSubCellSizeL(aIndex, aSize);
    cellData->SetSubCellAlignmentL(aIndex, CGraphicsContext::ECenter);

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenMonthContainer::DrawColLabel
// Drawing days of the week.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenMonthContainer::DrawColLabel( CWindowGc& aGc, TBool /*aUseWeeks*/ ) const
    {
    TRACE_ENTRY_POINT;

    TRect main_pane( ReducePreview( Rect() ) );

    TAknLayoutRect cale_month_day_heading_pane;
    TAknLayoutRect main_cale_month_pane;
    TInt layoutVariant = LayoutVariantIndex( EMainCaleMonthPane );
    main_cale_month_pane.LayoutRect( main_pane, AknLayoutScalable_Apps::main_cale_month_pane(layoutVariant).LayoutLine() );

    layoutVariant = LayoutVariantIndex( ECaleMonthDayHeadingPane );
    cale_month_day_heading_pane.LayoutRect( main_cale_month_pane.Rect(), AknLayoutScalable_Apps::cale_month_day_heading_pane( layoutVariant ).LayoutLine() );
    TRect tmpRect = cale_month_day_heading_pane.Rect();

    // Draw weekday names
    TLocale locale;
    aGc.SetBrushStyle( CGraphicsContext::ENullBrush );
    TAknLayoutText layoutText;
    TRgb textColor;
    MAknsSkinInstance* skinInst = AknsUtils::SkinInstance();
    TRect parent( main_pane );

    TAknLayoutScalableTableLimits limits = AknLayoutScalable_Apps::cale_month_day_heading_pane_t_Limits();
    TInt index = limits.FirstIndex();
    TInt end = limits.LastIndex() +1;
    TRect parentRect = tmpRect /*cale_month_day_heading_pane.Rect()*/;

    for( ; index < end; ++index )
        {
        // Calculate day number of week
        TInt dayNoInWeek( index + locale.StartOfWeek() );
        if (dayNoInWeek >= KCalenDaysInWeek)
            {
            dayNoInWeek -= KCalenDaysInWeek;
            }

        // Layout
        layoutVariant = LayoutVariantIndex( ECaleMonthDayHeadingPaneT );
        layoutText.LayoutText( parentRect, AknLayoutScalable_Apps::cale_month_day_heading_pane_t( index, layoutVariant ).LayoutLine() );

        // Get colors
        textColor = layoutText.Color();
        AknsUtils::GetCachedColor( skinInst, textColor,
                                   KAknsIIDQsnTextColors, EAknsCIQsnTextColorsCG23);
        
        TRAPD(error,
        const CDesCArrayFlat& abbArray = MonthView().AbbreviatedDayArrayL();
        // Draw correct week day name
        layoutText.DrawText( aGc,
                             abbArray[dayNoInWeek],
                             ETrue,
                             textColor );
                             );
        if(error!=KErrNone)
			{
			// Do nothing to avoid warning	
			}
        }

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenMonthContainer::DrawRowLabel
// Drawing week numbers.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenMonthContainer::DrawRowLabel( CWindowGc& aGc ) const
    {
    TRACE_ENTRY_POINT;

    TRect main_pane( ReducePreview( Rect() ) );

    TAknLayoutRect main_cale_month_pane;
    TInt layoutVariant = LayoutVariantIndex( EMainCaleMonthPane );
    main_cale_month_pane.LayoutRect( main_pane, AknLayoutScalable_Apps::main_cale_month_pane(layoutVariant).LayoutLine() );

    MAknsSkinInstance* skinInst = AknsUtils::SkinInstance();

    // clear week number area
    TRect weekArea;

    // Side (week) pane layout
    TAknLayoutRect cale_month_week_pane;
    layoutVariant = LayoutVariantIndex( ECaleMonthWeekPane );
    cale_month_week_pane.LayoutRect( main_cale_month_pane.Rect(), AknLayoutScalable_Apps::cale_month_week_pane( layoutVariant ).LayoutLine() );

    AknsDrawUtils::Background( skinInst, AknsDrawUtils::ControlContext( this ), NULL, aGc, 
                               cale_month_week_pane.Rect(), KAknsDrawParamNoClearUnderImage );

    TAknLayoutText weekNumberLayout;

    TAknLayoutScalableTableLimits cale_month_week_pane_t_Limits = AknLayoutScalable_Apps::cale_month_week_pane_t_Limits();
    TInt first = cale_month_week_pane_t_Limits.FirstIndex();
    TInt last  = cale_month_week_pane_t_Limits.LastIndex();

    for ( TInt i = first; i <= last; ++i )
        {
        // Calculate week number
        TTime currentWeek( iFirstDayOfGrid + TTimeIntervalDays( KCalenDaysInWeek * i) );
        TInt64 weekNumber( currentWeek.WeekNoInYear() );

        // Visual stirng
        TBuf<3> weekNumberBuffer;
        weekNumberBuffer.Num( weekNumber );
        AknTextUtils::DisplayTextLanguageSpecificNumberConversion( weekNumberBuffer );

        // Layout
        layoutVariant = LayoutVariantIndex( ECaleMonthWeekPaneT);
        weekNumberLayout.LayoutText( cale_month_week_pane.Rect(), AknLayoutScalable_Apps::cale_month_week_pane_t( i, layoutVariant ).LayoutLine() );


        // Decide color
        TRgb textColor = weekNumberLayout.Color();
        AknsUtils::GetCachedColor( skinInst, textColor,
                                   KAknsIIDQsnTextColors, EAknsCIQsnTextColorsCG23);

        weekNumberLayout.DrawText( aGc, weekNumberBuffer, ETrue, textColor );
        }

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenMonthContainer::SetDateL
// Set Date(1-28,29,30,31) to iMonthDataArray.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenMonthContainer::SetDateL()
    {
    TRACE_ENTRY_POINT;

    iMonthDataArray->Reset();
    for (TInt i(0); i < KNumberOfRows * KCalenDaysInWeek; ++i)
        {
        TTime currentDay(iFirstDayOfGrid + TTimeIntervalDays(i));
        TCalenMonthData element(currentDay);
        iMonthDataArray->InsertL(i, element);
        }

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenMonthContainer::HandleDayChangeL
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenMonthContainer::HandleDayChangeL( TTime aNewActiveDay )
    {
    TRACE_ENTRY_POINT;

    SetActiveDayL( aNewActiveDay );

    // dont focus the preview popup/previewpane if any dialog or faster app is active
    if( !iView->IsEditorActiveOrFasterAppExit())
    	{
    	iPreview->FocusChangedL( aNewActiveDay );
    	}

    UpdateStatusPaneAndExtensionsL();
	// Hide/unhide "today" toolbar item based on the focused day
    UpdateTodayToolbarItemL();

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenMonthContainer::NotifyChangeDateL
// Date change operation notification handler.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenMonthContainer::NotifyChangeDateL()
    {
    TRACE_ENTRY_POINT;

    TTime newActiveDay = iServices.Context().FocusDateAndTimeL().TimeLocalL();
	
	//test
	TDateTime newActiveDayTime = newActiveDay.DateTime();
    if ( CalenDateUtils::OnSameMonth( iDate, newActiveDay ) )
        {
        HandleDayChangeL(newActiveDay);
        SetCursorToActiveDay();
        // Changed DrawDeferred to Invalidate
        Window().Invalidate();
        }
    else
        {
        iGrid->View()->SetDisableRedraw( ETrue );
        iView->BeginRepopulationL();
        }
    
    // Hide/unhide "today" toolbar item based on the focused day
    UpdateTodayToolbarItemL();
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenMonthContainer::CountComponentControls
// return number of components
// ----------------------------------------------------------------------------
TInt CCalenMonthContainer::CountComponentControls() const
    {
    TRACE_ENTRY_POINT;
    
    TInt controlCount;
    
    if ( iLayoutManager->ControlOrNull() )
        {
        controlCount = 2;
        }
    else
        {
        controlCount = 1;
        }

    TRACE_EXIT_POINT;
    return controlCount;    
    }

// ----------------------------------------------------------------------------
// CCalenMonthContainer::ComponentControl
// retrun the component specified by aIndex
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
CCoeControl* CCalenMonthContainer::ComponentControl(TInt aIndex) const
    {
    TRACE_ENTRY_POINT;

    CCoeControl* control( NULL );

    switch (aIndex)
        {
        case 0:
            control = iGrid;
            break;
        
        case 1:
            control = iLayoutManager->ControlOrNull();
            break;
        
        default:
            __ASSERT_DEBUG( 0, User::Invariant() );
            break;
        }
        
    TRACE_EXIT_POINT;
    return control;
    }

// ----------------------------------------------------------------------------
// CCalenMonthContainer::OfferKeyEventL
// processing of a key event
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
TKeyResponse CCalenMonthContainer::OfferKeyEventL( const TKeyEvent& aKeyEvent,
                                                   TEventCode aType )
    {
    TRACE_ENTRY_POINT;

    if (aType == EEventKeyDown)
        {
        iPreview->Hide();
        }

    // Common view key handling - view switching etc.
    TKeyResponse exitCode( EKeyWasNotConsumed );
    
    exitCode = CCalenContainer::OfferKeyEventL( aKeyEvent, aType );


    if ( exitCode == EKeyWasNotConsumed && aType == EEventKey )
        {
        switch (aKeyEvent.iCode)
            {
            case EKeyOK:
                {
                iServices.IssueCommandL( ECalenForwardsToDayView );
                exitCode = EKeyWasConsumed;
                }
                break;
            case EKeyEnter:    
	        			{
	            	iView->HandleCommandL(EAknSoftkeyOpen);
	            	exitCode = EKeyWasConsumed;
	            	}
	            	break;
            case EKeyUpArrow:       /* fall through... */
            case EKeyDownArrow:     /* fall through... */
            case EKeyRightArrow:    /* fall through... */
            case EKeyLeftArrow:
#ifdef SUPPORT_NAVISCROLL_EVENTS
            case EKeyNext:
            case EKeyPrevious:
#endif
                {
                // Update the view's data.
               	exitCode =  HandleAttemptToMoveL( aKeyEvent.iCode );                
                }
                break;
            default:
                break;
            }
        }
        
        
        // grid key handling
    if (exitCode == EKeyWasConsumed || !iGrid)
        {
        // do nothing
        }
    else
        {
        // Attempt to move the highlight of the AknGrid.
        TInt oldIndex( iGrid->CurrentItemIndex() );
        CAknGridM* gridModel = static_cast<CAknGridM*>( iGrid->Model() );
        if ( gridModel->NumberOfData() != 0 )
            {
            exitCode = iGrid->OfferKeyEventL(aKeyEvent, aType);
            }
            
            
		if (exitCode == EKeyWasConsumed)
            {
            TInt newIndex(iGrid->CurrentItemIndex());
            if (oldIndex != newIndex)
                {
                TInt gridIndex = GridIndex();
                TTime newActiveDay(iFirstDayOfGrid +
                                   TTimeIntervalDays(gridIndex));
                HandleDayChangeL(newActiveDay);
                }
            }
        }


    TRACE_EXIT_POINT;
    return exitCode;
    }

// ----------------------------------------------------------------------------
// CCalenMonthContainer::HandleAttemptToMoveL
// Try to move by the given amount of days.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
TKeyResponse CCalenMonthContainer::HandleAttemptToMoveL( TUint aKeyCode )
    {
    TRACE_ENTRY_POINT;

    const TBool mirrored( AknLayoutUtils::LayoutMirrored() );
    TInt intervalDays( 0 );
    switch( aKeyCode )
        {
        case EKeyUpArrow:
            intervalDays = -KCalenDaysInWeek;
            break;
        case EKeyDownArrow:
            intervalDays = KCalenDaysInWeek;
            break;
        case EKeyRightArrow:
            intervalDays = mirrored ? -1 : 1;
            break;
        case EKeyLeftArrow:
            intervalDays = mirrored ? 1 : -1;
            break;
        default:
            ASSERT( EFalse );
            break;
        }

    TTime newDay( iDate + TTimeIntervalDays( intervalDays ) );

    if ( !CalenDateUtils::IsValidDay( newDay ) )
        {
        CalenViewUtils::ShowDateOutOfRangeErrorNoteL();

        TRACE_EXIT_POINT;
        return EKeyWasConsumed;
        }
	// test
	TDateTime newDayTime = newDay.DateTime();
    TCalTime time;
    time.SetTimeLocalL( newDay );
    iServices.Context().SetFocusDateL( time,
                                          TVwsViewId( KUidCalendar, KUidCalenMonthView ) );

    // Check to see if we go over a month boundary.
    TMonth activeMonth( iDate.DateTime().Month() );
    TMonth newMonth( newDay.DateTime().Month() );
    if ( activeMonth != newMonth )
        {
        ChangeMonthL();
        TRACE_EXIT_POINT;        
        return EKeyWasConsumed;
        }

    TRACE_EXIT_POINT;
    return EKeyWasNotConsumed;
    }

// ----------------------------------------------------------------------------
// CCalenMonthContainer::Draw
// drawing month view
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenMonthContainer::Draw(const TRect& /*aRect*/) const
    {
    TRACE_ENTRY_POINT;

    const TBool useWeeks( UseWeeks() );

    CWindowGc& gc = SystemGc();
    TAknLayoutRect area;
    MAknsSkinInstance* skinInst = AknsUtils::SkinInstance();

    TRect main_pane( ReducePreview( Rect() ) );


    MAknsControlContext* cc = AknsDrawUtils::ControlContext( this );
    AknsDrawUtils::Background( skinInst,
                               cc,
                               NULL,
                               gc,
                               Rect()
                               );

    gc.SetPenStyle( CGraphicsContext::ENullPen );
    gc.SetBrushStyle( CGraphicsContext::ESolidBrush );


    // Draw week & day name separator lines


    TAknLayoutRect main_cale_month_pane;
    TInt layoutVariant = LayoutVariantIndex( EMainCaleMonthPane );
    main_cale_month_pane.LayoutRect( main_pane, AknLayoutScalable_Apps::main_cale_month_pane(layoutVariant).LayoutLine() );

    // draw vertical line between week numbers and grid elements
    if( useWeeks )
        {
        TAknLayoutRect cale_month_pane_g2;
        layoutVariant = LayoutVariantIndex( ECaleMonthPaneG2 );
        cale_month_pane_g2.LayoutRect( main_cale_month_pane.Rect(), AknLayoutScalable_Apps::cale_month_pane_g2( layoutVariant).LayoutLine() );

        DrawPrimaryLine(gc, cale_month_pane_g2);
        }

    // draw horizontal line between week days and grid elements
    TAknLayoutRect cale_month_pane_g1;
    layoutVariant = LayoutVariantIndex(ECaleMonthPaneG1);
    cale_month_pane_g1.LayoutRect( main_cale_month_pane.Rect(), AknLayoutScalable_Apps::cale_month_pane_g1( layoutVariant ).LayoutLine() );
    DrawPrimaryLine(gc, cale_month_pane_g1);

    // write week days.
    DrawColLabel( gc, useWeeks );

    // write week numbers
    if( useWeeks )
        {
        DrawRowLabel( gc );
        }

    TRACE_EXIT_POINT;
    }


// ----------------------------------------------------------------------------
// CCalenMonthContainer::GetHelpContext
// Gets help context
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenMonthContainer::GetHelpContext( TCoeHelpContext& aContext ) const
    {
    TRACE_ENTRY_POINT;

    aContext.iMajor = KUidCalendar;
    // This is specified in HRH file.
    aContext.iContext = KCALE_HLP_MONTH_VIEW;

    TRACE_EXIT_POINT;
    }

void CCalenMonthContainer::SizeChanged()
    {
    TRACE_ENTRY_POINT;

    PIM_TRAPD_HANDLE( DoSizeChangedL() );

    TRACE_EXIT_POINT;
    }
// ----------------------------------------------------------------------------
// CCalenMonthContainer::DoSizeChanged
// Resizes child controls
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenMonthContainer::DoSizeChangedL()
    {
    TRACE_ENTRY_POINT;

    // Setup grid layout
    CCalenContainer::SizeChanged( Rect() );
    
    iLayoutManager->GetLayoutAndExtensionL();

    TRect main_pane( ReducePreview( Rect() ) );
    
    TAknLayoutRect grid_cale_month_pane, cell, vLine, hLine;

    TAknLayoutRect main_cale_month_pane;
    TInt layoutVariant = LayoutVariantIndex( EMainCaleMonthPane );
    
    
    main_cale_month_pane.LayoutRect( main_pane, AknLayoutScalable_Apps::main_cale_month_pane( layoutVariant ).LayoutLine() );
         
    
    layoutVariant = LayoutVariantIndex( EGridCaleMonthPane );
    grid_cale_month_pane.LayoutRect( main_cale_month_pane.Rect(), AknLayoutScalable_Apps::grid_cale_month_pane(layoutVariant).LayoutLine() );
        
    TRect gridRect = grid_cale_month_pane.Rect();

    iGrid->SetRect( gridRect );

    layoutVariant = LayoutVariantIndex( ECellCaleMonthPane );
    cell.LayoutRect( gridRect, AknLayoutScalable_Apps::cell_cale_month_pane( layoutVariant, 0, 0 ).LayoutLine() );
    layoutVariant = LayoutVariantIndex( ECaleMonthPaneG3 );
    vLine.LayoutRect( main_cale_month_pane.Rect(), AknLayoutScalable_Apps::cale_month_pane_g3( layoutVariant ).LayoutLine() );
    layoutVariant = LayoutVariantIndex( ECaleMonthPaneG9 );
    hLine.LayoutRect( main_cale_month_pane.Rect(), AknLayoutScalable_Apps::cale_month_pane_g9( layoutVariant ).LayoutLine() );

    // This could be done somewhere else, e.g. in construction
    // and HandleResourceChange (for dynamic layout changes)
    iGrid->SetLayoutL( EFalse,                             // aVerticalOrientation
                       !AknLayoutUtils::LayoutMirrored(),  // aLeftToRight
                       ETrue,                              // aTopToBottom
                       KCalenDaysInWeek,                   // aNumOfItemsInPrimaryOrient
                       KNumberOfRows,                      // aNumOfItemsInSecondaryOrient
                       cell.Rect().Size(),                 // aSizeOfItems
                       vLine.Rect().Width(),               // aWidthOfSpaceBetweenItems=0
                       hLine.Rect().Height() );            // aHeightOfSpaceBetweenItems=0
                                                   


    // Setup grid cell texts and graphics properties
    ApplySelGridGraphicStyleL( *iGrid );
                         
    // Remove scrollbar
    CEikScrollBarFrame* frame = iGrid->ScrollBarFrame();
    if( frame )
        {
        frame->SetScrollBarVisibilityL( CEikScrollBarFrame::EOff, CEikScrollBarFrame::EOff );
        }

    // Set size for skin backgrounds
    if( iBackgroundSkinContext && iHeadingBgContext && iSideBgContext )
        {
        // Calendar background skin
        TAknLayoutRect bg_cale_pane_cp02;
        layoutVariant = LayoutVariantIndex( EBgCalePaneCp02);
        bg_cale_pane_cp02.LayoutRect( main_cale_month_pane.Rect(), AknLayoutScalable_Apps::bg_cale_pane_cp02( layoutVariant ).LayoutLine() );

        TRect tempRect = bg_cale_pane_cp02.Rect();

        TAknLayoutRect cale_bg_pane_g1; // skin-center
        cale_bg_pane_g1.LayoutRect( tempRect /*bg_cale_pane_cp02.Rect()*/, AknLayoutScalable_Apps::cale_bg_pane_g1().LayoutLine() );

        iBackgroundSkinContext->SetFrameRects( bg_cale_pane_cp02.Rect(), cale_bg_pane_g1.Rect() );


        // Heading pane skin
        TAknLayoutRect bg_cale_heading_pane_cp01;
        layoutVariant = LayoutVariantIndex( EBgCaleHeadingPaneCp01);
        bg_cale_heading_pane_cp01.LayoutRect( main_cale_month_pane.Rect(), AknLayoutScalable_Apps::bg_cale_heading_pane_cp01( layoutVariant ).LayoutLine() );

        tempRect = bg_cale_heading_pane_cp01.Rect();

        TAknLayoutRect bg_cale_heading_pane_g1;
        bg_cale_heading_pane_g1.LayoutRect( tempRect /*bg_cale_heading_pane_cp01.Rect() */ , AknLayoutScalable_Apps::bg_cale_heading_pane_g1().LayoutLine() );

        iHeadingBgContext->SetFrameRects( bg_cale_heading_pane_cp01.Rect(), bg_cale_heading_pane_g1.Rect() );

        // Side pane skin
        if(UseWeeks())
            {
            TAknLayoutRect cale_month_week_pane;
            layoutVariant = LayoutVariantIndex( ECaleMonthWeekPane );
            cale_month_week_pane.LayoutRect( main_cale_month_pane.Rect(), AknLayoutScalable_Apps::cale_month_week_pane( layoutVariant ).LayoutLine() );

            tempRect = cale_month_week_pane.Rect();
            TAknLayoutRect bg_cale_side_pane_cp01;
            layoutVariant = LayoutVariantIndex( EBgCaleSidePaneCp01 );
            bg_cale_side_pane_cp01.LayoutRect( tempRect /*cale_month_week_pane.Rect()*/, AknLayoutScalable_Apps::bg_cale_side_pane_cp01(layoutVariant).LayoutLine() );

            TAknLayoutRect bg_cale_side_pane_g1;
            bg_cale_side_pane_g1.LayoutRect( bg_cale_side_pane_cp01.Rect(), AknLayoutScalable_Apps::bg_cale_side_pane_g1().LayoutLine() );

            iSideBgContext->SetFrameRects( bg_cale_side_pane_cp01.Rect(), bg_cale_side_pane_g1.Rect() );
            }

        iBackgroundSkinContext->SetParentContext( iBgContext );
        iHeadingBgContext->SetParentContext( iBackgroundSkinContext );
        iSideBgContext->SetParentContext( iHeadingBgContext );
        }

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenMonthContainer::FocusChanged
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenMonthContainer::FocusChanged( TDrawNow aDrawNow )
    {
    TRACE_ENTRY_POINT;

    CCalenContainer::FocusChanged( aDrawNow );
    if( iGrid )
        {
        iGrid->SetFocus( IsFocused() );
        }

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenMonthContainer::HandleResourceChange
// Handles a resource change relative events
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenMonthContainer::HandleResourceChange(TInt aType)
    {
    TRACE_ENTRY_POINT;

    CCalenContainer::HandleResourceChange( aType );

    if ( aType == KAknsMessageSkinChange || aType == KEikDynamicLayoutVariantSwitch )
        {
        CArrayPtr<CGulIcon>* iconArray = iGrid->ItemDrawer()->FormattedCellData()->IconArray();
        if(iconArray)
            {
            iconArray->ResetAndDestroy();
            delete iconArray; // owned by CFormattedCellListBoxData
            iconArray = NULL; // to suppress codescanner error
            iGrid->ItemDrawer()->FormattedCellData()->SetIconArray(NULL);
            }

        TRAPD(leave, iconArray = CreateIconsL( iIconIndices ));
        if(!leave && iconArray)
            {
            iGrid->ItemDrawer()->FormattedCellData()->SetIconArray(iconArray);
            }

        SizeChanged();
        
        // Update date formats to status pane
	    TRAP_IGNORE(UpdateStatusPaneAndExtensionsL());
        
        // if we have layout change and resource change is allowed do the refresh
        if (aType == KEikDynamicLayoutVariantSwitch)
            {
            iLayoutManager->HandleResourceChange( aType );
            TRAP_IGNORE(iView->BeginRepopulationL());
            }
        }

    CCoeControl::HandleResourceChange( aType );

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenMonthContainer::ConstructImplL
// Third phase constructor.
// This function was called CCalenNativeView::ConstructL().
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenMonthContainer::ConstructImplL()
    {
    TRACE_ENTRY_POINT;

    iChangeMonth = EFalse;
    iDayFormat = KMonthCellFormat;

    iMonthDataArray = new(ELeave) CMonthDataArray(KCalenDaysInWeek * KNumberOfRows);

    TLocale locale;

    // Create preview pane
    TRect aRect = PreviewRectL();
    iPreview = iServices.CustomPreviewPaneL(aRect);
    if (iPreview == NULL) 
		{
		iPreview = CCalenPreview::NewL( iView, locale, iServices );
		iNativePreview = ETrue;
		}
     
    // grid
    iGrid = new(ELeave) CCalenMonthGrid(iFirstDayOfGrid,this);
    iGrid->SetContainerWindowL(*this);
    iGrid->ConstructL(this, CEikListBox::EMultipleSelection);

    iGrid->SetListBoxObserver(this );
    iGrid->DisableScrolling( ETrue );
    iGrid->DisableSingleClick( ETrue );


    CDesCArray* itemArray =
        static_cast<CDesCArray*>( iGrid->Model()->ItemTextArray() );
    _LIT(KDesAny, "a"); // Evasion of CAknGrid's bug
    itemArray->AppendL(KDesAny);

    iGrid->SetPrimaryScrollingType(
        CAknGridView::EScrollFollowsItemsAndLoops );
    iGrid->SetSecondaryScrollingType(
        CAknGridView::EScrollFollowsItemsAndLoops );


    CArrayPtr<CGulIcon>* iconArray = iGrid->ItemDrawer()->FormattedCellData()->IconArray();
    if(iconArray)
        {
        iconArray->ResetAndDestroy();
        delete iconArray; // owned by CFormattedCellListBoxData
        iconArray = NULL; // to suppress codescanner error
        iGrid->ItemDrawer()->FormattedCellData()->SetIconArray(NULL);
        }

    iGrid->ItemDrawer()->FormattedCellData()->SetIconArray( CreateIconsL( iIconIndices ));
    iGrid->View()->SetDisableRedraw(ETrue);

    PIM_TRAPD_HANDLE( ConstructBackgroundContextL() );

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenMonthContainer::ConstructBackgroundContextL
// Constructs the background context
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenMonthContainer::ConstructBackgroundContextL()
    {
    TRACE_ENTRY_POINT;

    TRect dummy(0,0,0,0);
    if( AknLayoutUtils::ScalableLayoutInterfaceAvailable() )
        {
        iHeadingBgContext = CAknsFrameBackgroundControlContext::NewL( KAknsIIDQsnFrCaleHeading, dummy, dummy, EFalse );
        iSideBgContext    = CAknsFrameBackgroundControlContext::NewL( KAknsIIDQsnFrCaleSide, dummy, dummy, EFalse );
        iBackgroundSkinContext = CAknsFrameBackgroundControlContext::NewL(
            KAknsIIDQsnFrCale, dummy, dummy, EFalse );
        }

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenMonthContainer::UpdateSize
// Do layout changes after calendar settings are
// changed.
// Implementation for month view without week numbers.
// ----------------------------------------------------------------------------
//
void CCalenMonthContainer::UpdateSize()
    {
    TRACE_ENTRY_POINT;

    SizeChanged();
    // Update date formats to status pane
    TRAPD(error,UpdateStatusPaneAndExtensionsL());
	if(error!=KErrNone)
		{
		// do avoid warning
		}

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenMonthContainer::MopSupplyObject()
// Pass skin information if needed.
// ----------------------------------------------------------------------------
//
TTypeUid::Ptr CCalenMonthContainer::MopSupplyObject(TTypeUid aId)
    {
    TRACE_ENTRY_POINT;
    TBool useWeeksBool = EFalse;
    useWeeksBool = UseWeeks();
    
    MAknsControlContext* cc = useWeeksBool ? iSideBgContext : iHeadingBgContext;

    if( !cc )
        {
        cc = iBgContext;
        }

    if( aId.iUid == MAknsControlContext::ETypeId && cc )
        {
        TRACE_EXIT_POINT;
        return MAknsControlContext::SupplyMopObject( aId, cc );
        }

    TRACE_EXIT_POINT;
    return CCoeControl::MopSupplyObject( aId );
    }

// ----------------------------------------------------------------------------
// CCalenMonthContainer::GridIndex
// ----------------------------------------------------------------------------
//
TInt CCalenMonthContainer::GridIndex() const
    {
    TRACE_ENTRY_POINT;

    TRACE_EXIT_POINT;
    return reinterpret_cast<CAknGridView*>( iGrid->View() )
                                  ->ActualDataIndex(iGrid->CurrentItemIndex());
    }

// ----------------------------------------------------------------------------
// CCalenMonthContainer::UseWeeks
// ----------------------------------------------------------------------------
TBool CCalenMonthContainer::UseWeeks() const
    {
    TRACE_ENTRY_POINT;

    TBool useWeeks( EFalse );
    
    TRAPD(err,useWeeks = UseWeeksL());
    if(err==KErrNone)
        {
        // to avoid warning
        }

    TRACE_EXIT_POINT;
    return useWeeks;
    }

// ----------------------------------------------------------------------------
// CCalenMonthContainer::UseWeeks
// ----------------------------------------------------------------------------
TBool CCalenMonthContainer::UseWeeksL() const
    {
    TRACE_ENTRY_POINT;
    
    TBool useWeeks( EFalse );
    
    // Week numbers are not shown if layout is mirrored
    // or monday isn't the default week start day
    if( !AknLayoutUtils::LayoutMirrored() )
        {
        CCalenSetting* setting(NULL);
        setting = CCalenSetting::InstanceL();
 
        CleanupReleasePushL( *setting ) ;

        if( setting->WeekFormat() == EMonday )
            {
            useWeeks = ETrue;
            }
        CleanupStack::PopAndDestroy( setting );
        }

    TRACE_EXIT_POINT;
    return useWeeks;
    }

// ----------------------------------------------------------------------------
// CCalenMonthContainer::LayoutVariantIndex
// ----------------------------------------------------------------------------
//
TInt CCalenMonthContainer::LayoutVariantIndex(TMonthLayoutVariant aLayout) const
    {
    TRACE_ENTRY_POINT;

    TBitFlags layoutFlags = iLayoutManager->LayoutFlags();

    TInt layoutVariant (ENoLayoutVariantSet);
    switch (aLayout)
        {
        case ENoLayoutVariantSet:
            {
            ASSERT(EFalse);
            }
        case EMainCaleMonthPane:
            {
            if( layoutFlags.IsSet( CCalenContainerLayoutManager::ECalenContainerToolbar ) )
                layoutVariant = 1;
            else
                layoutVariant = 0;
            break;
            }
        case ECaleMonthPaneT1:
            {
            if( layoutFlags.IsSet( CCalenContainerLayoutManager::ECalenContainerToolbar ) )
                {
                if(UseWeeks() && layoutFlags.IsSet( CCalenContainerLayoutManager::ECalenContainerInfobar ))
                    layoutVariant = 2;
                else if( layoutFlags.IsSet( CCalenContainerLayoutManager::ECalenContainerInfobar ))
                    layoutVariant = 3;
                else
                    __ASSERT_DEBUG(0, User::Invariant());
                }
            else
                {
                if(UseWeeks() && layoutFlags.IsSet( CCalenContainerLayoutManager::ECalenContainerInfobar ))
                    layoutVariant = 0;
                else if( layoutFlags.IsSet( CCalenContainerLayoutManager::ECalenContainerInfobar ))
                    layoutVariant = 1;
                else
                    __ASSERT_DEBUG(0, User::Invariant());
                }
            break;
            }
        case ECaleMonthWeekPane:
        case ECaleMonthPaneG2:
            {
            if( layoutFlags.IsSet( CCalenContainerLayoutManager::ECalenContainerToolbar ) )
                {
                if(UseWeeks() && layoutFlags.IsSet( CCalenContainerLayoutManager::ECalenContainerInfobar ))
                    layoutVariant = 3;
                else if(UseWeeks())
                    layoutVariant = 2;
                else
                    __ASSERT_DEBUG(0, User::Invariant());
                }
            else
                {
                if(UseWeeks() && layoutFlags.IsSet( CCalenContainerLayoutManager::ECalenContainerInfobar ))
                    layoutVariant = 1;
                else if(UseWeeks())
                    layoutVariant = 0;
                else
                    __ASSERT_DEBUG(0, User::Invariant());
                }
            break;
            }
        case EBgCalePaneCp02:
        case ECaleMonthDayHeadingPane:
        case EGridCaleMonthPane:
        case ECaleMonthPaneG:
        case ECaleMonthPaneG1:
        case ECaleMonthPaneG3:
        case ECaleMonthPaneG9:
        case EBgCaleHeadingPaneCp01:
        case ECaleMonthDayHeadingPaneT:
        case ECellCaleMonthPane:
            {
            if( layoutFlags.IsSet( CCalenContainerLayoutManager::ECalenContainerToolbar ) )
                {
                if(UseWeeks() && layoutFlags.IsSet( CCalenContainerLayoutManager::ECalenContainerInfobar ))
                    layoutVariant = 6;
                else if ( layoutFlags.IsSet( CCalenContainerLayoutManager::ECalenContainerInfobar ))
                    layoutVariant = 7;
                else if(UseWeeks())
                    layoutVariant = 4;
                else
                    layoutVariant = 5;
                }
            else
                {
                if(UseWeeks() && layoutFlags.IsSet( CCalenContainerLayoutManager::ECalenContainerInfobar ))
                    layoutVariant = 2;
                else if (layoutFlags.IsSet( CCalenContainerLayoutManager::ECalenContainerInfobar ))
                    layoutVariant = 3;
                else if(UseWeeks())
                    layoutVariant = 0;
                else
                    layoutVariant = 1;
                }
            break;
            }

        case ECaleMonthWeekPaneT:
        case EBgCaleSidePaneCp01:
            {
            if( layoutFlags.IsSet( CCalenContainerLayoutManager::ECalenContainerToolbar ) )
                {
                if(layoutFlags.IsSet( CCalenContainerLayoutManager::ECalenContainerInfobar ))
                    layoutVariant = 3;
                else
                    layoutVariant = 2;
                }
            else
                {
                if(layoutFlags.IsSet( CCalenContainerLayoutManager::ECalenContainerInfobar ))
                    layoutVariant = 1;
                else
                    layoutVariant = 0;
                }
            break;
            }

        case ECellCaleMonthPaneG1:
            {
            if( layoutFlags.IsSet( CCalenContainerLayoutManager::ECalenContainerToolbar ) )
                {
                if(UseWeeks() && layoutFlags.IsSet( CCalenContainerLayoutManager::ECalenContainerInfobar ))
                    layoutVariant = 6;
                else if(layoutFlags.IsSet( CCalenContainerLayoutManager::ECalenContainerInfobar ))
                    layoutVariant = 7;
                else if(UseWeeks())
                    layoutVariant = 4;
                else
                    layoutVariant = 5;
                }
            else
                {
                if(UseWeeks() && layoutFlags.IsSet( CCalenContainerLayoutManager::ECalenContainerInfobar ))
                    layoutVariant = 2;
                else if(layoutFlags.IsSet( CCalenContainerLayoutManager::ECalenContainerInfobar ))
                    layoutVariant = 3;
                else if(UseWeeks())
                    layoutVariant = 0;
                else
                    layoutVariant = 1;
                }
            break;
            }
        case ECellCaleMonthPaneT1:
            {
            if( layoutFlags.IsSet( CCalenContainerLayoutManager::ECalenContainerToolbar ) )
                {
                if(UseWeeks() && layoutFlags.IsSet( CCalenContainerLayoutManager::ECalenContainerInfobar ))
                    layoutVariant = 7;
                else if(layoutFlags.IsSet( CCalenContainerLayoutManager::ECalenContainerInfobar ))
                    layoutVariant = 8;
                else if(UseWeeks())
                    layoutVariant = 5;
                else
                    layoutVariant = 6;
                }
            else
                {
                if(UseWeeks() && layoutFlags.IsSet( CCalenContainerLayoutManager::ECalenContainerInfobar ))
                    layoutVariant = 3;
                else if(layoutFlags.IsSet( CCalenContainerLayoutManager::ECalenContainerInfobar ))
                    layoutVariant = 4;
                else if(UseWeeks())
                    layoutVariant = 0;
                else
                    layoutVariant = 2;
                }
            break;
            }
        }

    TRACE_EXIT_POINT;
    return layoutVariant;
    }

// ----------------------------------------------------------------------------
// CCalenMonthContainer::DoEntryTimeCompareL
// ----------------------------------------------------------------------------
//
TInt CCalenMonthContainer::DoEntryTimeCompareL( const CCalInstance& aInstance_1,
                                                const CCalInstance& aInstance_2 )
    {
    TRACE_ENTRY_POINT;

    TInt ret( EEqual );

    TTime time_1;
    TTime time_2;

    if( aInstance_1.Entry().EntryTypeL() == CCalEntry::ETodo )
        {
        time_1 = aInstance_1.EndTimeL().TimeUtcL();
        }
    else
        {
        time_1 = aInstance_1.StartTimeL().TimeUtcL();
        }

    if( aInstance_2.Entry().EntryTypeL() == CCalEntry::ETodo )
        {
        time_2 = aInstance_2.EndTimeL().TimeUtcL();
        }
    else
        {
        time_2 = aInstance_2.StartTimeL().TimeUtcL();
        }

    if( time_1 == time_2 )
        {
        ret = EEqual;
        }
    else if( time_1 > time_2 )
        {
        ret = EGreaterThan; // oldest first
        }
    else
        {
        ret = ELessThan;
        }

    TRACE_EXIT_POINT;
    return ret;
    }

// ----------------------------------------------------------------------------
// Utility function to compare entry times, handles Todos correctly.
// ----------------------------------------------------------------------------
//
TInt CCalenMonthContainer::EntryTimeCompare(const CCalInstance& aInstance_1,
                                            const CCalInstance& aInstance_2)
    {
    TRACE_ENTRY_POINT;

    TInt ret( EEqual );
    PIM_TRAPD_HANDLE( (ret = DoEntryTimeCompareL(aInstance_1, aInstance_2)) );

    TRACE_EXIT_POINT;
    return ret;
    }

// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
//
CCalenMonthView& CCalenMonthContainer::MonthView() const
    {
    TRACE_ENTRY_POINT;

    TRACE_EXIT_POINT;
    return *(static_cast<CCalenMonthView*>(iView));
    }


// ----------------------------------------------------------------------------
// CCalenMonthContainer::HandleNaviDecorationPointerEventL
// processing of a navi decoration event
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenMonthContainer::HandleNaviDecoratorEventL(TInt aEventID)
    {
    TRACE_ENTRY_POINT;

    if(AknLayoutUtils::PenEnabled())
        {
        TInt direction(0);
        if(aEventID == EAknNaviDecoratorEventLeftTabArrow)
            {
            if(AknLayoutUtils::LayoutMirrored())
				{
                direction = 1;
				}
            else
				{
                direction = -1;
				}
            }
            
        else if(aEventID == EAknNaviDecoratorEventRightTabArrow)
            { 
            if(AknLayoutUtils::LayoutMirrored())
				{
                direction = -1;
				}
            else
				{
                direction = 1;
				}
            }
        else
            return;
        /**
         * Tapping of navi arrows in month view
         * should change month.
         */
        TTime newDay( iDate + TTimeIntervalMonths(direction) );
		
		// test 
		TDateTime newDayTime = newDay.DateTime();
        
        // set the context
        TCalTime time;
		time.SetTimeLocalL( newDay );
		iServices.Context().SetFocusDateL( time,
                                         TVwsViewId( KUidCalendar, KUidCalenMonthView ) );

        
        if (!CalenDateUtils::IsValidDay(newDay))
            {
            CalenViewUtils::ShowDateOutOfRangeErrorNoteL();
            }
        else
            {
            iGrid->View()->SetDisableRedraw(ETrue);
            iView->BeginRepopulationL();
            }
		// Hide/unhide "today" toolbar item based on the focused day
        UpdateTodayToolbarItemL();
        }

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenMonthContainer::HandleWeekNumberTapL
// processing of a week number tap
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenMonthContainer::HandleWeekNumberTapL(const TPoint &aPosition)
    {
    TRACE_ENTRY_POINT;

    TInt layoutVariant( ENoLayoutVariantSet );
    if(AknLayoutUtils::PenEnabled())
        {
        /* Steps to implement week number tapping
         * 1. find out tapped row in week number area.
         * 2. change current day of the week active in new row
         * 3. open week view
         */
        TRect main_pane( ReducePreview( Rect() ) );

        TAknLayoutRect main_cale_month_pane;
        layoutVariant = LayoutVariantIndex( EMainCaleMonthPane );
        main_cale_month_pane.LayoutRect(main_pane,
                                        AknLayoutScalable_Apps::main_cale_month_pane(layoutVariant).LayoutLine());
        TAknLayoutRect cale_month_week_pane;
        layoutVariant = LayoutVariantIndex( ECaleMonthWeekPane);
        cale_month_week_pane.LayoutRect(main_cale_month_pane.Rect(),
                                        AknLayoutScalable_Apps::cale_month_week_pane(layoutVariant).LayoutLine());
        if(cale_month_week_pane.Rect().Contains(aPosition))
            {
            TInt tappedRow (0);
            tappedRow = (aPosition.iY - cale_month_week_pane.Rect().iTl.iY)/
                (iGrid->View()->ItemSize(iGrid->CurrentItemIndex()).iHeight);

            TAknLayoutScalableTableLimits cale_month_week_pane_t_limits =
                AknLayoutScalable_Apps::cale_month_week_pane_t_Limits();
            if(tappedRow > cale_month_week_pane_t_limits.LastIndex())
                tappedRow = cale_month_week_pane_t_limits.LastIndex();

            TTime newWeekFirst(iFirstDayOfGrid +
                               TTimeIntervalDays(KCalenDaysInWeek * tappedRow));
            iDate = newWeekFirst;
            TInt direction = newWeekFirst.WeekNoInYear() -
                iDate.WeekNoInYear();

            TTime newActiveDay(iDate +
                               TTimeIntervalDays(KCalenDaysInWeek * direction));
         if ( !CalenDateUtils::IsValidDay(newActiveDay) )
                {
                CalenViewUtils::ShowDateOutOfRangeErrorNoteL();
                return;
                }
            
            // set the context
            TCalTime time;
		    time.SetTimeLocalL( newActiveDay );
		    iServices.Context().SetFocusDateL( time,
        		                                 TVwsViewId( KUidCalendar, KUidCalenMonthView ) );
            
            SetActiveDayL( newActiveDay );
            iServices.IssueCommandL( ECalenWeekView );
            }
        }

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenMonthContainer::HandleLongTapEventL
// processing of a long tap event
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenMonthContainer::HandleLongTapEventL( const TPoint& /*aPenEventLocation*/,
                                                const TPoint& /*aPenEventScreenLocation*/ )
    {
    TRACE_ENTRY_POINT;


    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenMonthContainer::HandlePointerEventL
// processing of a pointer event
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenMonthContainer::HandlePointerEventL(const TPointerEvent& aPointerEvent)
    {
    TRACE_ENTRY_POINT;

    if(AknLayoutUtils::PenEnabled())
        {
        TInt pointerIndex(-1);
        TBool isItem (iGrid->View()->XYPosToItemIndex(aPointerEvent.iPosition, pointerIndex));
        CAknGridM* gridModel = static_cast<CAknGridM*>(iGrid->Model());

        if(gridModel->NumberOfData() <= 0)
            return;

        if(isItem == EFalse)
            /* Handle pointer event in week number area */
            {
            switch(aPointerEvent.iType)
                {
                case TPointerEvent::EButton1Down:
                    {
                    HandleWeekNumberTapL(aPointerEvent.iPosition);
                    break;
                    }
                default:
                    break;
                }
            }
        else
            /* Handle pointer event in months day area */
            {

            RDebug::Print( _L("pointerEvent.iModifiers = 0x%08x"),
                    aPointerEvent.iModifiers );
            // Event modifiers are ignored for now because
            // those are not needed but cause problems
            TPointerEvent pointerEvent( aPointerEvent );
            pointerEvent.iModifiers =
                ( aPointerEvent.iModifiers & ( ~EAllModifiers ) );
            TInt index( iGrid->CurrentItemIndex() );
            TTime isValidDay;
            if( TPointerEvent::EButton1Up == aPointerEvent.iType )
                {
                isValidDay = iOldFirstDayOfGrid +
                TTimeIntervalDays(pointerIndex);
                }
            else
                {
                iOldFirstDayOfGrid = iFirstDayOfGrid;
                isValidDay = iFirstDayOfGrid +
                TTimeIntervalDays(pointerIndex);
                }
            if (!CalenDateUtils::IsValidDay(isValidDay))
                {
                if( aPointerEvent.iType == TPointerEvent::EButton1Up )
                    {
                    CalenViewUtils::ShowDateOutOfRangeErrorNoteL();
                    }
                TRACE_EXIT_POINT;
                return;
                }

            if ( aPointerEvent.iType == TPointerEvent::EButton1Down
                    || aPointerEvent.iType == TPointerEvent::EDrag )
                {
                TTime newActiveDay(
                        iDate + TTimeIntervalDays( pointerIndex - index ) );
                TDateTime newActiveDayTime = newActiveDay.DateTime();
                
                // set the context
                TCalTime time;
                time.SetTimeLocalL( newActiveDay );
                iServices.Context().SetFocusDateL( time,
                        TVwsViewId( KUidCalendar, KUidCalenMonthView ) );
                TMonth activeMonth( iDate.DateTime().Month() );
                TMonth newMonth( newActiveDay.DateTime().Month() );
                if ( aPointerEvent.iType == TPointerEvent::EButton1Down )
                    {
                    iGrid->HandlePointerEventL( pointerEvent );
                    iPreview->Hide();
                    if ( pointerIndex != index )
                        {
                        iChangeMonth = EFalse;
                        if ( activeMonth != newMonth )
                            {
                            iChangeMonth = ETrue;   
                            iGrid->View()->SetDisableRedraw(ETrue);
                            iView->BeginRepopulationL();
                            }
                        HandleDayChangeL( newActiveDay );
                        }
                    }
                else if ( activeMonth == newMonth
                        && newActiveDay != iDate  && !iChangeMonth )
                    {
                    iGrid->HandlePointerEventL( pointerEvent );
                    // use grid index to make sure that correct grid 
                    // element is focused when dragged
                    TInt gridIndex = GridIndex();
                    newActiveDay =
                        ( iFirstDayOfGrid + TTimeIntervalDays( gridIndex ) );
                    HandleDayChangeL( newActiveDay );
                    iChangeMonth = EFalse;
                    }
                }
            else if ( aPointerEvent.iType == TPointerEvent::EButton1Up )
                {
                iGrid->HandlePointerEventL( pointerEvent );
                }
            }
        }

    TRACE_EXIT_POINT;
    }



// ----------------------------------------------------------------------------
// CCalenMonthContainer::HandleListBoxEventL
// ----------------------------------------------------------------------------
//
void CCalenMonthContainer::HandleListBoxEventL(
        CEikListBox* /*aListBox*/, TListBoxEvent aEventType )
    {
    switch ( aEventType )
        {
        case ( MEikListBoxObserver::EEventItemDoubleClicked ):
            {
            if ( !iView->MenuBar()->IsDisplayed() )
                {
                iServices.IssueCommandL( ECalenForwardsToDayView );
                }
            break;
            }
        }
    }



void CCalenMonthContainer::HidePopup()
    {
    TRACE_ENTRY_POINT;
    
    iPreview->Hide();
    
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenMonthContainer::RedrawPreviewPane
// Redraws the preview pane
// ----------------------------------------------------------------------------
void CCalenMonthContainer::RedrawPreviewPane()
	{
    // dont focus the preview popup/previewpane if any dialog or faster app is active
	if(iPreview && ! iView->IsEditorActiveOrFasterAppExit())
		{
		TRAPD(error,iPreview->FocusChangedL( iServices.Context().FocusDateAndTimeL().TimeLocalL() ) );
		if(error!=KErrNone)
			{
			// Do nothing to avoid warning
			}
		}
	}

// ----------------------------------------------------------------------------
// CCalenMonthContainer::PrepareForPopulationL
// First population step (with no instance view).
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenMonthContainer::PrepareForPopulationL()
    {
    TRACE_ENTRY_POINT;

    iGrid->View()->SetDisableRedraw( ETrue );
    HidePopup();
    UpdateSize();
    SetActiveDayL( CCalenContainer::DateFromContextL( iServices.Context() ) );
    SetDateL();

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenMonthContainer::PopulateWithInstanceViewL
// Second population step (first with instance view).
// Populates data from agenda server and sets number of notes.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenMonthContainer::PopulateWithInstanceViewL()
    {
    TRACE_ENTRY_POINT;
    RArray<TInt> colIdArray;
    CCalenNativeView::GetActiveCollectionidsL( iServices, colIdArray );
    
    CCalInstanceView* instView = NULL;
    if( colIdArray.Count() > 0 )
        {
        instView = iServices.InstanceViewL( colIdArray );
        }
    
    colIdArray.Reset();
    
    if( !instView )
        {
        TRACE_EXIT_POINT;
        return;
        }

    // 1) Find passed incompleted todos
    RPointerArray<CCalInstance> list;
    CleanupResetAndDestroyPushL( list );

    TCalTime dummy;
    CalCommon::TCalTimeRange dayRange( dummy, dummy );

    const TTime today( CalenDateUtils::Today() );
    const TTime gridStart( CalenDateUtils::BeginningOfDay( iFirstDayOfGrid ) );

    const TInt todayIndex( today.DaysFrom( gridStart ).Int() ); // grid index for "today"
    const TInt cellCount( KCalenDaysInWeek * KNumberOfRows );

    CalCommon::TCalViewFilter filter;
    
    //If we are viewing months in the past
    if( todayIndex >= cellCount )
    	{
        filter = CalCommon::EIncludeEvents | 
                 CalCommon::EIncludeAppts  |
                 CalCommon::EIncludeReminder |
                 CalCommon::EIncludeAnnivs;
    	}
    else //else we are viewing current or future months
    	{
        filter = CalCommon::EIncludeEvents |
                 CalCommon::EIncludeIncompletedTodos  |
                 CalCommon::EIncludeAppts  |
                 CalCommon::EIncludeReminder |
                 CalCommon::EIncludeAnnivs;    
    	}

  	// Find To Dos, Meetings, Reminders, Anniversaries and Day Notes
    CalenDateUtils::GetDayRangeL( gridStart,
                                  gridStart + TTimeIntervalDays(cellCount-1),
                                  dayRange );
    instView->FindInstanceL( list, filter, dayRange );
	
	//Parse fetched items
    if( list.Count() )
   		{
    	CCalInstance* instance = NULL;
    	for (TInt i(0); i < list.Count(); i++)	
    		{
    		instance = list[i];
    		CCalEntry& entry = instance->Entry();
         	//Parse  Day Notes, Appointments and Reminders
         	//Start and end time of Event/Appointment/Reminders has to be indentified before marking the tags
         	//EEvents/Appointments can span multiple days, while Anniversaries cannot.
         	if( ( entry.EntryTypeL() == CCalEntry::EEvent    ) ||
         	    ( entry.EntryTypeL() == CCalEntry::EAppt     ) ||
         	    ( entry.EntryTypeL() == CCalEntry::EReminder ) )
         		{
         		// The start time of the instance
         	    TTime instStartTime = instance->StartTimeL().TimeLocalL();
         	    // The instance's start index from the grid start         	
         		TInt startIndex(instStartTime.DaysFrom(gridStart).Int());
         		// The end time of the instance
         		TTime instEndTime = instance->EndTimeL().TimeLocalL();

         		if( CalenAgendaUtils::EndsAtStartOfDayL( instance, instEndTime ) )
		            {
		            // prevent problems with items ending tomorrow at 00:00
		            instEndTime -= TTimeIntervalMinutes( 1 );

		            if( instEndTime <= gridStart )
		                {
		                // ends at the start of the grid -> not visible
		                continue; // skip
		                }
		            }
         		// The instance's end index from the grid start         	
         		TInt endIndex(instEndTime.DaysFrom(gridStart).Int());

         		// Mark the days from start index to end index
         		for( ; startIndex <= endIndex; startIndex++)
         			{
         			if( startIndex >= 0 && startIndex < cellCount )
	         			{
	         			iMonthDataArray->At( startIndex ).SetHasEvents( ETrue );	
	         			}
         			}
         		}
         	//Parse Anniversaries
         	else if( entry.EntryTypeL() == CCalEntry::EAnniv )
         		{
	            // The start time of the instance
	            TTime instStartTime = instance->StartTimeL().TimeLocalL();
	         	// The instance's start index from the grid start         	
	         	TInt index(instStartTime.DaysFrom(gridStart).Int());
	         	// Mark the particular day
		        if( index >= 0 && index < cellCount)
		        	{
	                iMonthDataArray->At( index ).SetHasEvents( ETrue );
		        	}
         		}
         	//Parse To DOs
         	else if( entry.EntryTypeL() == CCalEntry::ETodo )
         		{
	            // The start time of the instance
	            TTime instStartTime = instance->StartTimeL().TimeLocalL();
	            TInt index;
	            // Mark the particular day if the instance's start time is not in the past
	            if( instStartTime >= today )
	            	{
		            index = instStartTime.DaysFrom(gridStart).Int();
	            	}
	            else  // Mark for today if the instance's start time is in the past
	            	{
	            	index = todayIndex;
	            	}
		        if( index >= 0 && index < cellCount)
		        	{
	                iMonthDataArray->At( index ).SetHasEvents( ETrue );
		        	}
    	    	}
    		}
    	}
    list.ResetAndDestroy();

    //Fetch incomplete To Dos if todayindex is not marked
	if( todayIndex >= 0 && todayIndex < cellCount )
    	{
        // If today is not marked
    	if( !iMonthDataArray->At( todayIndex ).HasEvents() )
    		{
		    // Find incomplete ToDos from MinTime() to end of this month
		    CalenDateUtils::GetDayRangeL( TCalTime::MinTime(),
		                                  today,
		                                  dayRange );
	        instView->FindInstanceL( list, CalCommon::EIncludeIncompletedTodos |
	                             // only fetch the first instance for repeating to-dos!
                                 CalCommon::EIncludeRptsNextInstanceOnly, dayRange );
	    
		    // If there exists any To do, the mark for today
		    if( list.Count() )
				{
		        iMonthDataArray->At( todayIndex ).SetHasEvents( ETrue );	   	
				}
		    list.ResetAndDestroy();
	    	}
    	}
    CleanupStack::PopAndDestroy( &list );

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenMonthContainer::SetIndicatorL
// Set event indicator to each day.
// Third population step (second with instance view).
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenMonthContainer::SetIndicatorL()
    {
    TRACE_ENTRY_POINT;

    // set data for note's indicators
    CDesCArray* itemArray =
        static_cast<CDesCArray*>( iGrid->Model()->ItemTextArray() );
    itemArray->Reset();

    TDateTime currentTime( iDate.DateTime() );
    TInt days(Time::DaysInMonth(currentTime.Year(), currentTime.Month()));
    TLocale locale;
    TTime firstDay(iDate -
                   TTimeIntervalDays(iDate.DayNoInMonth()));
    TInt start(firstDay.DayNoInWeek() - locale.StartOfWeek());
    if (start < 0)
        {
        start += KCalenDaysInWeek;
        }
    TInt end(days + start);

    TInt i(0);
    for ( ; i < KNumberOfRows * KCalenDaysInWeek; ++i )
        {
        TBuf<16> data; // Enough for day number (2) + '\t' + icon number
        TCalenMonthData orgData(iMonthDataArray->At(i));
        TTime time(orgData.Day());
        time.FormatL(data, iDayFormat);

        TIconIndex iconIndex = EIconNull;
        if (orgData.HasEvents())
            {
            if (i < start || i >= end )
                {
                iconIndex = EIconGrayed;
                }
            else
                {
                iconIndex =  EIconMany;
                }
            }
        else
            {
            iconIndex = EIconNull;
            }

        TBuf<3> icon;
        icon.Num(static_cast<TInt64>(iconIndex));
        data.Append(icon);

        itemArray->AppendL(data);
        }
    iGrid->HandleItemAdditionL();

    // if month of item of index is same as month of active day,
    // then item is selected,if not, item is deselected.
    // Deselected items will be writted in light.
    for ( i = 0; i < KNumberOfRows * KCalenDaysInWeek; ++i )
        {
        TCalenMonthData orgData(iMonthDataArray->At(i));
        TTime time(orgData.Day());

        TInt listBoxIndex(static_cast<CAknGridView*>(
                                      iGrid->View())->ListBoxIndex(i));

        if (time.DateTime().Month() == iDate.DateTime().Month())
            {
            iGrid->View()->SelectItemL(listBoxIndex);
            }
        else
            {
            iGrid->View()->DeselectItem(listBoxIndex);
            }
        }

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenMonthContainer::SetFocusL
// Sets the current focus.
// Fourth population step (third with instance view).
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenMonthContainer::SetFocusL()
    {
    TRACE_ENTRY_POINT;

    SetActiveDayL( CCalenContainer::DateFromContextL( iServices.Context() ) );
    iGrid->View()->SetDisableRedraw( EFalse );
    SetCursorToActiveDay();
    DrawDeferred();

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenMonthContainer::CompletePopulationL
// Completes population.
// Fifth population step (fourth with instance view).
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenMonthContainer::CompletePopulationL()
    {
    TRACE_ENTRY_POINT;
    
    AddToStackAndMakeVisibleL();
		
		iView->SetEditorActive( EFalse );
		
    // dont focus the preview popup/previewpane if any dialog or faster app is active
    if(!iView->IsEditorActiveOrFasterAppExit())
       {
       // preview popup is shown when application comes to foreground
       iPreview->FocusChangedL( iServices.Context().FocusDateAndTimeL().TimeLocalL() );
       }
    
    UpdateStatusPaneAndExtensionsL();
	// Hide/unhide "today" toolbar item based on the focused day
    UpdateTodayToolbarItemL();
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenMonthContainer::InfoBarRectL
// Returns the available info bar rect for this container
// ----------------------------------------------------------------------------
TRect CCalenMonthContainer::InfoBarRectL( TBool aToolbarAvailable )
    {
    TRACE_ENTRY_POINT;

    TRect rect = Rect();
    
    // Get the main pane
    TAknLayoutRect main_cale_month_pane;
    main_cale_month_pane.LayoutRect( Rect(), 
        AknLayoutScalable_Apps::main_cale_month_pane().LayoutLine() );

    TRect monthrect = main_cale_month_pane.Rect();

    // Create a dummy label to find the layout rect
    CEikLabel* dummyLabel = new( ELeave ) CEikLabel;
    CleanupStack::PushL( dummyLabel );

    // Get the layout variant for the month view infobar.
    // We assume that we have one for this check, as we need to
    // find the size that would be available if we do have one.
    TInt layoutVariant = 0;
    if( aToolbarAvailable )
        {
        layoutVariant = UseWeeks() ? 2 : 3 ;
        }
    else
        {
        layoutVariant = UseWeeks() ? 0 : 1 ;
        }
    
    // Layout the dummy label to set its size
    AknLayoutUtils::LayoutLabel( dummyLabel, main_cale_month_pane.Rect(),
        AknLayoutScalable_Apps::cale_month_pane_t1( layoutVariant ).LayoutLine() );
              
    TRect infoRect = dummyLabel->Rect();
    //TRect rect1;

    // Discard the label
    CleanupStack::PopAndDestroy( dummyLabel );
    
    TRACE_EXIT_POINT;
    return infoRect;
    }

// ----------------------------------------------------------------------------
// CCalenMonthContainer::CreateIconIndicesL
// Create icon index for day view
// ----------------------------------------------------------------------------
void CCalenMonthContainer::CreateIconIndicesL( RArray<MCalenServices::TCalenIcons>& aIndexArray)
    {
    TRACE_ENTRY_POINT;
    // Icons needed for the day view
    aIndexArray.Reset();
    aIndexArray.AppendL( MCalenServices::ECalenMonthManyEventIcon );
    aIndexArray.AppendL( MCalenServices::ECalenMonthGrayedEventIcon );

    TRACE_EXIT_POINT;
    }

// End of File
