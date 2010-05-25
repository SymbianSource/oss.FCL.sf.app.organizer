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
* Description:  Container control for Week View .
 *
*/

#include <calcommon.h>
#include <caltime.h>
#include <calinstance.h>
#include <calentry.h>
#include <calinstanceview.h>
#include <calcalendarinfo.h>
#include <AknIconArray.h>
#include <AknsDrawUtils.h>
#include <AknsFrameBackgroundControlContext.h>
#include <AknUtils.h>
#include <AknIconUtils.h>
#include <eikenv.h>
#include <eikfrlbd.h>
#include <eiklabel.h>
#include <gulicon.h>
#include <StringLoader.h>
#include <calencontext.h>
#include <aknlayoutscalable_apps.cdl.h>
#include <caleninstanceid.h>            // TCalenInstanceId
#include <layoutmetadata.cdl.h>
#include <centralrepository.h>
#include <calenviewutils.h>
#include <calenservices.h>
#include <calenagendautils.h>
#include <calendateutils.h>
#include <mcalenpreview.h>
#include <Calendar.rsg>
#include <csxhelp/cale.hlp.hrh>

#include "calendarui_debug.h"
#include "calenweekcontainer.h"
#include "calencontainerlayoutmanager.h"
#include "calencontroller.h"
#include "calendrawutils.h"
#include "calenicons.h"
#include "calenpreview.h"
#include "CalenUid.h"
#include "CalendarPrivateCRKeys.h"
#include "calenweeklistboxview.h"
#include "calenweeklistbox.h"
#include "calenweeklistboxlayout.h"
#include "calenweekview.h"
#include "CalenUid.h"
#include "calendar.hrh"

//  LOCAL CONSTANTS AND MACROS
const TInt KNonTimedNoteDefautDisplayTime(8);
const TInt KNullMinutes = -1;
const TInt KBeforeActiveTime(0);
const TInt KHoursInADay( 24 );


