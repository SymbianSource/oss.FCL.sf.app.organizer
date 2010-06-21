/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  For Day view container of calendar application.
 *                The class derived from CCalenContainer
 *
*/

#include <Calendar.rsg>
#include <calenconstants.h>
#include <calendateutils.h>
#include <calenagendautils.h>
#include <calenservices.h>
#include <csxhelp/cale.hlp.hrh>
#include <AknsDrawUtils.h>
#include <AknsFrameBackgroundControlContext.h>
#include <AknUtils.h>
#include <AknIconArray.h>
#include <eikfrlbd.h>
#include <eiklabel.h>
#include <eikmenub.h>
#include <StringLoader.h>
#include <calenviewutils.h>
#include <aknlayoutscalable_apps.cdl.h>
#include <calinstance.h>
#include <calentry.h>
#include <calinstanceview.h>
#include <caltime.h>
#include <AknDef.h>
#include <calencontext.h>
#include <akntoolbar.h>
#include <aknstyluspopupmenu.h>
#include <barsread.h>
#include <gesturehelper.h> //CGestureHelper

// INCLUDE FILES
#include "calendaycontainer.h"
#include "calencontainerlayoutmanager.h"
#include "calencontroller.h"
#include "calenlocationutil.h"
#include "calendaylistboxmodel.h"
#include "calendaylistboxview.h"
#include "calendaylistbox.h"
#include "calendayview.h"
#include "CalenUid.h"
#include "calendar.hrh"
//debug
#include "calendarui_debug.h"

//  LOCAL CONSTANTS AND MACROS
const TInt KLayoutTableGranularity(10);
const TInt KNullMinutes = -1;

// ================= MEMBER FUNCTIONS =======================

// ----------------------------------------------------------------------------
// CCalenDayContainer::CCalenDayContainer
// C++ constructor
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
CCalenDayContainer::CCalenDayContainer( CCalenNativeView* aView,
                                        TTime& aDate,
                                        TInt& aHighlightedRowNumber,
                                        TInt& aFirstEntryOnScreenIndex,
                                        MCalenServices& aServices )
    : CCalenContainer( aView, aServices ),
      iDeletedIndex( KIndexError ),
      iDate( aDate ),
      iHighlightedRowNumber( aHighlightedRowNumber ),
      iFirstEntryOnScreenIndex( aFirstEntryOnScreenIndex )
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenDayContainer::~CCalenDayContainer
// Destructor
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
CCalenDayContainer::~CCalenDayContainer()
    {
    TRACE_ENTRY_POINT;

    delete iListBoxModel;
    delete iListBox;
    iInstanceList.ResetAndDestroy();
    delete iLayoutTable;
    // Skin contexts
    delete iBackgroundSkinContext;
    delete iGestureHelper;

    TRACE_EXIT_POINT;
    }
	
// ---------------------------------------------------------
// CCalenDayOnlyEventContainer::HandleListBoxEventL
// Handle events incoming from list instance. 
// ---------------------------------------------------------
//
void CCalenDayContainer::HandleListBoxEventL(CEikListBox* /*aListBox*/, 
                                                      TListBoxEvent aEventType)
    {
    TRACE_ENTRY_POINT;
    switch( aEventType )
        {
        // Single click integration
        case EEventItemSingleClicked:
            {
            // left/right gestures (touch down, drag, touch release) are treated
             // by CEikListBox as single click, probably this should be fixed by AVKON team
             if ( !iGestureHandled )
                 {
                 if(iView->MenuBar()->IsDisplayed() == EFalse)
                     {
                     iView->HandleCommandL( ECalenViewCurrentEntry );
                     }
                 }
            break;
            }
        case EEventEmptyListClicked:
            {
            // left/right gestures (touch down, drag, touch release) are treated
            // by CEikListBox as single click, probably this should be fixed by AVKON team
            if ( !iGestureHandled )
                {
                if (iListBox->Model()->NumberOfItems() <= 0)
                    {
                    iView->HandleCommandL(ECalenCmdOpenMskDialog);
                    }
                }
            break;
            }
            
        case EEventEmptyAreaClicked:
            {
            // left/right gestures (touch down, drag, touch release) are treated
            // by CEikListBox as single click, probably this should be fixed by AVKON team
			break;
            }
			
        // Single click integration
        case EEventEnterKeyPressed:
            {
            iView->HandleCommandL( ECalenViewCurrentEntry );
            break;
            }
        default:
            break;
        };
    TRACE_EXIT_POINT;
    }
	

