/*
* Copyright (c) 2009-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Day view control of calendar
*
*/

// SYSTEM INCLUDES
#include <calencommands.hrh>
#include <calinstanceview.h>
#include <calinstance.h>
#include <calentry.h>
#include <calsession.h>
#include <calcommon.h>
#include <caltime.h>
#include <calenviewutils.h>
#include <calenactionuiutils.h>
#include <StringLoader.h>
#include <aknlistquerydialog.h>
#include <calendar.rsg>
#include <e32math.h>
#include <CalenInterimUtils2.h>
#include <aknphysics.h>
#include <aknitemactionmenu.h> //stylus pop-up
#include <gesturehelper.h> //CGestureHelper
#include <AknUtils.h>
#include <AknNaviDecoratorObserver.h>
#include <calcalendarinfo.h>

// TOOLKIT INCLUDES
#include <alf/alfenv.h>
#include <alf/alfdisplay.h>
#include <alf/alfroster.h>
#include <alf/alfevent.h>
#include <alf/alfdecklayout.h>
#include <alf/alfanchorlayout.h>
#include <alf/alfgridlayout.h>
#include <alf/alfviewportlayout.h>
#include <alf/alftextvisual.h>
#include <alf/alfimagevisual.h>
#include <alf/alfbrusharray.h>
#include <alf/alfcontrolgroup.h>
#include <alf/alfgradientbrush.h>
#include <alf/alfborderbrush.h>
#include <alf/alfcanvasvisual.h>

// PROJECT INCLUDES
#include <bcviewplugingui.rsg>
#include "cbcdaycontainer.h"
#include "cbcbrushmanager.h"
#include "cbctextstylemanager.h"
#include "cbclayouthandler.h"
#include "cbcalendefs.h"
#include "mbcresourcemanager.h"
#include "calenagendautils.h"
#include "calenservices.h"
#include "mbcalenuienv.h"
#include "calencontext.h"
#include "caleninstanceid.h"
#include "calendateutils.h"
#include "calendar.hrh"
#include "cbcpreviewpopup.h"
#include "cleanupresetanddestroy.h"
#include "cbcdayphysicshandler.h"
#include "tbcprofile.h" // profiler
#include "calendarui_debug.h" // debug
#include "cbcviewbase.h"
#include "calenuid.h"

using namespace GestureHelper;

// ======== CONSTANTS ========
namespace{ // codescanner::namespace
const TInt KBCDayLayoutTableGranularity(10);
const TInt KBCDayUntimedEventTextIndex(1);
const TInt KBCDayPictureHMargin(4);
const TInt KFSCalDayEventTextPaddingX(4);
const TInt KFSCalDayEventTextPaddingY(1);
const TInt KBCDayTimeTextPadding(2);
const TInt KBCMinutesInVSInterval(30);
const TReal32 KVSAreaOpacity(0.7);
}

/**
 * KReplaceWhitespaceChars contains some characters that should be replaced by space 
 * in Calendar popup, day view etc. 
 * Following characters are replaced with space 
 * \x0009 horizontal tab
 * \x000a new line 
 * \x000b line tabulation (vertical
 * \x000c form feed
 * \x000d carriage return
 * \x2028 line separator
 * \x2029 paragraph separator
 */
_LIT( KReplaceWhitespaceChars, "\x0009\x000A\x000B\x000C\x000D\x2028\x2029" );

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CBCDayContainer::CBCDayContainer
// C++ constructor can NOT contain any code that maybe leave
// ---------------------------------------------------------------------------
//
CBCDayContainer::CBCDayContainer( 
        CAlfEnv& aEnv,
        CBCViewBase* aView,
        MCalenServices* aServices, 
        MBCResourceManager& aResManager)
    : CBCContainerBase( aEnv, aView, *aServices, aResManager )
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CBCDayContainer::ViewPositionChanged()
// -----------------------------------------------------------------------------
//
void CBCDayContainer::ViewPositionChanged( 
        const TPoint& aNewPosition,
        TBool aDrawNow,
        TUint /*aFlags*/ )
    {
    TRACE_ENTRY_POINT;
    
    //if aDraw is EFalse we should update the logical state only
    //physics handler ensures that the last frame is always drawn
    if ( aDrawNow )
        {
        //hide "hidden events" arrows - show them when the view stops
        TRAPD( error, DisplayScrollArrowsL( EFalse, EFalse, EFalse ) );
        if ( error )
            {
            //
            }
        
        SetViewPos(TAlfRealPoint((0.0),aNewPosition.iY), 0);
        }
    else
        {
        iHorScrollPos = aNewPosition.iX;
        iVerScrollPos = aNewPosition.iY;
        
        if( iPhysics )
            {
            iPhysics->SetHScrollPos( iHorScrollPos );
            iPhysics->SetVScrollPos( iVerScrollPos );
            } 
        }

    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CBCDayContainer::PhysicEmulationEnded()
// -----------------------------------------------------------------------------
//
void CBCDayContainer::PhysicEmulationEnded()
    {
    TRACE_ENTRY_POINT;
    //calculate iTopVisibleSlot
    //top left corner of the view
    TInt tlPos( iVerScrollPos );
    TInt newTopVisibleSlot(KErrNotFound);
    newTopVisibleSlot = tlPos / iSlotHeight;
    newTopVisibleSlot++;
    //safety checks
    if ( newTopVisibleSlot < 0 )
        {
        newTopVisibleSlot = 0;
        }
    else if ( newTopVisibleSlot + iVisibleSlots >= KFSCalHourSlotCount + iExtraSlots )
        {
        newTopVisibleSlot = KFSCalHourSlotCount + iExtraSlots - iVisibleSlots;
        }
    iTopVisibleSlot = newTopVisibleSlot;
    
    //show (if necessary) "hidden events" arrows
    TRAPD( error, UpdateUnvisibleEventArrowsL( EFalse ) );
    if ( error )
       {
       //
       }
    TRACE_EXIT_POINT;
    }


// -----------------------------------------------------------------------------
// CBCDayContainer::ViewPosition()
// -----------------------------------------------------------------------------
//
TPoint CBCDayContainer::ViewPosition() const
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    return TPoint( 
          iViewSize.iWidth / 2 + iHorScrollPos,
          iViewSize.iHeight / 2 + iVerScrollPos );
    }

// ---------------------------------------------------------------------------
// CBCDayContainer::CollectionState()
// Returns the collection's state. The state is combination of flags defined 
// in @c TStateFlag.
// ---------------------------------------------------------------------------
//
TUint CBCDayContainer::CollectionState() const
    {
    TRACE_ENTRY_POINT;
    TUint flag(0);
    flag |= MAknCollection::EStateCollectionVisible;
    if ( IsSelectorEnabled() )
        {
        flag |= MAknCollection::EStateHighlightVisible;
        }
    TRACE_EXIT_POINT;
    return flag;
    }