// METHOD DEFINITIONS
// ----------------------------------------------------------------------------
// ?classname::?member_function
// ?implementation_description
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenWeekContainer::DrawBackground(CWindowGc& aGc) const
    {
    TRACE_ENTRY_POINT;

    MAknsControlContext* cc =AknsDrawUtils::ControlContext( this );

    if( cc )
        {
        TRect main_pane( Rect() );
        AknsDrawUtils::Background( AknsUtils::SkinInstance(),
                                   cc,
                                   NULL,
                                   aGc,
                                   main_pane );
        }

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// Draws horizontal and vertical grid lines
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenWeekContainer::DrawGridLines(CWindowGc& aGc) const
    {
    TRACE_ENTRY_POINT;
    
    TRect main_pane_Rect = ReducePreview( Rect() );

    TAknLayoutRect main_cale_week_pane;
    main_cale_week_pane.LayoutRect( main_pane_Rect, AknLayoutScalable_Apps::main_cale_week_pane().LayoutLine() );

    // FIXME: use member variable?
    TAknLayoutRect listscroll_cale_week_pane;
    TWeekListBoxLayout lay( UseInfobar(), UseToolbar(), UsePreview(), main_pane_Rect );
    TInt layoutVariant = lay.LayoutVariantIndex(TWeekListBoxLayout::EListScrollCaleWeekPane );
    listscroll_cale_week_pane.LayoutRect( main_cale_week_pane.Rect(), AknLayoutScalable_Apps::listscroll_cale_week_pane(layoutVariant).LayoutLine() );

    layoutVariant = lay.LayoutVariantIndex(TWeekListBoxLayout::ECaleWeekScrollPaneG1);
    const TRect parentRect = listscroll_cale_week_pane.Rect();
    // Primary lines were drawn in different functions
    TAknLayoutRect horizontalPrimaryLine;
    horizontalPrimaryLine.LayoutRect( parentRect, AknLayoutScalable_Apps::cale_week_scroll_pane_g1( layoutVariant ).LayoutLine() );

    TAknLayoutRect verticalPrimaryLine;
    verticalPrimaryLine.LayoutRect( parentRect, AknLayoutScalable_Apps::cale_week_scroll_pane_g2( layoutVariant ).LayoutLine() );

    TRgb lineColor( verticalPrimaryLine.Color() );
    AknsUtils::GetCachedColor(AknsUtils::SkinInstance(), lineColor,
                              KAknsIIDQsnLineColors,
                              EAknsCIQsnLineColorsCG1);
    aGc.SetBrushColor( lineColor );
    aGc.SetBrushStyle( CGraphicsContext::ESolidBrush );

    aGc.Clear( verticalPrimaryLine.Rect() );
    aGc.Clear( horizontalPrimaryLine.Rect() );

    // There are six secondary vertical lines
    const TInt verticalLineCount = 6;
    TAknLayoutRect verticalLines[ verticalLineCount ];
    verticalLines[0].LayoutRect( parentRect, AknLayoutScalable_Apps::cale_week_scroll_pane_g3( layoutVariant ).LayoutLine() );
    verticalLines[1].LayoutRect( parentRect, AknLayoutScalable_Apps::cale_week_scroll_pane_g4( layoutVariant ).LayoutLine() );
    verticalLines[2].LayoutRect( parentRect, AknLayoutScalable_Apps::cale_week_scroll_pane_g5( layoutVariant ).LayoutLine() );
    verticalLines[3].LayoutRect( parentRect, AknLayoutScalable_Apps::cale_week_scroll_pane_g6( layoutVariant ).LayoutLine() );
    verticalLines[4].LayoutRect( parentRect, AknLayoutScalable_Apps::cale_week_scroll_pane_g7( layoutVariant ).LayoutLine() );
    verticalLines[5].LayoutRect( parentRect, AknLayoutScalable_Apps::cale_week_scroll_pane_g8( layoutVariant ).LayoutLine() );

    // and seven secondary horizontal lines
    const TInt horizontalLineCount = 7;
    TAknLayoutRect horizontalLines[ horizontalLineCount ];
    horizontalLines[0].LayoutRect( parentRect, AknLayoutScalable_Apps::cale_week_scroll_pane_g9( layoutVariant ).LayoutLine() );
    horizontalLines[1].LayoutRect( parentRect, AknLayoutScalable_Apps::cale_week_scroll_pane_g10( layoutVariant ).LayoutLine() );
    horizontalLines[2].LayoutRect( parentRect, AknLayoutScalable_Apps::cale_week_scroll_pane_g11( layoutVariant ).LayoutLine() );
    horizontalLines[3].LayoutRect( parentRect, AknLayoutScalable_Apps::cale_week_scroll_pane_g12( layoutVariant ).LayoutLine() );
    horizontalLines[4].LayoutRect( parentRect, AknLayoutScalable_Apps::cale_week_scroll_pane_g13( layoutVariant ).LayoutLine() );
    horizontalLines[5].LayoutRect( parentRect, AknLayoutScalable_Apps::cale_week_scroll_pane_g14( layoutVariant ).LayoutLine() );
    horizontalLines[6].LayoutRect( parentRect, AknLayoutScalable_Apps::cale_week_scroll_pane_g15( layoutVariant ).LayoutLine() );

    lineColor = verticalLines[0].Color();
    AknsUtils::GetCachedColor(AknsUtils::SkinInstance(), lineColor,
                              KAknsIIDQsnLineColors,
                              EAknsCIQsnLineColorsCG2);
    aGc.SetBrushColor( lineColor );
    aGc.SetBrushStyle( CGraphicsContext::ESolidBrush );

    for (TInt i=0; i < verticalLineCount; ++i)
        {
        aGc.Clear( verticalLines[i].Rect() );
        }

    for (TInt i=0; i < horizontalLineCount; ++i)
        {
        aGc.Clear( horizontalLines[i].Rect() );
        }

    TRACE_EXIT_POINT;
    }



// ================= MEMBER FUNCTIONS =======================

// ----------------------------------------------------------------------------
// CCalenWeekContainer::CCalenWeekContainer
// C++ constructor can NOT contain any code, that might leave.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
CCalenWeekContainer::CCalenWeekContainer( CCalenNativeView* aView,
                                          TTime& aTime,
                                          TInt& aSelectedRowNumber,
                                          TTimeIntervalMinutes& aFirstRowTime,
                                          MCalenServices& aServices )
    : CCalenContainer( aView, aServices ),
//      iCurrentDay( TInt64(0) ),
      iStartDay( TInt64(0) ),
      iViewPopulationComplete( EFalse ),
      iTime( aTime ),
      iSelectedRowNumber( aSelectedRowNumber ),
      iFirstRowTime( aFirstRowTime )
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenWeekContainer::~CCalenWeekContainer
// Destructor
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
CCalenWeekContainer::~CCalenWeekContainer()
    {
    TRACE_ENTRY_POINT;

    delete iListBox;
    
    delete iDesArray;

    CleanupInstances();

    ResetSlotTable();
    delete iBackgroundSkinContext;

    TRACE_EXIT_POINT;
    }



// ---------------------------------------------------------
// CCalenWeekContainer::SetCursorToActiveDayL
// Sets cursor in Active day
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenWeekContainer::SetCursorToActiveDayL()
    {
    TRACE_ENTRY_POINT;

    iTime = CCalenContainer::DateFromContextL( iServices.Context() );

    TLocale locale;

    TInt column( iTime.DayNoInWeek() - locale.StartOfWeek() );

    if( column < 0 )
        {
        column += KCalenDaysInWeek;
        }
    iColumn = column;

    TTime startDay( iTime - TTimeIntervalDays( column ) );

    if (iStartDay != startDay)
        {
        iStartDay = startDay;
        iView->BeginRepopulationL();
        iListBox->View()->SetDisableRedraw(ETrue);
        }
    else if ( iViewPopulationComplete )
        {
        iListBox->View()->SetDisableRedraw(ETrue);
        SelectHighlightedCellAndVisibleRangeL();
        iListBox->View()->SetDisableRedraw(EFalse);
        FocusChangeForPopupL();
        UpdateStatusPaneAndExtensionsL();
        DrawNow();
        }
    
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenWeekContainer::RedrawLocaleChangeL
// Redrawing when locale change
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenWeekContainer::RedrawLocaleChangeL(TLocaleChangeRedraw aFlag) // ERedrawAll | ERedrawWeek
    {
    TRACE_ENTRY_POINT;

    if (aFlag == ERedrawAll)
        {
        iStartDay = Time::NullTTime();
        SetCursorToActiveDayL();
        }
    FocusChangeForPopupL();    
    UpdateStatusPaneAndExtensionsL();

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// ?classname::?member_function
// ?implementation_description
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
TInt CCalenWeekContainer::HighlightRowFromActiveContextL(TInt aColumn)
    {
    TRACE_ENTRY_POINT;
    
    TInt row = KErrNotFound;

    MCalenContext& context = iServices.Context();
    
    // row is selected while moving up/dowm left/right
    if ( iSelectedRowNumber != KErrNotFound )  
       {
        row = iSelectedRowNumber;
        iFirstRowTime = context.FocusTime();
		
		if(iFirstRowTime != TTimeIntervalMinutes(KNullMinutes))
			{
			iRow = EFalse;
			}
		else
			{
			iRow = ETrue;
			row = FindItemFromColumnL( context.InstanceId(),aColumn );
			}
		
		if(iFirstRowTime == TTimeIntervalMinutes(0))
			{
			row = KErrNotFound;
			iFirstRowTime = TTimeIntervalMinutes(KNullMinutes);
			}
	    
        }
     else if ( !(context.InstanceId() == TCalenInstanceId::NullInstanceId() ) )
        {
        // based on the instanceid
        row = FindItemFromColumnL( context.InstanceId(),
                                          aColumn );
                                          
        }
     else if ( context.FocusDateAndTimeL().TimeUtcL() != Time::NullTTime() )
        {
        // weekview is activated from other views/view switching
        if(	iSelectedRowNumber == KErrNotFound && 
        	iFirstRowTime == TTimeIntervalMinutes(KNullMinutes)  )
        	{
        	row = KErrNotFound;
        	}
        else
        	{
        	// based on the context time
        	row = FindRowForTime(
            	    CalenDateUtils::RoundToPreviousHour(
                	CalenDateUtils::TimeOfDay(
                    context.FocusDateAndTimeL().TimeLocalL().DateTime() ) ) );
        	}
        }

    // if we still haven't found anything to highlight,
    // highlight to first entry or 8:00 row.
    if (row == KErrNotFound)
        {
        row = FindFirstItemOfColumn(aColumn);

        if (row == KErrNotFound)
            {
            TInt eightOclock = KNonTimedNoteDefautDisplayTime * KCalenMinutesInHour;
            row = FindRowForTime( eightOclock );
            iTopRowDefault = ETrue;
            }
        }
    ASSERT( row != KErrNotFound );
	
	TRACE_EXIT_POINT;
    return row;
    
    }

// ----------------------------------------------------------------------------
// CCalenWeekContainer::SelectHighlightedCellAndVisibleRangeL()
// Sets highlight item in ListBox at active time
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenWeekContainer::SelectHighlightedCellAndVisibleRangeL()
    {
    TRACE_ENTRY_POINT;

    TInt row = HighlightRowFromActiveContextL( iColumn );
	TInt topRow = SearchTopIndexL( iColumn );
	
    if (row == KErrNotFound)
        {
        // If not yet found, Then just take 8:00 row
        TInt eightOclock = KNonTimedNoteDefautDisplayTime * KCalenMinutesInHour;
        row = FindRowForTime(eightOclock);
        }

    SetHighlightAndVisibleRange( row, iColumn, topRow );
	SetActiveContextFromHighlightL();
	 
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenWeekContainer::SetActiveContextFromHighlightL
// Set Active date and time
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenWeekContainer::SetActiveContextFromHighlightL(TBool aInstAvailable)
    {
    TRACE_ENTRY_POINT;

    MCalenContext& context = iServices.Context();
    TCalTime currentDayCalTime;
    TDateTime dt = iTime.DateTime();
    currentDayCalTime.SetTimeLocalL( iTime );
    context.SetFocusDateAndTimeL( currentDayCalTime,
                                  TVwsViewId( KUidCalendar, KUidCalenWeekView ) );

    TInt row = iListBox->View()->CurrentItemIndex();
    // In under processing of DoStepL(), row is -1.
    if (row >= 0)
        {
        CWeekSlotArrayPtr slotTable = iSlotTable[iColumn];
        const CCalHourItem& itemInfo = *(*slotTable)[row];
        // Focused item
        // 1. Non-timed item, 1 for each cell, on same/ different day
        //    * Focus on instanceId and datetime
        // 2. Timed item, 1-n for each cell, on same/ different day
        //    * Focus on datetime
        if( itemInfo.HasInstance() && aInstAvailable)
            {
            TCalenInstanceId instId = TCalenInstanceId::CreateL( *itemInfo.iInstance );
            if( !itemInfo.IsTimed() )       // todo/anniv/memo
                {
                dt.SetHour( KNonTimedNoteDefautDisplayTime );
                currentDayCalTime.SetTimeLocalL( TTime( dt ) );
                context.SetFocusDateAndTimeAndInstanceL( currentDayCalTime, instId, 
                                                TVwsViewId( KUidCalendar, KUidCalenWeekView ) );
                }
            else    // meeting
                {
                TTime focusTime = CalenDateUtils::BeginningOfDay( iTime ) + itemInfo.iStartTime;
                currentDayCalTime.SetTimeLocalL( focusTime );
                context.SetFocusDateAndTimeAndInstanceL( currentDayCalTime, instId, 
                                            TVwsViewId( KUidCalendar, KUidCalenWeekView ) );
                }
            }
        else if( itemInfo.iStartTime.Int() == KErrNotFound )    // empty non-timed cell
            {
            dt.SetHour( KNonTimedNoteDefautDisplayTime );
            currentDayCalTime.SetTimeLocalL( TTime( dt ) );
            context.SetFocusDateAndTimeL( currentDayCalTime,
                                                TVwsViewId( KUidCalendar, KUidCalenWeekView ) );
            }
        else    // empty timed cell
            {
            TTime focusTime = CalenDateUtils::BeginningOfDay( iTime ) + itemInfo.iStartTime;
            currentDayCalTime.SetTimeLocalL( focusTime );
            context.SetFocusDateAndTimeL( currentDayCalTime,
                                                TVwsViewId( KUidCalendar, KUidCalenWeekView ) );
            }
        FocusChangeForPopupL();
        }
	// Hide/unhide "today" toolbar item based on the focused day
    UpdateTodayToolbarItemL();
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenWeekContainer::CalcDayFromColumn
// Calculates and Sets day from ListBox column
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenWeekContainer::CalcDayFromColumn()
    {
    TRACE_ENTRY_POINT;

    iTime = iStartDay + TTimeIntervalDays( iColumn );
    TDateTime dt = iStartDay.DateTime();

    TRACE_EXIT_POINT;
    }


// ----------------------------------------------------------------------------
// CCalenWeekContainer::PopulatesDayListsL
// Populate whole week daylist from Agenda server
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenWeekContainer::PopulatesDayListsL()
    {
    TRACE_ENTRY_POINT;

    TTime indexDay( iStartDay );
    TTimeIntervalDays oneDay( 1 );
    
    RArray<TInt> colIdArray;
    CCalenNativeView::GetActiveCollectionidsL( iServices, colIdArray );
    
    if(colIdArray.Count() > 0)
         {    
         for( TInt i(0); i < KCalenDaysInWeek; ++i, indexDay+=oneDay )
            {
            iIdList[i].ResetAndDestroy();
            CalenAgendaUtils::CreateEntryIdListForDayL( iIdList[i],
                                                        iServices.InstanceViewL(colIdArray),
                                                        indexDay );
            }
         }
     else
         {
         for( TInt i(0); i < KCalenDaysInWeek; ++i, indexDay+=oneDay )
             {
             iIdList[i].ResetAndDestroy();
             CalenAgendaUtils::CreateEntryIdListForDayL( iIdList[i],
                                                         iServices.InstanceViewL(),
                                                         indexDay );
             }
         }
    
    colIdArray.Reset();
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenWeekContainer::InsertTimedNotesToSlotTableL
//
// After this function, slotTable for each column (weekday)
// will contain:
// 1) for each hour row without meetings:
//       SHourItem.iStartTime = hour as a minutes from beginning of day
//       SHourItem.iInstanceId = NullInstanceId
//       SHourItem.iTimedNote = EFalse
//       SHourItem.iHourData = -- (nothing set)
// 2) for each hour row with at least one meeting:
//       SHourItem.iStartTime = hour as a minutes from beginning of day
//       SHourItem.iInstanceId = instance id of first entry falling to the hour
//                               (sorting order as in day view)
//       SHourItem.iTimedNote = ETrue
//       SHourItem.iHourData = hour data as a table of six color values.
//
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenWeekContainer::InsertTimedNotesToSlotTableL()
    {
    TRACE_ENTRY_POINT;
		//Parsing items for each day in week

    TTime instStart;
    TTime instEnd;

    TDateTime daybegin;
    TDateTime startDateTime;
    TDateTime endDateTime;
    TTimeIntervalMinutes durationInMinutes;

	TTimeIntervalHours instStartHour;
	TTimeIntervalHours instEndHour;
	TTimeIntervalMinutes instStartMin;
	TTimeIntervalMinutes instEndMin;
	
	TInt markStart;
	TInt markEnd;
	TInt instEndHourInt;
	TInt instStartMinInt;
	TInt instEndMinInt;
	TInt eventsInHour;
	TInt hours;
	
    TInt KEveryMinutes = KCalenMinutesInHour / KEventsInHour;	//10min

    // For each day in a week
    for (TInt dayIx(0); dayIx < KCalenDaysInWeek; dayIx++)
    	{
        TTime day((iStartDay + TTimeIntervalDays(dayIx)));
        CWeekSlotArrayPtr slotTable = iSlotTable[dayIx];
        RPointerArray<CCalInstance>& dayList = iIdList[dayIx];	//Event for day
        
        //Get start time of day
        TTime dayBegin = CalenDateUtils::BeginningOfDay( day );
		
		//Initialize the ptr list with empty items but take account the non timed events
		TInt beginMinutes(0);
		
		// For each hour in a day initialisation for the slot table and other hour item required.
		for(TInt i(0) ; i < KCalenHoursInDay; i++)
 			{				      
	   		CCalHourItem* emptyInfo = CCalHourItem::NewL();
	        CleanupStack::PushL( emptyInfo );
	        emptyInfo->iInstance = NULL;
	        emptyInfo->iStartTime = TTimeIntervalMinutes( beginMinutes );
	        emptyInfo->iTimedNote = EFalse;
			
		    // For instances before non - timed notes
			if(i < KNonTimedNoteDefautDisplayTime)
            	{
            	slotTable->InsertL(i, emptyInfo);
            	}
        	else // For instances after non - timed notes
            	{
                emptyInfo->iTimedNote=ETrue;
            	slotTable->AppendL(emptyInfo);
            	}				
	 		CleanupStack::Pop();
	 		beginMinutes = beginMinutes + KCalenMinutesInHour;
 			}
        
        

 	    //Go through the whole instance list
		for(TInt item(0);  item < dayList.Count();  item++)
 			{
 			RPointerArray<CCalCalendarInfo> calendarInfoList;
 			iServices.GetAllCalendarInfoL(calendarInfoList);
 			CleanupClosePushL(calendarInfoList);  
 			CCalInstance* inst = dayList[item];
 			CCalEntry& entry = inst->Entry();
       		TCalCollectionId colId = inst->InstanceIdL().iCollectionId;
            HBufC* calendarFileName = iServices.GetCalFileNameForCollectionId(colId).AllocLC();
            TInt index = calendarInfoList.Find( *calendarFileName, 
       	         CCalenWeekContainer::CalendarInfoIdentifierL);
       	    CleanupStack::PopAndDestroy(calendarFileName);
            
            //Handle only the timed node, ignore nontimed and non-allday
            if( ( IsTimedNoteL( entry ) || CalenViewUtils::IsAlldayEventL(*inst) )
                && index != KErrNotFound && calendarInfoList[index]->Enabled() )
	        	{
	            //Get the start and end time of instance
				instStart = inst->Time().TimeLocalL();
				entry.EndTimeL().TimeLocalL().MinutesFrom( entry.StartTimeL().TimeLocalL(), durationInMinutes );
        		instEnd = instStart + durationInMinutes;
        		
        		//  Get the day' begin time, instance start time and
        		// instance end time in TDateTime format.
        		daybegin = dayBegin.DateTime();
        		startDateTime = instStart.DateTime();
        		endDateTime = instEnd.DateTime();
        		
        		// If the instance start time is before the day begin,
        		// then initialise it to day begin's time
        		if(startDateTime.Year() != daybegin.Year() ||
        		   startDateTime.Month() != daybegin.Month() ||
        		   startDateTime.Day() != daybegin.Day() )
            		{
            		startDateTime = daybegin;
            		}
        		
        		// If the instance end day one or more days ahead than
        		// day begin, then initialise it to end of day begin
        		if(endDateTime.Year() != daybegin.Year() ||
        		   endDateTime.Month() != daybegin.Month() ||
        		   endDateTime.Day() != daybegin.Day() )
            		{
            		endDateTime = daybegin;
            		endDateTime.SetHour(23);
            		endDateTime.SetMinute(59);
            		}
        		
        		//Get time interval between beginning of day and instance start and end time
        		// in minutes in TTimeIntervalMinutes format.
	        	TTime(startDateTime).HoursFrom(dayBegin, instStartHour);
	 			TTime(endDateTime).HoursFrom(dayBegin,instEndHour); 
	 			
	 			//Get time interval between beginning of day and instance start and end time				
        		// in hours in TTimeIntervalHours format.	        	
	        	TTime(startDateTime).MinutesFrom(dayBegin,instStartMin);
	 			TTime(endDateTime).MinutesFrom(dayBegin,instEndMin);

        		// The no. of hours from day begin to instance end time
        		instEndHourInt = instEndHour.Int();   //From Day Begin
        		
        		// The no. of min from day begin to instance start time.
        		instStartMinInt = instStartMin.Int();
        		// The no. of min from day begin to instance end time
        		instEndMinInt = instEndMin.Int();
        		
                // The hour to start with.
                hours = instStartHour.Int();
                eventsInHour = startDateTime.Minute() / 10;
                
                // The minute to start with.
        		markStart = instStartMin.Int();
	            
	            // The minute to end with.
                markEnd = markStart + KEveryMinutes;
                
                // From instance start hour to end hour
                for( ; hours <= instEndHourInt; hours++ )            	
	            	{
	                TInt itemIndex(0);
		       		// Before non - timed notes
		       		if(hours < KNonTimedNoteDefautDisplayTime )
		       			{
						itemIndex = hours;		       	  		
		       			}
			        else  // After non-time notes
			        	{
						itemIndex = hours + iNonTimedAreaRowCount;
			        	}
			        
		      //Get correct item
		      CCalHourItem& itemInfo = *(*slotTable)[itemIndex];
			  itemInfo.iInstance = inst;	
			  TUint32 color;
	                
	                // For all the events in this hour
	                for( ; eventsInHour < KEventsInHour; eventsInHour++ )	// 6 times
	                    {
	                    // If this instance is there between mark start and end
	                    if( ( instStartMinInt >= markStart && instStartMinInt < markEnd) ||
	                            ( instStartMinInt <= markStart && instEndMinInt > markStart) )
                            { 
                            if( itemInfo.iHourData.Event(eventsInHour) )
                                {
                                // conflicting meeting from same/different db
                                color = (TUint32)EManyEvents;
                                }
                            else
                                {
                                color = calendarInfoList[index]->Color().Value();
                                if(color == KRgbBlack.Value())
                                    {
                                    color = EBlackColoredEvent;
                                    }
                                }
                      		itemInfo.iHourData.SetEvent( eventsInHour, color );
                            }
	                    markStart += KEveryMinutes;
	                    markEnd += KEveryMinutes;
	                    }
	                	eventsInHour = 0;
	            	}
            	}
            CleanupStack::PopAndDestroy(&calendarInfoList);
 			}
 		ClearOneDayEntryCache();
    	}
    TRACE_EXIT_POINT; 

    }

// ----------------------------------------------------------------------------
// CCalenWeekContainer::InsertNonTimedNotesToSlotTableL
//
// After this function, slotTable for each column (weekday)
// will contain:
// 1) for each empty non-timed note row (those days that have less
//    non-timed notes than others, contain empty rows):
//       SHourItem.iStartTime = KCalenActiveTimeNull
//       SHourItem.iInstanceId = NullInstanceId
//       SHourItem.iTimedNote = EFalse
//       SHourItem.iHourData = -- (nothing set)
// 2) for each non-timed note row with note :
//       SHourItem.iStartTime = KCalenActiveTimeNull
//       SHourItem.iInstanceId = instance id of entry
//       SHourItem.iTimedNote = EFalse
//       SHourItem.iHourData = icon index representing non-timed note
//
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenWeekContainer::InsertNonTimedNotesToSlotTableL()
    {
    TRACE_ENTRY_POINT;

    TInt maxNotes( 0 );
    TInt nonTimedNotes[ KCalenDaysInWeek ];

    TInt dayIx( 0 );

    for( dayIx = 0; dayIx < KCalenDaysInWeek; ++dayIx )
        {
        nonTimedNotes[dayIx] = 0;

        // instances
        RPointerArray<CCalInstance>& dayList = iIdList[dayIx];


        for (TInt i(0); i < dayList.Count(); ++i)
            {
            CCalEntry& entry = dayList[i]->Entry();
            CCalEntry::TType type = entry.EntryTypeL();
            
            TCalCollectionId colId = dayList[i]->InstanceIdL().iCollectionId;
            
                    
                if ( type != CCalEntry::EAppt && !CalenViewUtils::IsAlldayEventL( *( dayList[i] ) ) )
                    {
                    nonTimedNotes[dayIx] += 1;
                    }
            }
        maxNotes = Max( maxNotes, nonTimedNotes[dayIx] );
        }
    iNonTimedAreaRowCount = maxNotes;


    for( dayIx = 0; dayIx < KCalenDaysInWeek; ++dayIx )
        {
        CWeekSlotArrayPtr slotTable = iSlotTable[dayIx];

        // Empty slots
        TInt nonTimedAreaRow( 0 );
        for ( ; nonTimedAreaRow < iNonTimedAreaRowCount - nonTimedNotes[dayIx]; ++nonTimedAreaRow )
            {
            // create new hour item
            CCalHourItem* itemInfo = CCalHourItem::NewL();
            CleanupStack::PushL( itemInfo );
            itemInfo->iStartTime = TTimeIntervalMinutes( KNullMinutes );
            itemInfo->iTimedNote = EFalse;
            // Empty slots
            itemInfo->iInstance = NULL;
            slotTable->AppendL( itemInfo );
            CleanupStack::Pop();  // iteminfo
            // we just put non-timed notes to slot table
            }

        // Slots with notes
        if (nonTimedAreaRow < iNonTimedAreaRowCount)
            {
            RPointerArray<CCalInstance>& dayList = iIdList[dayIx];

            for( TInt item(0); item < dayList.Count(); ++item )
                {
                CCalEntry& entry = dayList[item]->Entry();
                TCalCollectionId colId = dayList[item]->InstanceIdL().iCollectionId;
                
                if( !IsTimedNoteL( entry ) && !CalenViewUtils::IsAlldayEventL( *( dayList[item] ) ) ) ///*&& index != KErrNotFound && infoArray[index]->GetCalendarStatus()*/)
                    {
                    ++nonTimedAreaRow;

                    CCalHourItem* itemInfo = CCalHourItem::NewL();
                    CleanupStack::PushL( itemInfo );
                    itemInfo->iInstance = dayList[item];
                    // set icon index
                    TInt iconIndex = static_cast<TInt>( IconIndexFromEntryL( entry ,
                            CalenViewUtils::IsAlldayEventL( *( dayList[ item ] ) ) ) );
                    itemInfo->iHourData.SetIconIndex( iconIndex );
                    slotTable->AppendL( itemInfo );
                    CleanupStack::Pop(); // iteminfo
                    }
                }
            }
        }

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenWeekContainer::SetListBoxDataL
// Sets Week data to ListBox
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenWeekContainer::SetListBoxDataL()
    {
    TRACE_ENTRY_POINT;

    iListBox->View()->SetDisableRedraw(ETrue);

    TLocale locale;

    const TInt resID( locale.TimeFormat() == ETime24 ?
                      R_QTN_TIME_USUAL : R_CALEN_WEEKVIEW_TIMEFORMAT_12 );
    HBufC* timeFormat = StringLoader::LoadLC( resID, iEikonEnv );

    iDesArray->Reset();

    CWeekSlotArrayPtr firstDaySlotTable = iSlotTable[0];
    TInt count( firstDaySlotTable->Count() );

    for (TInt row(0); row < count; ++row)
        {
        TTime time(TInt64(0));
        const CCalHourItem& hourItemInfo = *(*firstDaySlotTable)[row];

        SDisplayData dispData;
        if( hourItemInfo.iStartTime.Int() >= 0 )
            {
            time += hourItemInfo.iStartTime;
            time.FormatL(dispData.iHourDes, *timeFormat);
            }

        for (TInt day(0); day < KCalenDaysInWeek; ++day)
            {
            CWeekSlotArrayPtr daySlotTable = iSlotTable[day];

            const CCalHourItem& itemInfo = *(*daySlotTable)[row];
            dispData.iHourData[day] = itemInfo.iHourData;
            }

        TInt size((sizeof(dispData) / sizeof(TText)));
        TPtr16 ptr((TUint16 *)&dispData, size, size);
        iDesArray->AppendL(ptr);
        }

    CleanupStack::PopAndDestroy(timeFormat);

    iListBox->SetStartDay(iStartDay);
    iListBox->HandleItemAdditionL();

    static_cast<CCalenWeekLBView*>(iListBox->View())->SetColumn(iColumn);
    SelectHighlightedCellAndVisibleRangeL();

    iListBox->View()->SetDisableRedraw(EFalse);
    DrawDeferred();

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenWeekContainer::SearchMatchedNoteL
// The note contained in within a time is searched.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenWeekContainer::SearchMatchedNoteL(RPointerArray<CCalInstance>& aDayList,// InstanceId array
                                             TTime aTime) // Search Timezone
    {
    TRACE_ENTRY_POINT;

    TTime aTimeEnd( aTime );
    aTimeEnd += TTimeIntervalHours( 1 );

    for( TInt i(0); i < aDayList.Count(); ++i )
        {
        CCalInstance* instance = aDayList[i];
        CCalEntry& entry = instance->Entry();

        if( IsTimedNoteL( entry ) )
            {
            TTime start( instance->Time().TimeLocalL() );
            TTimeIntervalMinutes duration;
            entry.EndTimeL().TimeLocalL().MinutesFrom( entry.StartTimeL().TimeLocalL(), duration );
            TTime end = start +duration;

            if( (start >= aTime && start < aTimeEnd) ||
                (start <= aTime && end > aTime) )
                {
                CleanupStack::PushL( instance );
                iEntryCache.AppendL( instance );
                CleanupStack::Pop(); // instance
                }
            }
        }

    if( !iEntryCache.Count() )
        {
        iEntryCache.Reset();
        }

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenWeekContainer::IsTimedNote
// Check specified entry whether Timed Note.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
TBool CCalenWeekContainer::IsTimedNoteL(CCalEntry& aEntry)
    {
    TRACE_ENTRY_POINT;

    TRACE_EXIT_POINT;
    return( aEntry.EntryTypeL() == CCalEntry::EAppt );
    }

// ----------------------------------------------------------------------------
// CCalenWeekContainer::ResetSlotTable
// Reset slot time table.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenWeekContainer::ResetSlotTable()
    {
    TRACE_ENTRY_POINT;

// loop through days
    for (TInt dayIx(0); dayIx < KCalenDaysInWeek; ++dayIx)
        {
// current day
        CWeekSlotArrayPtr daySlotTable = iSlotTable[dayIx];
// go thorugh items in a day
        if (daySlotTable)
            {
            for (TInt i=0; i<daySlotTable->Count(); ++i)
                {
                // free instances on a day
                CCalHourItem* itemInfo = (*daySlotTable)[i];
                delete itemInfo;
                (*daySlotTable)[i]=NULL;
                }

// delete day
            delete daySlotTable;
            }

        iSlotTable[dayIx] = NULL;
        }

    TRACE_EXIT_POINT;
    }

void CCalenWeekContainer::InitSlotTableL()
    {
    TRACE_ENTRY_POINT;

    ResetSlotTable();
    for (TInt dayIx(0); dayIx < KCalenDaysInWeek; ++dayIx)
        {
        iSlotTable[dayIx] = new(ELeave) CArrayFixFlat<CCalHourItem*>(10);
        }

    TRACE_EXIT_POINT;
    }

TInt CCalenWeekContainer::FindFirstItemOfColumn(TInt aColumn)
    {
    TRACE_ENTRY_POINT;

    CWeekSlotArrayPtr slotTable = iSlotTable[aColumn];
    TInt count = slotTable->Count();
    for(TInt index = 0; index < count; ++index)
        {
        const CCalHourItem& itemInfo = *(*slotTable)[index];
        if (!(itemInfo.iInstance==NULL))
            {
            TRACE_EXIT_POINT;
            return index;
            }
        }
    TRACE_EXIT_POINT;
    return KErrNotFound;
    }


TInt CCalenWeekContainer::FindItemFromColumnL( const TCalenInstanceId& aId, 
                                                                    TInt aColumn)
    {
    TRACE_ENTRY_POINT;

    CWeekSlotArrayPtr slotTable = iSlotTable[aColumn];
    TInt count = slotTable->Count();
    CCalEntry::TType type = aId.iType;

    if( type == CCalEntry::ETodo || 
        type == CCalEntry::EEvent || 
        type == CCalEntry::EReminder ||
        type == CCalEntry::EAnniv )
        {  // non-timed entry, each entry should occupy 1 cell in the weekview
        for(TInt index = 0; index < count; ++index)
            {
            const CCalHourItem& itemInfo = *(*slotTable)[index];
            CCalInstance * itemInstance = itemInfo.iInstance;

            if (itemInstance && (itemInstance->Entry().LocalUidL() == aId.iEntryLocalUid))
                {
                TRACE_EXIT_POINT;
                return index;
                }
            }
        }
    else if( aId.iType == CCalEntry::EAppt )
        {  // timed entry, e.g Meeting, should search for it's timed cell
        TInt hr = aId.iInstanceTime.DateTime().Hour();
        if( hr < KNonTimedNoteDefautDisplayTime )
            {
            hr = hr;
            }
        else
            {
            hr = hr + count - KHoursInADay;
            }
        TRACE_EXIT_POINT;
        return hr;
        }
        
   // unknown entry type
    TRACE_EXIT_POINT;
    return KErrNotFound;
    }

// ----------------------------------------------------------------------------
// CCalenWeekContainer::FindRowForTime
// Searches specified time
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
TInt CCalenWeekContainer::FindRowForTime(TTimeIntervalMinutes aTime)
    {
    TRACE_ENTRY_POINT;

    // We can use first column, as all columns have same amount of non-timed
    // rows.
    CWeekSlotArrayPtr slotTable = iSlotTable[0];
    for(TInt row = 0; row < slotTable->Count(); ++row)
        {
        const CCalHourItem& item = *(*slotTable)[row];
        if (item.iStartTime == aTime)
            {
            return row;
            }
        }

    TRACE_EXIT_POINT;
    return KErrNotFound;
    }

// ----------------------------------------------------------------------------
// CCalenWeekContainer::SetHighlightAndVisibleRange
// Sets highlight item in ListBox
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenWeekContainer::SetHighlightAndVisibleRange(
    TInt aRow,           // Selected item index in ListBox
    TInt aColumn,       // Day of the week
    TInt aTopRow)         // Top item index in ListBox
    {
    TRACE_ENTRY_POINT;

    __ASSERT_ALWAYS(aRow != KErrNotFound, User::Invariant());
    __ASSERT_ALWAYS(aColumn >= 0, User::Invariant());
    __ASSERT_ALWAYS(aColumn < KCalenDaysInWeek, User::Invariant());
    // Purpose of the function is to set position of focus and
    // set and select topmost visible row.

    // Set highlight
    iListBox->SetCurrentItemIndex(aRow);
    CCalenWeekLBView* view =
        static_cast<CCalenWeekLBView*>( iListBox->View() );
    if (aColumn != view->Column())
        {
        view->SetColumn(aColumn, ETrue);
        }

    // Select topmost visible row
    if (KErrNotFound == aTopRow)
        {
        aTopRow = aRow;
        }

    // FIXME: Take middle SetTopItemIndex calls away
    TInt top(aTopRow < 0 ? aRow - KBeforeActiveTime : aTopRow);
    TInt items(view->NumberOfItemsThatFitInRect(view->ViewRect()));
    CWeekSlotArrayPtr slotTable = iSlotTable[aColumn];
    top = Min(top, slotTable->Count() - items);
    top = Max(top, 0);
    iListBox->SetTopItemIndex(top);
    if (top > aRow)
        {
        iListBox->SetTopItemIndex(aRow);
        }
    else if (iListBox->BottomItemIndex() <= aRow)
        {
        iListBox->SetTopItemIndex(aRow - items + 1);
        CWeekSlotArrayPtr firstDaySlotTable = iSlotTable[0];
        const CCalHourItem& itemInfo = *(*firstDaySlotTable)[aRow];
        if ( (! itemInfo.IsTimed()) && aRow == iListBox->BottomItemIndex())
            {
            TTimeIntervalMinutes bottomTime =
                KNonTimedNoteDefautDisplayTime * KCalenMinutesInHour;
            TInt bottomIndex = FindRowForTime(bottomTime) - (items - 1);
            if (bottomIndex > aRow)
                {
                iListBox->SetCurrentItemIndex(bottomIndex);
                }
            }
        }

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenWeekContainer::ClearOneDayEntryCache
// Clear entry of array for cash
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenWeekContainer::ClearOneDayEntryCache()
    {
    TRACE_ENTRY_POINT;

    iOneDayEntryCache.ResetAndDestroy();

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenWeekContainer::TopTime
// Gets time of top item in ListBox
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
TTimeIntervalMinutes CCalenWeekContainer::TopTime() const
    {
    TRACE_ENTRY_POINT;

    CWeekSlotArrayPtr slotTable = iSlotTable[0];
    TInt index(iListBox->TopItemIndex());
    const CCalHourItem& item = *(*slotTable)[index];

    TRACE_EXIT_POINT;
    return item.iStartTime;
    }

// ----------------------------------------------------------------------------
// CCalenWeekContainer::UpdateSize
// Do layout changes after calendar settings are
// changed.  No necessary actions for this view.
// ----------------------------------------------------------------------------
//
void CCalenWeekContainer::UpdateSize()
    {
    TRACE_ENTRY_POINT;

    SizeChanged();

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenWeekContainer::ConstructImplL
// Third phase constructor.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenWeekContainer::ConstructImplL()
    {
    TRACE_ENTRY_POINT;

    TLocale locale;
    // Create preview pane
    TRect aRect = PreviewRectL();
    iPreview = iServices.CustomPreviewPaneL(aRect);
    
    // ownership of builder transferred

    // 3) Listbox
    iListBox = new (ELeave) CCalenWeekListbox( *this );
    // compile error argument is (CCoeControl*, TInt aFlag)
    iListBox->ConstructL(this);
    iListBox->SetBorder(TGulBorder::ENone);
    iListBox->DisableScrolling(ETrue);
     // Single click integration
    iListBox->DisableSingleClick( ETrue );


    // 4) Listbox model
    iDesArray = new(ELeave) CDesCArrayFlat(5);

    CTextListBoxModel* model = iListBox->Model();
    model->SetItemTextArray(iDesArray);
    model->SetOwnershipType(ELbmDoesNotOwnItemArray);

    iListBox->HandleItemAdditionL();


    // 5) Icons
    CArrayPtr<CGulIcon>* iconArray = iListBox->ItemDrawer()->FormattedCellData()->IconArray();
    if(iconArray)
        {
        iconArray->ResetAndDestroy();
        delete iconArray; // owned by CFormattedCellListBoxData
        iconArray = NULL; // to suppress codescanner error
        iListBox->ItemDrawer()->FormattedCellData()->SetIconArray(NULL);
        }

    CArrayPtr<CGulIcon>* icons = CreateIconsL( iIconIndices );
    iListBox->ItemDrawer()->FormattedCellData()->SetIconArray(icons);

    iListBox->SetCursorObserver(this);

    // 6) Scroll bar
    iListBox->CreateScrollBarFrameL(ETrue);
    iListBox->ScrollBarFrame()->SetScrollBarVisibilityL(
        CEikScrollBarFrame::EOff, CEikScrollBarFrame::EAuto);


    // 7) Background skin contexts
    PIM_TRAPD_HANDLE( CCalenWeekContainer::ConstructBackgroundContextL() );
    
    iRow = EFalse;
    iTopRowDefault =  EFalse;

    TRACE_EXIT_POINT;
    }

void CCalenWeekContainer::ConstructBackgroundContextL()
    {
    TRACE_ENTRY_POINT;

    TRect dummy(0,0,0,0);
    if( AknLayoutUtils::ScalableLayoutInterfaceAvailable() )
        {
            iBackgroundSkinContext = CAknsFrameBackgroundControlContext::NewL(
                KAknsIIDQsnFrCale, dummy, dummy, EFalse );
            iHeadingBgContext = CAknsFrameBackgroundControlContext::NewL(
                KAknsIIDQsnFrCaleHeading, dummy, dummy, EFalse );
            iSideBgContext = CAknsFrameBackgroundControlContext::NewL(
                KAknsIIDQsnFrCaleSide, dummy, dummy, EFalse );
        }

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenWeekContainer::NotifyChangeDateL
// Date change operation notification handler.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenWeekContainer::NotifyChangeDateL()
    {
    TRACE_ENTRY_POINT;

    SetCursorToActiveDayL();

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenWeekContainer::SizeChanged
// Resizes child controls
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenWeekContainer::SizeChanged()
    {
    TRACE_ENTRY_POINT;

    /* LAF doesn't specify week view listbox directly.
     * Calculation of listbox rect is done inside listbox.
     */  
    CCalenContainer::SizeChanged( Rect() );
    
    TRAP_IGNORE(iLayoutManager->GetLayoutAndExtensionL());

    TRect main_pane = ReducePreview( Rect() );

    TAknLayoutRect main_cale_week_pane;
    main_cale_week_pane.LayoutRect( main_pane, AknLayoutScalable_Apps::main_cale_week_pane().LayoutLine() );

    TWeekListBoxLayout layToolBar( UseInfobar(), UseToolbar(), UsePreview(), main_pane );
    TInt layoutToolBarVariant = layToolBar.LayoutVariantIndex(TWeekListBoxLayout::EListScrollCaleWeekPane);
    // Cached layout value:
    i_listscroll_cale_week_pane.LayoutRect( main_cale_week_pane.Rect(),
                                            AknLayoutScalable_Apps::listscroll_cale_week_pane(layoutToolBarVariant).LayoutLine() );
                                            

    TWeekListBoxLayout lay( UseInfobar(), UseToolbar(), UsePreview(), main_pane ); 

    iListBox->SetRect( iListBox->LayoutRect() );
    TInt layoutVariant = lay.LayoutVariantIndex(TWeekListBoxLayout::EScrollPaneCp08);
    AknLayoutUtils::LayoutVerticalScrollBar( iListBox->ScrollBarFrame(),
                                             i_listscroll_cale_week_pane.Rect(),
                                             AknLayoutScalable_Apps::scroll_pane_cp08( layoutVariant ).LayoutLine() );

    if( iBgContext && iHeadingBgContext && iSideBgContext )
        {
        TAknLayoutRect bg_cale_pane_cp01;
        layoutVariant = lay.LayoutVariantIndex(TWeekListBoxLayout::EBgCalePaneCp01);
        bg_cale_pane_cp01.LayoutRect( i_listscroll_cale_week_pane.Rect(), AknLayoutScalable_Apps::bg_cale_pane_cp01( layoutVariant ).LayoutLine() );
        
        TAknLayoutRect cale_bg_pane_center;
        cale_bg_pane_center.LayoutRect( bg_cale_pane_cp01.Rect(), AknLayoutScalable_Apps::cale_bg_pane_g1().LayoutLine() );

        iBackgroundSkinContext->SetFrameRects( bg_cale_pane_cp01.Rect(), cale_bg_pane_center.Rect() );

        // Heading pane layout
        TAknLayoutRect bg_cale_heading_pane;
        layoutVariant = lay.LayoutVariantIndex(TWeekListBoxLayout::EBgCaleHeadingPane);
        bg_cale_heading_pane.LayoutRect( i_listscroll_cale_week_pane.Rect(), AknLayoutScalable_Apps::bg_cale_heading_pane( layoutVariant ).LayoutLine() );

        // center
        TAknLayoutRect bg_cale_heading_pane_g1;
        bg_cale_heading_pane_g1.LayoutRect( bg_cale_heading_pane.Rect(), AknLayoutScalable_Apps::bg_cale_heading_pane_g1().LayoutLine() );

        iHeadingBgContext->SetFrameRects( bg_cale_heading_pane.Rect(), bg_cale_heading_pane_g1.Rect() );

        // Side pane layout
        TAknLayoutRect cale_week_time_pane;
        layoutVariant = lay.LayoutVariantIndex(TWeekListBoxLayout::ECaleWeekTimePane);
        cale_week_time_pane.LayoutRect( i_listscroll_cale_week_pane.Rect(), AknLayoutScalable_Apps::cale_week_time_pane( layoutVariant ).LayoutLine() );

        TAknLayoutRect bg_cale_side_pane;
        layoutVariant = lay.LayoutVariantIndex(TWeekListBoxLayout::EBgCaleSidePane);
        bg_cale_side_pane.LayoutRect( cale_week_time_pane.Rect(), AknLayoutScalable_Apps::bg_cale_side_pane(layoutVariant).LayoutLine() );

        // center
        TAknLayoutRect bg_cale_side_pane_g1;
        bg_cale_side_pane_g1.LayoutRect( bg_cale_side_pane.Rect(), AknLayoutScalable_Apps::bg_cale_side_pane_g1().LayoutLine() );

        iSideBgContext->SetFrameRects( bg_cale_side_pane.Rect(), bg_cale_side_pane_g1.Rect() );

        iBackgroundSkinContext->SetParentContext( iBgContext );
        iHeadingBgContext->SetParentContext( iBackgroundSkinContext );
        iSideBgContext->SetParentContext( iHeadingBgContext );
        }

    PIM_TRAPD_HANDLE( UpdateStatusPaneAndExtensionsL() );

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenWeekContainer::FocusChanged
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenWeekContainer::FocusChanged( TDrawNow aDrawNow )
    {
    TRACE_ENTRY_POINT;

    CCalenContainer::FocusChanged( aDrawNow );
    if( iListBox )
        {
        iListBox->SetFocus( IsFocused() );
        }

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenWeekContainer::HandleResourceChange(TInt aType)
// Called when layout or skin change occurs
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenWeekContainer::HandleResourceChange(TInt aType)
    {
    TRACE_ENTRY_POINT;

    CCalenContainer::HandleResourceChange( aType );

    if ( aType == KAknsMessageSkinChange || aType == KEikDynamicLayoutVariantSwitch )
        {
        CArrayPtr<CGulIcon>* iconArray = iListBox->ItemDrawer()->FormattedCellData()->IconArray();
        if(iconArray)
            {
            iconArray->ResetAndDestroy();
            delete iconArray; // owned by CFormattedCellListBoxData
            iconArray = NULL; // to suppress codescanner error
            iListBox->ItemDrawer()->FormattedCellData()->SetIconArray(NULL);
            }

        TRAPD(leave, iconArray = CreateIconsL( iIconIndices ) );
        if(!leave && iconArray)
            {
            iListBox->ItemDrawer()->FormattedCellData()->SetIconArray(iconArray);
            }

        SizeChanged();

        // if layout change and res.change is allowed do the refresh
        if ( aType == KEikDynamicLayoutVariantSwitch )
            {
            iLayoutManager->HandleResourceChange( aType );
            TRAP_IGNORE(iView->BeginRepopulationL());
            }
        }

    CCoeControl::HandleResourceChange( aType );

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenWeekContainer::CountComponentControls
// Return components count
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
TInt CCalenWeekContainer::CountComponentControls() const
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
// CCalenWeekContainer::ComponentControl
// Retrun the component specified by aIndex
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
CCoeControl* CCalenWeekContainer::ComponentControl
(TInt aIndex) const
    {
   TRACE_ENTRY_POINT;

    CCoeControl* control( NULL );

    switch (aIndex)
        {
        case 0:
            control = iListBox;
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
// CCalenWeekContainer::OfferKeyEventL
// processing of a key event
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
TKeyResponse CCalenWeekContainer::OfferKeyEventL( const TKeyEvent& aKeyEvent,
                                                  TEventCode aType )
    {
    TRACE_ENTRY_POINT;

    // always hide popup on key events
    if (iPreview && aType == EEventKeyDown)
        {
        iPreview->Hide();
        }

    // common view key handling.
    // NOTE! Common key handling has to be performed before custom handling
    // otherwise view-cycling state handling breaks.
    if (CCalenContainer::OfferKeyEventL(aKeyEvent, aType) == EKeyWasConsumed)
        {
        return EKeyWasConsumed;
        }

    // month view specific key handling
    TKeyResponse exitCode(EKeyWasNotConsumed);
    if (aType == EEventKey)
        {
        switch (aKeyEvent.iCode)
            {            	
            case EKeyOK:
                iServices.IssueCommandL( ECalenForwardsToDayView );
                exitCode = EKeyWasConsumed;
                break;
           case EKeyEnter:    	        		
	            	iView->HandleCommandL(EAknSoftkeyOpen);
	            	exitCode = EKeyWasConsumed;
                break;
            default:
                break;
            }
        }

    // listbox key handling
    if ( exitCode == EKeyWasConsumed || !iListBox || !iViewPopulationComplete )
        { // if DoDeactivate() is done, iListBox will be NULL.
        // do nothing
        }
    else
        {
        TInt oldRow(iListBox->View()->CurrentItemIndex());
        TInt oldColumn(iColumn);
        exitCode = iListBox->OfferKeyEventL(aKeyEvent, aType);

        if (exitCode == EKeyWasConsumed)
            {
            TInt newRow(iListBox->View()->CurrentItemIndex());
            TInt newColumn(iColumn);
            if (newRow != oldRow || newColumn != oldColumn)
                {
                iFirstRowTime = TopTime();
                CalcDayFromColumn();
                SetActiveContextFromHighlightL();
                if (newColumn != oldColumn)
                    {
                    UpdateStatusPaneAndExtensionsL();
                    }
                }
            }
        }

    TRACE_EXIT_POINT;
    return exitCode;
    }

// ----------------------------------------------------------------------------
// CCalenWeekContainer::GetHelpContext
// Gets help context
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenWeekContainer::GetHelpContext( TCoeHelpContext& aContext ) const
    {
    TRACE_ENTRY_POINT;

    aContext.iMajor = KUidCalendar;
    // This is specified in HRH file.
    aContext.iContext = KCALE_HLP_WEEK_VIEW;

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// ?classname::?member_function
// ?implementation_description
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
TKeyResponse CCalenWeekContainer::CursorMovedL(MCalenWeekCursorObserver::TCursorMove aDir)
    {
    TRACE_ENTRY_POINT;

    TKeyResponse consumed(EKeyWasNotConsumed);
    if (aDir == MCalenWeekCursorObserver::ECalenLeft)
        {
        consumed = HorizontalMoveL(-1);
        }
    else if (aDir == MCalenWeekCursorObserver::ECalenRight)
        {
        consumed = HorizontalMoveL(+1);
        }
    else
        {
        // up or down
        iSelectedRowNumber = iListBox->View()->CurrentItemIndex();
        }

    TRACE_EXIT_POINT;
    return consumed;
    }

// ----------------------------------------------------------------------------
// ?classname::?member_function
// ?implementation_description
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
TKeyResponse CCalenWeekContainer::HorizontalMoveL(TInt aDir)
    {
    TRACE_ENTRY_POINT;

    CCalenWeekLBView* view =
        static_cast<CCalenWeekLBView*>( iListBox->View() );

    TTime newDay = iTime + TTimeIntervalDays( aDir );

    if (CalenDateUtils::IsValidDay(newDay))
        {
        TInt newColumn = iColumn + aDir;
        if (newColumn < 0 || KCalenDaysInWeek <= newColumn) // went over left or right border
            {
            if( newColumn < 0 )
                {
                iColumn = KCalenDaysInWeek - 1;
                }
            else
                {
                iColumn = 0;
                }

            view->SetColumn(iColumn);
            iTime = newDay;
            iStartDay = iTime - TTimeIntervalDays( iColumn );
            iSelectedRowNumber = KErrNotFound; // Recalculate when repopulation occurs

            // Refresh listbox
            iView->BeginRepopulationL();
            iListBox->View()->SetDisableRedraw(ETrue);

            TRACE_EXIT_POINT;
            return EKeyWasConsumed;
            }
        else if ( iViewPopulationComplete )
            {
            // Actual processing is performed by the List box side.
            iColumn += aDir;

            TRACE_EXIT_POINT;
            return EKeyWasNotConsumed;
            }
        else
            {
            TRACE_EXIT_POINT;
            return EKeyWasNotConsumed;
            }
        }
    else
        {
        view->SetColumn(iColumn);
        CalenViewUtils::ShowDateOutOfRangeErrorNoteL();

        TRACE_EXIT_POINT;
        return EKeyWasConsumed;
        }
    }

// ----------------------------------------------------------------------------
// CCalenWeekContainer::MopSupplyObject()
// Pass skin information if needed.
// ----------------------------------------------------------------------------
//
TTypeUid::Ptr CCalenWeekContainer::MopSupplyObject(TTypeUid aId)
    {
    TRACE_ENTRY_POINT;

    if( aId.iUid == MAknsControlContext::ETypeId )
        {
        MAknsControlContext* cc = iSideBgContext;

        if( !cc )
            {
            cc = iBgContext;
            }
        if ( cc )
            {
            TRACE_EXIT_POINT;
            return MAknsControlContext::SupplyMopObject( aId, cc );
            }
        }

    TRACE_EXIT_POINT;
    return CCoeControl::MopSupplyObject( aId );
    }

// ----------------------------------------------------------------------------
// ?classname::?member_function
// ?implementation_description
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenWeekContainer::FocusChangeForPopupL()
    {
    TRACE_ENTRY_POINT;

    if( iViewPopulationComplete )
        {
        CalcDayFromColumn();
        
        CWeekSlotArrayPtr slotTable = iSlotTable[iColumn];
        CCalHourItem& itemInfo = *(*slotTable)[iListBox->CurrentItemIndex()];
        
        // dont focus the preview popup/previewpane if any dialog or faster app is active        
		if(iPreview && !iView->IsEditorActiveOrFasterAppExit())
			{
	        if( !itemInfo.IsTimed() )
	            { 
	            // Non-timed cell
	            iPreview->FocusChangedL(itemInfo.iInstance);
	            }
	        else
	            {
	            TDateTime dt = iTime.DateTime();    // TACOTEMP
	            iPreview->FocusChangedL( iServices.Context().FocusDateAndTimeL().TimeLocalL() );
	            }
			}
        }
    else
        {
        HidePopup();
        }

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenWeekContainer::Draw
// Draws most of the main pane dependend parts of week view
// - skinned or normal background
// - Day names
// - grid lines
// - line between day names and grid
// - line between hours and grid (which is drawn also inside
//                list, to make scrolling work)
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenWeekContainer::Draw(const TRect& /*aRect*/) const
    {
    TRACE_ENTRY_POINT;

    CWindowGc& gc = SystemGc();

    DrawBackground(gc);
    TRAPD(error,DrawDayNamesL(gc));
    if(error!=KErrNone)
        {
        // to avoid warning
        }
    DrawGridLines(gc);

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenWeekListbox::DrawDayNames
// Draw day names
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenWeekContainer::DrawDayNamesL(CWindowGc& aGc) const      // Window graphic context
    {
    TRACE_ENTRY_POINT;

    // No need to clear background in this function, because this is
    // called only from Draw.
    // If popup needs to call this from DrawDayNamesNow, clearing
    // should be done there.
    TRect mainPane = ReducePreview( Rect() );
    TWeekListBoxLayout lay( UseInfobar(), UseToolbar(), UsePreview(), mainPane );
    TInt layoutVariant = lay.LayoutVariantIndex(TWeekListBoxLayout::ECaleWeekDayHeadingPane);
    TAknLayoutRect cale_week_day_heading_pane;
    cale_week_day_heading_pane.LayoutRect( i_listscroll_cale_week_pane.Rect(),
                                           AknLayoutScalable_Apps::cale_week_day_heading_pane( layoutVariant ).LayoutLine() );

    TRect dayHeadingRect = cale_week_day_heading_pane.Rect();
    TAknLayoutText dayHeadingLayouts[ KCalenDaysInWeek ];
    layoutVariant = lay.LayoutVariantIndex(TWeekListBoxLayout::ECaleWeekDayHeadingPaneT1);
    dayHeadingLayouts[0].LayoutText( dayHeadingRect, AknLayoutScalable_Apps::cale_week_day_heading_pane_t1(layoutVariant).LayoutLine() );
    dayHeadingLayouts[1].LayoutText( dayHeadingRect, AknLayoutScalable_Apps::cale_week_day_heading_pane_t2(layoutVariant).LayoutLine() );
    dayHeadingLayouts[2].LayoutText( dayHeadingRect, AknLayoutScalable_Apps::cale_week_day_heading_pane_t3(layoutVariant).LayoutLine() );
    dayHeadingLayouts[3].LayoutText( dayHeadingRect, AknLayoutScalable_Apps::cale_week_day_heading_pane_t4(layoutVariant).LayoutLine() );
    dayHeadingLayouts[4].LayoutText( dayHeadingRect, AknLayoutScalable_Apps::cale_week_day_heading_pane_t5(layoutVariant).LayoutLine() );
    dayHeadingLayouts[5].LayoutText( dayHeadingRect, AknLayoutScalable_Apps::cale_week_day_heading_pane_t6(layoutVariant).LayoutLine() );
    dayHeadingLayouts[6].LayoutText( dayHeadingRect, AknLayoutScalable_Apps::cale_week_day_heading_pane_t7(layoutVariant).LayoutLine() );

    // Get drawing color
    TRgb textColor = dayHeadingLayouts[0].Color();
    AknsUtils::GetCachedColor(AknsUtils::SkinInstance(), textColor,
                              KAknsIIDQsnTextColors,
                              EAknsCIQsnTextColorsCG23);

    TTime date( iStartDay );
    TLocale locale;

    for (TInt i=0; i < KCalenDaysInWeek; ++i)
        {
        TBool isToday = CalenDateUtils::OnSameDay( CalenDateUtils::Today(), date );
        aGc.SetUnderlineStyle( isToday ? EUnderlineOn : EUnderlineOff );
        TInt dayNoInWeek( i + locale.StartOfWeek() );
        if( dayNoInWeek >= KCalenDaysInWeek )
            {
            dayNoInWeek -= KCalenDaysInWeek;
            }
        dayHeadingLayouts[ i ].DrawText( aGc,
                                         WeekView().AbbreviatedDayArrayL()[dayNoInWeek],
                                         ETrue,
                                         textColor);
        date += TTimeIntervalDays( 1 );
        }

    TRACE_EXIT_POINT;
    }

CCalenWeekView& CCalenWeekContainer::WeekView() const
    {
    TRACE_ENTRY_POINT;

    TRACE_EXIT_POINT;
    return *(static_cast<CCalenWeekView*>(iView));
    }

// ---------------- class CCHourItem ------------------------ //

// ----------------------------------------------------------------------------
// ?classname::?member_function
// ?implementation_description
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
CCalHourItem::CCalHourItem()
    {
    TRACE_ENTRY_POINT;

    iStartTime = TTimeIntervalMinutes( KNullMinutes );
    iInstance = NULL;
    iTimedNote = EFalse;

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// ?classname::?member_function
// ?implementation_description
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
CCalHourItem::~CCalHourItem()
    {
    TRACE_ENTRY_POINT;

// not owned
//    delete iInstance;
    iInstance = NULL;

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// ?classname::?member_function
// ?implementation_description
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalHourItem::ConstructL()
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// ?classname::?member_function
// ?implementation_description
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalHourItem::ConstructL(CCalInstance& aInstance)
    {
    TRACE_ENTRY_POINT;

    iInstance = &aInstance;

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// ?classname::?member_function
// ?implementation_description
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
CCalHourItem* CCalHourItem::NewL()
    {
    TRACE_ENTRY_POINT;

    CCalHourItem* self=new (ELeave) CCalHourItem();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop( self );

    TRACE_EXIT_POINT;
    return self;
    }

// ----------------------------------------------------------------------------
// ?classname::?member_function
// ?implementation_description
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
CCalHourItem* CCalHourItem::NewL(CCalInstance& aInstance)
    {
    TRACE_ENTRY_POINT;

    CCalHourItem* self=new (ELeave) CCalHourItem();
    CleanupStack::PushL(self);
    self->ConstructL(aInstance);
    CleanupStack::Pop(self);

    TRACE_EXIT_POINT;
    return self;
    }

// ----------------------------------------------------------------------------
// ?classname::?member_function
// ?implementation_description
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalHourItem::SetDataL( CCalInstance& aInstance,
                             TTimeIntervalMinutes aStartTime,
                             TBool aTimedNote,
                             TCalenWeekHour aHourData )
    {
    TRACE_ENTRY_POINT;

    ConstructL( aInstance );
    iStartTime = aStartTime;
    iTimedNote = aTimedNote;
    iHourData = aHourData;

    TRACE_EXIT_POINT;
    }


TBool CCalHourItem::HasInstance() const
    {
    TRACE_ENTRY_POINT;

    TRACE_EXIT_POINT;
    return iInstance != NULL;
    }


TBool CCalHourItem::IsTimed() const
    {
    TRACE_ENTRY_POINT;

    TRACE_EXIT_POINT;
    return iTimedNote;
    }


// ----------------------------------------------------------------------------
// CCalenWeekContainer::HorizontalWeekMoveL
// processing of a week move (from touch ui navi decoration)
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenWeekContainer::HorizontalWeekMoveL(TInt aDir)
    {
    TRACE_ENTRY_POINT;

    if(AknLayoutUtils::PenEnabled())
        {
        __ASSERT_ALWAYS( (aDir == 1 || aDir == -1), User::Invariant());
        CCalenWeekLBView* view =
            static_cast<CCalenWeekLBView*>( iListBox->View() );

        TTime newDay = iTime + TTimeIntervalDays( aDir*KCalenDaysInWeek );

        if ( CalenDateUtils::IsValidDay( newDay ) )
            {
            iTime = newDay;
            iStartDay = iTime - TTimeIntervalDays(iColumn);
            // Refresh listbox
            iListBox->View()->SetDisableRedraw(ETrue);
            }
        else
            {
            view->SetColumn(iColumn);
            CalenViewUtils::ShowDateOutOfRangeErrorNoteL();
            }
        }

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenWeekContainer::HandleNaviDecorationPointerEventL
// processing of a navi decoration event
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenWeekContainer::HandleNaviDecoratorEventL(TInt aEventID)
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
        
        HorizontalWeekMoveL(direction);
        SetActiveContextFromHighlightL(EFalse);
        WeekView().SetStatusPaneFromActiveContextL();
        iView->BeginRepopulationL();
        }

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenWeekContainer::PointerMovedL
// processing of a move to new column (from touch ui)
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenWeekContainer::PointerMovedL(TInt aNewColumn)
    {
    TRACE_ENTRY_POINT;

    if(AknLayoutUtils::PenEnabled())
        {
        CCalenWeekLBView* view =
            static_cast<CCalenWeekLBView*>( iListBox->View() );
               TInt indexDiff = aNewColumn-iColumn;
               TTime newDay = iTime + TTimeIntervalDays(indexDiff);
               if (!CalenDateUtils::IsValidDay(newDay)) 
                    {    
                    iValidDay=ETrue;
                    view->SetColumn(iColumn);                                              
                    CalenViewUtils::ShowDateOutOfRangeErrorNoteL();                   
                    return ;                                           
                    }
                else
                    {   
                    iValidDay=EFalse;
                    view->PointerMoveToItemL(aNewColumn);
                    iColumn = aNewColumn;
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
void CCalenWeekContainer::HandleLongTapEventL( const TPoint& /*aPenEventLocation*/,
                                               const TPoint& /*aPenEventScreenLocation*/ )
    {
    TRACE_ENTRY_POINT;


    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenWeekContainer::HandlePointerEventL
// processing of a navi decoration event
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenWeekContainer::HandlePointerEventL(const TPointerEvent& aPointerEvent)
    {
    TRACE_ENTRY_POINT;

    if(AknLayoutUtils::PenEnabled())
        {
        TInt pointerIndex(-1);
        TInt oldRow(0);
        TInt oldColumn(0);
        TInt newRow(0);
        TInt newColumn(0);
        
        TBool isItem (iListBox->View()->XYPosToItemIndex(aPointerEvent.iPosition,
                                                         pointerIndex));

        if( !iListBox->IsValidPointer(aPointerEvent.iPosition) &&
            aPointerEvent.iType == TPointerEvent::EButton1Down )
            {
            IgnoreEventsUntilNextPointerUp();
            CCoeControl* control(NULL);
            if (aPointerEvent.iType == TPointerEvent::EButton1Down)
                {
                control = iLayoutManager->ControlOrNull();
                if (control)
                    {
                    if (control->Rect().Contains(aPointerEvent.iPosition))
                        {
                        control->HandlePointerEventL(aPointerEvent);
                        }
                    }
                }
            return;
            }
        
        switch(aPointerEvent.iType)
            {
            case TPointerEvent::EButton1Down:
                {
                if(isItem)
                    {
                    oldRow = iListBox->View()->CurrentItemIndex();
                    oldColumn = iColumn;
                    iListBox->HandlePointerEventL(aPointerEvent);
                    newRow = iListBox->View()->CurrentItemIndex();
                    newColumn = iColumn;
                    if (newRow != oldRow || newColumn != oldColumn)
                        {
                        CalcDayFromColumn();
                        SetActiveContextFromHighlightL();
                        if (newColumn != oldColumn)
                            {
                            UpdateStatusPaneAndExtensionsL();
                            }
                        }
                    }
                else
                    {
                    IgnoreEventsUntilNextPointerUp();
                    }
                break;
                }
            case TPointerEvent::EButtonRepeat:
                {
                if(isItem)
                    break;
                } // else this is drag event
            case TPointerEvent::EDrag:
                {
                // for themable support - clear Pressed Down State when dragging
                oldRow = iListBox->View()->CurrentItemIndex();
                oldColumn = iColumn;
                newColumn = iListBox->PointerEventColumn(aPointerEvent.iPosition);
                newRow = pointerIndex;
                if (newRow != oldRow || newColumn != oldColumn)
                    {
                    iHourChange = ETrue;
                    }
                break;
                }
            case TPointerEvent::EButton1Up:
                if(isItem &&
                   iView->MenuBar()->IsDisplayed() == EFalse && !iHourChange)
                    {
                    iServices.IssueCommandL( ECalenForwardsToDayView );
                    }
                iHourChange = EFalse;
                break;
            default:
                break;
            }
        }


    TRACE_EXIT_POINT;
    }



// ----------------------------------------------------------------------------
// CCalenWeekContainer::HidePopup
// Hides the popup window
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenWeekContainer::HidePopup()
    {
    TRACE_ENTRY_POINT;
    if (iPreview)
        {
        iPreview->Hide();
        }
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenWeekContainer::BeginPopulationWithInstanceViewL
// Beings populating the view. First stage of view population.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenWeekContainer::BeginPopulationWithInstanceViewL()
    {
    TRACE_ENTRY_POINT;
    SizeChanged();
    iViewPopulationComplete = EFalse;
    
    if (iPreview)
        {
        iPreview->Hide();
        }
    iListBox->View()->SetDisableRedraw( ETrue );
    PopulatesDayListsL();

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenWeekContainer::FirstPopulateOfSlotTableL
// Prepares and puts non timed notes into the slot table.
// Second stage of view population.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenWeekContainer::FirstPopulateOfSlotTableL()
    {
    TRACE_ENTRY_POINT;

    InitSlotTableL();
    InsertNonTimedNotesToSlotTableL();

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenWeekContainer::SecondPopulateOfSlotTableL
// Puts timed notes into the slot table. Third stage of view population.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenWeekContainer::SecondPopulateOfSlotTableL()
    {
    TRACE_ENTRY_POINT;

    InsertTimedNotesToSlotTableL();

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenWeekContainer::CompletePopulationL
// Completes population. Fourth and final stage of view population.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenWeekContainer::CompletePopulationL()
    {
    TRACE_ENTRY_POINT;
    
    iListBox->View()->SetDisableRedraw( EFalse );
    SetCursorToActiveDayL();  
    SetListBoxDataL();
    AddToStackAndMakeVisibleL();
    iViewPopulationComplete = ETrue;
		
		iView->SetEditorActive( EFalse );
		
    // Popup needs to check if iViewPopulationComplete is set
    FocusChangeForPopupL();
    
    UpdateStatusPaneAndExtensionsL();
    DrawDeferred();
	// Hide/unhide "today" toolbar item based on the focused day
    UpdateTodayToolbarItemL();
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenWeekContainer::CreateIconIndicesL
// Create icon index for day view
// ----------------------------------------------------------------------------
//
void CCalenWeekContainer::CreateIconIndicesL( RArray<MCalenServices::TCalenIcons>& aIndexArray )
    {
    TRACE_ENTRY_POINT;
    // Icons needed for the week view
    aIndexArray.Reset();
    aIndexArray.AppendL( MCalenServices::ECalenBirthdayIcon );
    aIndexArray.AppendL( MCalenServices::ECalenMeetingIcon );
    aIndexArray.AppendL( MCalenServices::ECalenDaynoteIcon );
    aIndexArray.AppendL( MCalenServices::ECalenToDoIcon );
    
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenWeekContainer::InfoBarRectL
// Returns the available info bar rect for this container
// ----------------------------------------------------------------------------
TRect CCalenWeekContainer::InfoBarRectL( TBool aToolbarAvailable )
    {
    TRACE_ENTRY_POINT;
   
    // Get the main pane
    TAknLayoutRect main_cale_week_pane;
    main_cale_week_pane.LayoutRect( Rect(), 
                    AknLayoutScalable_Apps::main_cale_week_pane().LayoutLine() );
        
    TAknLayoutRect listscroll_cale_week_pane;
    listscroll_cale_week_pane.LayoutRect( main_cale_week_pane.Rect(), 
            AknLayoutScalable_Apps::listscroll_cale_week_pane(1).LayoutLine() );

    // Create a dummy label to find the layout rect
    CEikLabel* dummyLabel = new( ELeave ) CEikLabel;
    CleanupStack::PushL( dummyLabel );

    // Get the layout variant for the week view infobar.
    // We assume that we have one for this check, as we need to
    // find the size that would be available if we do have one.
    TInt layoutVariant = 0;
    if( aToolbarAvailable )
        {
        layoutVariant = 1;
        }
    else
        {
        layoutVariant = 0;
        }
           
    AknLayoutUtils::LayoutLabel( dummyLabel, listscroll_cale_week_pane.Rect(),
        AknLayoutScalable_Apps::listscroll_cale_week_pane_t1( layoutVariant ).LayoutLine() );        
              
    TRect infoRect = dummyLabel->Rect();
    TRect rect;
    // Discard the label
    CleanupStack::PopAndDestroy( dummyLabel );
    
    TRACE_EXIT_POINT;
    return infoRect;
    }

// ----------------------------------------------------------------------------
// CCalenWeekContainer::CheckLayoutAndExtensionsL
// check with layoutmanager for any layout changes
// ----------------------------------------------------------------------------
void CCalenWeekContainer::CheckLayoutAndExtensionL()
    {
    TRACE_ENTRY_POINT;
    
    iLayoutManager->GetLayoutAndExtensionL();
    
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenWeekContainer::SearchTopIndexL
// Searches for the top index
// ----------------------------------------------------------------------------    
TInt CCalenWeekContainer::SearchTopIndexL(TInt /*aColumn*/)    
	{
	TRACE_ENTRY_POINT;
	
	TInt row = KErrNotFound;

	MCalenContext& context = iServices.Context();
		
	if( iSelectedRowNumber == KErrNotFound && iRow)
    	{
    	row = KErrNotFound;
    	}
    else
    	{
    	if(iListBox->TopItemIndex()!=0 && !iTopRowDefault)
    		{
    		// row is top item index..
    		row = iListBox->TopItemIndex();
    		}
    	else
    		{
    		// row for the time
    		row = FindRowForTime(iFirstRowTime);	
    		iTopRowDefault =  EFalse;
    		}
    	}
    
	TRACE_EXIT_POINT;
	
    return row;	
	}
	
// -----------------------------------------------------------------------------
// CCalenWeekContainer::PreviewPane
// Return preview pane pointer
// -----------------------------------------------------------------------------
//	
const MCalenPreview* CCalenWeekContainer::PreviewPane()
    {
    TRACE_ENTRY_POINT
    TRACE_EXIT_POINT
    return iPreview;
    }	

// ----------------------------------------------------------------------------
// CCalenWeekContainer::UpdatePreviewPaneL
// Updates preview pane whenever application comes to foreground
// ----------------------------------------------------------------------------    
void CCalenWeekContainer::UpdatePreviewPaneL()
    {
    TRACE_ENTRY_POINT;
    
    if( iViewPopulationComplete )
        {
        CalcDayFromColumn();
        
        CWeekSlotArrayPtr slotTable = iSlotTable[iColumn];
        CCalHourItem& itemInfo = *(*slotTable)[iListBox->CurrentItemIndex()];
        
        // dont focus the preview popup/previewpane if any dialog or faster app is active
        if( iPreview && !iView->IsEditorActiveOrFasterAppExit())
            {
            if( !itemInfo.IsTimed() )
                { 
                // Non-timed cell
                iPreview->FocusChangedL(itemInfo.iInstance);
                }
            else
                {
                TDateTime dt = iTime.DateTime();    // TACOTEMP
                iPreview->FocusChangedL( iServices.Context().FocusDateAndTimeL().TimeLocalL() );
                }
            }
        }
    else
        {
        HidePopup();
        }
    
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenWeekContainer::CalendarInfoIdentifierL
// Searches for the index in calendar info list based on calendar file name
// -----------------------------------------------------------------------------
//
TBool CCalenWeekContainer::CalendarInfoIdentifierL( const HBufC* aName,
                                        const CCalCalendarInfo& aCalendarInfo)
    {
    TRACE_ENTRY_POINT;
    TBool retVal = EFalse;
    HBufC* calendarFileName = aCalendarInfo.FileNameL().AllocLC();
    retVal = calendarFileName->CompareF(*aName);
    CleanupStack::PopAndDestroy(calendarFileName);
    TRACE_EXIT_POINT;
    return (!retVal);
    }

// --------------------------------------------------------------------------
// CCalenWeekContainer::CleanupInstancesL
// (other items were commented in a header).
// --------------------------------------------------------------------------
//
void CCalenWeekContainer::CleanupInstances()
    {
    TRACE_ENTRY_POINT
    for (TInt loop(0); loop < KCalenDaysInWeek; ++loop)
          {
          iIdList[loop].ResetAndDestroy();
          }
    TRACE_EXIT_POINT     
    }


// End of File