// ----------------------------------------------------------------------------
// CCalenDayContainer::UpdateContainerL
// Refresh Day view data and draws.
// Data is based on active context.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenDayContainer::UpdateContainerL()
    {
    TRACE_ENTRY_POINT;

    UpdateStatusPaneAndExtensionsL();

    iView->BeginRepopulationL();
    iListBox->View()->SetDisableRedraw(ETrue);   

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenDayContainer::UpdateTimeFormat
// ?implementation_description
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenDayContainer::UpdateTimeFormat()
    {
    TRACE_ENTRY_POINT;

    SizeChanged();

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenDayContainer::DestroyInstanceListL
// ?implementation_description
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenDayContainer::DestroyInstanceListL()
    {
    TRACE_ENTRY_POINT;

    // NULL temporary instance references in layout table
    for ( TInt i=0; i < iLayoutTable->Count(); ++i )
        {
        iLayoutTable->At(i).iTmpInstance = NULL;
        }

    // Destroy instances.
    iInstanceList.ResetAndDestroy();

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenDayContainer::CreateListBoxDataL
// Set Listbox data to ListBox
// Step 4 of construction of ListBox data
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenDayContainer::CreateListBoxDataL()
    {
    TRACE_ENTRY_POINT;

    MCalenContext& context = iServices.Context();
	TInt aIndex = FindItemIndex( context.InstanceId() );
	
    // ---------------------
    // Listbox model and view setup
    iListBox->View()->SetDisableRedraw(ETrue);

    CArrayFixFlat<TInt>* cellArray = new(ELeave)CArrayFixFlat<TInt>(5);
    CleanupStack::PushL(cellArray);

    iListBoxModel->CreateListBoxDataL( *iLayoutTable,
                                       *cellArray,
                                       iServices,
                                       iDate );

    CleanupStack::Pop( cellArray );
    View()->SetCellArray( cellArray );
    
    UpdateHighlightedRowIfDeletedL();
    if(iHighlightedRowNumber!=KErrNotFound)
        {
        // Cycled to from another view when no one changed the context,
        // or refreshing current view. No need to set context.
      	if (aIndex!=KErrNotFound ) 
       		{
       		SItemInfo& itemInfo = (*iLayoutTable)[aIndex];
        	iHighlightedRowNumber = itemInfo.iTopLine;  
       		}
      	else if(iDeletedIndex!=KErrNotFound)
      	    {
      	    iDeletedIndex = KErrNotFound;
      	    }
      	else 
       	    {
       	    iHighlightedRowNumber = KErrNotFound;
       	    iFirstEntryOnScreenIndex = KErrNotFound;
       	    }
        }

    if(iDateWasNull) // iDate was null when population was started.
        {
        // Cycled to from another view after someone else changed the context.
        // Set the new highlight based on the context.
        SetHighlightFromContextL();
        }
    SetContextFromHighlightL();

    DestroyInstanceListL();

    iListBox->HandleItemAdditionL(); // Is this causing unnecessary draw?

    iListBox->View()->SetDisableRedraw(EFalse);
    iListBox->SetFocus(ETrue);
    iListBox->DrawNow();
    DayView().UpdateCbaL();

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenDayContainer::IsEmptyView
// ?implementation_description
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
TBool CCalenDayContainer::IsEmptyView() const
    {
    TRACE_ENTRY_POINT;

    TRACE_EXIT_POINT;
    return (iListBox->Model()->NumberOfItems() <= 0);
    }

// ----------------------------------------------------------------------------
// CCalenDayContainer::FindItem
// ?implementation_description
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
CCalenDayContainer::SItemInfo& CCalenDayContainer::FindItem( TInt aLineIndex ) const
    {
    TRACE_ENTRY_POINT;

    ASSERT( aLineIndex >= 0 );

    TInt itemIx = FindItemIndexForRow( aLineIndex );
    __ASSERT_DEBUG(itemIx >= 0, User::Invariant());

    SItemInfo& result = (*iLayoutTable)[itemIx];

    TRACE_EXIT_POINT;
    return result;
    }

// ----------------------------------------------------------------------------
// ?classname::?member_function
// ?implementation_description
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
MCalenServices& CCalenDayContainer::Services()
    {
    TRACE_ENTRY_POINT
    TRACE_EXIT_POINT
    return iServices;
    }

// ----------------------------------------------------------------------------
// ?classname::?member_function
// ?implementation_description
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
TInt CCalenDayContainer::FindItemIndexForRow( TInt aListIndex ) const
    {
    TRACE_ENTRY_POINT;

    for ( TInt i(0); i < iLayoutTable->Count(); ++i )
        {
        SItemInfo& item = (*iLayoutTable)[i];
        if ( aListIndex >= item.iTopLine && aListIndex <= item.iBottomLine )
            {
            TRACE_EXIT_POINT;
            return i;
            }
        }
        
    TRACE_EXIT_POINT;
    return KIndexError;
    }

// ----------------------------------------------------------------------------
// CCalenDayContainer::UpdateHighlightedRowIfDeletedL
// Updates the highlighted index based on whether an entry has just been
// deleted.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenDayContainer::UpdateHighlightedRowIfDeletedL()
    {
    TRACE_ENTRY_POINT;

    CCalenDayListBoxView* view =  View();
    if ( iDeletedIndex != KErrNotFound )
        // If just deleted an entry
        {
        if ( iLayoutTable->Count() > 0 )
            {
            // Precondition: iDeletedIndex can contain only index to first line
            // of deleted item.
            // Fixed to work as specifed in Selection Service
            // 2.1.1 Focus while adding or deleting items.
            //TInt previousLine = iDeletedIndex;
            //highlightLine = previousLine;

            iHighlightedRowNumber = iDeletedIndex-1;
            }
        else
            {
            iHighlightedRowNumber = KErrNotFound;
            }
       // iDeletedIndex = KErrNotFound;
        }

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenDayContainer::SetHighlightFromContextL
// Sets the highlighted row based on the context.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenDayContainer::SetHighlightFromContextL()
    {
    TRACE_ENTRY_POINT;

    MCalenContext& context = iServices.Context();
    TCalenInstanceId instId = context.InstanceId();

    if ( instId == TCalenInstanceId::NullInstanceId() )
        {
        iHighlightedRowNumber = 0;

        // Context points at a day/time, not an entry.
        // If we have an entry for this day set it to be the context,
        // as long as we weren't cycled to.
        if ( iLayoutTable->Count() )
            {
            if ( context.FocusTime().Int() == -1 )
                {
                // Context has a date but no time and no instance.
                // We're not being cycled to, so set the context.
                SetContextFromHighlightL();
                }
            // else context has a date and time but no instance. This happens
            // for example when the week view highlights a cell with no instance,
            // then cycles to us - don't reset the context.
            }
//      else Nothing on this day. Don't set context, as it's already set to this day.
        }
    else
        {
        TInt index = FindItemIndex( context.InstanceId() );
        if(index != KErrNotFound)
            {
            SItemInfo& itemInfo = (*iLayoutTable)[index];
            iHighlightedRowNumber = itemInfo.iTopLine;
            iFirstEntryOnScreenIndex = iListBox->TopItemIndex();
            SetHighlightAndVisibleItem( iHighlightedRowNumber, iFirstEntryOnScreenIndex );
            }
        }

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenDayContainer::SetContextFromHighlightL
// Sets the context based on what's highlighted.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenDayContainer::SetContextFromHighlightL()
    {
    TRACE_ENTRY_POINT;

    MCalenContext& context = iServices.Context();
	
    if ( iLayoutTable->Count() <= 0 )
        {
        // Nothing in the day view. Set context to this day.
        TCalTime dayCalTime;
        dayCalTime.SetTimeLocalL( iDate );
        context.SetFocusDateL( dayCalTime,
                               TVwsViewId( KUidCalendar, KUidCalenDayView ) );
        iHighlightedRowNumber = KErrNotFound;
        }
    else
        {
        // Stuff is present for this day.
     	
     	SetHighlightAndVisibleItem( iHighlightedRowNumber, iFirstEntryOnScreenIndex );
		
		TInt lineIx = iListBox->View()->CurrentItemIndex();
	
    	// If list is empty, index is -1
    	if (lineIx >= 0)
        	{ 
        	TInt itemIx = FindItemIndexForRow(lineIx);
        	if( itemIx != KIndexError)
            	{
          		SItemInfo& item = (*iLayoutTable)[itemIx];
          		iHighlightedRowNumber = item.iTopLine;
          
          		if ( item.iTimedNote )
            		{
            		// Timed entry.
            		TCalTime dayCalTime;
            		dayCalTime.SetTimeLocalL( CalenDateUtils::BeginningOfDay( iDate ) + item.iStartTime );
            		context.SetFocusDateAndTimeAndInstanceL( dayCalTime,
                    		                                 item.iId,
                            		                         TVwsViewId( KUidCalendar, KUidCalenDayView ) );
            		}
        		else
            		{
            		// Untimed entry.
            		if ( item.iId.iType == CCalEntry::ETodo &&
                 		item.iId.iInstanceTime < CalenDateUtils::Today() )
                		{
                		TCalTime dayCalTime;
                		dayCalTime.SetTimeLocalL( CalenDateUtils::Today() );
                		// For todos in the past, set focus on the todo but view it today.
                		context.SetFocusDateAndTimeAndInstanceL(
                        		                            dayCalTime,
                                		                    item.iId,
                                        		            TVwsViewId( KUidCalendar, KUidCalenDayView ) );
                		}
            		else
                		{
                		if ( CalenDateUtils::OnSameDay( item.iId.iInstanceTime, iDate ) )
                    		{
                    		context.SetInstanceIdL( item.iId,
                            		                TVwsViewId( KUidCalendar, KUidCalenDayView ) );
                    		}
                		else
                    		{
                    		// Multi-day untimed note (either multi-day day note or weird todo).
                    		// Focus on the instance and set the focus time to the highlighted day.
                    		TCalTime dayCalTime;
                    		dayCalTime.SetTimeLocalL( iDate );
                    		context.SetFocusDateAndTimeAndInstanceL( dayCalTime,
                            		                                 item.iId,
                                                             TVwsViewId( KUidCalendar, KUidCalenDayView ) );
 		                   }
        		        }
            		}
            
            
            
             	}
        	else 
            	{
            	// FIXME: If item is not found, should we do something? 
            	__ASSERT_DEBUG(EFalse, User::Invariant());
            	}
        	}
    	else
        	{ // Empty list
        	// do nothing
        	}
        }

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenDayContainer::FindItemIndex
// Returns the index into iLayoutTable of the given instance id (for highlighting).
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
TInt CCalenDayContainer::FindItemIndex( const TCalenInstanceId& aId ) const
    {
    TRACE_ENTRY_POINT;

    // For instances finishing the next day (now possible with unified DateTime editor),
    // we have to do our best to match the instance time exactly - otherwise we could
    // match the LocalUid to the incorrect instance in a series.
    for(TInt index(0); index < iLayoutTable->Count(); ++index)
        {
        SItemInfo& itemInfo = (*iLayoutTable)[index];

        if( itemInfo.iId.iEntryLocalUid == aId.iEntryLocalUid )
            {
            // Check the instance time matches.
            if( itemInfo.iId.iInstanceTime == aId.iInstanceTime )
                {
                return index;
                }
            }
        }

    // Couldn't match the instance time exactly - just use the instance
    // with the same LocalUid as the one we're looking for.
    // In theory this shouldn't happen but it's good to have a failsafe.
    for(TInt index(0); index < iLayoutTable->Count(); ++index)
        {
        SItemInfo& itemInfo = (*iLayoutTable)[index];

        if( itemInfo.iId.iEntryLocalUid == aId.iEntryLocalUid )
            {
            return index;
            }
        }

    TRACE_EXIT_POINT;
    return KErrNotFound;
    }

// ----------------------------------------------------------------------------
// CCalenDayContainer::SetHighlightAndVisibleItem
// Sets highlight item in ListBox
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenDayContainer::SetHighlightAndVisibleItem( TInt aIndex,
                                                     TInt aTopItem )
    {
    TRACE_ENTRY_POINT;

    TInt count(iListBox->Model()->NumberOfItems());
    if ( count <= 0 )
        {
        return;
        }

    // aIndex and aTopItem can be indexes to any
    // line, not just first lines of items.
    // But we want to make sure that current item and top item indexes points
    // always only to the first line of item.

    // Limit range
    aIndex = Min(count - 1, aIndex);
    aIndex = Max(0, aIndex);

    // FIXME: can be done with layout table, no need for view-> calls  !
    TInt firstLine = View()->FirstLineOfCell(aIndex);
    iListBox->SetCurrentItemIndex(firstLine);

    SetVisibleLines(firstLine, aTopItem);

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// ?classname::?member_function
// ?implementation_description
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenDayContainer::SetVisibleLines( TInt aHighlightFirstLine,
                                          TInt aPreferredTopLine )
    {
    TRACE_ENTRY_POINT;

    CCalenDayListBoxView* view =  View();

    TInt highLightLastLine = view->LastLineOfCell( aHighlightFirstLine );

    TInt maxVisibleLines = view->NumberOfItemsThatFitInRect( view->ViewRect() );
    TInt totalListLines = iListBox->Model()->NumberOfItems();

    // Initial top line
    TInt topLine = aPreferredTopLine == KIndexError ? aHighlightFirstLine : aPreferredTopLine;
    topLine = Min(totalListLines - 1, topLine);
    topLine = Max(0, topLine);

    if ( aHighlightFirstLine < topLine )
        {
        topLine = aHighlightFirstLine;
        }

    // Adjust bottom line
    TInt bottomLine = topLine + maxVisibleLines - 1;
    TInt lastListLine = totalListLines - 1;
    bottomLine = bottomLine > lastListLine ? lastListLine : bottomLine;
    if ( highLightLastLine > bottomLine )
        {
        bottomLine = highLightLastLine;
        }

    TInt currentlyVisibleLines = bottomLine - topLine + 1;
    if ( currentlyVisibleLines > maxVisibleLines )
        {
        topLine = bottomLine - maxVisibleLines + 1;
        }
    // Now we have ensured that highlight is between [topLine, bottomLine]
    // If there are room for more lines in display, and still more lines
    // in list, we can scroll view up

    currentlyVisibleLines = bottomLine - topLine + 1;
    if ( (currentlyVisibleLines < maxVisibleLines - 1)
         && (totalListLines >= currentlyVisibleLines))
        {
        topLine = bottomLine - maxVisibleLines + 1;
        topLine = topLine < 0 ? 0 : topLine;
        }

    iListBox->SetTopItemIndex( topLine );

    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------
// CCalenDayContainer::SearchNearestItem
// Searches list item that is nearest to aTime
// (other items were commented in a header).
// ---------------------------------------------------------
//
TInt CCalenDayContainer::SearchNearestItem( TTimeIntervalMinutes aTime ) const
    {
    TRACE_ENTRY_POINT;

    TInt foundIndex( KIndexError );
    const TInt count( iLayoutTable->Count() );

    for( TInt index(0); index < count; ++index )
        {
        SItemInfo& itemInfo = (*iLayoutTable)[index];

        // times match
        if( itemInfo.iStartTime == aTime )
            {
            foundIndex = index;
            break;
            }
        // times fall into same hour
        else if( (itemInfo.iStartTime.Int()/KCalenMinutesInHour) == (aTime.Int() / KCalenMinutesInHour)
                 && foundIndex == KIndexError )
            {
                // Same starting hour
                foundIndex = index;
            }
        }

    TRACE_EXIT_POINT;
    return foundIndex;
    }

// ----------------------------------------------------------------------------
// CCalenDayContainer::SizeChanged
// Resizes child controls
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenDayContainer::SizeChanged()
    {
    TRACE_ENTRY_POINT;

    CCalenContainer::SizeChanged( Rect() );
    
    TRAPD(error,iLayoutManager->GetLayoutAndExtensionL());
    if(error!=KErrNone)
    	{
    	// do avoid warning
    	}
              
    // Find the layout variant based upon a preview pane building present or not.
    TInt layoutVariant = LayoutVariantIndex( EListScrollCaleDayPane );
       
    TAknLayoutRect main_cale_day_pane;
    main_cale_day_pane.LayoutRect( Rect(), 
                    AknLayoutScalable_Apps::main_cale_day_pane(0).LayoutLine() );
        
    TAknLayoutRect listscroll_cale_day_pane;
    listscroll_cale_day_pane.LayoutRect( main_cale_day_pane.Rect(), 
            AknLayoutScalable_Apps::listscroll_cale_day_pane( layoutVariant ).LayoutLine() );
    

#ifdef RD_CALEN_MIDNIGHT_VISUALIZATION
    PIM_TRAPD_HANDLE(GetMidnightVisualizationL()) // sets iMidnight
#endif // RD_CALEN_MIDNIGHT_VISUALIZATION


    iListBoxModel->UpdateLayoutValues( layoutVariant
#ifdef RD_CALEN_MIDNIGHT_VISUALIZATION
        , iMidnight
#endif // RD_CALEN_MIDNIGHT_VISUALIZATION
        );

    // Now get the correct version for if we have a info bar or not
    listscroll_cale_day_pane.LayoutRect( main_cale_day_pane.Rect(), AknLayoutScalable_Apps::listscroll_cale_day_pane(layoutVariant).LayoutLine() );


    layoutVariant = LayoutVariantIndex( EBgCalePane );

    TAknLayoutRect bg_cale_pane;
    bg_cale_pane.LayoutRect( listscroll_cale_day_pane.Rect(), AknLayoutScalable_Apps::bg_cale_pane( layoutVariant ).LayoutLine() );

    TAknLayoutRect cale_bg_pane_center;
    cale_bg_pane_center.LayoutRect( bg_cale_pane.Rect(), AknLayoutScalable_Apps::cale_bg_pane_g1().LayoutLine() );

    iBackgroundSkinContext->SetFrameRects( bg_cale_pane.Rect(), cale_bg_pane_center.Rect() );
    iBackgroundSkinContext->SetParentContext( iBgContext );

    // Listbox layout
    TAknLayoutRect list_cale_pane;
    list_cale_pane.LayoutRect( main_cale_day_pane.Rect(), AknLayoutScalable_Apps::list_cale_pane( layoutVariant ).LayoutLine() );

    iListBox->SetRect( list_cale_pane.Rect() );

    // Inside of listbox.
    iListBox->ItemDrawer()->FormattedCellData()->SetBackgroundSkinStyle( &KAknsIIDQsnFrCaleCenter, cale_bg_pane_center.Rect() );
    layoutVariant = LayoutVariantIndex( EScrollPaneCp09 );
    // Scrollbar layout
    AknLayoutUtils::LayoutVerticalScrollBar( iListBox->ScrollBarFrame(),
                                             listscroll_cale_day_pane.Rect(),
                                             AknLayoutScalable_Apps::scroll_pane_cp09( layoutVariant ).LayoutLine() );
    
   
       
    PIM_TRAPD_HANDLE( UpdateStatusPaneAndExtensionsL() )
    
    CCoeControl* infoControl = iLayoutManager->ControlOrNull();
    if(infoControl)
    	{
    	TRAP_IGNORE(infoControl->ActivateL());
    	}
        
    
    DrawNow();
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenDayContainer::FocusChanged
// ?implementation_description
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenDayContainer::FocusChanged( TDrawNow aDrawNow )
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
// CCalenDayContainer::UpdateIconsL
// update icons
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenDayContainer::UpdateIconsL()
    {
    TRACE_ENTRY_POINT;
    
    CAknIconArray* iconList = CreateIconsL( iIconIndices );
    if(iListBox)
    {
    CArrayPtr<CGulIcon>* iconArray =iListBox->ItemDrawer()->FormattedCellData()->IconArray();
    delete iconArray;
    iconArray = NULL;
    iListBox->ItemDrawer()->ColumnData()->SetIconArray( iconList );
    }
    
    TRACE_EXIT_POINT;
    }
    
// ----------------------------------------------------------------------------
// CCalenDayContainer::HandleResourceChange
// Called when layout or skin changes
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenDayContainer::HandleResourceChange( TInt aType )
    {
    TRACE_ENTRY_POINT;

    CCalenContainer::HandleResourceChange( aType );

    if ( aType == KAknsMessageSkinChange || aType == KEikDynamicLayoutVariantSwitch )
        {
        TRAPD(error,UpdateIconsL());
	    if(error!=KErrNone)
	       	{
	       	// do avoid warning
	       	}     
        // refresh
        TRAPD(err,iView->BeginRepopulationL());
	    if(err!=KErrNone)
	       	{
	       	// do avoid warning
	       	}
	    SizeChanged();

	            // Update date formats to status pane
	    TRAP_IGNORE(UpdateStatusPaneAndExtensionsL());

        }

    CCoeControl::HandleResourceChange( aType );

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenDayContainer::CountComponentControls
// Gets child control count
// (other items were commented in a header).
// ----------------------------------------------------------------------------
TInt CCalenDayContainer::CountComponentControls() const
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
// CCalenDayContainer::ComponentControl
// Gets child control pointer
// (other items were commented in a header).
// ----------------------------------------------------------------------------
CCoeControl* CCalenDayContainer::ComponentControl(TInt aIndex) const
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
// CCalenDayContainer::OfferKeyEventL
// Process key event
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
TKeyResponse CCalenDayContainer::OfferKeyEventL( const TKeyEvent& aKeyEvent,
                                                          TEventCode aType )
    {
    TRACE_ENTRY_POINT;

    // common view key handling
    if (CCalenContainer::OfferKeyEventL(aKeyEvent, aType) == EKeyWasConsumed)
        {
        return EKeyWasConsumed;
        }

    // day view specific key handling
    TKeyResponse exitCode(EKeyWasNotConsumed);

    if (aType == EEventKey)
        {
        TBool mirrored(AknLayoutUtils::LayoutMirrored());
        switch (aKeyEvent.iCode)
            {
            case EKeyLeftArrow:
                {
                HandleHorizontalScrollL(TTimeIntervalDays(mirrored ? 1 : -1));
                exitCode = EKeyWasConsumed;
                break;
                }
            case EKeyRightArrow:
                {
                HandleHorizontalScrollL(TTimeIntervalDays(mirrored ? -1 : 1));
                exitCode = EKeyWasConsumed;
                break;
                }
            case EKeyBackspace:
                {
                 // Single click integration
                if( iView->MenuBar()->ItemSpecificCommandsEnabled()
                        && !IsEmptyView() )
	                {
	                iView->HandleCommandL( ECalenDeleteCurrentEntry );
	                exitCode = EKeyWasConsumed;
	                }
                break;
                }
            case EKeyOK:
                {
                if(iListBox->Model()->NumberOfItems() > 0)
                    {
                     // Single click integration
                    iListBox->OfferKeyEventL( aKeyEvent, aType );
                    }
                else
                    {
                    CEikMenuBar* menuBar = iView->MenuBar();
                    menuBar->SetContextMenuTitleResourceId( R_CALENDAR_DAY_CASE_MENUBAR );
                    menuBar->TryDisplayContextMenuBarL();
                    }
                exitCode = EKeyWasConsumed;
                break;
                }
            case EKeyEnter:    
        		{
        		if(IsEmptyView())
                   	{ 
                   	iView->HandleCommandL(ECalenCmdOpenMskDialog);                    
                    }
                else
	                {
                     // Single click integration
                    iListBox->OfferKeyEventL( aKeyEvent, aType );
	            	}
         		exitCode = EKeyWasConsumed;	
                break;
        		}
            default:
                break;
            }
        }

	// For handling geokeys on corolla hardware
	// For Add location hard key
	if( aKeyEvent.iScanCode == EStdKeyApplication1C && aType == EEventKeyUp )
		{
         // Single click integration
        if( iView->MenuBar()->ItemSpecificCommandsEnabled()
                && CCalenLocationUtil::IsMapProviderAvailableL() )
			{
			MCalenContext& context = iServices.Context();
			TCalLocalUid instanceId = context.InstanceId().iEntryLocalUid;
			CCalEntry* entry = iServices.EntryViewL(context.InstanceId().iColId)->FetchL(instanceId);
			CleanupStack::PushL( entry );
			if(entry)
				{
				CCalEntry::TType entryType = entry->EntryTypeL();
				if(entryType == CCalEntry::EAppt)
					{
					iServices.IssueCommandL(ECalenGetLocationAndSave);
					}
				}
			CleanupStack::PopAndDestroy( entry );
			}
		exitCode = EKeyWasConsumed;
		}
	// For Show location hard key
    // Single click integration 
    else if( aKeyEvent.iScanCode == EStdKeyApplication1B && aType == EEventKeyUp
            && iView->MenuBar()->ItemSpecificCommandsEnabled() )
		{
		MCalenContext& context = iServices.Context();
		TCalLocalUid instanceId = context.InstanceId().iEntryLocalUid;
		
		CCalEntry* entry = iServices.EntryViewL(context.InstanceId().iColId)->FetchL(instanceId);
		CleanupStack::PushL( entry );
		if(entry)
			{
			CCalEntry::TType entryType = entry->EntryTypeL();
			if(entryType == CCalEntry::EAppt)
				{
				TPtrC location = entry->LocationL();
				if( entry->GeoValueL())
					{ 
					iServices.IssueCommandL(ECalenShowLocation);
					}
				else if(location.Length())
					{
					iServices.IssueCommandL(ECalenGetLocationAndReplace);
					}
				else
					{
					iServices.IssueCommandL(ECalenShowLocationQuery);
					}
				}
			}
		CleanupStack::PopAndDestroy( entry );
		}
		
    // listbox key handling
    if (exitCode == EKeyWasConsumed || !iListBox)
        {        // if DoDeactivate() is done, iListBox will be NULL.
        // do nothing
        }
    else
        {
        TInt oldCursor = iListBox->View()->CurrentItemIndex();
        exitCode = iListBox->OfferKeyEventL( aKeyEvent, aType );
        TInt newCursor = iListBox->View()->CurrentItemIndex();
        if (oldCursor != newCursor)
            {
            HandleVerticalScrollL( newCursor );
            }
        }

    TRACE_EXIT_POINT;
    return exitCode;
    }

// ----------------------------------------------------------------------------
// CCalenDayContainer::HandleHorizontalScrollL
// Handles moving left or right to a new day.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenDayContainer::HandleHorizontalScrollL( TTimeIntervalDays aDirection )
    {
    TRACE_ENTRY_POINT;

    TTime time = CalenDateUtils::BeginningOfDay( iDate + aDirection );
    time += iServices.Context().DefaultTimeForViews();

    if ( CalenDateUtils::IsValidDay( time ) )
        {
        iDate = time;
        iHighlightedRowNumber = KErrNotFound;
        iFirstEntryOnScreenIndex = KErrNotFound;

        // set the idate in to context
        TCalTime time;
        time.SetTimeLocalL( iDate );
        iServices.Context().SetFocusDateL( time,
                         TVwsViewId( KUidCalendar, KUidCalenDayView ) );

        UpdateContainerL(); // Repopulate.
        }
    else
        {
        CalenViewUtils::ShowDateOutOfRangeErrorNoteL();
        }

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenDayContainer::HandleVerticalScrollL
// Handles moving up or down in the same day.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenDayContainer::HandleVerticalScrollL( TInt aNewCursor )
    {
    TRACE_ENTRY_POINT;

    iHighlightedRowNumber = aNewCursor;
    ASSERT( iHighlightedRowNumber >= 0 );
    iFirstEntryOnScreenIndex = iListBox->TopItemIndex();
    SetContextFromHighlightL();

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenDayContainer::Draw
// Drawing day name
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenDayContainer::Draw(const TRect& /*aRect*/) const  // Clipping rectangle
    {
    TRACE_ENTRY_POINT;

    CWindowGc& gc = SystemGc();

    MAknsControlContext* cc =AknsDrawUtils::ControlContext( this );

    if( cc )
        {
        TRect main_pane( Rect() );

        // To prevent "tearing" (tearing resembles flickering), we don't want to clear
        // area under bottom-most skin image (KAknsDrawParamNoClearUnderImage).
        // But if bottom-most skin is semitransparent, we need to draw it without alpha channel,
        // so that something else doesn't show through it (KAknsDrawParamBottomLevelRGBOnly).
        const TInt drawParams = KAknsDrawParamNoClearUnderImage | KAknsDrawParamBottomLevelRGBOnly;
        AknsDrawUtils::Background( AknsUtils::SkinInstance(),
                                   cc,
                                   NULL,
                                   gc,
                                   main_pane,
                                   drawParams);
        }

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenDayContainer::GetHelpContext
// Gets help context
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenDayContainer::GetHelpContext( TCoeHelpContext& aContext ) const
    {
    TRACE_ENTRY_POINT;

    aContext.iMajor = KUidCalendar;
    // This is specified in HRH file.
    aContext.iContext = KCALE_HLP_DAY_VIEW;

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenDayContainer::UpdateSize
// Do layout changes after calendar settings are
// changed.  No necessary actions for this view.
// ----------------------------------------------------------------------------
//
void CCalenDayContainer::UpdateSize()
    {
    TRACE_ENTRY_POINT;

    SizeChanged();

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenDayContainer::ConstructImplL
// Third phase constructor.
// This function was called CCalenNativeView::ConstructL().
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenDayContainer::ConstructImplL()
    {
    TRACE_ENTRY_POINT;

    // 1) Extension plugins
  //  InitializeUiExtensionL();

    // 2) Background Skins
    iBackgroundSkinContext = CAknsFrameBackgroundControlContext::NewL(
        KAknsIIDQsnFrCale, TRect(0,0,0,0), TRect(0,0,0,0), EFalse );

    // 3) Listbox initialization
    iListBox = CCalenDayListBox::NewL(this, iServices);
    iListBox->SetListBoxObserver( this );

    iListBoxModel = CCalenDayListBoxModel::NewL( this );
    CTextListBoxModel* model = iListBox->Model();
    model->SetItemTextArray(iListBoxModel->ItemTextArray());
    model->SetOwnershipType(ELbmDoesNotOwnItemArray);
    
    CAknIconArray* iconList = CreateIconsL( iIconIndices );
    iListBox->ItemDrawer()->ColumnData()->SetIconArray( iconList );
    
    iGestureHelper = GestureHelper::CGestureHelper::NewL( *this );
    iGestureHelper->SetHoldingEnabled( EFalse );
    iGestureHelper->SetDoubleTapEnabled( EFalse );
    
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenDayContainer::NotifyDeleteItemL
// Delete note command notification handler
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenDayContainer::NotifyDeleteItemL()
    {
    TRACE_ENTRY_POINT;

    iDeletedIndex = iListBox->View()->CurrentItemIndex();
//    CCalenContainer::NotifyDeleteItemL();

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenDayContainer::NotifyChangeDateL
// Date cahnge operation notification handler.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenDayContainer::NotifyChangeDateL()
    {
    TRACE_ENTRY_POINT;

    TTime newDay = iServices.Context().FocusDateAndTimeL().TimeLocalL();

    if ( ! CalenDateUtils::OnSameDay( iDate, newDay ) )
        {
        iDate = newDay;
        UpdateContainerL();
        }
    else
        {
        iListBox->View()->SetDisableRedraw( ETrue );
        SetContextFromHighlightL();
        iListBox->View()->SetDisableRedraw( EFalse );
        iListBox->DrawNow();
        }

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenDayContainer::MopSupplyObject
// Pass skin information if needed.
// ----------------------------------------------------------------------------
//
TTypeUid::Ptr CCalenDayContainer::MopSupplyObject(TTypeUid aId)
    {
    TRACE_ENTRY_POINT;

    if(aId.iUid == MAknsControlContext::ETypeId )
        {
        MAknsControlContext* cc = iBackgroundSkinContext;
        if ( !cc ) cc = iBgContext;

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
// CCalenDayContainer::View
// ?implementation_description
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
CCalenDayListBoxView* CCalenDayContainer::View()
    {
    TRACE_ENTRY_POINT;

    TRACE_EXIT_POINT;
    return static_cast<CCalenDayListBoxView*>(iListBox->View());
    }

// ----------------------------------------------------------------------------
// CCalenDayContainer::DayView
// ?implementation_description
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
CCalenDayView& CCalenDayContainer::DayView() const
    {
    TRACE_ENTRY_POINT;

    TRACE_EXIT_POINT;
    return *(static_cast<CCalenDayView*>(iView));
    }

// ----------------------------------------------------------------------------
// CCalenDayContainer::LayoutVariantIndex
// ----------------------------------------------------------------------------
//
TInt CCalenDayContainer::LayoutVariantIndex( TDayLayoutVariant aLayout ) const
    {
    TRACE_ENTRY_POINT;
    
    TBitFlags layoutFlags = iLayoutManager->LayoutFlags();

    TInt layoutVariant(ENoLayoutVariantSet);
    switch (aLayout)
        {
        case EListScrollCaleDayPane:
            {
            if( layoutFlags.IsSet( 
                    CCalenContainerLayoutManager::ECalenContainerToolbar ) )
                layoutVariant = 1;
            else
                layoutVariant = 0;
            break;
            }
        case EPopupToolbarWindowCp02:
            {
            if( layoutFlags.IsSet( 
                    CCalenContainerLayoutManager::ECalenContainerToolbar ) )
                layoutVariant = 0;
            else
                __ASSERT_DEBUG(0, User::Invariant());
            break;
            }
        case EBgCalePane:
        case EListCalePane:
        case EScrollPaneCp09:
            {
            if( layoutFlags.IsSet( 
                    CCalenContainerLayoutManager::ECalenContainerToolbar ) && 
                layoutFlags.IsSet( 
                    CCalenContainerLayoutManager::ECalenContainerInfobar ) )
                layoutVariant = 3;
                
            else if( layoutFlags.IsSet( 
                    CCalenContainerLayoutManager::ECalenContainerToolbar ) )
                layoutVariant = 2;
                
            else if( layoutFlags.IsSet( 
                    CCalenContainerLayoutManager::ECalenContainerInfobar ) )
                layoutVariant = 3;
            else
                layoutVariant = 0;
            break;
            }
        case EListScrollCaleDayPaneT1:
            {
            if( layoutFlags.IsSet(
                    CCalenContainerLayoutManager::ECalenContainerToolbar ) && 
                layoutFlags.IsSet( 
                    CCalenContainerLayoutManager::ECalenContainerInfobar ) )
                layoutVariant = 1;
                
            else if( layoutFlags.IsSet( 
                    CCalenContainerLayoutManager::ECalenContainerInfobar ) )
                layoutVariant = 0;
            else
                __ASSERT_DEBUG(0, User::Invariant());
            break;
            }
         case ENoLayoutVariantSet:
            {
            ASSERT(EFalse);
            break;
            }
        }

    TRACE_EXIT_POINT;
    return layoutVariant;
    }


// ----------------------------------------------------------------------------
// CCalenDayContainer::HandleLongTapEventL
// processing of a long tap event
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenDayContainer::HandleLongTapEventL( const TPoint& /*aPenEventLocation*/,
                                              const TPoint& /*aPenEventScreenLocation*/ )
    {
    TRACE_ENTRY_POINT;


    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenDayContainer::HandlePointerEventL
// ?implementation_description
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenDayContainer::HandlePointerEventL( const TPointerEvent& aPointerEvent )
    {
    TRACE_ENTRY_POINT;
    CCoeControl* control( NULL );

    if(! AknLayoutUtils::PenEnabled() )
        {
        return;
        }
        
    if(aPointerEvent.iType == TPointerEvent::EButton1Down)
        {
        this->GenerateTactileFeedback(); //Tactile feedback.
        control = iLayoutManager->ControlOrNull();
        if(control)
            {
            if(control->Rect().Contains(aPointerEvent.iPosition))
                {
                control->HandlePointerEventL(aPointerEvent);	
                return;
                }
            }
        }
		if(iGestureHelper)
        {
        iGestureHelper->HandlePointerEventL( aPointerEvent );
        if (  iGestureHandled )
            {
            TRACE_EXIT_POINT;
            return;
            }
        }
	        	
	        	
    TInt pointerIndex(-1);    
     // Single click integration
    if ( aPointerEvent.iType == TPointerEvent::EButton1Down
            && iListBox->View()->XYPosToItemIndex(
                    aPointerEvent.iPosition, pointerIndex ) )
        {
        CCalenDayListBoxView* view =
            static_cast<CCalenDayListBoxView*>( iListBox->View() );
        TInt pointerItemIndex( view->FirstLineOfCell( pointerIndex ) );
        if ( iListBox->CurrentItemIndex() != pointerItemIndex )
            {
            HandleVerticalScrollL( pointerItemIndex );
            }
        }
    if ( aPointerEvent.iType != TPointerEvent::EButtonRepeat )
        {
        iListBox->HandlePointerEventL( aPointerEvent );
        }

    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------------------------
// CCalenDayContainer::HandleGestureL
// Handle the gesture event
// ---------------------------------------------------------------------------
//
void CCalenDayContainer::HandleGestureL( const GestureHelper::MGestureEvent& aEvent )
    {
    TRACE_ENTRY_POINT;
    GestureHelper::TGestureCode code = aEvent.Code( GestureHelper::MGestureEvent::EAxisBoth );
    
    switch ( code )
        {
        case GestureHelper::EGestureStart:
            {
            iGestureHandled = EFalse;
            break;
            }
        case GestureHelper::EGestureSwipeRight:
            {
            HandleNaviDecoratorEventL( EAknNaviDecoratorEventLeftTabArrow );
            iGestureHandled = ETrue;
            break;
            }
        case GestureHelper::EGestureSwipeLeft:
            {
            HandleNaviDecoratorEventL( EAknNaviDecoratorEventRightTabArrow );
            iGestureHandled = ETrue;
            break;
            }
        default:
            // Other gestures are not handled here
            break;
        }
    
    TRACE_EXIT_POINT;
    }
// ----------------------------------------------------------------------------
// CCalenDayContainer::HandleNaviDecoratorEventL
// ?implementation_description
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenDayContainer::HandleNaviDecoratorEventL( TInt aEventID )
    {
    TRACE_ENTRY_POINT;

    if(AknLayoutUtils::PenEnabled())
        {
        TInt direction(0);
        if(aEventID == EAknNaviDecoratorEventLeftTabArrow)
                    {
                    if(AknLayoutUtils::LayoutMirrored())
                        direction = 1;
                    else
                        direction = -1;
                    }
                    
                else if(aEventID == EAknNaviDecoratorEventRightTabArrow)
                    { 
                    if(AknLayoutUtils::LayoutMirrored())
                         direction = -1;
                    else
                         direction = 1;
                    }
        else
            return;

        HandleHorizontalScrollL(TTimeIntervalDays(direction));
        }

    TRACE_EXIT_POINT;
    }


#ifdef RD_CALEN_MIDNIGHT_VISUALIZATION
void CCalenDayContainer::GetMidnightVisualizationL()
    {
    TRACE_ENTRY_POINT;

    iMidnight = EFalse;
    TTime focusedDayBeginning = CalenDateUtils::BeginningOfDay(iDate);
    TTime focusedDayEnd = focusedDayBeginning + TTimeIntervalDays(1);

    TInt count = iInstanceList.Count();
    for(TInt i = 0; i < count; ++i )
        {
        CCalInstance* instance = iInstanceList[i];
        if((instance->StartTimeL().TimeLocalL() < focusedDayBeginning)
        || (instance->EndTimeL().TimeLocalL() > focusedDayEnd))
            {
            iMidnight = ETrue;
            break;
            }
        }

    TRACE_EXIT_POINT;
    }

TBool CCalenDayContainer::IsMidnightVisualization() const
    {
    TRACE_ENTRY_POINT;

    TRACE_EXIT_POINT;
    return iMidnight;
    }

#endif // RD_CALEN_MIDNIGHT_VISUALIZATION

// ----------------------------------------------------------------------------
// CCalenDayContainer::GetInstanceListL
// Get instances for day. List of instances is processed during construction
// of list box data and destroyed once it's done.
// Step 1 of construction of listbox data.
// ----------------------------------------------------------------------------
//
void CCalenDayContainer::GetInstanceListL()
    {
    TRACE_ENTRY_POINT;

    iInstanceList.ResetAndDestroy();

    iDateWasNull = iDate == Time::NullTTime();
    iDate = CCalenContainer::DateFromContextL( iServices.Context() );
    iDateWasNull = EFalse;
    
    RArray<TInt> colIdArray;
    CCalenNativeView::GetActiveCollectionidsL( iServices, colIdArray );
    
    if(colIdArray.Count() > 0)
          {
          CalenAgendaUtils::CreateEntryIdListForDayL( iInstanceList,
                                                iServices.InstanceViewL(colIdArray),
                                                iDate );          
          }
    else
        {
        CalenAgendaUtils::CreateEntryIdListForDayL( iInstanceList,
                                                      iServices.InstanceViewL(),
                                                      iDate );
        }
    colIdArray.Reset();
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenDayContainer::CreateSlotTableL
// Setup layout table. Step 2 of construction of listbox data
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenDayContainer::CreateSlotTableL()
    {
    TRACE_ENTRY_POINT;

    // Init layoyt table
    delete iLayoutTable;
    iLayoutTable = NULL;
    iLayoutTable = new( ELeave )CArrayFixFlat<SItemInfo>( KLayoutTableGranularity );
    
        // Process non-timed entries second
    for ( TInt instanceIndex = 0; instanceIndex < iInstanceList.Count(); ++instanceIndex )
        {
        CCalInstance* instance = iInstanceList[ instanceIndex ];
        if ( ! CalenAgendaUtils::IsTimedEntryL( instance->Entry().EntryTypeL()) 
            && (!CalenViewUtils::IsAlldayEventL( *instance))  )
            {
            SItemInfo itemInfo;
            itemInfo.iId = TCalenInstanceId::CreateL( *instance );
            itemInfo.iStartTime = TTimeIntervalMinutes( KNullMinutes );
            itemInfo.iTimedNote = EFalse;
            itemInfo.iTopLine = KErrNotFound;

            itemInfo.iTmpInstance = instance;

            iLayoutTable->AppendL( itemInfo );
            }
        }
    
    // Process Alldayevent timed entries first
    for ( TInt instanceIndex = 0; instanceIndex < iInstanceList.Count(); ++instanceIndex )
        {
        CCalInstance* instance = iInstanceList[ instanceIndex ];
        if( (instance->Entry().EntryTypeL() == CCalEntry::EAppt
             || instance->Entry().EntryTypeL() == CCalEntry::EEvent) 
             && (CalenViewUtils::IsAlldayEventL( *instance)) )
            {
            TTime start = instance->Time().TimeLocalL();
            SItemInfo itemInfo;
            itemInfo.iId = TCalenInstanceId::CreateL( *instance );
            itemInfo.iStartTime = CalenDateUtils::TimeOfDay( start );
            itemInfo.iTimedNote = EFalse;
            itemInfo.iTopLine = KErrNotFound;
            itemInfo.iTmpInstance = instance;

            iLayoutTable->AppendL( itemInfo );
            }
        }


    // Process timed entries second
    for ( TInt instanceIndex = 0; instanceIndex < iInstanceList.Count(); ++instanceIndex )
        {
        CCalInstance* instance = iInstanceList[ instanceIndex ];
        if ( ( CalenAgendaUtils::IsTimedEntryL( instance->Entry().EntryTypeL() ) )
             && ( !CalenViewUtils::IsAlldayEventL( *instance ) ) ) 
            {
            TTime start = instance->Time().TimeLocalL();

            SItemInfo itemInfo;
            itemInfo.iId = TCalenInstanceId::CreateL( *instance );
            itemInfo.iStartTime = CalenDateUtils::TimeOfDay( start );
            itemInfo.iTimedNote = ETrue;
            itemInfo.iTopLine = KErrNotFound;

            itemInfo.iTmpInstance = instance;

            iLayoutTable->AppendL( itemInfo );
            }
        }

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenDayContainer::CompletePopulationL
// Completes population. Third and last stage stage of population.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenDayContainer::CompletePopulationL()
    {
    TRACE_ENTRY_POINT;

    CreateListBoxDataL();
    AddToStackAndMakeVisibleL();
    UpdateStatusPaneAndExtensionsL();
	// Hide/unhide "today" toolbar item based on the focused day
    UpdateTodayToolbarItemL();
    DrawNow();
    TRACE_EXIT_POINT;
    }
    
// ----------------------------------------------------------------------------
// CCalenDayContainer::CreateIconIndicesL
// Create icon index for day view
// ----------------------------------------------------------------------------
//
void CCalenDayContainer::CreateIconIndicesL( RArray<MCalenServices::TCalenIcons>& aIndexArray )
    {
    TRACE_ENTRY_POINT;
    // Icons needed for the day view
    aIndexArray.Reset();
    aIndexArray.AppendL( MCalenServices::ECalenBirthdayIcon );
    aIndexArray.AppendL( MCalenServices::ECalenMeetingIcon );
    aIndexArray.AppendL( MCalenServices::ECalenDaynoteIcon );
    aIndexArray.AppendL( MCalenServices::ECalenToDoIcon );
    aIndexArray.AppendL( MCalenServices::ECalenAlarmIcon );
    aIndexArray.AppendL( MCalenServices::ECalenRepeatIcon );
    aIndexArray.AppendL( MCalenServices::ECalenRepeatExceptionIcon );
    aIndexArray.AppendL( MCalenServices::ECalenNotePriorityLow );
    aIndexArray.AppendL( MCalenServices::ECalenNotePriorityHigh );
    aIndexArray.AppendL( MCalenServices::ECalenMapIcon );

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenDayContainer::InfoBarRectL
// Returns the available info bar rect for this container
// ----------------------------------------------------------------------------
TRect CCalenDayContainer::InfoBarRectL( TBool aToolbarAvailable )
    {
    TRACE_ENTRY_POINT;
   
    // Get the main pane
    TAknLayoutRect main_cale_day_pane;
    main_cale_day_pane.LayoutRect( Rect(), 
                    AknLayoutScalable_Apps::main_cale_day_pane().LayoutLine() );
        
    TAknLayoutRect listscroll_cale_day_pane;
    listscroll_cale_day_pane.LayoutRect( main_cale_day_pane.Rect(), 
            AknLayoutScalable_Apps::listscroll_cale_day_pane(1).LayoutLine() );

    // Create a dummy label to find the layout rect
    CEikLabel* dummyLabel = new( ELeave ) CEikLabel;
    CleanupStack::PushL( dummyLabel );

    // Get the layout variant for the month view infobar.
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
           
    AknLayoutUtils::LayoutLabel( dummyLabel, listscroll_cale_day_pane.Rect(),
        AknLayoutScalable_Apps::listscroll_cale_day_pane_t1( layoutVariant ).LayoutLine() );        
              
    TRect infoRect = dummyLabel->Rect();

    // Discard the label
    CleanupStack::PopAndDestroy( dummyLabel );
    
    TRACE_EXIT_POINT;
    return infoRect;
    }
    
// ----------------------------------------------------------------------------
// CCalenDayContainer::PreviewRectL
// Returns an empty rect - Day view does not support preview pane
// ----------------------------------------------------------------------------
TRect CCalenDayContainer::PreviewRectL()
    {
    TRACE_ENTRY_POINT;

    TRect rect;

    TRACE_EXIT_POINT;
    return rect;
    }

// ----------------------------------------------------------------------------
// CCalenDayContainer::IsEventHasMapLocationL
// Stores the necessary information in the context
// and returns ETrue if event has geo coordinates else EFalse
// ----------------------------------------------------------------------------
TBool CCalenDayContainer::IsEventHasMapLocationL()
	{
	MCalenContext& context = iServices.Context();
	TCalLocalUid instanceId = context.InstanceId().iEntryLocalUid;
	
	CCalEntry* entry = iServices.EntryViewL(context.InstanceId().iColId)->FetchL(instanceId);

	CCalGeoValue* geoValue = entry->GeoValueL();
	if(geoValue)
		{
		delete geoValue;
		delete entry;
		// Event has saved map location, put "Show on Map"
		return 	ETrue;
		}
	else
		{
		return EFalse;
		}
	}
	
// ----------------------------------------------------------------------------
// CCalenDayContainer::IsEventHasNoLocationTextL
// Returns ETrue if event has location text else EFalse
// ----------------------------------------------------------------------------
TBool CCalenDayContainer::IsEventHasNoLocationTextL()
	{
	MCalenContext& context = iServices.Context();
	TCalLocalUid instanceId = context.InstanceId().iEntryLocalUid;
	
	CCalEntry* entry = iServices.EntryViewL(context.InstanceId().iColId)->FetchL(instanceId);
	TPtrC location = entry->LocationL();
	if(!location.Length())
		{
		return ETrue;
		}
	else
		{
		return EFalse;
		}
	}
// ----------------------------------------------------------------------------
// CCalenDayContainer::IsValidEntryL()
// Check whether entry exists or not
// ----------------------------------------------------------------------------
TBool CCalenDayContainer::IsValidEntryL()
    {
    TRACE_ENTRY_POINT;
    
    MCalenContext& context = iServices.Context();
    TCalLocalUid instanceId = context.InstanceId().iEntryLocalUid;
        
    CCalEntry* entry = iServices.EntryViewL(context.InstanceId().iColId)->FetchL(instanceId);
    CleanupStack::PushL(entry);
    TBool iSEntry(EFalse);
    if(entry)
        {
        iSEntry = ETrue;
        }
    else 
        {
        iSEntry = EFalse;
        }
    CleanupStack::PopAndDestroy(entry);
    TRACE_EXIT_POINT;
    return iSEntry;
    }
// End of File