// ---------------------------------------------------------------------------
// CBCDayContainer::ItemActionMenuClosed()
// Notifies that item action menu (@c CAknItemActionMenu) was closed. 
// ---------------------------------------------------------------------------
//
void CBCDayContainer::ItemActionMenuClosed()
    {
    TRACE_ENTRY_POINT;
    TRAP_IGNORE( SetSelectorStateL( ESelectorOff ) );
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------------------------
// CBCDayContainer::CollectionExtension()
// Extension function.
// ---------------------------------------------------------------------------
//
TInt CBCDayContainer::CollectionExtension( TUint /*aExtensionId*/, TAny*& /*a0*/, TAny* /*a1*/ )
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CBCDayContainer::HandleGestureL
// Handle the gesture event
// ---------------------------------------------------------------------------
//
void CBCDayContainer::HandleGestureL( const GestureHelper::MGestureEvent& aEvent )
    {
    TRACE_ENTRY_POINT;
    TGestureCode code = aEvent.Code(MGestureEvent::EAxisBoth);
    switch ( code )
        {
        case EGestureStart:
            HandleGestureStartL( aEvent );
            break;
        case EGestureDrag:
            HandleDragEventL( aEvent );
            break;
        case EGestureReleased:
            HandleGestureReleasedL( aEvent );
            break;
        // gestures
        // these codes are sent when user lifts stylus (if holding not started)
        case EGestureTap:
            HandleSingleTapL( aEvent );
            break;
        case EGestureSwipeLeft:
            HandleSwipeLeftL( aEvent );
            break; 
        case EGestureSwipeRight:
            HandleSwipeRightL( aEvent );
            break;
        //gestures we don't handle
        case EGesturePinch:
        case EGestureSwipeDown: //swipe up/down initiate kinetic scrolling
        case EGestureSwipeUp:   //handling moved to PhysicsHandler
        
        case EGestureDoubleTap: //double tap is off
        case EGestureUnknown:   //unknown gesture
        case EGestureHoldLeft:  //hold swipes are off
        case EGestureHoldRight:
        case EGestureHoldUp:
        case EGestureHoldDown:
        case EGestureMultiTouchStart: //multitouch start/release for future use
        case EGestureMultiTouchReleased: //in case we use Pinch Zoom
        default:
            break;
        }
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------------------------
// CBCDayContainer::HandleGestureStartL
// Handle start of a gesture.
// ---------------------------------------------------------------------------
//
void CBCDayContainer::HandleGestureStartL( const GestureHelper::MGestureEvent& aEvent )
    {
    TRACE_ENTRY_POINT;
    //user started to interact with touch - hide the focus
    SetKeyboardActive( EFalse );
    SetSelectorStateL( ESelectorOff );
    
    if ( !iVSMode )
        {
        if ( IsInEventArea( aEvent.StartPos() ) )
            {
            SetHighlightFromTapL( aEvent.StartPos() );
            SetSelectorStateL( ESelectorNormal );
            InstantFeedback( ETouchFeedbackList );
            }
        TPointerEvent event( TPointerEvent::EButton1Down, 0, aEvent.StartPos(), aEvent.StartPos() );
        iPhysics->HandlePointerEventL( event );
        }
    else
        {
        if( aEvent.Visual() == iVSLowerArrowTouchArea )
            {
            //check if lower VS handle is already focused
            //if not - focus lower VS handle
            if ( !iVSFocusOnLowerArrow )
                {
                FocusVSArrowL( ETrue );
                }
            iStartPoint = aEvent.StartPos();
            iVSDragTime.HomeTime();
            }
        else if ( aEvent.Visual() == iVSUpperArrowTouchArea )
            {
            //check if upper VS handle is already focused
            //if not - focus upper VS handle
            if ( iVSFocusOnLowerArrow )
                {
                FocusVSArrowL( EFalse );
                }
            iStartPoint = aEvent.StartPos();
            iVSDragTime.HomeTime();
            }
        else
            {
            TPointerEvent event( TPointerEvent::EButton1Down, 0, aEvent.StartPos(), aEvent.StartPos() );
            iPhysics->HandlePointerEventL( event );
            }
         
        }
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------------------------
// CBCDayContainer::HandleDragEventL
// Hanlde drag event.
// ---------------------------------------------------------------------------
//
void CBCDayContainer::HandleDragEventL( const GestureHelper::MGestureEvent& aEvent )
    {
    TRACE_ENTRY_POINT;
    
    SetSelectorStateL( ESelectorOff );
    
    if ( iVSMode )
        {
        if( aEvent.Visual() == iVSLowerArrowTouchArea ||
            aEvent.Visual() == iVSUpperArrowTouchArea )
            {
            TPoint drag( aEvent.CurrentPos() - iStartPoint );
            if ( Abs( drag.iY ) > 0 )
                {
                iStartPoint = aEvent.CurrentPos();
                TTime now;
                now.HomeTime();
                TInt moveTime( now.MicroSecondsFrom( iVSDragTime ).Int64() / 1000 );
                iVSDragTime.HomeTime();
                if ( !ExpandVisualSelectionL( drag.iY, moveTime ) )
                    {//cannot move further; cancel gesture
                    iGestureHelper->Cancel();
                    }
                }
            }
        else
            {
            TPointerEvent event( TPointerEvent::EDrag, 0, aEvent.CurrentPos(), aEvent.CurrentPos() );
            iPhysics->HandlePointerEventL( event );
            }
        }
    else
        {
        TPointerEvent event( TPointerEvent::EDrag, 0, aEvent.CurrentPos(), aEvent.CurrentPos() );
        iPhysics->HandlePointerEventL( event );
        }
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------------------------
// CBCDayContainer::HandleGestureReleasedL
// Handle release of a gesture.
// ---------------------------------------------------------------------------
//
void CBCDayContainer::HandleGestureReleasedL( const GestureHelper::MGestureEvent& aEvent )
    {
    TRACE_ENTRY_POINT;
    
    SetSelectorStateL( ESelectorOff );
    
    if ( iVSMode )
        {
        if( aEvent.Visual() == iVSLowerArrowTouchArea ||
            aEvent.Visual() == iVSUpperArrowTouchArea )
            {
            AlignVisualSelectionToFullSlotL();
            }
        else
            {
            TPointerEvent event( TPointerEvent::EButton1Up, 0, aEvent.CurrentPos(), aEvent.CurrentPos() );
            iPhysics->HandlePointerEventL( event );
            }
        }
    else
        {
        TPointerEvent event( TPointerEvent::EButton1Up, 0, aEvent.CurrentPos(), aEvent.CurrentPos() );
        iPhysics->HandlePointerEventL( event );
        }
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------------------------
// CBCDayContainer::HandleSingleTapL
// Handle single tap event.
// ---------------------------------------------------------------------------
//
void CBCDayContainer::HandleSingleTapL( const GestureHelper::MGestureEvent& aEvent )
    {
    TRACE_ENTRY_POINT;
    HandleTapL( EFalse, aEvent.CurrentPos(), aEvent.Visual() );
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------------------------
// CBCDayContainer::HandleLongTapL
// Handles long tap event.
// ---------------------------------------------------------------------------
//
void CBCDayContainer::HandleLongTapL( const TPointerEvent& aPointerEvent )
    {
    TRACE_ENTRY_POINT;
    if( AknLayoutUtils::PenEnabled() && aPointerEvent.iType == TPointerEvent::EButtonRepeat )
        {
        CAlfVisual* pointedVisual(NULL);
        if ( IsVisualSelectionMode() )
            {
            TPoint pos = aPointerEvent.iParentPosition;
            TRect lowerHandleDownRect( iVSLowerArrowTouchArea->DisplayRectTarget() );
            TRect upperHandleUpRect( iVSUpperArrowTouchArea->DisplayRectTarget() );
            TRect vsRect( iVSVisual->DisplayRectTarget() );
            TRect vsTextRect( iVSText->DisplayRectTarget() );

            if ( lowerHandleDownRect.Contains( pos ) )
                {
                pointedVisual = iVSLowerArrowTouchArea;
                }
            else if ( upperHandleUpRect.Contains( pos ) )
                {
                pointedVisual = iVSUpperArrowTouchArea;
                }
            else if ( vsRect.Contains( pos ) )
                {
                pointedVisual = iVSVisual;
                }
            else if ( vsTextRect.Contains( pos ) )
                {
                pointedVisual = iVSText;
                }
            }
        HandleTapL( ETrue, aPointerEvent.iParentPosition, pointedVisual );
        }
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------------------------
// CBCDayContainer::HandleTapL
// Handle single/long tap event.
// ---------------------------------------------------------------------------
//
void CBCDayContainer::HandleTapL( TBool aLongTap, TPoint aCurrentPoint, CAlfVisual* aPointedVisual )
    {
    TRACE_ENTRY_POINT;
    TBool longTap( aLongTap );
    if ( IsVisualSelectionMode() )
        {
        //we're in Visual Selection mode
        //single and long tap are handled in the same way:
        if ( aPointedVisual == iVSVisual || aPointedVisual == iVSText )
            {
            DoSelectVisualSelectionL();
            }
        else if( aPointedVisual == iVSUpperArrowTouchArea ||
                 aPointedVisual == iVSLowerArrowTouchArea )
            {
            //skip the event
            }
        else
            {
            //tapped outside the VS area - release Visual Selection
            ReleaseVisualSelectionL();
            }
        }
    else
        {
        if ( IsInEventArea( aCurrentPoint ) )
            {
            if ( longTap )
                {//long tap works only on events
                if ( !DoLongSelectEventL( aCurrentPoint ) )
                    {
                    longTap = EFalse;
                    }
                }
            //long tap could be changed to single tap above
            if ( !longTap )
                {
                if ( !DoSelectEventL() )
                    {
                    SetSelectorStateL( ESelectorOff );
                    // No events focused, select timeslot
                    iView->HandleCommandL( ECalenCmdSelect );
                    }
                }
            }
        }
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------------------------
// CBCDayContainer::SetHighlightFromTapL
// Selects pointed slot.
// ---------------------------------------------------------------------------
//
void CBCDayContainer::SetHighlightFromTapL( const TPoint& aCurrentPoint )
    {
    TRACE_ENTRY_POINT;
    if ( !IsVisualSelectionMode() )
        {
        if ( !iSlotStorage )
            {
            TRACE_EXIT_POINT;
            return;
            }
        //set context from the tap event
        //first check if the tap is in correct area
        TBool handled(EFalse);
        TInt pointedSlotIdx = GetSlotIndex( aCurrentPoint );
        if ( pointedSlotIdx != KErrNotFound )
            {//cliked within event grid
            CAlfVisual& startVisual = GetSlotVisual( pointedSlotIdx );
            TInt eventVisualWidth( startVisual.Size().Target().AsSize().iWidth );
            
            TInt x(0);
            x = aCurrentPoint.iX;
            TBool mirrored(AknLayoutUtils::LayoutMirrored());
            if ( mirrored )
                {
                TRect screenRect;
                AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EScreen, screenRect );
                x = screenRect.Width() - x;
                }
            
            x -= iTimeGridWidth;
            //1.all-day events
            TInt allDayCount(0);
            allDayCount = iSlotStorage->AlldayCount();
            //check if all day event was selected
            if ( allDayCount > 0 )
                {
                TInt allDayAreaWidth(0);
                allDayAreaWidth = iAlldayEventWidth + iEventMargin;
                if ( x <= allDayAreaWidth )
                    {
                    //which all-day event was selected
                    TInt allDayWidth(0), allDayIdx(KErrNotFound);
                    allDayWidth = allDayAreaWidth / allDayCount;
                    //take separating margin(s) into all-day event(s) width(s) - 
                    //- more space to select with finger
                    allDayIdx = x / allDayWidth;
                    //safety check; last pixel
                    if ( allDayIdx < 0 )
                        {
                        allDayIdx = 0;
                        }
                    else if ( allDayIdx > allDayCount - 1 )
                        {
                        allDayIdx = allDayCount - 1;
                        }
    
                    const TCalenTimedEventInfo& allDay = iSlotStorage->AlldayEvent( allDayIdx );
                    if ( KErrNone == iSlotStorage->SelectEvent( allDay.iId ) )
                        {
                        handled = ETrue;
                        }
                    else
                        {
                        return;
                        }
                    }
                //all day event(s) present - move along the x
                eventVisualWidth -= allDayAreaWidth;
                x -= allDayAreaWidth;
                }
            //2. to-do(s), untimed events
            if ( !handled && iSlotStorage->IsExtraSlot( pointedSlotIdx ) )
                {
                iSlotStorage->SelectSlot( pointedSlotIdx );
                handled = ETrue;
                }
            //3. timed event(s), empty hours
            if ( !handled )
                {
                //TODO: k.g.: the 'for' below is fugly and stupid - rewrite it
            
                TCalenSlotInterval interval;
                interval.iStartSlot = pointedSlotIdx;
                interval.iEndSlot = pointedSlotIdx + 1;
                //check if pointed interval is overlaped by some region
                TInt regCount(0);
                regCount = iSlotStorage->RegionList().Count();
                for ( TInt regIdx=0; regIdx < regCount; regIdx++ )
                    {
                    const TCalenTimeRegion& region = iSlotStorage->RegionList()[regIdx];
                    if ( region.Overlaps( interval ) )
                        {
                        //interval is overlapped by a region
                        TInt colCount(0);
                        colCount = region.iColumns.Count();
                        //first check if we point to the empty area to the right
                        if ( x > eventVisualWidth - iSelectionSpaceOnRight - iEventMargin )
                            {
                            TInt tmpSlotIdx = iSlotStorage->SlotIndexFromHour( iSlotStorage->HourFromSlotIndex( pointedSlotIdx ));
                            iSlotStorage->SetSelectionInRegion( regIdx, colCount, tmpSlotIdx );
                            handled = ETrue;
                            break;
                            }
                        
                        eventVisualWidth = eventVisualWidth - (iSelectionSpaceOnRight + iEventMargin);
                        //which column is selected?
                        if ( !handled )
                            {
                            TInt eventWidth(0);
                            eventWidth = eventVisualWidth/colCount;
                            TInt colIdx(0);
                            colIdx = x/eventWidth;
                            //safety check
                            if ( colIdx >= colCount )
                                {
                                colIdx = colCount - 1;
                                }
                            if ( colIdx < 0 )
                                {
                                colIdx = 0;
                                }
                            
                            TInt eventCount(0);
                            eventCount = region.iColumns[colIdx].iEventArray.Count();
                            for ( TInt k = 0; k < eventCount; k++ )
                                {
                                if ( region.iColumns[colIdx].iEventArray[k].Overlaps(interval) )
                                    {//select an event and set slot index
                                    if (!iSlotStorage->SetSelectionInRegion( regIdx, colIdx, pointedSlotIdx ) )
                                        {
                                        iSlotStorage->SelectEvent( region.iColumns[colIdx].iEventArray[k].iId );                                        
                                        }
                                    handled = ETrue;
                                    break;
                                    }
                                }
                            //empty space in region - X,Y is not overlapped by event in column
                            if ( !handled )
                                {
                                //check if theres a full hour empty area that we can focus
                                //otherwise focus empty area outside the region
                                TCalenSlotInterval intrvl;
                                if ( !iSlotStorage->IsHourStartSlot(pointedSlotIdx) )
                                    {
                                    intrvl.iStartSlot = iSlotStorage->SlotIndexFromHour( iSlotStorage->HourFromSlotIndex( pointedSlotIdx ) );
                                    }
                                else
                                    {
                                    intrvl.iStartSlot = pointedSlotIdx;
                                    }
                                intrvl.iEndSlot = intrvl.iStartSlot + KFSCalSlotsInHour;
                                
                                eventCount = region.iColumns[colIdx].iEventArray.Count();
                                for ( TInt k = 0; k < eventCount; k++ )
                                    {
                                    if ( region.iColumns[colIdx].iEventArray[k].Overlaps(intrvl) )
                                        {//full hour overlaps with an event -
                                        //focus empty place right to the region
                                        iSlotStorage->SetSelectionInRegion( regIdx, colCount, intrvl.iStartSlot );
                                        handled = ETrue;
                                        break;
                                        }
                                    }
                                if (!handled)
                                    {//there's a full-hour empty area in the region that can be focued 
                                    iSlotStorage->SetSelectionInRegion( regIdx, colIdx, intrvl.iStartSlot );
                                    handled = ETrue;
                                    break;
                                    }
                                }
                            }
                        }
                    }
                //event at (X,Y) not found in any region - empty hour 
                if ( !handled )
                    {
                    iSlotStorage->SelectSlot( iSlotStorage->SlotIndexFromHour( iSlotStorage->HourFromSlotIndex( pointedSlotIdx ) ));
                    handled = ETrue;
                    }
                }
            }
        }
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------------------------
// CBCDayContainer::HandleSwipeLeftL
// Handle swipe left gesture.
// ---------------------------------------------------------------------------
//
void CBCDayContainer::HandleSwipeLeftL( const GestureHelper::MGestureEvent& /*aEvent*/ )
    {
    TRACE_ENTRY_POINT;
    TBool mirrored(AknLayoutUtils::LayoutMirrored());
    if ( iVSMode )
        {
        ReleaseVisualSelectionL();
        }    
    ChangeDateL(TTimeIntervalDays(mirrored ? -1 : 1));
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------------------------
// CBCDayContainer::HandleSwipeRightL
// Handle swipe right gesture.
// ---------------------------------------------------------------------------
//
void CBCDayContainer::HandleSwipeRightL( const GestureHelper::MGestureEvent& /*aEvent*/ )
    {
    TRACE_ENTRY_POINT;
    TBool mirrored(AknLayoutUtils::LayoutMirrored());
    if ( iVSMode )
        {
        ReleaseVisualSelectionL();
        }
    ChangeDateL(TTimeIntervalDays(mirrored ? 1 : -1));
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------------------------
// CBCDayContainer::ConstructL
// Symbian OS default constructor
// ---------------------------------------------------------------------------
//
void CBCDayContainer::ConstructImplL()
    {
    TRACE_ENTRY_POINT;
    FSCALENDAR_PROFILE_START( FsCalendarProfile::EFsCalDayViewConstructL )
    
    // Disabling Alf Env due to performance imporvement,
    // Enabling Alf Env again at the end of CFSCalenDayContainer::DoStepL()
    iAlfEnv.PauseRefresh();
    iAlfEnv.SetRefreshMode( EAlfRefreshModeManual );
    iPreviousGridIndex = 0;
    
    CAlfControl::ConstructL( iAlfEnv );
    iViewOpening = ETrue;

    iDate = iServices.Context().FocusDateAndTimeL().TimeLocalL();

    iSlotStorage = CBCSlotInfoStorage::NewL(
            CBCSlotInfoStorage::TSlotsInHour( KFSCalSlotsInHour ) );

    FetchLayoutValues();

    // Create layouts and visuals

    // Parent includes all other layouts and visuals
    iParentLayout = CAlfDeckLayout::AddNewL(*this);
    iParentLayout->SetFlag( EAlfVisualFlagAutomaticLocaleMirroringEnabled );
    iParentLayout->SetFlag ( EAlfVisualFlagFreezeLayout );

    // Base layout for positioning the static elements in the view
    iBaseLayout = CAlfAnchorLayout::AddNewL( *this, iParentLayout );
    iBaseLayout->SetFlag(EAlfVisualFlagAutomaticLocaleMirroringEnabled);
    iBaseLayout->SetFlag( EAlfVisualFlagFreezeLayout );

    // Scrollable grid:
    ConstructScrollableGridL();

    //Position the layouts
    PositionLayouts();
    
    //Construct Selector visual
    ConstructSelectorL();

    // Create preview popup controller.
    iPreviewPopup = CBCPreviewPopup::NewL( this, iScrollableLayout, 
            iScrollableLayout, iResManager, this);
    
    CAlfDisplay& display = iAlfEnv.Display(0);
    CCoeControl* ccoeControl = dynamic_cast<CCoeControl*>(display.ObjectProvider());
    iPhysics = CBCDayPhysicsHandler::NewL(*ccoeControl, *this);
    
    //touch
    CAlfRoster& roster = display.Roster();
    
    roster.AddPointerEventObserver( EAlfPointerEventReportDrag, *this );
    roster.AddPointerEventObserver( EAlfPointerEventReportUnhandled, *this );
    roster.AddPointerEventObserver( EAlfPointerEventReportLongTap, *this );
    //we don't want to receive long tap events when dragging is ongoing
    roster.DisableLongTapEventsWhenDragging( *this, ETrue );
    
    TInt dragThreshold = iPhysics->DragThreshold();
    //needed to prevent accidentally drag evnets (e.g. durring long tap)
	roster.SetPointerDragThreshold( *this,
			TAlfXYMetric( TAlfMetric(dragThreshold), TAlfMetric(dragThreshold) ));
	
    //long tap/stylus pop-up
    iItemActionMenu = CAknItemActionMenu::RegisterCollectionL( *this );
    
    iGestureHelper = CGestureHelper::NewL( *this );
    iGestureHelper->SetHoldingEnabled( EFalse );
    iGestureHelper->SetDoubleTapEnabled( EFalse );

    //pointer to the feedback interface
    iFeedback = MTouchFeedback::Instance();
    
    FSCALENDAR_PROFILE_END( FsCalendarProfile::EFsCalDayViewConstructL )
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------------------------
// CBCDayContainer::~CBCDayContainer
// Destructor
// ---------------------------------------------------------------------------
//
CBCDayContainer::~CBCDayContainer()
    {
    TRACE_ENTRY_POINT;
    
    iInstanceList.ResetAndDestroy();
    delete iLayoutTable;

    delete iSlotStorage; iSlotStorage = NULL;
    RemoveTimedEvents();
    iApptVisualList.Close();
    ClosePreviewWindow( EFalse );
    delete iPreviewPopup;
    
    if(iPhysics)
    	{
    	delete iPhysics;
    	iPhysics=NULL;
    	}
    
    //stylus pop-up; remove current collection
    if ( iItemActionMenu )
        {
        iItemActionMenu->RemoveCollection( *this );
        }
    //delete gesture handler
    delete iGestureHelper;

    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------------------------
// CBCDayContainer::UpdateContainer
// Refresh Day view data and draws.
// Data is based on active context.
// ---------------------------------------------------------------------------
//
void CBCDayContainer::UpdateContainer()
    {
    TRACE_ENTRY_POINT;

    FreezeLayout( ETrue );
    
    iAlfEnv.PauseRefresh();
    iAlfEnv.SetRefreshMode( EAlfRefreshModeManual );
    
    Refresh();

    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------------------------
// CBCDayContainer::UpdateControlBarL
// ---------------------------------------------------------------------------
//
void CBCDayContainer::UpdateControlBarL()
    {
    TRACE_ENTRY_POINT;
    FSCALENDAR_PROFILE_START( FsCalendarProfile::EFsCalDayViewUpdateControlBarL )
    
    // construct the control bar when lunar data used.
    if ( iServices.Infobar() != KNullDesC )
        {
        ConstructControlBarL();
        CAlfTextVisual& controlPaneText = static_cast<CAlfTextVisual&>( iControlBar->Visual(0) );
        controlPaneText.SetTextL( iServices.Infobar() );
        controlPaneText.UpdateChildrenLayout();
		// fix for ou1cimx1#549997
		// this is quick workaround
    	TRect nullRect (0, 0, 0, 0);
    	iLabelControl = iServices.Infobar( nullRect );
        }
    else
        {
    	iLabelControl = NULL;
        RemoveControlBar();
        }
    
    FetchLayoutValues();
    PositionLayouts();
    FSCALENDAR_PROFILE_END( FsCalendarProfile::EFsCalDayViewUpdateControlBarL )
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------------------------
// CBCDayContainer::SetActiveContextFromHighlightL
// Set Active date and time
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CBCDayContainer::SetActiveContextFromHighlightL()
    {
    TRACE_ENTRY_POINT;

    MCalenContext& context = iServices.Context();
    

    // Find out if some entry is focused:
    if ( iSlotStorage )
        {
        if ( iSlotStorage->IsEventSelected() )
            {
            if ( iSlotStorage->IsMultipleEventsSelected() )
                {
                // focus on the to-do row with multiple events
                TInt columnIndex; //dummy variable required by GetSelectedSlot
                TInt columns;     //dummy variable required by GetSelectedSlot
                TInt region;
                iSlotStorage->GetSelectedSlot( iSelectedSlot, region, columnIndex, columns );
                }
            else
                {
				//single event
				TCalenInstanceId eventId = iSlotStorage->SelectedEvent();
				TCalTime calTime;
				if  ( CalenAgendaUtils::IsTimedEntryL( eventId.iType ) )
					{
					calTime.SetTimeLocalL( eventId.iInstanceTime );
					}
				else
					{
					TInt selectedSlot, columnIndex, columns, region;
					iSlotStorage->GetSelectedSlot( selectedSlot, region, columnIndex, columns );
					
					do
						{
						++selectedSlot;
						}
					while ( iSlotStorage->IsExtraSlot( selectedSlot ) );
					
					TInt selectedHour = iSlotStorage->HourFromSlotIndex( selectedSlot );
					TDateTime dateTime = iDate.DateTime();
		            dateTime.SetHour( selectedHour );
		            dateTime.SetMinute( 0 );
					TTime time ( dateTime );
					
					calTime.SetTimeLocalL( time );
					}
				context.SetFocusDateAndTimeAndInstanceL( calTime, eventId, iView->ViewId() );
                }
            }
        else
            {
            TInt selectedSlot;
            TInt columnIndex, columns, region;
            iSlotStorage->GetSelectedSlot( selectedSlot, region, columnIndex, columns );

            TInt selectedHour = iSlotStorage->HourFromSlotIndex( selectedSlot );
            TDateTime dateTime = iDate.DateTime();
            dateTime.SetHour( selectedHour );
            dateTime.SetMinute( 0 );
            TTime focusTime( dateTime );
            TCalTime today;
            today.SetTimeLocalL( focusTime );
            context.SetFocusDateAndTimeL( today, iView->ViewId() );
            }
        }
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CBCDayContainer::IsEmptyView
// -----------------------------------------------------------------------------
//
TBool CBCDayContainer::IsEmptyView() const
    {
    TRACE_ENTRY_POINT;

    TInt eventCount(iExtraSlots);
    if ( eventCount <= 0 )
        {
        // No extra slots, check the timed events
        if ( iLayoutTable )
            {
            eventCount = iLayoutTable->Count();
            }
        }

    TRACE_EXIT_POINT;

    return eventCount <= 0;
    }

// -----------------------------------------------------------------------------
// CBCDayContainer::IsSingleEventSelected
// -----------------------------------------------------------------------------
//
TBool CBCDayContainer::IsSingleEventSelected()
    {
    TRACE_ENTRY_POINT;

    TBool eventSelected( EFalse );
    if ( iSlotStorage )
        {
        eventSelected = iSlotStorage->IsEventSelected() &&
            !iSlotStorage->IsMultipleEventsSelected();
        }
    TRACE_EXIT_POINT;

    return eventSelected;
    }

// -----------------------------------------------------------------------------
// CBCDayContainer::IsMultipleEventsSelected
// -----------------------------------------------------------------------------
//
TBool CBCDayContainer::IsMultipleEventsSelected()
    {
    TRACE_ENTRY_POINT;

    TBool eventsSelected( EFalse );
    if ( iSlotStorage )
        {
        eventsSelected = iSlotStorage->IsMultipleEventsSelected();
        }
    TRACE_EXIT_POINT;

    return eventsSelected;
    }

// -----------------------------------------------------------------------------
// CBCDayContainer::IsEventSelected
// -----------------------------------------------------------------------------
//
TBool CBCDayContainer::IsEventSelected()
    {
    TRACE_ENTRY_POINT;
    TBool eventsSelected( EFalse );
    if ( iSlotStorage )
        {
        eventsSelected = iSlotStorage->IsEventSelected();
        }
    
    TRACE_EXIT_POINT;
    return eventsSelected;
    }
// ---------------------------------------------------------------------------
// CBCDayContainer::UpdateTimeFormat
// Update hour text after locale change.
// ---------------------------------------------------------------------------
//
void CBCDayContainer::UpdateTimeFormatL()
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    
    if ( iTimeGridLayout )
        {
        TInt rowCount = iTimeGridLayout->RowCount();

        // Get time format
        HBufC* timeFormat = NULL;
        TLocale local;
        local.Refresh(); 
        if (local.TimeFormat() == ETime24)
            {
            timeFormat = StringLoader::LoadLC(R_VIEWPLUGIN__DAYVIEW_TIMEFORMAT24_1);
            }
        else
            {
            timeFormat = StringLoader::LoadLC(R_VIEWPLUGIN_DAYVIEW_TIMEFORMAT);
            }

        // Update the time text for each row
        TTimeIntervalHours hour(0);
        for ( TInt row(0); row < rowCount; row++ )
            {
            TBool emptyText(EFalse);
            if ( ( row < iExtraSlotGridPos ) || ( iExtraSlots < 1 ) )
                {
                hour = row;
                }
            else if ( row == iExtraSlotGridPos )
                {
                // no time text for extra rows
                emptyText = ETrue;
                }
            else
                {
                // rows after the extra row
                hour = row - 1;
                }

            if ( !emptyText )
                {
                TTime time(0);
                TBuf<KDayWeekViewTimeTextLength> hourDes;
                time += hour;
                time.FormatL(hourDes, *timeFormat);

                CAlfLayout& rowLayout = static_cast<CAlfLayout&>(iTimeGridLayout->Visual(row));
                CAlfTextVisual& hourText = static_cast<CAlfTextVisual&>( rowLayout.Visual(0) );

                AknTextUtils::LanguageSpecificNumberConversion( hourDes );
                hourText.SetTextL(hourDes);
                //update text's style
                iResManager.TextStyleManager().SetVisualStyleL( EDayViewTimeGridText, &hourText );
                }
            }

        CleanupStack::PopAndDestroy(timeFormat);
//
//        // For Czech language, also update the layout positions
//        if( User::Language() == ELangCzech )
//            {
//            FetchLayoutValues();
//
//            // Position elements inside the scrollable grid:
//            // Time grid
//            iGridParentLayout->SetAnchor(EAlfAnchorTopLeft, 0,
//                EAlfAnchorOriginLeft, EAlfAnchorOriginTop,
//                EAlfAnchorMetricRelativeToSize, EAlfAnchorMetricRelativeToSize,
//                TAlfTimedPoint(0.0, 0.0));
//            iGridParentLayout->SetAnchor(EAlfAnchorBottomRight, 0,
//                EAlfAnchorOriginLeft, EAlfAnchorOriginTop,
//                EAlfAnchorMetricAbsolute, EAlfAnchorMetricRelativeToSize,
//                TAlfTimedPoint(iTimeGridWidth, 1.0));
//
//            // Day grid
//            iGridParentLayout->SetAnchor(EAlfAnchorTopLeft, 1,
//                EAlfAnchorOriginLeft, EAlfAnchorOriginTop,
//                EAlfAnchorMetricAbsolute, EAlfAnchorMetricRelativeToSize,
//                TAlfTimedPoint(iTimeGridWidth, 0.0));
//            iGridParentLayout->SetAnchor(EAlfAnchorBottomRight, 1,
//                EAlfAnchorOriginLeft, EAlfAnchorOriginTop,
//                EAlfAnchorMetricRelativeToSize, EAlfAnchorMetricRelativeToSize,
//                TAlfTimedPoint(1.0, 1.0));
//
//            PositionLayouts();
//            UpdateSelectorL();
//            }
        }
    }


// ---------------------------------------------------------------------------
// CBCDayContainer::OfferEventL
// From CAlfControl, handle key events.
// ---------------------------------------------------------------------------
//
TBool CBCDayContainer::OfferEventL(const TAlfEvent& aEvent)
    {
    TRACE_ENTRY_POINT;
    TBool consumed = EFalse;
    // First give the event to the base class (view cycling check)
    consumed = CBCContainerBase::OfferEventL(aEvent);    
    
    if (!consumed)
        {
        if (aEvent.IsKeyEvent() && aEvent.Code() == EEventKey)
            {
            TBool mirrored(AknLayoutUtils::LayoutMirrored());

            if ( EnableSelectorWithKeyEventL( aEvent ) )
                {
                consumed = ETrue;
                }
            
            if ( !consumed && IsKeyboardActive() )
                {
                switch (aEvent.KeyEvent().iCode)
                    {
                    case EKeyBackspace:
                        {
                        if ( IsSingleEventSelected() )
                            {
                            // if event selected, delete the event
                            iView->HandleCommandL( ECalenDeleteCurrentEntry );
                            }
                        consumed = ETrue;
                        }
                        break;
                    case EKeyEnter:
                    case EKeyDevice3:
                        {
                        if ( IsVisualSelectionMode() )
                            {
                            DoSelectVisualSelectionL();
                            }
                        else
                            {
                            if ( !DoSelectEventL() )
                                {
                                // No events focused, select timeslot
                                iView->HandleCommandL( ECalenCmdSelect );
                                }
                            }
                        consumed = ETrue;
                        }
                        break;
    
                    case EKeyUpArrow:
                        {
                        if (iVSMode)
                            {
                            HandleKeyVisualSelectionL( EKeyUpArrow );
                            }
                        else
                            {
                            TBool moveFocus = ETrue;
                            if ( iSlotStorage->IsEventSelected() )
                                {
                                TCalenSlotInterval selection = iSlotStorage->SelectedInterval();
                                if ( selection.iStartSlot < iTopVisibleSlot )
                                    {
                                    ScrollViewL( EScrollUp, KFSCalSlotsInHour );
                                    iSlotStorage->MoveSelectionInEvent( EScrollUp );
                                    moveFocus = EFalse;
    
                                    TInt selectedSlot;
                                    TInt columnIndex, columns, region;
                                    iSlotStorage->GetSelectedSlot( selectedSlot,
                                                                   region,
                                                                   columnIndex,
                                                                   columns );
                                    CAlfVisual& selectedSlotVisual = GetSlotVisual( selectedSlot );
    
                                    if ( iPreviewPopup )
                                        {
                                        iPreviewPopup->UpdateFocusedSlotL(
                                                &selectedSlotVisual );
                                        }
                                    }
                                }
    
                            if ( moveFocus )
                                {
                                TBool update = ETrue;
                                if ( !iSlotStorage->MoveSelection( EScrollUp ) )
                                    {
                                    // cannot move up -> select the last slot
                                    if ( aEvent.KeyEvent().iRepeats == 0 )
                                        {
                                        // no repeat/first event -> do the wraparound
                                        iSlotStorage->SelectSlot(
                                            KFSCalHourSlotCount + iExtraSlots - 1 );
                                        }
                                    else
                                        {
                                        // keyrepeat -> don't do any wraparound at all
                                        update = EFalse;
                                        }
                                    }
    
                                if ( update )
                                    {
                                    UpdateSelectorL();
                                    }
                                }
                            }
                        consumed=ETrue;
                        }
                        break;
    
                    case EKeyDownArrow:
                        {
                        if (iVSMode)
                            {
                            HandleKeyVisualSelectionL( EKeyDownArrow );
                            }
                        else
                            {
                            TBool moveFocus = ETrue;
                            if ( iSlotStorage->IsEventSelected() )
                                {
                                TCalenSlotInterval selection = iSlotStorage->SelectedInterval();
                                TInt lastVisibleSlot( iTopVisibleSlot + iVisibleSlots - 1 );
                                if ( selection.iEndSlot - 1 > lastVisibleSlot )
                                    {
                                    ScrollViewL( EScrollDown, KFSCalSlotsInHour );
                                    iSlotStorage->MoveSelectionInEvent( EScrollDown );
                                    moveFocus = EFalse;
    
                                    TInt selectedSlot;
                                    TInt columnIndex, columns, region;
                                    iSlotStorage->GetSelectedSlot( selectedSlot,
                                                                   region,
                                                                   columnIndex,
                                                                   columns );
                                    CAlfVisual& selectedSlotVisual = GetSlotVisual( selectedSlot );
    
                                    if ( iPreviewPopup )
                                        {
                                        iPreviewPopup->UpdateFocusedSlotL(
                                                &selectedSlotVisual );
                                        }
                                    }
                                }
    
                            if ( moveFocus )
                                {
                                TBool update = ETrue;
                                if ( !iSlotStorage->MoveSelection( EScrollDown ) )
                                    {
                                    // cannot move down -> select the first slot
                                    if ( aEvent.KeyEvent().iRepeats == 0 )
                                        {
                                        // no repeat/first event -> do the wraparound
                                        iSlotStorage->SelectSlot( 0 );
                                        }
                                    else
                                        {
                                        // keyrepeat -> don't do any wraparound at all
                                        update = EFalse;
                                        }
                                    }
    
                                if ( update )
                                    {
                                    UpdateSelectorL();
                                    }
                                }
                            }
                        consumed=ETrue;
                        }
                        break;
    
                    case EKeyLeftArrow:
                        {
                        if (iVSMode)
                            {
                            HandleKeyVisualSelectionL( EKeyLeftArrow );
                            }
                        else
                            {
                            if ( !iSlotStorage->MoveSelection(
                                 mirrored ? EScrollRight : EScrollLeft ) )
                                {                            
                                ChangeDateL(TTimeIntervalDays(mirrored ? 1 : -1));
                                }
                            else
                                {
                                UpdateSelectorL();
                                }
                            }
                        consumed = ETrue;
                        }
                    break;
                    case EKeyRightArrow:
                        {
                        if (iVSMode)
                            {
                            HandleKeyVisualSelectionL( EKeyRightArrow );
                            }
                        else
                            {
                            if ( !iSlotStorage->MoveSelection(
                                 mirrored ? EScrollLeft : EScrollRight ) )
                                {
                                ChangeDateL(TTimeIntervalDays(mirrored ? -1 : 1));
                                }
                            else
                                {
                                UpdateSelectorL();
                                }
                            }
                        consumed = ETrue;
                        }
                    break;
                    default:
                        break;
                    }
                }
            }
        else if ( aEvent.IsPointerEvent() )
        	{
        	consumed = HandlePointerEventL( aEvent );
        	}
        }
    TRACE_EXIT_POINT;
    return consumed;
    }

// ---------------------------------------------------------------------------
// CBCDayContainer::HandlePointerEventL
// Handles pointer events.
// ---------------------------------------------------------------------------
//
TBool CBCDayContainer::HandlePointerEventL( const TAlfEvent& aPointerEvent )
	{
	TRACE_ENTRY_POINT;
    TBool consumed(EFalse);
    
    if(!AknLayoutUtils::PenEnabled())
        {//touch is not enabled
        return consumed;
        }
    
    TPointerEvent::TType type = aPointerEvent.PointerEvent().iType;
	// fix for ou1cimx1#549997
	// this is fast workaround
    if( iControlBar && aPointerEvent.Visual() == &iControlBar->Visual(0) )
        {
    	if (iLabelControl)
    		{
            iLabelControl->HandlePointerEventL(aPointerEvent.PointerEvent());
    		}
        consumed = ETrue;
        }
    else if ( type == TPointerEvent::EButtonRepeat )
        {
        //Long tap event
        iGestureHelper->Cancel();
        HandleLongTapL( aPointerEvent.PointerEvent() );
        consumed = ETrue;
        }
    else
        {
        consumed = iGestureHelper->OfferEventL( aPointerEvent );
        }
    TRACE_EXIT_POINT;
    return consumed;
    }

// ---------------------------------------------------------------------------
// CBCDayContainer::GetSlotIndex
// Get index of a slot pointed by aPosition
// ---------------------------------------------------------------------------
//
TInt CBCDayContainer::GetSlotIndex( TPoint aPosition )
    {
    TRACE_ENTRY_POINT;
    TInt slotIdx(KErrNotFound);
    //check whether aPosition is within the grid layout
    if ( IsInEventArea( aPosition ) )
        {
        //get pointer position in event local grid coordinates
        TPoint localPoint = iGridLayout->DisplayToLocal( aPosition );
        //calculate slot number
        TInt pointedSlot = localPoint.iY / iSlotHeight;
        slotIdx = pointedSlot;
        }
    TRACE_EXIT_POINT;
    return slotIdx;
    }

// ---------------------------------------------------------------------------
// CBCDayContainer::IsInEventArea
// Returns whether a point is within event area.
// ---------------------------------------------------------------------------
//
TBool CBCDayContainer::IsInEventArea( const TPoint& aPoint )
    {
    TRACE_ENTRY_POINT;
    TBool retVal( EFalse );
    TRect eventAreaRect( iGridLayout->DisplayRectTarget( ) );
    if ( eventAreaRect.Contains( aPoint ))
        {
        retVal = ETrue;
        }
    TRACE_EXIT_POINT;
    return retVal;
    }

// ---------------------------------------------------------------------------
// CBCDayContainer::SetSelectorStateL
// Enables or disables the focus (highlight)
// ---------------------------------------------------------------------------
//
void CBCDayContainer::SetSelectorStateL( TSelectorState aState )
    {
    TRACE_ENTRY_POINT;
    if ( iSelectorState != aState )
        {//change in selector's state
        if ( iKeyboardActive )
            {//keyboard is active - selector always visible
             //selector can be in normal or pressed state
            if ( aState == ESelectorNormal ||
                 aState == ESelectorPressed )
                {
                iSelectorState = aState;
                UpdateSelectorL( 0, EFalse, ETrue );
                SetSelectorBrushL( );
                }
            }
        else
            {//touch active - selector can be off, normal or pressed
            iSelectorState = aState;
            UpdateSelectorL( 0, EFalse, ETrue );
            SetSelectorBrushL( );
            }
        //show/hide VS handle focus
        if ( IsVisualSelectionMode() )
            {
            FocusVSArrowL( iVSFocusOnLowerArrow );
            }
        }
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------------------------
// CBCDayContainer::IsSelectorEnabled()
// The function checkes whether the selector is visible.
// ---------------------------------------------------------------------------
//
TBool CBCDayContainer::IsSelectorEnabled() const
    {
    TRACE_ENTRY_POINT;
    TBool enabled(ETrue);
    if ( iSelectorState == ESelectorOff )
        {
        enabled = EFalse;
        }
    TRACE_EXIT_POINT;
    return enabled;
    }

// ---------------------------------------------------------------------------
// CBCDayContainer::SelectorState()
// Returns selector's state.
// ---------------------------------------------------------------------------
//
TSelectorState CBCDayContainer::SelectorState()
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    return iSelectorState;
    }

// ---------------------------------------------------------------------------
// CBCDayContainer::EnableSelectorWithKeyEventL()
// Enables focus with key event.
// ---------------------------------------------------------------------------
//
TBool CBCDayContainer::EnableSelectorWithKeyEventL( const TAlfEvent& aEvent )
    {
    TRACE_ENTRY_POINT;
    TBool focusEnabled(EFalse);
    
    if ( aEvent.IsKeyEvent() )
        {
        if ( !IsKeyboardActive() )
            {
            //focus is hidden:
            //check if supported key was pressed and turn the focus on
            TUint code(aEvent.KeyEvent().iCode);
            TInt scanCode(aEvent.KeyEvent().iScanCode);
            
            if ( code == EKeyUpArrow ||
                 code == EKeyDownArrow ||
                 code == EKeyLeftArrow ||
                 code == EKeyRightArrow ||
                 scanCode == EStdKeyDevice3 )
                {
                if ( !iVSMode )
                    {
                    TInt slot(KErrNotFound);
                    TInt column(KErrNotFound);
                    TInt columns(KErrNotFound);
                    TInt region(KErrNotFound);
                    iSlotStorage->GetSelectedSlot( slot,region, column, columns );
                    if ( slot < iTopVisibleSlot || slot > iTopVisibleSlot + iVisibleSlots - 1 )
                        {
                        slot = iTopVisibleSlot;
                        CAlfVisual& vis = GetSlotVisual( slot );
                        //is slot fully visible?
                        if ( vis.Pos().Target().iY <= iVerScrollPos )
                            {
                            slot++;
                            }
                        else if ( vis.Pos().Target().iY + 2*iSlotHeight >= iVerScrollPos + iScrollableLayout->Size().Target().iY )//iViewSize.iHeight )
                            {
                            slot--;
                            }
                        iSlotStorage->SelectSlot( slot );
                        }
                    }
                
                SetKeyboardActive( ETrue );
                SetSelectorStateL( ESelectorNormal );
                focusEnabled = ETrue;
                }
            }
        }
    TRACE_EXIT_POINT;
    return focusEnabled;
    }

// ---------------------------------------------------------------------------
// CBCDayContainer::HandleViewCyclingL
// ---------------------------------------------------------------------------
//
void CBCDayContainer::HandleViewCyclingL()
    {
    TRACE_ENTRY_POINT;

    SetActiveContextFromHighlightL();
    iView->HandleCommandL( ECalenNextView );

    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CBCDayContainer::RefreshLayoutL
// This is called when display size is changed.
// The content of the view is not changed.
// -----------------------------------------------------------------------------
//
void CBCDayContainer::RefreshLayoutL()
    {
    TRACE_ENTRY_POINT;
    FreezeLayout( ETrue );
    
    iAlfEnv.PauseRefresh();
    iAlfEnv.SetRefreshMode( EAlfRefreshModeManual );

    // Update iParentLayout size, this doesn't happen automatically
    // when application is in background and display size changes.
    // Changing from landscape to portrate mode e.g.
    iParentLayout->SetSize( DisplayArea().Size(), 0 );
    
    TInt columnIndex; //dummy variable required by GetSelectedSlot
    TInt columns;     //dummy variable required by GetSelectedSlot
    TInt region;
    iSlotStorage->GetSelectedSlot( iSelectedSlot, region, columnIndex, columns );
    FetchLayoutValues();
    UpdateControlBarL();
    PositionLayouts();
    UpdateViewportValues();

    // Layout must be valid before UpdateEventVisuals
    FreezeLayout( EFalse );
    iParentLayout->UpdateChildrenLayout();
    FreezeLayout( ETrue );
    
    UpdateTimeFormatL();
    UpdateTimedEventsL();    
    UpdateUntimedEventsL();

    // Selector update requires up-to-date layout
    FreezeLayout( EFalse );
    iParentLayout->UpdateChildrenLayout();
    FreezeLayout( ETrue );
    
    UpdateSelectorL( KLayoutTime, ETrue, ETrue );
    UpdateVisualSelectionL();
    UpdateUnvisibleEventArrowsL( ETrue );
    
    if ( iPhysics )
        {
        iPhysics->InitPhysicsL(iWorldSize, iViewSize);
        }

    iAlfEnv.SetRefreshMode( EAlfRefreshModeAutomatic );
    iAlfEnv.ContinueRefresh();
    TRACE_EXIT_POINT;
    }


// -----------------------------------------------------------------------------
// From CBCContainerBase
// CBCDayContainer::NotifyChangeDateL
// Date change operation notification handler.
// -----------------------------------------------------------------------------
//
void CBCDayContainer::NotifyChangeDateL()
    {
    TRACE_ENTRY_POINT;
    
    MCalenContext& context = iServices.Context();
    TTime newDay = context.FocusDateAndTimeL().TimeLocalL();
    ConvertToDate( newDay );
    
    if ( ! CalenDateUtils::OnSameDay( iDate, newDay ) )
        {
        UpdateContainer();
        }

    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// From CBCContainerBase
// CBCDayContainer::OnEditorClosedL
// Handles "Editor closed" (canceled) notification.
// -----------------------------------------------------------------------------
//
void CBCDayContainer::OnEditorClosedL()
    {
    TRACE_ENTRY_POINT;
    if( IsVisualSelectionMode() )
        {
        ReleaseVisualSelectionL( );
        }
    //set default selector's state
    TSelectorState selectorState( ESelectorOff );
    if ( IsKeyboardActive() )
        {
        selectorState = ESelectorNormal;
        }
    SetSelectorStateL( selectorState );
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------
// CBCDayContainer::MakeScrollDownRequestL
// Checks how many slots are needed to be scrolled in order
// to make room for aPixels. Scrolls down if possible and
// returns ETrue. If it is not possible, returns EFalse.
// ---------------------------------------------------------
//
TBool CBCDayContainer::MakeScrollDownRequestL( const TInt aPixels )
    {
    TRACE_ENTRY_POINT;
    TBool fullScrollingPossible( EFalse );

    // Calculate the height of one time slot
    TSize gridSize = iScrollableLayout->Size().ValueNow().AsSize();
    TInt slotHeight = gridSize.iHeight / iVisibleSlots;

    TInt scrollSlotCount = aPixels / slotHeight;
    if ( scrollSlotCount * slotHeight < aPixels )
        {
        scrollSlotCount++;
        }

    // check if full scrolling is possible
    if ( iTopVisibleSlot + scrollSlotCount + iVisibleSlots
         <= KFSCalHourSlotCount + iExtraSlots )
        {
        fullScrollingPossible = ETrue;
        }

    // scroll the view, even if cannot scroll the whole pixel amount
    ScrollViewL( EScrollDown, scrollSlotCount );

    // what if aPixels == 0, should return EFalse?

    TRACE_EXIT_POINT;
    return fullScrollingPossible;
    }

// -----------------------------------------------------------------------------
// CBCDayContainer::VisualLayoutUpdated
// Update visuals's layout.
// -----------------------------------------------------------------------------
//
void CBCDayContainer::VisualLayoutUpdated( CAlfVisual& /*aVisual*/ )
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------------------------
// CBCDayContainer::FetchLayoutValues
// Fetches the layout values from the layout handler. Called when starting
// the view and when the screen resolution is changed.
// ---------------------------------------------------------------------------
//
void CBCDayContainer::FetchLayoutValues()
    {
    TRACE_ENTRY_POINT;
    // Get layout data
    CBCLayoutHandler& layoutHandler = iResManager.LayoutHandler();
    iControlBarHeight = layoutHandler.ControlBarHeight();
    iTimeGridWidth = layoutHandler.DayViewTimeGridWidth();
    iAlldayEventWidth = layoutHandler.DayViewAlldayEventWidth();
    iSelectionSpaceOnRight = layoutHandler.DayViewSmallSelectionWidth();
    iEventMargin = layoutHandler.DayViewEventMargin();
    iArrowSize = layoutHandler.HiddenEventsArrowSize();
    iSlotHeight = layoutHandler.DayViewSlotHeight();
    iVSAreaWidth = layoutHandler.VisualSelectionAreaWidth();
    iVSUpArrowsSize = layoutHandler.VisualSelectionUpArrowSize();
    iVSUpArrowXPosition = layoutHandler.VisualSelectionUpArrowXPosition();
    iVSDownArrowSize = layoutHandler.VisualSelectionDownArrowSize();
    iVSDownArrowXPosition = layoutHandler.VisualSelectionDownArrowXPosition();
    iEventColorLineWidth = layoutHandler.DayViewEventColorLineWidth();

    // Calculate the amount of visible slots based on the slot height
    TInt viewportHeight = DisplayArea().Height();
    if ( iControlBar )
        {
        viewportHeight -= iControlBarHeight;
        }

    iVisibleSlots = viewportHeight / iSlotHeight;
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------------------------
// CBCDayContainer::ConstructControlBarL
// ---------------------------------------------------------------------------
//
void CBCDayContainer::ConstructControlBarL()
    {
    TRACE_ENTRY_POINT;
    if ( !iControlBar )
        {
        iControlBar = CAlfGridLayout::AddNewL( *this, 1, 1, iBaseLayout );
        iControlBar->SetFlag( EAlfVisualFlagAutomaticLocaleMirroringEnabled );
        iControlBar->SetFlag( EAlfVisualFlagDrawAfterOthers );
        
        iResManager.BrushManager().AppendBrushL( *iControlBar, ECalendarNullBrush );

        // Create visuals
        CAlfTextVisual* textVisual = iResManager.TextStyleManager().
            CreateTextVisualL( EControlBarText, *this, iControlBar );

        textVisual->SetAlign( EAlfAlignHCenter, EAlfAlignVCenter );
        textVisual->SetWrapping( CAlfTextVisual::ELineWrapTruncate );
        }
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------------------------
// CBCDayContainer::RemoveControlBar
// ---------------------------------------------------------------------------
//
void CBCDayContainer::RemoveControlBar()
    {
    TRACE_ENTRY_POINT;
    if ( iControlBar )
        {
        iControlBar->RemoveAndDestroyAllD();
        iControlBar = NULL;
        }
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------------------------
// CBCDayContainer::ConstructScrollableGridL
// ---------------------------------------------------------------------------
//
void CBCDayContainer::ConstructScrollableGridL()
    {
    TRACE_ENTRY_POINT;
    FSCALENDAR_PROFILE_START( FsCalendarProfile::EFsCalDayViewConstructScrollableGridL )

    // Parent layout for scrollable area
    iScrollableLayout = CAlfViewportLayout::AddNewL(*this, iBaseLayout);
    iScrollableLayout->SetFlag( EAlfVisualFlagAutomaticLocaleMirroringEnabled );
    iScrollableLayout->SetFlag( EAlfVisualFlagFreezeLayout );
    
    iResManager.BrushManager().AppendBrushL( *iScrollableLayout,
                                              EDayViewGridBg );

    // Parent layout for grids, for positioning the grids inside scrollable area
    iGridParentLayout = CAlfAnchorLayout::AddNewL(*this, iScrollableLayout);
    iGridParentLayout->SetFlag( EAlfVisualFlagAutomaticLocaleMirroringEnabled );
    iGridParentLayout->SetFlag( EAlfVisualFlagFreezeLayout );

    // There is one grid row per hour
    TInt rowCount = KCalenHoursInDay;
    TInt colCount = 1;

    // Time grid
    iTimeGridLayout = CAlfGridLayout::AddNewL(
        *this, colCount, rowCount, iGridParentLayout );
    iTimeGridLayout->SetFlag( EAlfVisualFlagAutomaticLocaleMirroringEnabled );
    iTimeGridLayout->SetFlag( EAlfVisualFlagFreezeLayout );
    
    iResManager.BrushManager().AppendBrushL( *iTimeGridLayout,
                                              EDayViewTimeGridBg );


    // Day grid
    iEventParentLayout = CAlfDeckLayout::AddNewL( *this, iGridParentLayout);
    iEventParentLayout->SetFlag( EAlfVisualFlagAutomaticLocaleMirroringEnabled );
    iEventParentLayout->SetFlag( EAlfVisualFlagFreezeLayout );

    iSelectorParentLayout = CAlfDeckLayout::AddNewL(*this, iEventParentLayout );
    iSelectorParentLayout->SetFlag( EAlfVisualFlagAutomaticLocaleMirroringEnabled );
    iSelectorParentLayout->SetFlag( EAlfVisualFlagFreezeLayout );

    iGridLayout = CAlfGridLayout::AddNewL(
        *this, colCount, rowCount, iEventParentLayout );
    iGridLayout->SetFlag( EAlfVisualFlagAutomaticLocaleMirroringEnabled );
    iGridLayout->SetFlag( EAlfVisualFlagFreezeLayout );

    //Visual Selection layout
    iVisualSelectionLayout = CAlfDeckLayout::AddNewL(*this, iEventParentLayout );
    //iVisualSelectionLayout->SetFlag( EAlfVisualFlagAutomaticLocaleMirroringEnabled );
    iVisualSelectionLayout->SetFlag( EAlfVisualFlagFreezeLayout );
    
    iResManager.BrushManager().AppendBrushL( *iGridLayout, EHourGridBorder );

    // Create visuals for grids
    CAlfTextVisual* timeText = NULL;

    HBufC* timeFormat = NULL;
    TLocale local;
    local.Refresh();    

    if (local.TimeFormat() == ETime24)
        {
        timeFormat = StringLoader::LoadLC(R_VIEWPLUGIN__DAYVIEW_TIMEFORMAT24_1);
        }
    else
        {
        timeFormat = StringLoader::LoadLC(R_VIEWPLUGIN_DAYVIEW_TIMEFORMAT);
        }


    TBool drawHourBorder( EFalse );
    
    FSCALENDAR_PROFILE_START( FsCalendarProfile::EFsCalDayViewConstructScrollableGridLForLoopOnly )
    
    for ( TInt i = 0; i < rowCount; i++ )
        {

        // Create text visual in time grid
        CAlfAnchorLayout* hourTimeSlotLayout = CAlfAnchorLayout::AddNewL(
            *this, iTimeGridLayout );
        hourTimeSlotLayout->SetFlag( EAlfVisualFlagAutomaticLocaleMirroringEnabled );

        timeText = iResManager.TextStyleManager().CreateTextVisualL(
            EDayViewTimeGridText, *this, hourTimeSlotLayout );
        timeText->SetAlign( EAlfAlignHRight, EAlfAlignVCenter );
        timeText->SetPadding( KBCDayTimeTextPadding );

        // Set time text
        TTime time(0);
        TTimeIntervalHours hour(i);
        time += hour;
        TBuf<KDayWeekViewTimeTextLength> hourDes;
        time.FormatL(hourDes, *timeFormat);
        AknTextUtils::LanguageSpecificNumberConversion( hourDes );
        timeText->SetTextL(hourDes);

        hourTimeSlotLayout->SetRelativeAnchorRect(0,
            EAlfAnchorOriginLeft, EAlfAnchorOriginTop, TAlfRealPoint(0.0, 0.0),
            EAlfAnchorOriginLeft, EAlfAnchorOriginTop, TAlfRealPoint(1.0, 0.5));

        // Create hour slot visual
        CAlfVisual* hourSlotLayout =
            AppendVisualL(EAlfVisualTypeVisual, iGridLayout);

        hourSlotLayout->SetFlag( EAlfVisualFlagAutomaticLocaleMirroringEnabled );
        
        if ( drawHourBorder )
            {
            iResManager.BrushManager().AppendBrushL( *hourSlotLayout,
                                                      EHourBorder );
            iResManager.BrushManager().AppendBrushL( *hourTimeSlotLayout,
                                                      EHourBorder );
            }
        drawHourBorder = !drawHourBorder;
        }
    
    FSCALENDAR_PROFILE_END( FsCalendarProfile::EFsCalDayViewConstructScrollableGridLForLoopOnly )
    
    CleanupStack::PopAndDestroy(timeFormat);

    // Define relative positions for grids:

    // Time grid
    iGridParentLayout->SetAnchor(EAlfAnchorTopLeft, 0,
        EAlfAnchorOriginLeft, EAlfAnchorOriginTop,
        EAlfAnchorMetricAbsolute, EAlfAnchorMetricAbsolute,
        TAlfTimedPoint(0, 0));
    iGridParentLayout->SetAnchor(EAlfAnchorBottomRight, 0,
        EAlfAnchorOriginLeft, EAlfAnchorOriginTop,
        EAlfAnchorMetricAbsolute, EAlfAnchorMetricRelativeToSize,
        TAlfTimedPoint(iTimeGridWidth, 1.0));


    // Day grid
    iGridParentLayout->SetAnchor(EAlfAnchorTopLeft, 1,
        EAlfAnchorOriginLeft, EAlfAnchorOriginTop,
        EAlfAnchorMetricAbsolute, EAlfAnchorMetricAbsolute,
        TAlfTimedPoint(iTimeGridWidth, 0));
    iGridParentLayout->SetAnchor(EAlfAnchorBottomRight, 1,
        EAlfAnchorOriginLeft, EAlfAnchorOriginTop,
        EAlfAnchorMetricRelativeToSize, EAlfAnchorMetricRelativeToSize,
        TAlfTimedPoint(1.0, 1.0));


    // Set viewport values:
    UpdateViewportValues();

    FSCALENDAR_PROFILE_END( FsCalendarProfile::EFsCalDayViewConstructScrollableGridL )
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------------------------
// CBCDayContainer::ConstructSelectorL
// ---------------------------------------------------------------------------
//
void CBCDayContainer::ConstructSelectorL()
    {
    TRACE_ENTRY_POINT;
    iSelectorVisual = CAlfImageVisual::AddNewL(*this, iSelectorParentLayout);
    iSelectorVisual->SetFlag( EAlfVisualFlagFreezeLayout );
    
    SetSelectorBrushL( );

    iSelectorParentLayout->MoveVisualToBack( *iSelectorVisual );

    // Set the selector visual size and pos.
    iSelectorVisual->SetFlag(EAlfVisualFlagManualSize);
    iSelectorVisual->SetFlag(EAlfVisualFlagManualPosition);
    iSelectorVisual->SetPos(TAlfRealPoint(0, 0));
    iSelectorVisual->SetSize(TSize(0, 0), 0);
    
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------------------------
// CBCDayContainer::SetSelectorBrushL
// Sets correct brush to selector.
// ---------------------------------------------------------------------------
//
void CBCDayContainer::SetSelectorBrushL( )
    {
    if ( iSelectorVisual )
        {
        //remove old brush if present
        CAlfBrushArray* brushArray = iSelectorVisual->Brushes();
        if ( brushArray )
            {
            TInt brushCount(0);
            brushCount = brushArray->Count();
            if ( brushCount >= 1 )
                {
                brushArray->Remove( brushCount - 1 );
                }
            }
        
        //set correct brush
        switch ( SelectorState() )
            {
            case ESelectorNormal:
                {
                iResManager.BrushManager().AppendBrushL( *iSelectorVisual,
                                                         EDaySelector );
                }
                break;
            case ESelectorPressed:
                {
                iResManager.BrushManager().AppendBrushL( *iSelectorVisual, 
                                                         EDaySelectorPressed );
                }
            case ESelectorOff:
            default:
                break; //dont set the brush
            }
        iSelectorVisual->SetFlag( EAlfVisualChanged );
        }
    }

// ---------------------------------------------------------------------------
// CBCDayContainer::ConstructHiddenEventsArrowsL
// Constructs arrows for indicating events outside the visible area
// ---------------------------------------------------------------------------
//
void CBCDayContainer::ConstructHiddenEventsArrowsL()
    {
    TRACE_ENTRY_POINT;
    TAlfTimedValue opacity;
    TPoint scrollAreaPos;
    TInt xPos;
    
    opacity.SetTarget( 0.0, 0 );
    //Arrow Up
    iScrollArrowUp = CAlfImageVisual::AddNewL( *this, iBaseLayout );
    iResManager.BrushManager().AppendBrushL( *iScrollArrowUp,
                                              EUnvisibleEventsArrowUp );
    iScrollArrowUp->SetFlag( EAlfVisualFlagManualSize );
    iScrollArrowUp->SetFlag( EAlfVisualFlagManualPosition );
    //initially "arrow up" is hidden
    iScrollArrowUp->SetOpacity( opacity );
    
    iScrollArrowUp->SetSize( iArrowSize );
    scrollAreaPos = iScrollableLayout->Pos().Target();
    xPos = ( iTimeGridWidth - iArrowSize.iWidth ) / 2;
    if ( xPos < 0 )
        {
        xPos = 0;
        }
    xPos += scrollAreaPos.iX;
    iScrollArrowUp->SetPos( TAlfRealPoint( xPos, scrollAreaPos.iY ) );
    
    //Arrow Down
    iScrollArrowDown = CAlfImageVisual::AddNewL( *this, iBaseLayout );
    iResManager.BrushManager().AppendBrushL( *iScrollArrowDown,
                                              EUnvisibleEventsArrowDown );
    iScrollArrowDown->SetFlag( EAlfVisualFlagManualSize );
    iScrollArrowDown->SetFlag( EAlfVisualFlagManualPosition );
    //initially "arrow down" is hidden
    iScrollArrowDown->SetOpacity( opacity );
    
    iScrollArrowDown->SetSize( iArrowSize );
    scrollAreaPos = iScrollableLayout->Pos().Target();
    xPos = ( iTimeGridWidth - iArrowSize.iWidth ) / 2;
    if ( xPos < 0 )
        {
        xPos = 0;
        }
    xPos += scrollAreaPos.iX;
    TSize screenSize = iParentLayout->Size().Target().AsSize();
    iScrollArrowDown->SetPos(
        TAlfRealPoint( xPos, screenSize.iHeight - iArrowSize.iHeight ) );
    
    TRACE_EXIT_POINT;
    }
    
// ---------------------------------------------------------------------------
// CBCDayContainer::PositionLayouts
// ---------------------------------------------------------------------------
//
void CBCDayContainer::PositionLayouts()
    {
    TRACE_ENTRY_POINT;
    FSCALENDAR_PROFILE_START( FsCalendarProfile::EFsCalDayViewViewPositionLayouts )
    
    // Position the elements:
    TInt yPos(0);

    // Control bar:
    if ( iControlBar )
        {

        iBaseLayout->SetAnchor(EAlfAnchorTopLeft, 1,
                EAlfAnchorOriginLeft, EAlfAnchorOriginTop,
                EAlfAnchorMetricAbsolute, EAlfAnchorMetricAbsolute,
                TAlfTimedPoint(0, yPos));
        iBaseLayout->SetAnchor(EAlfAnchorBottomRight, 1,
                EAlfAnchorOriginRight, EAlfAnchorOriginTop,
                EAlfAnchorMetricAbsolute, EAlfAnchorMetricAbsolute,
                TAlfTimedPoint(0, yPos + iControlBarHeight));

        yPos += iControlBarHeight;
        }

    // Scrollable grid:
    iBaseLayout->SetAnchor(EAlfAnchorTopLeft, 0,
        EAlfAnchorOriginLeft, EAlfAnchorOriginTop,
        EAlfAnchorMetricAbsolute, EAlfAnchorMetricAbsolute,
        TAlfTimedPoint(0, yPos));
    iBaseLayout->SetAnchor(EAlfAnchorBottomRight, 0,
        EAlfAnchorOriginRight, EAlfAnchorOriginTop,
        EAlfAnchorMetricAbsolute, EAlfAnchorMetricRelativeToSize,
        TAlfTimedPoint( 0, 1.0));

    iBaseLayout->UpdateChildrenLayout();

    UpdateViewportValues();

    FSCALENDAR_PROFILE_END( FsCalendarProfile::EFsCalDayViewViewPositionLayouts )
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------------------------
// CBCDayContainer::CreateTimedEventsL
// Create event visuals. Remove old ones if they exist.
// ---------------------------------------------------------------------------
//
void CBCDayContainer::CreateTimedEventsL()
    {
    TRACE_ENTRY_POINT;
    // First destroy old ones
    RemoveTimedEvents();
    UpdateTimedEventsL();
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------------------------
// CBCDayContainer::RemoveTimedEvents
// Remove event visuals.
// ---------------------------------------------------------------------------
//
void CBCDayContainer::RemoveTimedEvents()
    {
    TRACE_ENTRY_POINT;
    ClosePreviewWindow( EFalse );
    for ( TInt i=iApptVisualList.Count()-1; i >=0; i-- )
        {
        CAlfVisual* eventVisual = iApptVisualList[i];
        if ( eventVisual )
            {
            eventVisual->RemoveAndDestroyAllD();
            eventVisual=NULL;
            }
        }
    iApptVisualList.Reset();
    iPreviouslyFocusedEvent = NULL;
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------------------------
// CBCDayContainer::UpdateTimedEventsL
// Update event visuals. Create visuals if they don't exist.
// ---------------------------------------------------------------------------
//
void CBCDayContainer::UpdateTimedEventsL()
    {
    TRACE_ENTRY_POINT;
    FSCALENDAR_PROFILE_START( FsCalendarProfile::EFsCalDayViewUpdateEventVisualsLAll )
    
    if ( iLayoutTable )
        {
        for ( TInt i=0; i < iLayoutTable->Count(); i++)
            {
            SCalenApptInfo& event = iLayoutTable->At(i);

            // First try to find the visual if it already exists
            TBuf8<KTagTextLength> tagText;
            tagText.AppendNum( event.iId.iEntryLocalUid );
            tagText.Append('*');
            tagText.AppendNum( event.iId.iColId );

            CAlfVisual* eventVisual(NULL);
            eventVisual = iGridParentLayout->FindTag( tagText );

            if ( !eventVisual )
                {
                // Create new visual
                eventVisual = CreateTimedEventL( event );
                }
            UpdateTimedEventL( eventVisual, event );
            }
        }
    
    FSCALENDAR_PROFILE_END( FsCalendarProfile::EFsCalDayViewUpdateEventVisualsLAll )
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------------------------
// CBCDayContainer::CreateTimedEventL
// Creates event visual.
// ---------------------------------------------------------------------------
//
CAlfVisual* CBCDayContainer::CreateTimedEventL( SCalenApptInfo& aEvent )
    {
    TRACE_ENTRY_POINT;
    TBuf8<KTagTextLength> tagText;
    tagText.AppendNum( aEvent.iId.iEntryLocalUid );
    tagText.Append('*');
    tagText.AppendNum( aEvent.iId.iColId );

    // Creates the visual
    CAlfDeckLayout* eventVisual = CAlfDeckLayout::AddNewL( *this,
                                                           iEventParentLayout );
    eventVisual->SetFlag( EAlfVisualFlagAutomaticLocaleMirroringEnabled );

    iApptVisualList.AppendL( eventVisual );
    eventVisual->SetFlag(EAlfVisualFlagManualSize);
    eventVisual->SetFlag(EAlfVisualFlagManualPosition);

    eventVisual->SetTagL( tagText );

    //create color line for indicating Calendar the event belongs to
    CreateEventColorLineL( eventVisual );
    
    CAlfTextVisual* textVisual =
        iResManager.TextStyleManager().CreateTextVisualL(
        EMeetingText, *this, eventVisual );
    
    textVisual->SetFlag( EAlfVisualFlagAutomaticLocaleMirroringEnabled );
    textVisual->SetWrapping( CAlfTextVisual::ELineWrapTruncate );
    textVisual->SetAlign( EAlfAlignHCenter, EAlfAlignVCenter );
    
    //tag the text visual
    tagText.Copy( _L8("TxtVis") );
    textVisual->SetTagL( tagText );
    
    // Set background
    TBCalendarBrushId brushId( EMeetingBg );
    iResManager.BrushManager().AppendBrushL( *eventVisual, brushId );
    
    // Set text
    if ( aEvent.iAllDay )
        {
        textVisual->SetTextL( KNullDesC );
        }
    else
        {
        textVisual->SetTextL( aEvent.iSummary );
        }
    
    TRACE_EXIT_POINT;
    return eventVisual;
    }

// ---------------------------------------------------------------------------
// CBCDayContainer::CreateEventColorLineL
// ---------------------------------------------------------------------------
//
void CBCDayContainer::CreateEventColorLineL( CAlfLayout* aParenLayout )
    {
    TRACE_ENTRY_POINT;

    CAlfCanvasVisual* colorLine = CAlfCanvasVisual::AddNewL( *this, aParenLayout );
    colorLine->SetFlag(EAlfVisualFlagManualSize);
    colorLine->SetFlag(EAlfVisualFlagManualPosition);
//not needed
//    colorLine->SetClipping( ETrue );
    
    //tag the visual
    TBuf8<10> tagText;
    tagText.Append( _L8("LineVis") );
    colorLine->SetTagL( tagText );
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------------------------
// CBCDayContainer::UpdateTimedEventL
// Updates event visual.
// ---------------------------------------------------------------------------
//
void CBCDayContainer::UpdateTimedEventL( CAlfVisual* aVisual,
                                         SCalenApptInfo& aEvent )
    {
    TRACE_ENTRY_POINT;
    aVisual->SetFlag( EAlfVisualFlagFreezeLayout );   

    TInt startSlot, endSlot, columnIndex, columns;
    iSlotStorage->GetLocation( aEvent, startSlot, endSlot, columnIndex, columns );

    TTime visualStartTime( iDate ), visualEndTime( iDate );
    ConvertToDate( visualStartTime );
    ConvertToDate( visualEndTime );
    TInt startHour = iSlotStorage->HourFromSlotIndex( startSlot );
    TInt endHour = iSlotStorage->HourFromSlotIndex( endSlot );
    
    visualStartTime += TTimeIntervalMinutes( startHour * KCalenMinutesInHour );
    if ( !iSlotStorage->IsHourStartSlot( startSlot ) )
        {
        visualStartTime += TTimeIntervalMinutes( KCalenMinutesInHalfHour );    
        }
    visualEndTime += TTimeIntervalMinutes( endHour * KCalenMinutesInHour );
    if ( !iSlotStorage->IsHourStartSlot( endSlot ) )
        {
        visualEndTime += TTimeIntervalMinutes( KCalenMinutesInHalfHour );
        }
    
    TTimeIntervalMinutes visualDuration;
    visualEndTime.MinutesFrom( visualStartTime, visualDuration );
    
    // Get the extra slot visual, or full hour visual
    CAlfVisual& startVisual = GetSlotVisual( startSlot );
    
    TInt eventVisualWidth( startVisual.Size().Target().AsSize().iWidth );

    // Leave space to the right
    eventVisualWidth -= (iSelectionSpaceOnRight + iEventMargin );

    // Get the absolute display coordinates of the visual.
    TPoint displayPosStart = startVisual.Pos().Target();

    TPoint eventPos( displayPosStart );

    // Take allday events into account for all other events
    TInt alldayCount = iSlotStorage->AlldayCount();
    if  ( ( alldayCount > 0 ) && ( !aEvent.iAllDay ) )
        {
        TInt alldayTotalWidth = iAlldayEventWidth + iEventMargin;
        eventPos.iX += alldayTotalWidth;
        eventVisualWidth -= alldayTotalWidth;
        }

    if ( !iSlotStorage->IsHourStartSlot( startSlot ) )
        {
        eventPos.iY += iSlotHeight;
        }

    TBool lastUntilMidnight(EFalse);
    if ( endSlot >= KFSCalHourSlotCount + iExtraSlots )
        {
        endSlot = KFSCalHourSlotCount + iExtraSlots - 1;
        lastUntilMidnight = ETrue;
        }

    CAlfVisual& endVisual = GetSlotVisual( endSlot );
    
    // Get the position of the visual.
    TPoint displayPosEnd = endVisual.Pos().Target();

    if ( !iSlotStorage->IsHourStartSlot( endSlot ) )
        {
        displayPosEnd.iY += iSlotHeight;
        }

    if ( lastUntilMidnight )
        {
        displayPosEnd.iY += iSlotHeight;
        }

    // Set the size of the event visual.
    if ( aEvent.iAllDay )
        {
        eventVisualWidth = (iAlldayEventWidth + iEventMargin) / alldayCount;
        eventVisualWidth -= iEventMargin;
        eventPos.iX += iEventMargin;
        }
    else
        {
        // left margin
        eventVisualWidth -= iEventMargin;
        eventPos.iX += iEventMargin;

        eventVisualWidth /= columns;
        // right margin
        eventVisualWidth -= iEventMargin;
        }

    TInt eventVisualHeight = displayPosEnd.iY - eventPos.iY;

    eventPos.iX += columnIndex * ( eventVisualWidth + iEventMargin );
    
    TBool startBorder(EFalse);
    CAlfBrushArray* brushes = startVisual.Brushes();
    if (brushes && brushes->Count() > 0)
        {
        startBorder=ETrue;
        }

    TBool endBorder(EFalse);
    brushes = endVisual.Brushes();
    if (brushes && brushes->Count() > 0)
        {
        endBorder=ETrue;
        }

    if ( startBorder )
        {
        eventPos.iY +=1;
        eventVisualHeight--;
        }
    if ( !endBorder )
        {
        eventVisualHeight--;
        }
    
    // Place the event visual to the correct grid position.
    aVisual->SetSize( TSize( eventVisualWidth, eventVisualHeight ) );
    aVisual->SetPos( eventPos );
    
    //set size/pos of the line visual
    CAlfVisual* visual(NULL);
    
    TAlfRealSize lineSize( iEventColorLineWidth, eventVisualHeight );
    TAlfRealPoint linePos( 6, 0 );
    
    TBuf8<20> tagText;
    tagText.Append( _L8("LineVis") );
    visual = aVisual->FindTag( tagText );
    if ( visual )
        {
        //get actual meeting duration in minutes to show the stripe
        //CreateLayoutTableL() 'cuts' the meetings to be within the current day
        //so we don't have to worry about events crossing midnight
//        TDateTime debugStartTime = aEvent.iStartTime.DateTime();
//        TDateTime debugEndTime = aEvent.iEndTime.DateTime();
    
        //decrease by 2 pix per frame
        lineSize.iHeight -= 4;
        linePos.iY += 2;
                
        TTimeIntervalMinutes meetingDuration;
        TTime startTime( aEvent.iStartTime ), endTime( aEvent.iEndTime );
        endTime.MinutesFrom( startTime, meetingDuration );
        if ( meetingDuration.Int() < KCalenMinutesFifteen )
            {//we show actual meeting duration with 15m accuracy
            meetingDuration = KCalenMinutesFifteen; 
            }
        
        TInt actualMeetingDurationInPix(0);
        actualMeetingDurationInPix = meetingDuration.Int() * lineSize.iHeight / visualDuration.Int();
        //get actual meeting start position within visual
        TTimeIntervalMinutes meetingStart;
        startTime.MinutesFrom( visualStartTime, meetingStart );
        TInt actualMeetingStartInPix(0);
        actualMeetingStartInPix =  meetingStart.Int() * lineSize.iHeight / visualDuration.Int();
        
        //safety check for positioning actual meeting inside the visual
        //e.g. 5 minutes long meeting is shown with 15m accuracy
        if ( actualMeetingStartInPix + actualMeetingDurationInPix > lineSize.iHeight )
            {
            actualMeetingStartInPix = lineSize.iHeight - actualMeetingDurationInPix;
            }
        if ( actualMeetingStartInPix < 0 )
            {
            actualMeetingStartInPix = 0;
            }
    
        //set size and position of the line visual
        visual->SetSize( lineSize );
        visual->SetPos( linePos );
        //redraw the line
        TRect drawingRect( TPoint( 0, actualMeetingStartInPix), 
                           TPoint( lineSize.iWidth, actualMeetingStartInPix + actualMeetingDurationInPix ) );
        UpdateEventColorLine(*(static_cast<CAlfCanvasVisual*>(visual)),aEvent, drawingRect );
        }
    else
        {
        lineSize.iWidth = 0.0;
        lineSize.iHeight = 0.0;
        linePos.iX = 0.0;
        linePos.iY = 0.0;
        }
    
    //update event text
    visual = NULL;
    tagText.Copy( _L8("TxtVis") );
    visual = aVisual->FindTag( tagText );
    if ( visual )
        {
        CAlfTextVisual* tv = static_cast<CAlfTextVisual*>(visual);
        //set text style and color
        iResManager.TextStyleManager().SetVisualStyleL( EMeetingText, tv );
        //set text padding
        TAlfBoxMetric padding;
        padding.iTop.iUnit = EAlfUnitPixel;
        padding.iTop.iMagnitude = KFSCalDayEventTextPaddingY;
        padding.iBottom.iUnit = EAlfUnitPixel;
        padding.iBottom.iMagnitude = KFSCalDayEventTextPaddingY;
        padding.iLeft.iUnit = EAlfUnitPixel;
        padding.iRight.iUnit = EAlfUnitPixel;
        if ( AknLayoutUtils::LayoutMirrored() )
            {//automatic mirroring doesn't seem to work with padding
            padding.iLeft.iMagnitude = KFSCalDayEventTextPaddingX;
            padding.iRight.iMagnitude = linePos.iX + lineSize.iWidth + KFSCalDayEventTextPaddingX;
            }
        else
            {
            padding.iLeft.iMagnitude = linePos.iX + lineSize.iWidth + KFSCalDayEventTextPaddingX;
            padding.iRight.iMagnitude = KFSCalDayEventTextPaddingX;
            }
        
        
        tv->SetPadding( padding );
        tv->SetAlign( EAlfAlignHCenter, EAlfAlignVCenter );
        tv->SetWrapping( CAlfTextVisual::ELineWrapTruncate );
        }
    
    aVisual->ClearFlag( EAlfVisualFlagFreezeLayout );   
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------------------------
// CBCDayContainer::UpdateEventColorLine
// Updates color line.
// ---------------------------------------------------------------------------
//
void CBCDayContainer::UpdateEventColorLine( CAlfCanvasVisual& aCanvas, 
		                                    SCalenApptInfo& aEvent,
						                    TRect& aInnerRect )
    {
	TRACE_ENTRY_POINT;
	
	TAlfPolygonDrawMode fillType( ENoFill );
	RArray<TAlfRealLine> lines;
	TReal32 lineWidth;
	RArray<TAlfRealRect> boundingRect;
	RArray<TAlfRealRect> drawingRect;
	
	drawingRect.Append( aInnerRect );
	TRect outerRect( TPoint(0.0,0.0), aCanvas.Size().Target() );
	
	if ( outerRect != drawingRect[0]  )
	    {//draw bounding rectangle
	    boundingRect.Append( outerRect );
	    }
	
	switch ( aEvent.iStatus )
		{
        case CCalEntry::ETentative:
        	{//draw diagonal stripes
        	fillType = ENoFill;
        	lineWidth = 1.0;
        	//prepare diagonal lines
        	if ( drawingRect.Count() > 0 )
        	    {
                TReal32 width( drawingRect[0].Width() - 1 );
                TReal32 endY (drawingRect[0].iBr.iY - 1);
                
                TAlfRealLine line;
                line.iStart.iX = width;
                line.iStart.iY = drawingRect[0].iTl.iY;
                line.iEnd.iX = drawingRect[0].iTl.iX;
                line.iEnd.iY = drawingRect[0].iTl.iY + width;        	
                
                while( line.iEnd.iY <= endY )
                    {
                    lines.Append( line );
                    line.iStart.iY += width;
                    line.iEnd.iY += width;
                    }
        	    }
        	}
            break;
        case CCalEntry::ECancelled:
        	{
        	fillType = ENoFill;
        	}
            break;
        case CCalEntry::EConfirmed:
        default: //draw filled rect
        	{
        	fillType = EFillNonZero;
        	}
		}
	
	aCanvas.BeginDraw();
	//set pen color - calendar color that the event belongs to
	aCanvas.SetPenColor( aEvent.iColor );
	//draw bounding rect?
	if ( boundingRect.Count() > 0 )
	    {//bounding rect always as empty rect
        aCanvas.SetPolygonDrawMode( ENoFill );
        aCanvas.DrawRects( &(boundingRect[0]), boundingRect.Count() );
	    }
	
	if ( drawingRect.Count() > 0 )
	    {
        aCanvas.SetPolygonDrawMode( fillType );
        aCanvas.DrawRects( &(drawingRect[0]), drawingRect.Count() );
	    }
	//draw lines?
	if ( lines.Count() > 0 )
	    {
        aCanvas.SetPolygonDrawMode( ENoFill );
        aCanvas.SetPenWidth( lineWidth );
        aCanvas.DrawLines( &(lines[0]), lines.Count() );
	    }
	
    aCanvas.EndDraw();
    
    lines.Close();
    boundingRect.Close();
    drawingRect.Close();
    
    TRACE_EXIT_POINT;
    return;     
    }
    
// ---------------------------------------------------------------------------
// CBCDayContainer::ConstructUntimedEventsGridL
// ---------------------------------------------------------------------------
//
void CBCDayContainer::ConstructUntimedEventsL()
    {
    TRACE_ENTRY_POINT;
    FSCALENDAR_PROFILE_START( FsCalendarProfile::EFsCalDayViewConstructUntimedEventsL )
    
    // Remove the old extra grid stuff
    RemoveExtraSlotsL();

    // Go through instance list to find the todos/memos/anniversaries
    TInt todoCount(0);
    TBool drawBorder( EFalse );

    TInt alldayCount = iSlotStorage->AlldayCount();

    for ( TInt i = 0; i < iInstanceList.Count(); i++ )
        {
        CCalEntry& entry = iInstanceList[i]->Entry();
        CCalEntry::TType type = entry.EntryTypeL();
        if ( !CalenAgendaUtils::IsTimedEntryL( type ) )
            {

            if ( type == CCalEntry::ETodo && todoCount > 0 )
                {
                // No need to add new line to grid, just update the storage
                todoCount++;
                iSlotStorage->InsertUntimedEventL( *(iInstanceList[i]) );
                }
            else
                {
                iExtraSlots++;

                if ( !iExtraSlotGrid )
                    {
                    // create the grid with one row
                    AddExtraLineToGridL();
                    }
                else
                    {
                    // just add rows
                    iExtraSlotGrid->SetRowsL( iExtraSlots );
                    }

                CAlfAnchorLayout* layoutVisual = CAlfAnchorLayout::AddNewL(
                                                            *this,
                                                            iExtraSlotGrid );
                layoutVisual->SetFlag(
                    EAlfVisualFlagAutomaticLocaleMirroringEnabled );
                layoutVisual->SetFlag( EAlfVisualFlagFreezeLayout );
                

                CAlfImageVisual* imageVisual = CAlfImageVisual::AddNewL(
                                                            *this,
                                                            layoutVisual );
                CAlfTextVisual* textVisual = 
                    iResManager.TextStyleManager().CreateTextVisualL(
                            EFocusedDayGridText, *this, layoutVisual );
                textVisual->SetWrapping( CAlfTextVisual::ELineWrapTruncate );
                textVisual->SetFlag(
                    EAlfVisualFlagAutomaticLocaleMirroringEnabled );
                textVisual->SetFlag( EAlfVisualFlagFreezeLayout );
                
                textVisual->SetAlign( EAlfAlignHLocale, EAlfAlignVCenter );

                HBufC* displayText = SummaryLocationCustomL( entry,
                                                   R_QTN_CALENDAR_DAY_UNNAMED );
                TBuf<KFSCalMaxDescriptionLength> cutText;
                cutText.Copy( displayText->Left( KFSCalMaxDescriptionLength ) );
                delete displayText;

                if ( type == CCalEntry::ETodo )
                    {
                    // We are adding the 1st to-do. Use todo name
                    iResManager.BrushManager().AppendBrushL( *imageVisual,
                                                              ETodoImage );
                    textVisual->SetTextL( cutText );
                    iExtraSlotGrid->MoveVisualToBack( *layoutVisual );
                    todoCount++;
                    }
                else if ( type == CCalEntry::EEvent )
                    {
                    iResManager.BrushManager().AppendBrushL( *imageVisual,
                                                              EMemoImage );
                    textVisual->SetTextL( cutText );
                    }

                else if ( type == CCalEntry::EAnniv )
                    {
                    iResManager.BrushManager().AppendBrushL( *imageVisual,
                                                              EAnnivImage );
                    textVisual->SetTextL( cutText );
                    }

                TInt index(0);
                TInt emptyWidth(0);
                if ( alldayCount > 0 )
                    {
                    emptyWidth = iAlldayEventWidth + iEventMargin;
                    }
                emptyWidth += iEventMargin;

                // Image
                TSize imageSize = iResManager.LayoutHandler().EventIconSize();
                TInt imageWidth = imageSize.iWidth;

                layoutVisual->SetAnchor( EAlfAnchorTopLeft, index,
                    EAlfAnchorOriginLeft, EAlfAnchorOriginTop,
                    EAlfAnchorMetricAbsolute, EAlfAnchorMetricRelativeToSize,
                    TAlfTimedPoint( emptyWidth + KBCDayPictureHMargin,
                                    0.0 ) );
                layoutVisual->SetAnchor( EAlfAnchorBottomRight, index,
                    EAlfAnchorOriginLeft, EAlfAnchorOriginTop,
                    EAlfAnchorMetricAbsolute, EAlfAnchorMetricRelativeToSize,
                    TAlfTimedPoint( emptyWidth + KBCDayPictureHMargin + imageWidth,
                                    1.0) );
                index++;

                imageWidth += 2 * KBCDayPictureHMargin;


                // Text
                layoutVisual->SetAnchor(EAlfAnchorTopLeft, index,
                    EAlfAnchorOriginLeft, EAlfAnchorOriginTop,
                    EAlfAnchorMetricAbsolute, EAlfAnchorMetricAbsolute,
                    TAlfTimedPoint( emptyWidth + imageWidth, 0));

                layoutVisual->SetAnchor(EAlfAnchorBottomRight, index,
                    EAlfAnchorOriginLeft, EAlfAnchorOriginTop,
                    EAlfAnchorMetricRelativeToSize, EAlfAnchorMetricRelativeToSize,
                    TAlfTimedPoint(1.0, 1.0));

                if ( drawBorder )
                    {
                    iResManager.BrushManager().AppendBrushL( *layoutVisual,
                                                              EHalfHourBorder );
                    }
                drawBorder = !drawBorder;


                // Add to slot storage
                iSlotStorage->InsertUntimedEventL( *(iInstanceList[i]) );
                
                layoutVisual->ClearFlag( EAlfVisualFlagFreezeLayout );
                textVisual->ClearFlag( EAlfVisualFlagFreezeLayout );
                }
            }
        }

    if ( todoCount > 1 )
        {

        // Change todo name to indicate how many uncompleted todos,
        // instead of to-do name
        CAlfLayout& todoSlotVisual = static_cast<CAlfLayout&>(
            iExtraSlotGrid->Visual(0));
        
        CAlfTextVisual& todoTextVisual = static_cast<CAlfTextVisual&>(
            todoSlotVisual.Visual( KBCDayUntimedEventTextIndex ));

        HBufC* todoText = StringLoader::LoadLC(
            R_QTN_CALENDAR_UNCOMPLETED_TASKS, todoCount );
        TPtr textDes = todoText->Des();
        AknTextUtils::LanguageSpecificNumberConversion( textDes );
        todoTextVisual.SetTextL( textDes );
        CleanupStack::PopAndDestroy( todoText );
        }

    // Change the row weights for the day grids
    if ( iExtraSlots )
        {

        // Set
        RArray<TInt> weights;
        CleanupClosePushL( weights );
        TBool border(EFalse);

        for ( TInt i=0; i < KCalenHoursInDay + 1; i++ )
            {
            if ( i == ( iExtraSlotGridPos ) )
                {
                weights.AppendL( iExtraSlots * iSlotHeight );
                }
            else
                {
                if ( border )
                    {
                    weights.AppendL( KFSCalSlotsInHour * iSlotHeight +1 );
                    }
                else
                    {
                    weights.AppendL( KFSCalSlotsInHour * iSlotHeight -1 );
                    }
                border = !border;
                }
            }
        iTimeGridLayout->SetRowsL( weights );
        iGridLayout->SetRowsL( weights );
        weights.Reset();

        if ( iExtraSlotBorderIndex == 0 )
            {
            weights.AppendL(1);
            weights.AppendL( iExtraSlots * iSlotHeight -1 );
            }
        else
            {
            weights.AppendL( iExtraSlots * iSlotHeight -1 );
            weights.AppendL(1);
            }
        iExtraSlotParent->SetRowsL( weights );
        iExtraTimeSlot->SetRowsL( weights );
        weights.Reset();

        // set weights within the extra grid rows
        border = EFalse;

        for ( TInt i=0; i < iExtraSlots ; i++ )
            {
            if ( !border )
                {
                if ( iExtraSlots == 1 )
                    {
                    weights.AppendL( iSlotHeight -1 );
                    }
                else if ( i == 0 || i == iExtraSlots - 1 )
                    {
                    weights.AppendL( iSlotHeight);
                    }
                else
                    {
                    weights.AppendL( iSlotHeight + 1 );
                    }
                }
            else
                {
                weights.AppendL( iSlotHeight -1 );
                }
            border = !border;
            }
        iExtraSlotGrid->SetRowsL( weights );
        CleanupStack::PopAndDestroy( &weights );
        }

    iSlotStorage->UpdateUntimedPos();
    UpdateViewportValues();
    
    FSCALENDAR_PROFILE_END( FsCalendarProfile::EFsCalDayViewConstructUntimedEventsL )
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------------------------
// CBCDayContainer::UpdateUntimedEventsL
// Update text style for each extra row.
// ---------------------------------------------------------------------------
//
void CBCDayContainer::UpdateUntimedEventsL()
    {
    TRACE_ENTRY_POINT;
    if ( iExtraSlotGrid )
        {
        TInt alldayCount = iSlotStorage->AlldayCount();
        TInt rowCount = iExtraSlotGrid->RowCount();
        for (TInt i(0); i < rowCount; i++ )
            {
            CAlfAnchorLayout& extraRow = static_cast<CAlfAnchorLayout&>(
                iExtraSlotGrid->Visual(i) );
            extraRow.SetFlag( EAlfVisualFlagFreezeLayout );
            
            CAlfTextVisual& textVisual = static_cast<CAlfTextVisual&>(
                extraRow.Visual( KBCDayUntimedEventTextIndex ) );
            
            iResManager.TextStyleManager().SetVisualStyleL(
                EFocusedDayGridText, &textVisual );

            TInt index(0);
            TInt emptyWidth(0);
            if ( alldayCount > 0 )
                {
                emptyWidth = iAlldayEventWidth + iEventMargin;
                }
            emptyWidth += iEventMargin;

            // Image
            TSize imageSize = iResManager.LayoutHandler().EventIconSize();
            TInt imageWidth = imageSize.iWidth;

            // Image
            extraRow.SetAnchor( EAlfAnchorTopLeft, index,
                    EAlfAnchorOriginLeft, EAlfAnchorOriginTop,
                    EAlfAnchorMetricAbsolute, EAlfAnchorMetricRelativeToSize,
                    TAlfTimedPoint( emptyWidth + KBCDayPictureHMargin,
                                    0.0 ) );
            extraRow.SetAnchor( EAlfAnchorBottomRight, index,
                    EAlfAnchorOriginLeft, EAlfAnchorOriginTop,
                    EAlfAnchorMetricAbsolute, EAlfAnchorMetricRelativeToSize,
                    TAlfTimedPoint( emptyWidth + KBCDayPictureHMargin + imageWidth,
                                    1.0 ) );
            index++;
            imageWidth += 2 * KBCDayPictureHMargin;

            // Text
            extraRow.SetAnchor(EAlfAnchorTopLeft, index,
                    EAlfAnchorOriginLeft, EAlfAnchorOriginTop,
                    EAlfAnchorMetricAbsolute, EAlfAnchorMetricAbsolute,
                    TAlfTimedPoint( emptyWidth + imageWidth, 0));

            extraRow.SetAnchor(EAlfAnchorBottomRight, index,
                    EAlfAnchorOriginLeft, EAlfAnchorOriginTop,
                    EAlfAnchorMetricRelativeToSize, EAlfAnchorMetricRelativeToSize,
                    TAlfTimedPoint(1.0, 1.0));
            
            extraRow.ClearFlag( EAlfVisualFlagFreezeLayout );
            }
        }
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------------------------
// CBCDayContainer::AddExtraLineToGridL
// ---------------------------------------------------------------------------
//
void CBCDayContainer::AddExtraLineToGridL()
    {
    TRACE_ENTRY_POINT;
    // Get the correct position. Extra rows are placed just before
    // the first event
    TInt firstExtraSlot = iSlotStorage->UpdateUntimedPos();
    iExtraSlotGridPos = firstExtraSlot / KFSCalSlotsInHour;

    // If the next hour has a border, place the border line in the
    // beginning of extra slot, otherwise it's after the extra rows.
    CAlfVisual& nextHour = iGridLayout->Visual( iExtraSlotGridPos );
    
    CAlfBrushArray* brushes = nextHour.Brushes();
    if ( brushes && brushes->Count() > 0 )
        {
        iExtraSlotBorderIndex = 0;
        }
    else
        {
        iExtraSlotBorderIndex = 1;
        }

    TInt rowCount = KCalenHoursInDay + 1;

    // Time grid. Time column is empty for extra slots.
    iTimeGridLayout->SetRowsL( rowCount );

    iExtraTimeSlot = CAlfGridLayout::AddNewL( *this, 1, 2, iTimeGridLayout );
    iExtraTimeSlot->SetFlag( EAlfVisualFlagFreezeLayout );
    
    CAlfVisual* emptyVisual = AppendVisualL( EAlfVisualTypeVisual, iExtraTimeSlot );
    
    CAlfVisual* lineVisual = AppendVisualL( EAlfVisualTypeVisual, iExtraTimeSlot );
    
    if ( iExtraSlotBorderIndex == 0 )
        {
        // move the line visual to beginning
        iExtraTimeSlot->Reorder( *lineVisual, 0 );
        }

    iResManager.BrushManager().AppendBrushL( *lineVisual, EHourBorder );
    iTimeGridLayout->Reorder( *iExtraTimeSlot, iExtraSlotGridPos);


    // Day grid. Create extra slot with one row
    iGridLayout->SetRowsL( rowCount );
    iExtraSlotParent = CAlfGridLayout::AddNewL( *this, 1, 2, iGridLayout );
    iExtraSlotParent->SetFlag( EAlfVisualFlagAutomaticLocaleMirroringEnabled );
    iExtraSlotParent->SetFlag( EAlfVisualFlagFreezeLayout );

    iExtraSlotGrid = CAlfGridLayout::AddNewL( *this, 1, 1, iExtraSlotParent );
    iExtraSlotGrid->SetFlag( EAlfVisualFlagAutomaticLocaleMirroringEnabled );
    iExtraSlotGrid->SetFlag( EAlfVisualFlagFreezeLayout );
    
    CAlfVisual* lineVisual2 = AppendVisualL( EAlfVisualTypeVisual, iExtraSlotParent );
    
    if ( iExtraSlotBorderIndex == 0 )
        {
        // move the line visual to beginning
        iExtraSlotParent->Reorder( *lineVisual2, 0 );
        }

    iResManager.BrushManager().AppendBrushL( *lineVisual2, EHourBorder );
    iGridLayout->Reorder( *iExtraSlotParent, iExtraSlotGridPos);
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------------------------
// CBCDayContainer::RemoveExtraSlotsL
// ---------------------------------------------------------------------------
//
void CBCDayContainer::RemoveExtraSlotsL()
    {
    TRACE_ENTRY_POINT;
    // First remove the extra slots
    if ( iExtraSlotGrid )
        {
        iExtraSlotGrid->RemoveAndDestroyAllD();
        iExtraSlotGrid = NULL;
        }

    if ( iExtraSlots )
        {
        // Remove extra row from time grid
        TInt rowCount = iTimeGridLayout->RowCount();
        if ( iExtraTimeSlot )
            {
            iTimeGridLayout->Reorder( *iExtraTimeSlot, rowCount-1 );
            iTimeGridLayout->SetRowsL( KCalenHoursInDay );
            iExtraTimeSlot->RemoveAndDestroyAllD();
            iExtraTimeSlot = NULL;
            }

        // Remove extra row from hour grid
        rowCount = iGridLayout->RowCount();
        if ( iExtraSlotParent )
            {
            iGridLayout->Reorder( *iExtraSlotParent, rowCount-1 );
            iGridLayout->SetRowsL( KCalenHoursInDay);
            iExtraSlotParent->RemoveAndDestroyAllD();
            iExtraSlotParent = NULL;
            }

        iExtraSlots = 0;

        // Update row weights
        RArray<TInt> weights;
        CleanupClosePushL( weights );
        TBool border(EFalse);
        for (TInt i=0; i < KCalenHoursInDay; i++)
            {
            if ( border )
                {
                weights.AppendL( KFSCalSlotsInHour * iSlotHeight +1 );
                }
            else
                {
                weights.AppendL( KFSCalSlotsInHour * iSlotHeight -1 );
                }
            }

        iTimeGridLayout->SetRowsL(weights);
        iGridLayout->SetRowsL(weights);
        CleanupStack::PopAndDestroy( &weights );

        }
    iPreviouslyFocusedUntimedSlot = KErrNotFound;
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------------------------
// CBCDayContainer::SetHighlightFromActiveContextL()
// Read calendar state and select correct hour
// ---------------------------------------------------------------------------
//
void CBCDayContainer::SetHighlightFromActiveContextL() 
    { 
    TRACE_ENTRY_POINT; 
  
    TCalenInstanceId instanceId = iServices.Context().InstanceId(); 
    iDate = iServices.Context().FocusDateAndTimeL().TimeLocalL(); 
    
    if ( instanceId == TCalenInstanceId::NullInstanceId() ) 
        {
		// No event focused, focus hour 
        TInt hour( iDate.DateTime().Hour() );
		
        if ( iServices.Context().ViewId().iViewUid == KUidCalenMonthView 
        		&& CalenDateUtils::IsOnToday( iDate ) )
        	{
			hour = CalenDateUtils::Now().DateTime().Hour();
        	}
         
        SelectHour( hour ); 
        } 
    else 
        { 
        TInt ret = iSlotStorage->SelectEvent( instanceId ); 
        if ( ret == KErrNone ) 
            { 
            FocusEventL(); 
            } 
        else 
            {
			// event was deleted from view, remove it from context
			
			TCalTime focusedTime = iServices.Context().FocusDateAndTimeL(); 

			iServices.Context().SetFocusDateAndTimeL( focusedTime, iView->ViewId() );
            SelectHour( iDate.DateTime().Hour() ); 
            } 
        } 
    TRACE_EXIT_POINT; 
    }

// ---------------------------------------------------------------------------
// CBCDayContainer::UpdateSelectorL
// ---------------------------------------------------------------------------
//
void CBCDayContainer::UpdateSelectorL( TInt aLayoutTime, TBool aAllowScroll,
                                       TBool aRefreshCurrent )
    {
    TRACE_ENTRY_POINT;
    //update context according to highlight
    SetActiveContextFromHighlightL();    
    
    TBool eventFocused( EFalse );
    if ( iSlotStorage->IsEventSelected() )
        {
        eventFocused = FocusEventL( aLayoutTime, aRefreshCurrent );
        }

    if ( !eventFocused )
        {
        // Focus the selected slot
        FocusSlotL( aLayoutTime );
        }
    
    if ( aAllowScroll )
        {
        UpdateScrollingL();
        }
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------------------------
// CBCDayContainer::FocusEventL
// ---------------------------------------------------------------------------
//
TBool CBCDayContainer::FocusEventL( TInt aLayoutTime, TBool aRefreshCurrent )
    {
    TRACE_ENTRY_POINT;
    TBool eventFocused( EFalse );

    if ( iSlotStorage->IsEventSelected() &&
         !iSlotStorage->IsMultipleEventsSelected() )
        {
        TCalenInstanceId eventId = iSlotStorage->SelectedEvent();

        TBuf8<KTagTextLength> tagText;
        tagText.AppendNum( eventId.iEntryLocalUid );
        tagText.Append('*');
        tagText.AppendNum( eventId.iColId ); //append collection id for this event
        
        CAlfLayout* eventVisual(NULL);
        eventVisual = static_cast<CAlfLayout*>(
            iGridParentLayout->FindTag( tagText ));
        
        if ( eventVisual )
            {
            eventFocused = ETrue;
            if ( iPreviouslyFocusedEvent != eventVisual || aRefreshCurrent )
                {
                //close popup with no animation
                //we want to see what's potentially behind the pop up
                ClosePreviewWindow( EFalse );

                ResetFocusTextsL();

                // Get the local coordinates of the event visual.
                TPoint eventPos = eventVisual->Pos().Target();

                // Move the selector visual to the correct position.
                iSelectorVisual->SetPos(eventPos, aLayoutTime);

                // Set the size of the selector.
                TSize size = eventVisual->Size().ValueNow().AsSize();
                iSelectorVisual->SetSize( size, aLayoutTime );

                iPreviouslyFocusedEvent = eventVisual;
                if ( IsSelectorEnabled() )
                    {
                    UpdateEventBackgroundL( eventVisual, ETrue );
    
                    if ( IsKeyboardActive() )
                        {
                        // Show preview popup when selecting event with keyboard
                        CCalInstance* dayInstance = NULL;
                        dayInstance = CalenActionUiUtils::FindPossibleInstanceL(
                                                            eventId,
                                                            InstanceViewL() );
                        if ( dayInstance )
                            {
                            iParentLayout->SetFlags(EAlfVisualFlagFreezeLayout);
                            CleanupStack::PushL( dayInstance );
        
                            TInt selectedSlot;
                            TInt columnIndex, columns, region;
                            iSlotStorage->GetSelectedSlot( selectedSlot, region, columnIndex, columns );
        
                            // Get the selected slot visual
                            CAlfVisual& selectedSlotVisual = GetSlotVisual( selectedSlot );
                            
                            if ( iPreviewPopup )
                                {                        
                                iPreviewPopup->ShowPreviewPopupL(
                                        dayInstance,
                                        eventVisual,
                                        &selectedSlotVisual,
                                        iSlotStorage->IsAlldayEventSelected() );
                                }
                            CleanupStack::Pop( dayInstance );
                            iParentLayout->ClearFlags(EAlfVisualFlagFreezeLayout);
                            }
                        }
                    }
                }
            }
        }
    TRACE_EXIT_POINT;
    return eventFocused;
    }

// ---------------------------------------------------------------------------
// CBCDayContainer::FocusSlotL
// ---------------------------------------------------------------------------
//
void CBCDayContainer::FocusSlotL( TInt aLayoutTime )
    {
    TRACE_ENTRY_POINT;
    ClosePreviewWindow( EFalse );
    ResetFocusTextsL();
    if ( iSlotStorage && IsSelectorEnabled() )
        {
        TInt selectedSlot;
        TInt columnIndex, columns, region;
        iSlotStorage->GetSelectedSlot( selectedSlot, region, columnIndex, columns );

        // Get the selected slot visual
        CAlfVisual& selectedSlotVisual = GetSlotVisual( selectedSlot );

        CAlfBrushArray* brushes = selectedSlotVisual.Brushes();
        TBool borderWithin( EFalse );
        if ( brushes && brushes->Count() > 0)
            {
            borderWithin = ETrue;
            }
        
        // Get the absolute display coordinates and size of the slot visual.
        TPoint displayPos = selectedSlotVisual.Pos().Target();
        TSize slotSize = selectedSlotVisual.Size().Target().AsSize();
        TSize selectorSize( slotSize );
        if ( iSlotStorage->IsExtraSlot( selectedSlot ) )
            {
            // Find the correct visual from the grid
            TPoint extraSlotPos = iExtraSlotParent->Pos().Target();
            displayPos.iY += extraSlotPos.iY;
            // border is extended outside the visual
            borderWithin = !borderWithin;
            }

        // Create margins for the selector
        displayPos.iX += iEventMargin;
        selectorSize.iWidth -= iEventMargin * 2;

        // Take other events into account -> selector is smaller

        // Allday events
        TInt alldayCount = iSlotStorage->AlldayCount();
        if ( alldayCount > 0 )
            {
            TInt alldayTotalWidth = iAlldayEventWidth + iEventMargin;
            selectorSize.iWidth -= alldayTotalWidth;
            displayPos.iX += alldayTotalWidth;
            }

        // Other events
        if ( columns > 0 )
            {
            if ( columnIndex < columns )
                {
                selectorSize.iWidth -= iSelectionSpaceOnRight;
                selectorSize.iWidth /= columns;
                displayPos.iX += columnIndex * selectorSize.iWidth;
                selectorSize.iWidth -= iEventMargin;
                }
            else
                {
                displayPos.iX =
                    (slotSize.iWidth - iSelectionSpaceOnRight - iEventMargin);
                selectorSize.iWidth = iSelectionSpaceOnRight;
                }
            }

        // Set selector position and size

        if ( borderWithin)
            {
            // this slot has a border, so shrink selector one pixel
            // from top and bottom
            displayPos.iY++;
            selectorSize.iHeight-=2;
            }

        iSelectorVisual->SetPos(displayPos, aLayoutTime);
        iSelectorVisual->SetSize( selectorSize, aLayoutTime );

        if ( iSlotStorage->IsExtraSlot( selectedSlot ) )
            {
            UpdateUntimedTextColorL( selectedSlot, ETrue );
            iPreviouslyFocusedUntimedSlot = selectedSlot;
            }
        }
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------------------------
// CBCDayContainer::UpdateEventBackgroundL
// ---------------------------------------------------------------------------
//
void CBCDayContainer::UpdateEventBackgroundL( CAlfVisual* eventVisual,
                                              TBool aFocused )
    {
    TRACE_ENTRY_POINT;
    CAlfBrushArray* brushArray = eventVisual->Brushes();

    // Get the text visual
    CAlfTextVisual& eventText = static_cast<CAlfTextVisual&>(
        static_cast<CAlfLayout*>(eventVisual)->Visual(1) );
    
    if ( aFocused )
        {
        TInt brushCount(0);
        if ( brushArray )
            {
            brushCount = brushArray->Count();
            if ( brushCount <= 1 )
                {
                // Add new background brush
                switch ( SelectorState() )
                    {
                    case ESelectorNormal:
                        {
                        iResManager.BrushManager().AppendBrushL( *eventVisual,
                                                                 EDaySelector );
                        iResManager.TextStyleManager().SetVisualStyleL( 
                                                 EDaySelectorText, &eventText );
                        }
                        break;
                    case ESelectorPressed:
                        {
                        iResManager.BrushManager().AppendBrushL( *eventVisual,
                                                          EDaySelectorPressed );
                        iResManager.TextStyleManager().SetVisualStyleL( 
                                                 EDaySelectorText, &eventText );
                        }
                        break;
                    case ESelectorOff:
                    default:
                        break; //dont set background brush
                    }
                }
            }
        }
    else
        {
        // Remove the old background, it's the last brush in the brush array
        TInt brushCount(0);
        if ( brushArray )
            {
            brushCount = brushArray->Count();
            // Remove the last background
            if ( brushCount > 1 )
                {
                brushArray->Remove( brushCount - 1 );
                }
            }

        iResManager.TextStyleManager().SetVisualStyleL( EMeetingText,
                                                        &eventText );
        }
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------------------------
// CBCDayContainer::UpdateUntimedTextColorL
// ---------------------------------------------------------------------------
//
void CBCDayContainer::UpdateUntimedTextColorL( const TInt& aSlotInd,
                                                       TBool aFocused )
    {
    TRACE_ENTRY_POINT;
    CAlfVisual& untimedSlotVisual = GetSlotVisual( aSlotInd );

    TInt alldayCount = iSlotStorage->AlldayCount();
    CAlfTextVisual& textVisual = static_cast<CAlfTextVisual&>(
        static_cast<CAlfLayout&>(untimedSlotVisual).Visual(KBCDayUntimedEventTextIndex) );

    TBCalenTextId textStyleId;
    if ( aFocused )
        {
        textStyleId = EDaySelectorText;
        }
    else
        {
        textStyleId = EFocusedDayGridText;
        }

    iResManager.TextStyleManager().SetVisualStyleL(textStyleId,
                                                      &textVisual );
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------------------------
// CBCDayContainer::ResetFocusTextsL
// ---------------------------------------------------------------------------
//
void CBCDayContainer::ResetFocusTextsL()
    {
    TRACE_ENTRY_POINT;
    if ( iPreviouslyFocusedEvent )
        {
        UpdateEventBackgroundL( iPreviouslyFocusedEvent, EFalse );
        iPreviouslyFocusedEvent = NULL;
        }

    if ( iPreviouslyFocusedUntimedSlot >= 0 )
        {
        UpdateUntimedTextColorL( iPreviouslyFocusedUntimedSlot, EFalse );
        iPreviouslyFocusedUntimedSlot = KErrNotFound;
        }
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CBCDayContainer::ClosePreviewWindow
// Closes the preview window
// -----------------------------------------------------------------------------
//
void CBCDayContainer::ClosePreviewWindow( const TBool& aAnimate )
    {
    TRACE_ENTRY_POINT;
    if ( iPreviewPopup )
        {
        iPreviewPopup->ClosePreviewPopup(aAnimate);
        }
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------------------------
// CBCDayContainer::SelectHour
// ---------------------------------------------------------------------------
//
void CBCDayContainer::SelectHour( const TInt aHour )
    {
    TRACE_ENTRY_POINT;
    iSlotStorage->SelectSlot( iSlotStorage->SlotIndexFromHour( aHour ) );
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------------------------
// CBCDayContainer::ScrollViewL
// ---------------------------------------------------------------------------
//
void CBCDayContainer::ScrollViewL( TScrollDirection aDirection,
                                           TInt aDelta )
    {
    TRACE_ENTRY_POINT;
    if ( aDelta > 0 )
        {
        TBool newTopSlot( iTopVisibleSlot );
        switch ( aDirection )
            {
            case EScrollUp:
                {
                newTopSlot -= aDelta;
                }
                break;
            case EScrollDown:
                {
                newTopSlot += aDelta;
                }
                break;
            default:
                break;
            }

        // Check the new value, are we really scrolling etc.
        if ( newTopSlot < 0 )
            {
            newTopSlot = 0;
            }
        else
            {
            if ( newTopSlot + iVisibleSlots >= KFSCalHourSlotCount + iExtraSlots )
                {
                newTopSlot = KFSCalHourSlotCount + iExtraSlots - iVisibleSlots;
                }
            }

        if ( newTopSlot != iTopVisibleSlot )
            {
            iTopVisibleSlot = newTopSlot;

            // Scroll the view
            UpdateViewportValues();

            UpdateUnvisibleEventArrowsL();

            if (!iVSMode)
                {
                UpdateSelectorL();
                }
            }
        }
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------------------------
// CBCDayContainer::UpdateScrollingL
// ---------------------------------------------------------------------------
//
void CBCDayContainer::UpdateScrollingL( TBool aInitialUpdate )
    {
    TRACE_ENTRY_POINT;

    TInt selectedSlot;
    TInt columnIndex, columns, region;
    iSlotStorage->GetSelectedSlot( selectedSlot, region, columnIndex, columns );

    TInt first = iTopVisibleSlot;
    TInt lastVisibleSlot( iTopVisibleSlot + iVisibleSlots - 1 );
    TInt last( lastVisibleSlot );

    if ( !iSlotStorage->IsExtraSlot( first ) )
        {
        first = iSlotStorage->SlotIndexFromHour(
            iSlotStorage->HourFromSlotIndex( first ) );
        }

    if ( !iSlotStorage->IsExtraSlot( last ) )
        {
        last = iSlotStorage->SlotIndexFromHour(
            iSlotStorage->HourFromSlotIndex( last ) );
        }

    if ( !iSlotStorage->IsExtraSlot( selectedSlot ) )
        {
        selectedSlot = iSlotStorage->SlotIndexFromHour(
            iSlotStorage->HourFromSlotIndex( selectedSlot ) );
        }


    if ( aInitialUpdate )
        {
        // Scroll the view so that the selected slot is on top
        TInt delta = iTopVisibleSlot - selectedSlot;
        
        if ( delta > 0 )
        	{
			ScrollViewL( EScrollUp, delta );
        	}
        else 
        	{
			ScrollViewL( EScrollDown, -delta );
        	}
        }
    else
        {
        if ( selectedSlot <= first && iTopVisibleSlot > 0 )
            {
            TInt delta = first - selectedSlot + KFSCalSlotsInHour;
            // round delta to a multiple of KFSCalSlotsInHour
            delta = KFSCalSlotsInHour * (delta / KFSCalSlotsInHour);
            ScrollViewL( EScrollUp, delta );
            }
        else if ( selectedSlot >= last &&
                  lastVisibleSlot < KFSCalHourSlotCount + iExtraSlots - 1 )
            {
            TInt delta = selectedSlot - last + KFSCalSlotsInHour;
            // round delta to a multiple of KFSCalSlotsInHour
            delta = KFSCalSlotsInHour * (delta / KFSCalSlotsInHour);
            ScrollViewL( EScrollDown, delta );
            }
        }
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------------------------
// CBCDayContainer::UpdateViewportValues
// ---------------------------------------------------------------------------
//
void CBCDayContainer::UpdateViewportValues()
    {
    TRACE_ENTRY_POINT;
    // Set viewport values:

    // Set virtual size for the viewport
    iViewSize = DisplayArea().Size();
    iWorldSize.iHeight = ( KFSCalHourSlotCount + iExtraSlots )*iSlotHeight;
    iScrollableLayout->SetVirtualSize(TAlfRealSize( iViewSize.iWidth,
            iWorldSize.iHeight ), 0 );

    // Set viewport size
    TInt viewportHeight = iViewSize.iHeight;
    if ( iControlBar )
        {
        viewportHeight -= iControlBarHeight;
        }

    iScrollableLayout->SetViewportSize(
        TAlfRealSize(  iViewSize.iWidth, viewportHeight ), 0 );

    // Initial setting of position
    if ( iTopVisibleSlot < 0 )
        {
        iTopVisibleSlot = 0;
        }
    else if ( iTopVisibleSlot + iVisibleSlots >= KFSCalHourSlotCount + iExtraSlots )
        {
        iTopVisibleSlot = KFSCalHourSlotCount + iExtraSlots - iVisibleSlots;
        }
    TReal32 startPos( iTopVisibleSlot * iSlotHeight );


    if ( startPos > iWorldSize.iHeight - viewportHeight )
        {
        startPos = iWorldSize.iHeight - viewportHeight;
        }

    SetViewPos( TAlfRealPoint( 0.0, startPos ), KLayoutTime);
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------------------------
// CBCDayContainer::::GetSlotVisual
// ---------------------------------------------------------------------------
//
CAlfVisual& CBCDayContainer::GetSlotVisual( TInt aSlotIndex )
    {
    TRACE_ENTRY_POINT;
    // slot index is a virtual index, we need to calculate the real index

    TInt extraSlotStartInd( iExtraSlotGridPos * KFSCalSlotsInHour );
    if ( iSlotStorage->IsExtraSlot( aSlotIndex ) )
        {
        return ( iExtraSlotGrid->Visual( aSlotIndex - extraSlotStartInd ));
        }

    // Slot is one of the hour slots. Return full hour slot.

    CAlfVisual* fullHourVisual(NULL);

    TInt gridIndex(0);
    if (( aSlotIndex < extraSlotStartInd ) || !iExtraSlots )
        {
        gridIndex = aSlotIndex / KFSCalSlotsInHour;
        }
    else
        {
        gridIndex = ( ( aSlotIndex - iExtraSlots ) / KFSCalSlotsInHour ) + 1;
        }

    // gridIndex might be out of bounds.
    // If so, let's restore the previous valid value
    if ( ( gridIndex < 0 ) || ( gridIndex > iGridLayout->Count() ) )
        {
        gridIndex = iPreviousGridIndex;
        }
    else
        {
        iPreviousGridIndex = gridIndex;
        }

    fullHourVisual = &( iGridLayout->Visual( gridIndex )) ;
    
    TRACE_EXIT_POINT;
    return *fullHourVisual;
    }

// -----------------------------------------------------------------------------
// CBCDayContainer::ChangeDateL
// -----------------------------------------------------------------------------
//
void CBCDayContainer::ChangeDateL( const TTimeIntervalDays& aDayChange )
    {    
    TRACE_ENTRY_POINT;

    TTime time(iDate);
    time += aDayChange;

    if(CalenDateUtils::IsValidDay(time))
        {
        SetActiveContextForFreshDayL(time);
        UpdateContainer();
        }
    else
        {
        CalenViewUtils::ShowDateOutOfRangeErrorNoteL();
        }    

    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CBCDayContainer::SetActiveContextForFreshDayL
// -----------------------------------------------------------------------------
//
void CBCDayContainer::SetActiveContextForFreshDayL(TTime& aDay)
    {
    TRACE_ENTRY_POINT;

    __ASSERT_DEBUG(CalenDateUtils::IsValidDay(aDay), User::Invariant());
    
    //newly focused day
    TDateTime date = aDay.DateTime();
    
    //maintain currently focused hour
    MCalenContext& context = iServices.Context();
    TTime currHour = context.FocusDateAndTimeL().TimeLocalL();
    
    //set focused hour
    date.SetHour( currHour.DateTime().Hour() );
    TTime dateTime( date );
    //set context to a new date and time
    TCalTime focusedTimeDate;
    focusedTimeDate.SetTimeLocalL( dateTime );
    context.SetFocusDateAndTimeL( focusedTimeDate, iView->ViewId() );
    
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CBCDayContainer::Refresh
// Redraw whole container.
// -----------------------------------------------------------------------------
//
void CBCDayContainer::Refresh()
    {
    TRACE_ENTRY_POINT;
    FSCALENDAR_PROFILE_START( FsCalendarProfile::EFsCalDayViewRefresh )

    FreezeLayout( ETrue );

    iAlfEnv.PauseRefresh();
    iAlfEnv.SetRefreshMode( EAlfRefreshModeManual );

    TRAPD( err, RefreshL() );
    if ( err != KErrNone )
        {
        CEikonEnv::Static()->HandleError( err );//codescanner::eikonenvstatic
        iAlfEnv.SetRefreshMode( EAlfRefreshModeAutomatic );
        iAlfEnv.ContinueRefresh();
        
        FreezeLayout( EFalse );
        iParentLayout->UpdateChildrenLayout();
        }
    
    FSCALENDAR_PROFILE_END( FsCalendarProfile::EFsCalDayViewRefresh )
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CBCDayContainer::GetInstanceListL
// -----------------------------------------------------------------------------
//
void CBCDayContainer::GetInstanceListL()
    {
    TRACE_ENTRY_POINT;

    iInstanceList.ResetAndDestroy();
    
    RArray<TInt> colIdArray;
    CBCViewBase::GetActiveCollectionidsL( iServices, colIdArray );
    
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

// ---------------------------------------------------------------------------
// CBCDayContainer::CreateLayoutTableL
// ---------------------------------------------------------------------------
//
void CBCDayContainer::CreateLayoutTableL()
    {
    TRACE_ENTRY_POINT;

    FSCALENDAR_PROFILE_START( FsCalendarProfile::EFsCalDayViewCreateLayoutTableL )
    
    // Create hour table
    iSlotStorage->Reset();

    // Init layout table
    delete iLayoutTable;
    iLayoutTable = NULL;
    iLayoutTable = new( ELeave )CArrayFixFlat<SCalenApptInfo>(
        KBCDayLayoutTableGranularity );

    RPointerArray<CCalCalendarInfo> calendarInfoList;
    iServices.GetAllCalendarInfoL(calendarInfoList);
    CleanupClosePushL(calendarInfoList);
    
    // Process timed
    for ( TInt i = 0; i < iInstanceList.Count(); i++ )
        {
        CCalInstance* instance = iInstanceList[i];
        if ( CalenAgendaUtils::IsTimedEntryL( instance->Entry().EntryTypeL() ) )
            {
            TTime start = instance->StartTimeL().TimeLocalL();
            TTime end = instance->EndTimeL().TimeLocalL();

            TBool onSameDay = CalenDateUtils::OnSameDay( start, end );

            // "Cut" the event to be within the current day,
            // if it expands over several days
            if ( !CalenDateUtils::OnSameDay( start, iDate ) )
                {
                start = CalenDateUtils::BeginningOfDay( iDate );
                }

            if ( !CalenDateUtils::OnSameDay( end, iDate ) )
                {
                TTimeIntervalDays oneDay(1);
                TTimeIntervalMinutes oneMinute(1);

                TTime tomorrow = (iDate + oneDay);
                end = CalenDateUtils::BeginningOfDay( tomorrow );
                end -= oneMinute;
                }

            TBool isAllDay = CBCSlotInfoStorage::IsAlldayEvent( *instance );

            HBufC* displayText = SummaryLocationCustomL( instance->Entry(),
                                                   R_QTN_CALENDAR_DAY_UNNAMED );
            TBuf<KFSCalMaxDescriptionLength> cutText;
            cutText.Copy( displayText->Left( KFSCalMaxDescriptionLength ) );
            delete displayText;

            TCalCollectionId collectionId = instance->InstanceIdL().iCollectionId;
            HBufC* calendarFileName = iServices.GetCalFileNameForCollectionId(collectionId).AllocLC();
            TInt index = calendarInfoList.Find( *calendarFileName,
                    CBCDayContainer::CalendarInfoIdentifierL );
            
            CleanupStack::PopAndDestroy(calendarFileName);
            
            if (index != KErrNotFound && calendarInfoList[index]->Enabled())
                {
                SCalenApptInfo itemInfo;
                
                itemInfo.iColor = calendarInfoList[index]->Color().Value();
                itemInfo.iId = TCalenInstanceId::CreateL( *instance );
                itemInfo.iStatus = instance->Entry().StatusL();
                itemInfo.iStartTime = start;
                itemInfo.iEndTime = end;
                itemInfo.iAllDay = isAllDay;
                itemInfo.iReplicationStatus = instance->Entry().ReplicationStatusL();
                itemInfo.iSummary = cutText;
                iLayoutTable->AppendL(itemInfo);
    
                if ( !isAllDay )
                    {
                    iSlotStorage->InsertTimedEvent( itemInfo );
                    }
                else
                    {
                    iSlotStorage->InsertAlldayEvent( itemInfo );
                    }
                }
            }
        }
    
    CleanupStack::PopAndDestroy(&calendarInfoList);
    
    FSCALENDAR_PROFILE_END( FsCalendarProfile::EFsCalDayViewCreateLayoutTableL )

    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------------------------
// CBCDayContainer::UpdateUnvisibleEventArrowsL
// Completes view population.
// ---------------------------------------------------------------------------
//
void CBCDayContainer::CompletePopulationL()
    {
    TRACE_ENTRY_POINT;

    if ( IsVisualSelectionMode() )
        {//new day appears without previous VS
        ReleaseVisualSelectionL( EFalse );
        }

    iResManager.LayoutHandler().ScreenResolutionChanged();
    FetchLayoutValues();

    UpdateStatusPaneAndExtensionsL();
    
    ConstructUntimedEventsL();
    iPhysics->InitPhysicsL( iWorldSize, iViewSize );
    
    SetHighlightFromActiveContextL();
    
    iInstanceList.ResetAndDestroy();
    
    // Layout must be up-to-date before Event visual update
    FreezeLayout( EFalse );
    iParentLayout->UpdateChildrenLayout();
    FreezeLayout( ETrue );
    
    CreateTimedEventsL();
    
    if ( iViewOpening )
        {
        UpdateScrollingL( iViewOpening );
        iViewOpening = EFalse;
        }
    
    // Layout must be up-to-date before selector update
    FreezeLayout( EFalse );
    iParentLayout->UpdateChildrenLayout();
    FreezeLayout( ETrue );
    
    TSelectorState selectorState( ESelectorOff );
    if ( IsKeyboardActive() )
        {
        selectorState = ESelectorNormal;
        }
    SetSelectorStateL( selectorState );
    UpdateSelectorL();
    UpdateUnvisibleEventArrowsL( ETrue );
    
    // View ready, enabling Alf Env again.
    // Disable is at the beginning of CFSCalenDayContainer::ContructL()
    iAlfEnv.SetRefreshMode( EAlfRefreshModeAutomatic );
    iAlfEnv.ContinueRefresh();
    
    // Propably there is no time left to show the animation,
    // but it is tried with 200ms delay
    FreezeLayout( EFalse );
    iParentLayout->UpdateChildrenLayout( KLayoutTime );
    
    iResManager.UiEnv().AlfDisplay().Roster().SetFocus( *this );
    
    // All data has been loaded, the control is ready
    // to accept input.
    ControlGroup()->SetAcceptInput( ETrue );
    
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------------------------
// CBCDayContainer::UpdateUnvisibleEventArrowsL
// ---------------------------------------------------------------------------
//
void CBCDayContainer::UpdateUnvisibleEventArrowsL(
                                                   const TBool& aForceUpdate )
    {
    TRACE_ENTRY_POINT;
    TBool displayUpArrow(EFalse);
    TBool displayDownArrow(EFalse);

    TInt earliestEndSlot = iSlotStorage->EarliestEndSlot();
    if ( earliestEndSlot != KErrNotFound &&
         earliestEndSlot <= iTopVisibleSlot )
        {
        // Up arrow
        displayUpArrow = ETrue;
        }
    TInt lastStartSlot = iSlotStorage->LastStartSlot();
    if ( lastStartSlot != KErrNotFound &&
         lastStartSlot >= iTopVisibleSlot + iVisibleSlots )
        {
        // Down arrow
        displayDownArrow = ETrue;
        }
    DisplayScrollArrowsL( displayUpArrow, displayDownArrow, aForceUpdate );
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CBCDayContainer::DisplayScrollArrowsL
// Show an arrow, if there are events up or down
// -----------------------------------------------------------------------------
//
void CBCDayContainer::DisplayScrollArrowsL( const TBool& aDisplayUp,
                                                 const TBool& aDisplayDown,
                                                 const TBool& aForceUpdate )
    {
    TRACE_ENTRY_POINT;
    iBaseLayout->SetFlags( EAlfVisualFlagFreezeLayout );
    
    if ( aForceUpdate )
        {
        //re-create the arrows
        if ( iScrollArrowUp )
            {
            iScrollArrowUp->RemoveAndDestroyAllD();
            iScrollArrowUp=NULL;
            }
        if ( iScrollArrowDown )
            {
            iScrollArrowDown->RemoveAndDestroyAllD();
            iScrollArrowDown=NULL;
            }
        ConstructHiddenEventsArrowsL();
        }
    
    TAlfTimedValue opacity;
    TInt transitionTime(0);
    TReal32 targetOpacity(0.0);
    TBool updateOpacity(EFalse);
    //show/hide Up arrow
    if ( iScrollArrowUp )
        {
        opacity = iScrollArrowUp->Opacity();
        targetOpacity = opacity.Target();
        if ( aDisplayUp && targetOpacity < 1.0 )
            {
            targetOpacity = 1.0;
            updateOpacity = ETrue;
            }
        if ( !aDisplayUp && targetOpacity > 0.0 )
            {
            targetOpacity = 0.0;
            updateOpacity = ETrue;
            }
        
        if ( updateOpacity )
            {
            //the time for full opacity's change 0<->1 is KLayoutTime
            transitionTime = opacity.TimeToTargetinMilliSeconds();
            if ( transitionTime > 0 )
                {//arrow is in the process of changing visibility
                transitionTime = KLayoutTime - transitionTime;
                if ( transitionTime < 0 )  //safety check
                    {
                    transitionTime = KLayoutTime;
                    }
                }
            else
                {
                transitionTime = KLayoutTime;
                }
            //set new opacity to arrow visual
            opacity.SetStyle(EAlfTimedValueStyleLinear);
            opacity.SetTarget( targetOpacity, transitionTime );
            iScrollArrowUp->SetOpacity( opacity );
            }
        }
    
    if ( iScrollArrowDown )
        {
        updateOpacity = EFalse;
        opacity = iScrollArrowDown->Opacity();
        targetOpacity = opacity.Target();
        if ( aDisplayDown && targetOpacity < 1.0 )
            {
            targetOpacity = 1.0;
            updateOpacity = ETrue;
            }
        if ( !aDisplayDown && targetOpacity > 0.0 )
            {
            targetOpacity = 0.0;
            updateOpacity = ETrue;
            }
        
        if ( updateOpacity )
            {
            transitionTime = opacity.TimeToTargetinMilliSeconds();
            if ( transitionTime > 0 )
                {
                transitionTime = KLayoutTime - transitionTime;
                if ( transitionTime < 0 )  //safety check
                    {
                    transitionTime = KLayoutTime;
                    }
                }
            else
                {
                transitionTime = KLayoutTime;
                }
            opacity.SetStyle(EAlfTimedValueStyleLinear);
            opacity.SetTarget( targetOpacity, transitionTime );
            iScrollArrowDown->SetOpacity( opacity );
            }
        }
 
    iBaseLayout->ClearFlags( EAlfVisualFlagFreezeLayout );
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------------------------
// CBCDayContainer::SelectFocusedTimeSlotL
// Activates the visual selection mode.
// ---------------------------------------------------------------------------
//
void CBCDayContainer::SelectFocusedTimeSlotL()
    {
    TRACE_ENTRY_POINT;
    if ( !iVSMode )
        {
        ConstructVisualSelectionL();
        }
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------------------------
// CBCDayContainer::ConstructVisualSelectionL
// ---------------------------------------------------------------------------
//
void CBCDayContainer::ConstructVisualSelectionL()
    {
    TRACE_ENTRY_POINT;
    if (!iSlotStorage)
        {
        return;
        }
    iVSMode = ETrue;
    
    TAlfTimedValue arrowOp,areaOp;
    arrowOp.SetValueNow( 0 );
    arrowOp.SetTarget( 1, KLayoutTime );
    areaOp.SetValueNow( 0 );
    areaOp.SetTarget( KVSAreaOpacity, KLayoutTime );
    
    iVisualSelectionLayout->SetFlag( EAlfVisualFlagFreezeLayout );
    
    TInt columns;
    iSlotStorage->GetSelectedSlot( iVSSelectedSlot, iVSSelectedRegion, iVSSelectedColumnIdx, columns );

    iVSStartSlot = iVSSelectedSlot;
    iVSStartSlotY = iVSStartSlot * iSlotHeight;
    iVSEndSlot = iVSStartSlot;
    iVSEndSlotY = ( iVSEndSlot + 1 ) * iSlotHeight;
    
    //Visual Selection area
    if ( !iVSVisual )
        {
        iVSVisual = CAlfImageVisual::AddNewL( *this, iVisualSelectionLayout );
        iVSVisual->SetFlag( EAlfVisualFlagManualSize );
        iVSVisual->SetFlag( EAlfVisualFlagManualPosition );
        iVSVisual->SetFlag( EAlfVisualFlagFreezeLayout );
        //move VS visual to the top - it will cover event visuals
        iEventParentLayout->MoveVisualToFront( *iVisualSelectionLayout );
        iResManager.BrushManager().AppendBrushL( *iVSVisual, EDayViewVisualSelectionBg );
        //initially hide the VS area visual
        iVSVisual->SetOpacity( areaOp );
        }
    //"New entry" text
    if ( !iVSText )
        {
        iVSText = iResManager.TextStyleManager().CreateTextVisualL(
                          EMeetingText, *this, iVisualSelectionLayout );
        iVSText->SetWrapping( CAlfTextVisual::ELineWrapTruncate );
        iVSText->SetFlag( EAlfVisualFlagManualSize );
        iVSText->SetFlag( EAlfVisualFlagManualPosition );
        iVSText->SetAlign( EAlfAlignHCenter, EAlfAlignVCenter );
        HBufC* text = NULL;
        text = StringLoader::LoadLC( R_QTN_BC_CALENDAR_DAY_NEW_ENTRY );
        iVSText->SetTextL( text->Des() );
        CleanupStack::PopAndDestroy( text );
        iVSText->SetOpacity( arrowOp );
        }

    //Visual Selection's upper handle
    if ( !iVSUpperArrow )
        {
        iVSUpperArrow = CAlfImageVisual::AddNewL( *this, iVisualSelectionLayout );
        iVSUpperArrow->EnableBrushesL( ETrue );
        iVSUpperArrow->SetFlag( EAlfVisualFlagManualSize );
        iVSUpperArrow->SetFlag( EAlfVisualFlagManualPosition );
        iVSUpperArrow->SetFlag( EAlfVisualFlagAutomaticLocaleMirroringEnabled );
        iVSUpperArrow->SetOpacity( arrowOp );
        }
    
    if ( !iVSUpperArrowTouchArea )
        {
        iVSUpperArrowTouchArea = AppendVisualL( EAlfVisualTypeVisual, 
                                                iVisualSelectionLayout );
        iVisualSelectionLayout->MoveVisualToFront(*iVSUpperArrowTouchArea);
        iVSUpperArrowTouchArea->SetFlag( EAlfVisualFlagManualSize );
        iVSUpperArrowTouchArea->SetFlag( EAlfVisualFlagManualPosition );
        iVSUpperArrowTouchArea->SetFlag( EAlfVisualFlagAutomaticLocaleMirroringEnabled );
        }
    
    //Visual Selection's lower handle
    if ( !iVSLowerArrow )
        {
        iVSLowerArrow = CAlfImageVisual::AddNewL( *this, iVisualSelectionLayout );
        iVSLowerArrow->EnableBrushesL( ETrue );
        iVSLowerArrow->SetFlag( EAlfVisualFlagManualSize );
        iVSLowerArrow->SetFlag( EAlfVisualFlagManualPosition );
        iVSLowerArrow->SetFlag( EAlfVisualFlagAutomaticLocaleMirroringEnabled );
        iVSLowerArrow->SetOpacity( arrowOp );
        }
    
    if ( !iVSLowerArrowTouchArea )
        {
        iVSLowerArrowTouchArea = AppendVisualL( EAlfVisualTypeVisual, 
                                                iVisualSelectionLayout );
        iVisualSelectionLayout->MoveVisualToFront(*iVSLowerArrowTouchArea);
        iVSLowerArrowTouchArea->SetFlag( EAlfVisualFlagManualSize );
        iVSLowerArrowTouchArea->SetFlag( EAlfVisualFlagManualPosition );
        iVSLowerArrowTouchArea->SetFlag( EAlfVisualFlagAutomaticLocaleMirroringEnabled );
        }
    
    iVSFocusOnLowerArrow = ETrue;
    //set correct brushes to arrows
    FocusVSArrowL( iVSFocusOnLowerArrow );
    
    UpdateVisualSelectionL( 0 );
    
    iVisualSelectionLayout->ClearFlag( EAlfVisualFlagFreezeLayout );
    
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------------------------
// CBCDayContainer::UpdateVisualSelectionL
// ---------------------------------------------------------------------------
//
void CBCDayContainer::UpdateVisualSelectionL()
    {
    TRACE_ENTRY_POINT;
    
    if ( !IsVisualSelectionMode() )
        {
        return;
        }
    
    //Display size has changed - update Visual Selection with new layout values
    iVSStartSlotY = iVSStartSlot * iSlotHeight;
    iVSEndSlotY = ( iVSEndSlot + 1 ) * iSlotHeight;
    
    //update "New entry" text's style
    iResManager.TextStyleManager().SetVisualStyleL( EMeetingText, iVSText );
    
    FocusVSArrowL( iVSFocusOnLowerArrow );
        
    UpdateVisualSelectionL( KLayoutTime );
    
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------------------------
// CBCDayContainer::UpdateVisualSelectionL
// ---------------------------------------------------------------------------
//
void CBCDayContainer::UpdateVisualSelectionL( TInt aTransitionTime )
    {
    TRACE_ENTRY_POINT;
    
    if (!IsVisualSelectionMode() )
        {
        return;
        }
    
    if( iVSFocusOnLowerArrow )
        {
        iSlotStorage->SelectSlot( iVSEndSlot );
        }
    else
        {
        iSlotStorage->SelectSlot( iVSStartSlot );
        }
    
    UpdateScrollingL();
//    iVisualSelectionLayout->SetFlags( EAlfVisualFlagFreezeLayout );
    
    TSize expandVSSize;
    expandVSSize.SetSize( iVSAreaWidth, iVSEndSlotY - iVSStartSlotY );

    TInt x = iEventMargin;
    TInt y = iVSStartSlotY;
    
    // Set visual selection position and size
    iVSVisual->SetPos( TPoint(x,y), aTransitionTime );
    iVSVisual->SetSize( expandVSSize, aTransitionTime );
    
    //move Upper handle
    iVSUpperArrow->SetSize( iVSUpArrowsSize );
    x = iVSUpArrowXPosition;
    y = iVSStartSlotY - iVSUpArrowsSize.iHeight / 2;
    iVSUpperArrow->SetPos( TAlfRealPoint( x, y ), aTransitionTime );
    
    TBool isMirrored(AknLayoutUtils::LayoutMirrored());
    
    TSize upperTouchAreaSize( iVSUpArrowsSize );
    upperTouchAreaSize.iHeight *= 2;
    upperTouchAreaSize.iWidth *= 1.25;
    iVSUpperArrowTouchArea->SetSize( upperTouchAreaSize );
    x = iVSUpArrowXPosition;
    y = iVSStartSlotY - upperTouchAreaSize.iHeight / 2;
    if ( !isMirrored )
        {
        x -= iVSUpArrowsSize.iWidth / 4;
        }
    iVSUpperArrowTouchArea->SetPos( TAlfRealPoint( x, y ), aTransitionTime );
    
    //move Lower handle
    iVSLowerArrow->SetSize( iVSDownArrowSize );
    x = iVSDownArrowXPosition;
    y = iVSEndSlotY - iVSDownArrowSize.iHeight / 2;
    iVSLowerArrow->SetPos( TAlfRealPoint( x, y ), aTransitionTime );
    
    TSize lowerTouchAreaSize( iVSDownArrowSize );
    lowerTouchAreaSize.iHeight *= 2;
    lowerTouchAreaSize.iWidth *= 1.25;
    iVSLowerArrowTouchArea->SetSize( lowerTouchAreaSize );
    x = iVSDownArrowXPosition;
    y = iVSEndSlotY - lowerTouchAreaSize.iHeight / 2;
    if ( isMirrored )
        {
        x -= iVSDownArrowSize.iWidth / 4;
        }
    iVSLowerArrowTouchArea->SetPos( TAlfRealPoint( x, y ), aTransitionTime );
    
    //"New entry" text
    iVSText->SetSize( TSize( iVSAreaWidth, iSlotHeight ));
    x = iEventMargin;
    y = iVSStartSlotY + ( expandVSSize.iHeight - iSlotHeight )/2;
    iVSText->SetPos( TPoint(x,y), aTransitionTime );
    
//    iVisualSelectionLayout->ClearFlags( EAlfVisualFlagFreezeLayout );
    if (!iSlotStorage->SetSelectionInRegion( iVSSelectedRegion, iVSSelectedColumnIdx, iVSSelectedSlot ))
        {
        iSlotStorage->SelectSlot( iVSSelectedSlot ); 
        }
    
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------------------------
// CBCDayContainer::ExpandVisualSelectionL
// ---------------------------------------------------------------------------
//
TBool CBCDayContainer::ExpandVisualSelectionL( TInt aDelta, TInt aMoveTime )
    {
    TRACE_ENTRY_POINT;
    TBool retVal( EFalse );
    TInt delta(0);
    delta = aDelta;
    if ( delta == 0 || !IsVisualSelectionMode() )
        {
        return retVal;
        }
    
    TBool updateVS(EFalse);
    if ( delta > 0 )
        {//expand visual selection Down
        if ( iVSFocusOnLowerArrow )
            {
            if ( iVSEndSlot >= KFSCalHourSlotCount - 1 + iExtraSlots )
                {//cant go lower
                updateVS = EFalse;
                retVal = EFalse;
                }
            else
                {
                TInt newEndSlot(iVSEndSlot);
                newEndSlot = (iVSEndSlotY - iSlotHeight + delta)/iSlotHeight;
                if ( newEndSlot >= KFSCalHourSlotCount - 1 + iExtraSlots )
                    {
                    iVSEndSlot = KFSCalHourSlotCount - 1 + iExtraSlots;
                    iVSEndSlotY = (KFSCalHourSlotCount + iExtraSlots) * iSlotHeight;
                    retVal = EFalse;
                    }
                else
                    {
                    iVSEndSlot = newEndSlot;
                    iVSEndSlotY += delta;
                    retVal = ETrue;
                    }
                updateVS = ETrue;
                }
            }
        else
            {
            if ( iVSStartSlot >= iVSEndSlot )
                {//cant go lower
                updateVS = EFalse;
                retVal = EFalse;
                }
            else
                {
                TInt newStartSlot(iVSStartSlot);
                newStartSlot = (iVSStartSlotY + delta)/iSlotHeight;
                if ( newStartSlot >= iVSEndSlot )
                    {
                    iVSStartSlot = iVSEndSlot;
                    iVSStartSlotY = iVSStartSlot * iSlotHeight;
                    retVal = EFalse;
                    }
                else
                    {
                    iVSStartSlot = newStartSlot;
                    iVSStartSlotY += delta;
                    retVal = ETrue;
                    }
                updateVS = ETrue;
                }
            }
        }
    else
        {//expand visual selection Up        
        if ( iVSFocusOnLowerArrow )
            {
            if ( iVSEndSlot <= iVSStartSlot )
                {//cant go higher
                updateVS = EFalse;
                retVal = EFalse;
                }
            else
                {//move end slot up
                TInt newEndSlot(iVSEndSlot);
                newEndSlot = (iVSEndSlotY - iSlotHeight + delta)/iSlotHeight;
                if ( newEndSlot <= iVSStartSlot )
                    {
                    iVSEndSlot = iVSStartSlot;
                    iVSEndSlotY = (iVSEndSlot + 1 ) * iSlotHeight;
                    retVal = EFalse;
                    }
                else
                    {
                    iVSEndSlot = newEndSlot;
                    iVSEndSlotY += delta;
                    retVal = ETrue;
                    }
                updateVS = ETrue;
                }
            }
        else
            {//expand higher edge up
            if ( iVSStartSlot <= 0 )
                {//cant go higher
                updateVS = EFalse;
                retVal = EFalse;
                }
            else
                {
                TInt newStartSlot(iVSStartSlot);
                newStartSlot = (iVSStartSlotY + delta)/iSlotHeight;
                if ( newStartSlot <= 0 )
                    {
                    iVSStartSlot = 0;
                    iVSStartSlotY = 0;
                    retVal = EFalse;
                    }
                else
                    {
                    iVSStartSlot = newStartSlot;
                    iVSStartSlotY += delta;
                    retVal = ETrue;
                    }

                updateVS = ETrue;
                }
            }
        }
    
    if ( updateVS )
        {
        UpdateVisualSelectionL( aMoveTime );//KLayoutTime );
        }
    TRACE_EXIT_POINT;
    return retVal;
    }

// ---------------------------------------------------------------------------
// CBCDayContainer::AlignVisualSelectionToFullSlot
// ---------------------------------------------------------------------------
//
void CBCDayContainer::AlignVisualSelectionToFullSlotL()
    {
    if ( !IsVisualSelectionMode() )
        {
        return;
        }
    TInt slotY(0), delta(0);
    slotY = iVSStartSlot * iSlotHeight;
    delta = iVSStartSlotY - slotY;
    if ( Abs(delta) >= iSlotHeight/2 )
        {
        if ( delta > 0 )
            {
            iVSStartSlot++;
            }
        else
            {
            iVSStartSlot--;
            }
        }
    iVSStartSlotY = iVSStartSlot * iSlotHeight;
    
    slotY = (iVSEndSlot + 1) * iSlotHeight;
    delta = iVSEndSlotY - slotY;
    if( Abs(delta) >= iSlotHeight/2 )
        {
        if ( delta > 0 )
            {
            iVSEndSlot++;
            }
        else
            {
            iVSEndSlot--;
            }
        }
    iVSEndSlotY = (iVSEndSlot + 1) * iSlotHeight;
    
    if ( iVSFocusOnLowerArrow )
        {
        iSlotStorage->SelectSlot( iVSEndSlot );
        }
    else
        {
        iSlotStorage->SelectSlot( iVSStartSlot );
        }
    
    UpdateVisualSelectionL( KLayoutTime );
    
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------------------------
// CBCDayContainer::HandleKeyVisualSelectionL
// ---------------------------------------------------------------------------
//
void CBCDayContainer::HandleKeyVisualSelectionL( TKeyCode aCode )
    {
//Western layout:
//EKeyUpArrow
//    -expands VS up 
//    -moves the focus to upper handle if focus was on lower handle and VS is 1 slot height
//EKeyDownArrow 
//    -expands VS down
//    -moves the focus to lower handle if focus was on upper handle and VS is 1 slot heigt
//EKeyRightArrow
//    -moves the focus to upper handle if focus was on lower handle
//    -discards VS if the focus was on upper handle
//EKeyLeftArrow
//    -moves the focus to lower handle if the focus was on upper handle
//    -discards VS if the focus was on lower handle
    
    TRACE_ENTRY_POINT;
    
    if ( !iVSMode && !(
         aCode == EKeyUpArrow ||
         aCode == EKeyDownArrow ||
         aCode == EKeyRightArrow ||
         aCode == EKeyLeftArrow) )
        {
        return;
        }
    
    TBool mirrored(AknLayoutUtils::LayoutMirrored());
    
    switch ( aCode )
    {
    case EKeyUpArrow:
        {
        if ( iVSFocusOnLowerArrow && (iVSStartSlot == iVSEndSlot))
            {
            FocusVSArrowL( EFalse );
            }
        else
            {
            ExpandVisualSelectionL( -iSlotHeight );//EFalse );
            }
        break;
        }
    case EKeyDownArrow:
        {
        if ( !iVSFocusOnLowerArrow && (iVSStartSlot == iVSEndSlot) )
            {
            FocusVSArrowL( ETrue );
            }
        else
            {
            ExpandVisualSelectionL( iSlotHeight );//ETrue );
            }
        break;
        }
    case EKeyRightArrow:
        {
        if ( mirrored )
            {
            if ( iVSFocusOnLowerArrow )
                {
                ReleaseVisualSelectionL();
                }
            else
                {
                FocusVSArrowL( ETrue );
                }
            }
        else
            {
            if ( iVSFocusOnLowerArrow )
                {
                FocusVSArrowL( EFalse );
                }
            else
                {
                ReleaseVisualSelectionL();
                }
            }
        break;
        }
    case EKeyLeftArrow:
        {
        if ( mirrored )
            {
            if ( iVSFocusOnLowerArrow )
                {
                FocusVSArrowL( EFalse );
                }
            else
                {
                ReleaseVisualSelectionL();
                }
            }
        else
            {
            if ( iVSFocusOnLowerArrow )
                {
                ReleaseVisualSelectionL();
                }
            else
                {
                FocusVSArrowL( ETrue );
                }
            }
        
        break;
        }
    }
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------------------------
// CBCDayContainer::FocusVSArrowL
// Sets a focus to specified arrow.
// ---------------------------------------------------------------------------
//
void CBCDayContainer::FocusVSArrowL( TBool aFocusDownArrow )
    {
    TRACE_ENTRY_POINT;
    if (!iVSMode)
        {
        return;
        }
    
    TInt brushCount(0);
    if ( aFocusDownArrow )
        {//focus Lower handle
        if( !iVSFocusOnLowerArrow )
            {//focus was previously on Upper handle
            iSlotStorage->SelectSlot( iVSEndSlot );
            UpdateScrollingL();
            }
        iVSFocusOnLowerArrow = ETrue;
        if ( iVSUpperArrow && iVSLowerArrow ) //safety check
            {
            //remove old brushes
            brushCount = iVSUpperArrow->Brushes()->Count();
            if ( brushCount > 0 )
                {
                iVSUpperArrow->Brushes()->Remove( brushCount - 1 );
                }
            brushCount = iVSLowerArrow->Brushes()->Count();
            if ( brushCount > 0 )
                {
                iVSLowerArrow->Brushes()->Remove( brushCount - 1 );
                }
            //set new brushes
            if ( IsSelectorEnabled() )
                {//set 'focused' brush to Lower arrow handle
                iResManager.BrushManager().AppendBrushL( *iVSLowerArrow,
                                      EDayViewVisualSelectionArrowDownSelected );
                }
            else
                {
                iResManager.BrushManager().AppendBrushL( *iVSLowerArrow,
                                             EDayViewVisualSelectionArrowDown );
                }
            //upper handle
            iResManager.BrushManager().AppendBrushL( *iVSUpperArrow,
                                               EDayViewVisualSelectionArrowUp );
            //ask toolkit to redraw VS arrows
            iVSUpperArrow->SetFlag( EAlfVisualChanged );
            iVSLowerArrow->SetFlag( EAlfVisualChanged );
            }
        }
    else
        {//focus Upper arrow handle
        if( iVSFocusOnLowerArrow )
            {//focus was previously on Down arrow.
            iSlotStorage->SelectSlot( iVSStartSlot );
            UpdateScrollingL();
            }
        iVSFocusOnLowerArrow = EFalse;
        if ( iVSUpperArrow && iVSLowerArrow ) //safety check
            {
            //remove old brushes
            brushCount = iVSUpperArrow->Brushes()->Count();
            if ( brushCount > 0 )
                {
                iVSUpperArrow->Brushes()->Remove( brushCount - 1 );
                }
            brushCount = iVSLowerArrow->Brushes()->Count();
            if ( brushCount > 0 )
                {
                iVSLowerArrow->Brushes()->Remove( brushCount - 1 );
                }
            //set new brushes
            if ( IsSelectorEnabled() )
                {//set 'focused' brush to Upper handle
                iResManager.BrushManager().AppendBrushL( *iVSUpperArrow,
                                        EDayViewVisualSelectionArrowUpSelected );
                }
            else
                {
                iResManager.BrushManager().AppendBrushL( *iVSUpperArrow,
                                               EDayViewVisualSelectionArrowUp );
                }
            iResManager.BrushManager().AppendBrushL( *iVSLowerArrow,
                                             EDayViewVisualSelectionArrowDown );
            //ask toolkit to redraw VS arrows
            iVSUpperArrow->SetFlag( EAlfVisualChanged );
            iVSLowerArrow->SetFlag( EAlfVisualChanged );
            }
        }
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------------------------
// CBCDayContainer::SetKeboardActive
// Sets whether keyboard/touch is in use.
// ---------------------------------------------------------------------------
//
void CBCDayContainer::SetKeyboardActive( TBool aActive )
    {
    TRACE_ENTRY_POINT;
    iKeyboardActive = aActive;
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------------------------
// CBCDayContainer::IsKeyboardActive
// Returns whether keyboard is in use.
// ---------------------------------------------------------------------------
//
TBool CBCDayContainer::IsKeyboardActive()
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    return iKeyboardActive;
    }

// ---------------------------------------------------------------------------
// CBCDayContainer::IsTouchActive
// Opens event. Draws pressed selector, gives haptic feedback.
// ---------------------------------------------------------------------------
//
TBool CBCDayContainer::DoSelectEventL()
    {
    TRACE_ENTRY_POINT;
    TBool eventSelected(EFalse);
    TCalenCommandId cmd( ECalenLastCommand );
    if ( IsSingleEventSelected() )
        {
        //single event selected - open the event
        cmd = ECalenEventView;
        eventSelected = ETrue;
        }
    else if ( IsMultipleEventsSelected() )
        {
        // Several todos, open To-do view
        cmd = ECalenTodoView;
        eventSelected = ETrue;
        }
    if ( eventSelected )
        {
        //haptic feedback
        InstantFeedback( ETouchFeedbackList );
        iView->HandleCommandL( cmd );
        }
    TRACE_EXIT_POINT;
    return eventSelected;
    }

// ---------------------------------------------------------------------------
// CBCDayContainer::DoLongSelectEventL
// Handles long-tap event selection. Displays stylus popup; draws selector.
// ---------------------------------------------------------------------------
//
TBool CBCDayContainer::DoLongSelectEventL( TPoint& aPoint )
    {
    TRACE_ENTRY_POINT;
    TBool handled(EFalse);
    if ( IsSingleEventSelected() && iItemActionMenu )
        {
        //init stylus popup - checks whether it has content
        if ( iItemActionMenu->InitMenuL() )
            {
            //focus should be visible when Stylus popup is On
            SetSelectorStateL( ESelectorNormal );
            //Stylus popup's position is described relative to screen
            TRect visibleArea = iResManager.UiEnv().AlfDisplay().VisibleArea();
            TPoint position( aPoint );
            position.iY += visibleArea.iTl.iY;
            iItemActionMenu->ShowMenuL( position, 0 );
            handled = ETrue;
            }
        else
            {//no items in stylus popup
            handled = EFalse;
            }
        }
    TRACE_EXIT_POINT;
    return handled;
    }

// ---------------------------------------------------------------------------
// CBCDayContainer::DoSelectVisualSelectionL
// Selects Visual Selection area. 
// ---------------------------------------------------------------------------
//
TBool CBCDayContainer::DoSelectVisualSelectionL()
    {
    TRACE_ENTRY_POINT;
    TBool vsSelected(EFalse);
    if ( IsVisualSelectionMode() )
        {
        //remove transparency
        if ( iVSVisual )
            {
            TAlfTimedValue areaOp;
            areaOp.SetTarget( 1.0, 0 );
            iVSVisual->SetOpacity( areaOp );
            }
        //haptic feedback
        InstantFeedback( ETouchFeedbackList );
        iView->HandleCommandL( ECalenNewMeeting );
        vsSelected = ETrue;
        }
    TRACE_EXIT_POINT;
    return vsSelected;
    }

// ---------------------------------------------------------------------------
// CBCDayContainer::InstantFeedback
// Gives direct haptic feedback.
// ---------------------------------------------------------------------------
//
void CBCDayContainer::InstantFeedback( TTouchLogicalFeedback aType )
    {
    TRACE_ENTRY_POINT;
    if ( iFeedback && !IsKeyboardActive() )
        {
        iFeedback->InstantFeedback( aType );
        }
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------------------------
// CBCDayContainer::ReleaseVisualSelectionL
// Releases slots visual selection.
// ---------------------------------------------------------------------------
//
void CBCDayContainer::ReleaseVisualSelectionL( TBool aFocusStartSlot )
    {
    TRACE_ENTRY_POINT;
    iVSMode = EFalse;

    iParentLayout->SetFlags( EAlfVisualFlagFreezeLayout );

    if ( iVSUpperArrow )
        {
        iVSUpperArrow->RemoveAndDestroyAllD();
        iVSUpperArrow = NULL;
        }
    if ( iVSLowerArrow )
        {
        iVSLowerArrow->RemoveAndDestroyAllD();
        iVSLowerArrow = NULL;
        }
    if ( iVSVisual )
        {
        iVSVisual->RemoveAndDestroyAllD();
        iVSVisual = NULL;
        }
    if ( iVSText )
        {
        iVSText->RemoveAndDestroyAllD();
        iVSText = NULL;
        }
    if ( iVSUpperArrowTouchArea )
        {
        iVSUpperArrowTouchArea->RemoveAndDestroyAllD();
        iVSUpperArrowTouchArea = NULL;
        }
    if( iVSLowerArrowTouchArea )
        {
        iVSLowerArrowTouchArea->RemoveAndDestroyAllD();
        iVSLowerArrowTouchArea = NULL;
        }
    
    iParentLayout->ClearFlags( EAlfVisualFlagFreezeLayout );

    if ( aFocusStartSlot && iSlotStorage )
        {
        if (!iSlotStorage->SetSelectionInRegion( iVSSelectedRegion, iVSSelectedColumnIdx, iVSSelectedSlot ))
            {
            iSlotStorage->SelectSlot( iVSSelectedSlot ); 
            }
        //reset VS state
        iVSSelectedSlotsNumber = 0;
        iVSSelectedSlot = 0;
        iVSSelectedRegion = KErrNotFound;
        iVSSelectedColumnIdx = KErrNotFound;
        UpdateSelectorL();
        }
    else
        {
        //reset VS state only
        iVSSelectedSlotsNumber = 0;
        iVSSelectedSlot = 0;
        iVSSelectedRegion = KErrNotFound;
        iVSSelectedColumnIdx = KErrNotFound;
        }
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CBCDayContainer::GetStartEndTimeL
// Gets visual selection start and end times.
// -----------------------------------------------------------------------------
//
void CBCDayContainer::GetStartEndTimeL( TCalTime& aInitialCalTime, TCalTime& aInitialCalEndTime )
    {
    TRACE_ENTRY_POINT;

    MCalenContext& ctx = iServices.Context();
    TTime vSStartTime( ctx.FocusDateAndTimeL().TimeLocalL() );
    ConvertToDate( vSStartTime );
    TTime vSEndTime( vSStartTime );

    TInt min( KBCMinutesInVSInterval * iVSStartSlot );
    TTimeIntervalMinutes minutes( min );
    vSStartTime = vSStartTime + minutes;
    min = KBCMinutesInVSInterval * ( iVSEndSlot + 1 );
    minutes = min;
    vSEndTime = vSEndTime + minutes;

    //Any memo, to-do, anniversary event    
    if ( iExtraSlots > 0 )
    	{
		TInt extraSlotStartInd( iExtraSlotGridPos * KFSCalSlotsInHour );
		if ( iVSStartSlot >= extraSlotStartInd )
			{
			TTimeIntervalMinutes extra;
			if ( iVSStartSlot >= extraSlotStartInd + iExtraSlots - 1 )
				{
				extra = iExtraSlots * KBCMinutesInVSInterval;
				}
			else
				{
				extra = (iVSStartSlot - extraSlotStartInd) * KBCMinutesInVSInterval;
				}
			vSStartTime -= extra;
			}
		if ( iVSEndSlot + 1 > extraSlotStartInd )
			{
			TTimeIntervalMinutes extra;
			if ( iVSEndSlot + 1 >  extraSlotStartInd + iExtraSlots - 1 )
				{
				extra = iExtraSlots * KBCMinutesInVSInterval;
				}
			else
				{
				extra = (iVSEndSlot + 1 - extraSlotStartInd) * KBCMinutesInVSInterval;
				}
			vSEndTime -= extra;
			}
		
		//correction for the case when only untimed events were selected with VS
		if ( vSEndTime <= vSStartTime )
			{
			vSEndTime = vSStartTime + TTimeIntervalMinutes(KBCMinutesInVSInterval);
			}
		
    	}
    aInitialCalTime.SetTimeLocalL( vSStartTime );
    aInitialCalEndTime.SetTimeLocalL( vSEndTime );
    
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------------------------
// CBCDayContainer::IsVisualSelectionMode
// Returns ETrue if in Visual Selection mode.
// ---------------------------------------------------------------------------
//
TBool CBCDayContainer::IsVisualSelectionMode()
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    return iVSMode;
    }

// ---------------------------------------------------------------------------
// CBCDayContainer::SelectedSlotsNumber
// Returns the number of selected slots in Visual Selection mode.
// ---------------------------------------------------------------------------
//
TInt CBCDayContainer::SelectedSlotsNumber()
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    return iVSSelectedSlotsNumber;
    }

// ---------------------------------------------------------------------------
// CBCDayContainer::SelectImplL
// Implements the Select command.
// ---------------------------------------------------------------------------
//
void CBCDayContainer::SelectImplL()
    {
    TRACE_ENTRY_POINT;
    SelectFocusedTimeSlotL();
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------------------------
// CBCDayContainer::BackImplL
// Implements the Back command.
// ---------------------------------------------------------------------------
//
void CBCDayContainer::BackImplL()
    {
    TRACE_ENTRY_POINT;
    if ( iVSMode )
        {
        ReleaseVisualSelectionL();
        }
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------------------------
// CBCDayContainer::::RefreshL
// ---------------------------------------------------------------------------
//
void CBCDayContainer::RefreshL()
    {
    TRACE_ENTRY_POINT;

    ControlGroup()->SetAcceptInput( EFalse );

    iAlfEnv.NotifyLayoutChangedL();

    iDate = iServices.Context().FocusDateAndTimeL().TimeLocalL();
    ConvertToDate( iDate );
    
    iView->BeginRepopulationL();

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CBCDayContainer::HandleNaviDecoratorEventL
// ?implementation_description
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CBCDayContainer::HandleNaviDecoratorEventL( TInt aEventID )
    {
    TRACE_ENTRY_POINT;
    if ( AknLayoutUtils::PenEnabled())
        {
        TInt direction(0);
        if (aEventID == MAknNaviDecoratorObserver::EAknNaviDecoratorEventLeftTabArrow)
            {
            if(AknLayoutUtils::LayoutMirrored())
                direction = 1;
            else
                direction = -1;
            ChangeDateL( TTimeIntervalDays( direction ) );
            }
            
        else if(aEventID == MAknNaviDecoratorObserver::EAknNaviDecoratorEventRightTabArrow)
            { 
            if(AknLayoutUtils::LayoutMirrored())
                 direction = -1;
            else
                 direction = 1;
            ChangeDateL( TTimeIntervalDays( direction ) );
            }
        }
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------------------------
// CBCDayContainer::FreezeLayout
// Freeze layouts so they are not updated automaticaly by the toolkit.
// ---------------------------------------------------------------------------
//
void CBCDayContainer::FreezeLayout( TBool aFreeze )
    {
    TRACE_ENTRY_POINT;
    CAlfVisual* visuals[] = {
            iParentLayout,
            iBaseLayout,
            iControlBar,
            iScrollableLayout,
            iEventParentLayout,
            iSelectorParentLayout,
            iGridParentLayout,
            iTimeGridLayout,
            iGridLayout,
            iExtraSlotParent,
            iExtraTimeSlot,
            iExtraSlotGrid,
            iSelectorVisual,
            iVSVisual,
            (CAlfVisual*)-1
        };
    
    CAlfVisual** visual = visuals;
    while ( *visual != (CAlfVisual*)-1 )
        {
        if ( *visual && aFreeze )
            {
            (*visual)->SetFlag( EAlfVisualFlagFreezeLayout );
            }
        else if ( *visual && !aFreeze )
            {
            (*visual)->ClearFlag( EAlfVisualFlagFreezeLayout );            
            }
        ++visual;
        }
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CBCDayContainer::CalendarInfoIdentifierL
// Searches for the index in calendar info list based on calendar file name
// -----------------------------------------------------------------------------
//
TBool CBCDayContainer::CalendarInfoIdentifierL(
        const HBufC* aName, const CCalCalendarInfo& aCalendarInfo)
    {
    TRACE_ENTRY_POINT;
    TBool retVal = EFalse;
    HBufC* calendarFileName = aCalendarInfo.FileNameL().AllocLC();
    retVal = calendarFileName->CompareF(*aName);
    CleanupStack::PopAndDestroy(calendarFileName);        
    TRACE_EXIT_POINT;
    return (!retVal);
    }

// ---------------------------------------------------------------------------
// CBCDayContainer::SetViewPos
// ---------------------------------------------------------------------------
//
void CBCDayContainer::SetViewPos( const TAlfRealPoint& aPos, TInt aTransitionTime )
    {
    iHorScrollPos = aPos.iX;
    iVerScrollPos = aPos.iY;
    
    iScrollableLayout->SetViewportPos( aPos, aTransitionTime );
    
    if( iPhysics )
        {
        iPhysics->SetHScrollPos( iHorScrollPos );
        iPhysics->SetVScrollPos( iVerScrollPos );
        }        
    }

// ----------------------------------------------------------------------------
// CBCDayContainer::EventHasMapLocationL
// Stores the necessary information in the context
// and returns ETrue if event has geo coordinates else EFalse
// ----------------------------------------------------------------------------
TBool CBCDayContainer::EventHasMapLocationL()
	{
	MCalenContext& context = iServices.Context();
	TCalLocalUid instanceId = context.InstanceId().iEntryLocalUid;
	
	CCalEntry* entry = iServices.EntryViewL(context.InstanceId().iColId)->FetchL(instanceId);
	CleanupStack::PushL( entry );
	CCalGeoValue* geoValue = entry->GeoValueL();
	CleanupStack::PopAndDestroy( entry );
	
	if( geoValue )
		{
		delete geoValue;
		// Event has saved map location, put "Show on Map"
		return 	ETrue;
		}
	else
		{
		return EFalse;
		}
	}
	
// ----------------------------------------------------------------------------
// CBCDayContainer::EventHasLocationTextL
// Returns ETrue if event has location text else EFalse
// ----------------------------------------------------------------------------
//
TBool CBCDayContainer::EventHasLocationTextL()
	{
	MCalenContext& context = iServices.Context();
	TCalLocalUid instanceId = context.InstanceId().iEntryLocalUid;
	
	CCalEntry* entry = iServices.EntryViewL(context.InstanceId().iColId)->FetchL(instanceId);
	CleanupStack::PushL( entry );
	TPtrC location = entry->LocationL();
	CleanupStack::PopAndDestroy( entry );
	
	if( location.Length() )
		{
		return ETrue;
		}
	else
		{
		return EFalse;
		}
	}

// ----------------------------------------------------------------------------
// CBCDayContainer::TrimAndCleanText
// 
// ----------------------------------------------------------------------------
//
void CBCDayContainer::TrimAndCleanText( TDes& aTarget )
    {
    TRACE_ENTRY_POINT;
    // Special chars cannot be removed, they need to be replaced.
    // For example: "Hi,\nHow are you?" would result in "Hi,How are you?"
    // if char is removed. See definition of KReplaceWhitespaceChars.
    AknTextUtils::ReplaceCharacters( aTarget, 
                                     KReplaceWhitespaceChars, 
                                     TChar(' ') );
    aTarget.TrimAll();
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CBCDayContainer::SummaryLocationCustomL
// 
// ----------------------------------------------------------------------------
//
HBufC* CBCDayContainer::SummaryLocationCustomL( const CCalEntry& aEntry,
                                                const TInt aResId )
    {
    TRACE_ENTRY_POINT;
    HBufC* displayText( NULL );

    // Load summary text and trim it.
    displayText = aEntry.SummaryL().AllocL();
    TPtr summaryPtr = displayText->Des();
    TrimAndCleanText( summaryPtr );
    if ( !displayText->Length() )
        {
        // summary text is empty, load location text instead.
        // But firs delete old one.
        delete displayText;
        displayText = NULL;
        
        displayText = aEntry.LocationL().AllocL();
        TPtr locationPtr = displayText->Des();
        TrimAndCleanText( locationPtr );
        if ( !displayText->Length() )
            {
            // location field is empty, load custom" text
            delete displayText;
            displayText = NULL;
            
            displayText = StringLoader::LoadL( aResId );
            }
        }

    TRACE_EXIT_POINT;
    return displayText;
    }

// ----------------------------------------------------------------------------
// CBCDayContainer::ConvertToDate
// 
// ----------------------------------------------------------------------------
//
void CBCDayContainer::ConvertToDate( TTime& aTime )
    {
    TRACE_ENTRY_POINT;
    TDateTime dateTime = aTime.DateTime();
    dateTime.SetMicroSecond( 0 );
    dateTime.SetSecond( 0 );
    dateTime.SetMinute( 0 );
    dateTime.SetHour( 0 );
    aTime = dateTime;
    TRACE_EXIT_POINT;
    }

// End of file
