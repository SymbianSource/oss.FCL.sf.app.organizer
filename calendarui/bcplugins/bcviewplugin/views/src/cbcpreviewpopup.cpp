/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Preview popup for calendar events showing more detailed
*                information about the events.
*
*/

#include "cbcpreviewpopup.h"
// Internal includes
#include "cbccontainerbase.h"
#include "cbcbrushmanager.h"
#include "cbclayouthandler.h"
#include "cbcslotinfostorage.h"
#include "cbcresourcemanager.h"
// System includes
#include <caltime.h>
#include <calinstance.h>
#include <StringLoader.h>
#include <avkon.rsg>
#include <AknUtils.h>
#include <calrrule.h>
#include "bcalencommon.hrh"
#include <bcviewplugingui.rsg>
#include "calendateutils.h"
//#include "calencommonui.h"
#include <alf/alfevent.h>
#include <alf/alfanchorlayout.h>
#include <alf/alfflowlayout.h>
#include <alf/alftextvisual.h>
#include <alf/alftextstyle.h>
#include <alf/alfdecklayout.h>
#include <alf/alfvisual.h>
#include <alf/alfcontrol.h>

namespace
{
//TODO:- Remove the global var cause of this is a dll.
// Margin information for visuals
//const TAlfMetric KBCPrevPopupNoMargin(0);
//const TAlfBoxMetric KBCPrevPopupTextVisualMargins(
//        KBCPrevPopupNoMargin,
//        KBCPrevPopupNoMargin,
//        KBCPrevPopupNoMargin,
//        KBCPrevPopupNoMargin );

// Popup margin from display edge at left and right side of popup (horizontal)
const TInt KBCPrevPopupHMarginOutsidePopup = 10;

// Line count for visuals
const TInt KBCPrevPopupMaxLineCount = 1;

// This value is used for placing the popup a little bit lower
// than the focused event visual.
const TInt KBCPrevPopupVerticalOffsetFromVisual = 5;

// The preview popup is moved down this much from the upper edge
// of the screen, if possible, to avoid overlap with the arrow
// for indicating events.
const TInt KBCPrevPopupVerticalExtraMargin = 20;

// Animation and delay information
const TInt KBCPrevPopupFadeInTime = 250; // in ms
const TInt KBCPrevPopupFadeOutTime = 250; // in ms
const TInt KBCPrevPopupTextFadeOutTime = 200; // in ms
const TInt KBCPrevPopupDelayBeforeOpen = 1000; // in ms, must be > fade in/out
const TInt KBCPrevPopupDelayAfterOpen = 8000 + KBCPrevPopupFadeInTime; // in ms

// One day in minutes. 24*60
const TInt KBCPrevPopupOneDayInMinutes = 1440;

// Shadow compensation
const TInt KShadowCompensation( 2 );

//TODO:- pls delete after nokia provide the function
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

//TODO:- pls delete after nokia provide the function
void TrimAndCleanText( TDes& aTarget )
    {
    // Special chars cannot be removed, they need to be replaced.
    // For example: "Hi,\nHow are you?" would result in "Hi,How are you?"
    // if char is removed. See definition of KReplaceWhitespaceChars.
    AknTextUtils::ReplaceCharacters( aTarget, 
                                     KReplaceWhitespaceChars, 
                                     TChar(' ') );
    aTarget.TrimAll();
    }
} // unnamed namespace

// ---------------------------------------------------------------------------
// CFSCalenPreviewPopup::NewL
// ---------------------------------------------------------------------------
//
CBCPreviewPopup* CBCPreviewPopup::NewL(
        CAlfControl* aOwner,
        CAlfLayout* aParentLayout,
        CAlfLayout* aViewportLayout,
        MBCResourceManager& aResManager,
        MBCPreviewPopupClient* aClient )
    {
    CBCPreviewPopup* self = CBCPreviewPopup::NewLC( aOwner,
                                                              aParentLayout,
                                                              aViewportLayout,
                                                              aResManager,
                                                              aClient );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CBCPreviewPopup::NewLC
// ---------------------------------------------------------------------------
//
CBCPreviewPopup* CBCPreviewPopup::NewLC(
        CAlfControl* aOwner,
        CAlfLayout* aParentLayout,
        CAlfLayout* aViewportLayout,
        MBCResourceManager& aResManager,
        MBCPreviewPopupClient* aClient )
    {
    CBCPreviewPopup* self = new (ELeave) CBCPreviewPopup(
                                                             aOwner,
                                                             aParentLayout,
                                                             aViewportLayout,
                                                             aResManager,
                                                             aClient);
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// ---------------------------------------------------------------------------
// CBCPreviewPopup::CBCPreviewPopup
// ---------------------------------------------------------------------------
//
CBCPreviewPopup::CBCPreviewPopup(
        CAlfControl* aOwner,
        CAlfLayout* aParentLayout,
        CAlfLayout* aViewportLayout,
        MBCResourceManager& aResManager,
        MBCPreviewPopupClient* aClient )
: iOwner                (aOwner),
  iParentLayout         (aParentLayout),
  iViewportLayout       (aViewportLayout),
  iResManager           (aResManager),
  iClient               (aClient),
  iPopupState           (EPopupDeleted)
    {
    // nothing to do here
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CBCPreviewPopup::~CBCPreviewPopup()
    {
    if ( iOwner )
        {
        iOwner->Env().CancelCustomCommands( this );
        iOwner->Env().CancelCommands ( this );
        }
    DeletePreviewPopup();
    if ( iCalInstance )
        {
        delete iCalInstance;
        }
    }

// ---------------------------------------------------------------------------
// 2nd phase constructor
// ---------------------------------------------------------------------------
//
void CBCPreviewPopup::ConstructL()
    {
    // nothing to construct here
    }

// ---------------------------------------------------------------------------
// Shows preview popup.
// Uses a delay before opening a popup. Saves the input data in order to use
// it after the delay.
// ---------------------------------------------------------------------------
//
void CBCPreviewPopup::ShowPreviewPopupL( CCalInstance* aCalInstance,
                                              CAlfVisual* aFocusedVisual,
                                              CAlfVisual* aFocusedSlot,
                                              const TBool aIsAllDayEventInUI )
    {
    // Save new input data in order to show the new focused event.
    // Old information is needed and such information is saved in
    // member variable iAnimInfo.

    if ( iCalInstance )
        {
        delete iCalInstance;
        iCalInstance = NULL;
        }
    iCalInstance = aCalInstance;

    iNextFocusedVisual = aFocusedVisual;
    iNextFocusedIsAllDayEvent = aIsAllDayEventInUI;
    iNextFocusedSlot = aFocusedSlot;

    // Cancel previous "show preview popup / open" command and send a new one.
    // i.e. start another timer.
    iOwner->Env().CancelCustomCommands( this, EOpenPopup );

    iOwner->Env().Send( TAlfCustomEventCommand( EOpenPopup, this ),
                       KBCPrevPopupDelayBeforeOpen );
    }

// ---------------------------------------------------------------------------
// Notify the preview popup that a new slot has been focused, within
// the currently selected event.
// ---------------------------------------------------------------------------
//
void CBCPreviewPopup::UpdateFocusedSlotL( CAlfVisual* aFocusedSlot )
    {
    iNextFocusedSlot = aFocusedSlot;
    }

// ---------------------------------------------------------------------------
// Closes the preview popup. Uses animation if requested.
// ---------------------------------------------------------------------------
//
void CBCPreviewPopup::ClosePreviewPopup( TBool aAnimate )
    {
      if ( !aAnimate )
        {
        iOwner->Env().CancelCustomCommands( this );
        DeletePreviewPopup();
        return;
        }

    switch ( iPopupState )
        {
        case EPopupDeleted:
            {
            iOwner->Env().CancelCustomCommands(this,
                                        EOpenPopup );
            break;
            }
        case EPopupOpening: // fall through to EPopupOpen
        case EPopupOpen:
            {
            // Closing timer can be shut off.
            iOwner->Env().CancelCustomCommands(this,
                                        EClosePopup);
            // fade out text
            TAlfTimedValue opacity;
            opacity.SetTarget(0.0,KBCPrevPopupTextFadeOutTime);
            iPopupLines->SetOpacity(opacity);

            // animate popup out after text has been faded out
            iOwner->Env().Send(
                    TAlfCustomEventCommand( EAnimateOutPopup, this ),
                    KBCPrevPopupTextFadeOutTime
                    );
            iPopupState = EPopupClosing;
            break;
            }
        case EPopupClosing:
            {
            iOwner->Env().CancelCustomCommands( this );
            break;
            }
        default:
            {
            break;
            }
        }
    }

// ---------------------------------------------------------------------------
// Handles events that have been originally sent by preview popup itself.
// Used for delays.
// ---------------------------------------------------------------------------
//
TBool CBCPreviewPopup::OfferEventL(const TAlfEvent& aEvent)
    {
    if ( aEvent.IsCustomEvent() )
        {
        switch ( aEvent.CustomParameter() )
            {
            case EOpenPopup:
                { // open popup event received

                // In some cases open popup event is triggered before
                // delete, so cancel delete command.
                iOwner->Env().CancelCustomCommands(this,
                                         EDeletePopup );

                // First delete the old one
                // if it exists and then construct a new one.
                if ( iPreviewPopup )
                    {
                    // if preview popup still exists, delete it.
                    DeletePreviewPopup();
                    }
                ConstructPreviewPopupL( iCalInstance->Entry() );

                iPopupState = EPopupOpen;
                iOwner->Env().Send( TAlfCustomEventCommand( EClosePopup, this ),
                        KBCPrevPopupDelayAfterOpen );
                return ETrue;
                }
            case EDeletePopup:
                {
                if ( iPopupState == EPopupClosing )
                    {
                    // Delete event received. Delete popup.
                    DeletePreviewPopup();
                    }
                return ETrue;
                }
            case EClosePopup:
                {
                // Popup has been open long enough and timer to close the popup has
                // been triggered. Close popup with animation.
                ClosePreviewPopup( ETrue );
                return ETrue;
                }
            case EAnimateOutPopup:
                {
                // Text has been faded out, popup can be animated out.
                AnimateOut();
                iOwner->Env().Send( TAlfCustomEventCommand( EDeletePopup, this ),
                                KBCPrevPopupFadeOutTime );
                return ETrue;
                }
            default:
                {
                return EFalse;
                }
            }
        }
    return EFalse;
    }

// ---------------------------------------------------------------------------
// Constructs preview popup.
// Creates only text visuals that will have some content.
// ---------------------------------------------------------------------------
//
void CBCPreviewPopup::ConstructPreviewPopupL( const CCalEntry& aEntry )
    {
    iPreviewPopup = CAlfDeckLayout::AddNewL( *iOwner, iParentLayout );
    iPreviewPopup->SetFlag( EAlfVisualFlagManualLayout );
    iPreviewPopup->SetOrigin( EAlfVisualHOriginCenter, EAlfVisualVOriginCenter );

    //Temporal size to get the real text visuals size
    //Set to 0 after all text visuals are constructed
    TSize size = iOwner->DisplayArea().Size();
    iPreviewPopup->SetSize( size, 0 );

    iPopupShadow = iOwner->AppendVisualL( EAlfVisualTypeVisual, iPreviewPopup );
    iPopupShadow->SetFlag( EAlfVisualFlagAutomaticLocaleMirroringEnabled );
    iResManager.BrushManager().AppendBrushL( *iPopupShadow, EBorderShadow );
    iPopupShadow->SetFlag(EAlfVisualFlagManualLayout);
    iPopupShadow->SetPos( TPoint( 1, 1 ) );

    iPopup = iOwner->AppendVisualL( EAlfVisualTypeVisual, iPreviewPopup );
    iPopup->SetFlag(EAlfVisualFlagManualLayout);
    iPopup->SetPos(TPoint(0, 0));//needs to be set manually. doesn't regignize parent padding

    iAnimInfo.iIsAllDayEvent = iNextFocusedIsAllDayEvent;

    iPopupLines = CAlfFlowLayout::AddNewL( *iOwner, iPreviewPopup );
    iPopupLines->SetFlowDirection(CAlfFlowLayout::EFlowVertical);
    iPopupLines->SetMode( CAlfFlowLayout::EModeFitPerpendicular );

    // Construct text visuals
    CreateVisualsL( aEntry );

    // Set the initial size to 0, to get correct results from
    // DisplayToLocal later on.
    iPreviewPopup->SetSize( TSize( 0, 0 ), 0 );

    SetBackgroundL( aEntry );//choose and set previewbg brush

    //
    // Initialise animation data
    //

    iAnimInfo.iTargetSize = CalculateLayout();
    iAnimInfo.iFocusedVisual = iNextFocusedVisual;

    TRect eventRect = iNextFocusedVisual->DisplayRectTarget();
    TRect viewportRect = iViewportLayout->DisplayRectTarget();

    // Find the center X coordinate for the popup.
    iAnimInfo.iTargetRealPos.iX = viewportRect.Center().iX;

    // Crop the event rect to the part visible on the screen currently
    eventRect.Intersection( viewportRect );

    // Avoid placing the event directly next to the top edge, if possible
    if ( eventRect.iTl.iY < viewportRect.iTl.iY + KBCPrevPopupVerticalExtraMargin &&
         eventRect.iBr.iY > viewportRect.iTl.iY + KBCPrevPopupVerticalExtraMargin )
        {
        eventRect.iTl.iY = viewportRect.iTl.iY + KBCPrevPopupVerticalExtraMargin;
        }

    if ( iAnimInfo.iIsAllDayEvent )
        {
        // The x coordinate is taken from the event (that is,
        // the correct column center)
        iAnimInfo.iSourceRealPos.iX = eventRect.Center().iX;
        // The y coordinate is taken from the slot visual
        // (that is, the virtual focus within the event)
        TRect slotRect = iNextFocusedSlot->DisplayRectTarget();
        iAnimInfo.iSourceRealPos.iY = slotRect.Center().iY;

        iAnimInfo.iTargetRealPos.iY = iAnimInfo.iSourceRealPos.iY;
        }
    else
        {
        // The start pos is the center of the event
        iAnimInfo.iSourceRealPos = eventRect.Center();
        // Show the popup slightly below the top edge of the event
        iAnimInfo.iTargetRealPos.iY = eventRect.iTl.iY
                    + KBCPrevPopupVerticalOffsetFromVisual
                    + iAnimInfo.iTargetSize.iHeight/2;
        }

    // Check that preview popup fits vertically into the visible area
    // of the display
    iPreviewPopup->UpdateChildrenLayout();
    TInt displayEndYCoord   (viewportRect.iBr.iY - KBCPrevPopupVerticalOffsetFromVisual);
    TInt displayStartYCoord (viewportRect.iTl.iY + KBCPrevPopupVerticalOffsetFromVisual);
    TInt startYCoord (iAnimInfo.iTargetRealPos.iY - iAnimInfo.iTargetSize.iHeight/2);
    TInt endYCoord   (iAnimInfo.iTargetRealPos.iY + iAnimInfo.iTargetSize.iHeight/2);
    if ( displayEndYCoord < endYCoord )
        {
        // Preview popup doesn't fit on screen --> Adjust popup position.
        iAnimInfo.iTargetRealPos.iY = displayEndYCoord;
        iAnimInfo.iTargetRealPos.iY -= iAnimInfo.iTargetSize.iHeight/2;
        }
    else if ( startYCoord < displayStartYCoord )
        {
        // Preview popup doesn't fit on screen --> Adjust popup position.
        iAnimInfo.iTargetRealPos.iY = displayStartYCoord;
        iAnimInfo.iTargetRealPos.iY += iAnimInfo.iTargetSize.iHeight/2;
        }


    // Translate display coordinates and positions into local coordinates.
    iAnimInfo.iSourceRealPos = iPreviewPopup->DisplayToLocal( iAnimInfo.iSourceRealPos );
    iAnimInfo.iTargetRealPos = iPreviewPopup->DisplayToLocal( iAnimInfo.iTargetRealPos );

    // Define animation.
    AnimateIn();

    iPreviewPopup->UpdateChildrenLayout();

    iPopupState = EPopupOpening;
    }

// ---------------------------------------------------------------------------
// Creates the necessary visuals for the preview popup.
// ---------------------------------------------------------------------------
//
void CBCPreviewPopup::CreateVisualsL( const CCalEntry& aEntry )
    {
    //
    // Create and initialise Time visual
    //
    CCalEntry::TType entryType = aEntry.EntryTypeL();
    if ( entryType == CCalEntry::EAppt ||
         entryType == CCalEntry::EEvent )
        {
        CreateTimeVisualL( aEntry, iPopupLines );

        CreateDateVisualL( aEntry, iPopupLines );
        }

    //
    // Create and initialise Summary visual
    //
    CreateSummaryVisualL( aEntry, iPopupLines );

    //
    // Create and initialise Location visual
    //
    CreateLocationVisualL( aEntry, iPopupLines );

    //
    // Create and initialise Description visual
    //
    CreateDescriptionVisualL( aEntry, iPopupLines );
    }

// ---------------------------------------------------------------------------
// Sets the background for preview popup, according to the status of the entry
// ---------------------------------------------------------------------------
//
void CBCPreviewPopup::SetBackgroundL( const CCalEntry& /*aEntry*/ )
    {
    TBCalendarBrushId brushId( EMeetingBg );
    iResManager.BrushManager().AppendBrushL( *iPopup, brushId );
    }

// ---------------------------------------------------------------------------
// Creates the summary (title) visual for the event.
// ---------------------------------------------------------------------------
//
void CBCPreviewPopup::CreateSummaryVisualL(
                                const CCalEntry& aEntry,
                                CAlfLayout* aLayout )
    {
    HBufC* summaryText = aEntry.SummaryL().AllocL();
    TPtr ptr( summaryText->Des() );
    
    //TODO:- displace
    //CCalenCommonUI::TrimAndCleanText( ptr );
    //TODO:- displace with CCalenCommonUI::TrimAndCleanText( ptr )
    // after nokia provide same function
    TrimAndCleanText(ptr);

    if ( !summaryText->Length() )
        {
        // meeting does not have a name. Use text "unnamed".
        delete summaryText;
        summaryText = NULL;
        summaryText = StringLoader::LoadL(R_QTN_CALENDAR_DAY_EVENT_UNNAMED);
        }
    CleanupStack::PushL( summaryText );

    iSummaryVisual = CAlfTextVisual::AddNewL( *iOwner, aLayout );

    iSummaryVisual->SetTextL( *summaryText );
    iSummaryVisualSize = SetTextVisualStyleL( iSummaryVisual );

    CleanupStack::PopAndDestroy( summaryText );
    }

// ---------------------------------------------------------------------------
// Creates flow layout which will contain time text visual and icons.
// Format of the string is fetched from resource.
// ---------------------------------------------------------------------------
//
void CBCPreviewPopup::CreateTimeVisualL(
                                const CCalEntry& aEntry,
                                CAlfLayout* aLayout )
    {
    //
    // Create a flow layout which will contain text and icons.
    //
    iTimeAndIconsLayout = CAlfFlowLayout::AddNewL(
                                *iOwner,
                                aLayout);
    iTimeAndIconsLayout->SetMode( CAlfFlowLayout::EModeFitPerpendicular );
    iTimeAndIconsLayout->SetFlowDirection(CAlfFlowLayout::EFlowHorizontal);
    iTimeAndIconsLayout->SetFlag( EAlfVisualFlagAutomaticLocaleMirroringEnabled );

    iTimeVisual = CAlfTextVisual::AddNewL( *iOwner, iTimeAndIconsLayout );

    TTime startTime = iCalInstance->StartTimeL().TimeLocalL();
    TTime endTime = iCalInstance->EndTimeL().TimeLocalL();

    if ( IsAllDayEvent( startTime, endTime) )
        {
        // in case of an all day event set text "all day event" to the visual
        HBufC* allDayText
            = StringLoader::LoadLC( R_QTN_CALENDAR_ALL_DAY_EXPANDED );

        iTimeVisual->SetTextL( *allDayText );

        iTimeVisualSize = SetTextVisualStyleL( iTimeVisual );

        CleanupStack::PopAndDestroy( allDayText );
        }
    else
        {
        TLocale locale;
        TBuf<KTimeUsualWithZeroLength> startBuf, endBuf;

        CDesCArrayFlat* timeArray = new (ELeave) CDesCArrayFlat(2);
        CleanupStack::PushL( timeArray );

        // Create and format start and end time of meeting
        HBufC* timeFormat = StringLoader::LoadLC( R_QTN_TIME_USUAL );
        startTime.FormatL( startBuf, *timeFormat );
        endTime.FormatL( endBuf, *timeFormat );
        CleanupStack::PopAndDestroy( timeFormat );

        // insert texts to an array and use them for loading
        // the formatted time period text
        timeArray->AppendL( startBuf );
        timeArray->AppendL( endBuf );
        HBufC* timeText = StringLoader::LoadLC( R_QTN_CALENDAR_TIME_SEPARATOR,
                                                *timeArray );
        TPtr visualText( timeText->Des() );
        AknTextUtils::LanguageSpecificNumberConversion( visualText );

        iTimeVisual->SetTextL( visualText );

        iTimeVisualSize = SetTextVisualStyleL( iTimeVisual );

        CleanupStack::PopAndDestroy( timeText );
        CleanupStack::PopAndDestroy( timeArray );
        }

    CBCLayoutHandler& layoutHandler = iResManager.LayoutHandler();

    // Add extra space for text visual.
    TInt spaceBetweenIconAndText(0);
    TInt iconPadding(0);
    layoutHandler.GetPreviewPopupTimePadding(
            spaceBetweenIconAndText,
            iconPadding );

    iTimeVisualSize.iWidth += ( spaceBetweenIconAndText - iconPadding );
    iTimeVisual->SetSize( iTimeVisualSize );
    iTimeAndIconsLayout->SetInnerPadding(
           TPoint(iconPadding, 0) );

    // Fetch size for icons.
    TSize iconSize = layoutHandler.EventIconSize();

    //
    // Add priority icon
    //
    TUint priority = aEntry.PriorityL();
    TBCalendarBrushId brushId(ECalendarNullBrush);

    switch ( priority )
        {
        case EBCalenMRPriorityHigh:
            {
            brushId = EPreviewPopupPriorityHigh;
            break;
            }
        case EBCalenMRPriorityLow:
            {
            brushId = EPreviewPopupPriorityLow;
            break;
            }
        default:
            {
            break;
            }
        }
    if ( brushId != ECalendarNullBrush )
        {
        CAlfVisual* priorityIcon =
            iOwner->AppendVisualL(EAlfVisualTypeVisual,iTimeAndIconsLayout);
        priorityIcon->SetSize( iconSize );
        iResManager.BrushManager().AppendBrushL( *priorityIcon, brushId );
        }

    //
    // Add alarm icon
    //
    CCalAlarm* alarmData = aEntry.AlarmL();
    CleanupStack::PushL( alarmData );
    if ( alarmData )
        {
        // entry has alarm data
        CAlfVisual* alarm = iOwner->AppendVisualL(EAlfVisualTypeVisual,iTimeAndIconsLayout);
        alarm->SetSize( iconSize );
        iResManager.BrushManager().AppendBrushL( *alarm,
                                                  EPreviewPopupAlarm );
        }
    CleanupStack::PopAndDestroy( alarmData );

    //
    // Add recurrence icon
    //

    // Resolve if recurrence icon is needed
    if ( IsRecurringEventL( aEntry ) )
        {
        // Entry has a rule, create an icon
        CAlfVisual* recurrence = iOwner->AppendVisualL( EAlfVisualTypeVisual,
                                                        iTimeAndIconsLayout);
        recurrence->SetSize( iconSize );
        iResManager.BrushManager().AppendBrushL( *recurrence,
                                                 EPreviewPopupRecurrence );
        }    
    else if( IsRecurringExceptionEventL( aEntry ))
        {
        // Entry is an exception, create an icon
        CAlfVisual* exception = iOwner->AppendVisualL( EAlfVisualTypeVisual, 
                                                       iTimeAndIconsLayout );
        exception->SetSize( iconSize );
        iResManager.BrushManager().AppendBrushL( *exception,
                                            EPreviewPopupRecurrenceException );
        }
    }

// ---------------------------------------------------------------------------
// CBCPreviewPopup::CreateDateVisualL
// Creates date visual that shows date information.
// Date visual is shown in following cases:
// - An all day event that lasts at least two days (starts at 12:00 am
//   and ends at 12:00 pm, total of 48 hours or more in 24 hour periods)
// - Event starts later than 12:00 am and lasts at least 24 hours
//   (i.e. start date != end date   AND   start time > 12am)
// ---------------------------------------------------------------------------
//
void CBCPreviewPopup::CreateDateVisualL(
        const CCalEntry& /*aEntry*/,
        CAlfLayout* aLayout )
    {
    TTime startTime = iCalInstance->StartTimeL().TimeLocalL();
    TTime endTime = iCalInstance->EndTimeL().TimeLocalL();

    TTimeIntervalMinutes durationInMin;
    endTime.MinutesFrom(startTime, durationInMin);

    // Initialise the "beginning of day" TTime object.
    TTime beginOfDay = CalenDateUtils::BeginningOfDay(startTime);

    // Check if meeting is an all day event and it lasts at least two days.
    TBool isTwoDayEventOrMore =
            IsAllDayEvent(startTime, endTime) &&
            durationInMin.Int() >= KBCPrevPopupOneDayInMinutes*2;

    // Check if event spans at least two days but is not an all-day event.
    TBool spansTwoDaysOrMore =
            !IsAllDayEvent( startTime, endTime) &&
            !CalenDateUtils::OnSameDay( startTime, endTime );

    if ( isTwoDayEventOrMore || spansTwoDaysOrMore  )
        {
        iDateVisual = CAlfTextVisual::AddNewL( *iOwner, aLayout );

        // Adjust the end time by taking one minute off to get the correct date
        // in case of atleast two-day events
        if ( isTwoDayEventOrMore )
            {
            endTime -= TTimeIntervalMinutes(1);
            }

        TBuf<KDateUsualWithZeroLength> startBuf, endBuf;

        CDesCArrayFlat* dateArray = new (ELeave) CDesCArrayFlat(2);
        CleanupStack::PushL( dateArray );

        // Create and format start and end time of meeting
        HBufC* dateFormat = StringLoader::LoadLC( R_QTN_DATE_USUAL );
        startTime.FormatL( startBuf, *dateFormat );
        endTime.FormatL( endBuf, *dateFormat );
        CleanupStack::PopAndDestroy( dateFormat );

        // insert texts to an array and use them for loading
        // the formatted time period text
        dateArray->AppendL( startBuf );
        dateArray->AppendL( endBuf );
        HBufC* dateText = StringLoader::LoadLC( R_QTN_CALENDAR_TIME_SEPARATOR,
                                                *dateArray );
        TPtr visualText( dateText->Des() );
        AknTextUtils::LanguageSpecificNumberConversion( visualText );

        iDateVisual->SetTextL(visualText);
        iDateVisualSize = SetTextVisualStyleL( iDateVisual );

        CleanupStack::PopAndDestroy( dateText );
        CleanupStack::PopAndDestroy( dateArray );
        }
    }

// ---------------------------------------------------------------------------
// Creates location visual.
// ---------------------------------------------------------------------------
//
void CBCPreviewPopup::CreateLocationVisualL(
                                const CCalEntry& aEntry,
                                CAlfLayout* aLayout )
    {
    HBufC* locationText = aEntry.LocationL().AllocLC();
    if ( locationText->Length() )
        {
        // text has more than zero characters.
        TPtr locationTextPtr = locationText->Des();
        
        //TODO:- displace
        //CCalenCommonUI::TrimAndCleanText( locationTextPtr );
        //TODO:- displace with CCalenCommonUI::TrimAndCleanText(  )
        // after nokia provide same function
        TrimAndCleanText( locationTextPtr );
        
        if ( locationText->Length() )
            {
            // text has "real text" (=other than just plain space chars)
            iLocationVisual = CAlfTextVisual::AddNewL( *iOwner, aLayout );

            HBufC* locationIndicator =
            StringLoader::LoadLC( R_QTN_CALENDAR_LOCATION_PARENTHESIS );

            //TODO see if this can be fixed in reasonable time
            // Now insert the location text between
            // the parenthesis (or whatever the indicator is)
            HBufC* formattedLocationText = HBufC::NewLC(
                                            locationIndicator->Length() +
                                            locationText->Length() );

            TPtr ptr = formattedLocationText->Des();
            StringLoader::Format( ptr, *locationIndicator, -1, *locationText );

            iLocationVisual->SetTextL( *formattedLocationText );
            iLocationVisualSize = SetTextVisualStyleL( iLocationVisual );

            CleanupStack::PopAndDestroy( formattedLocationText );
            CleanupStack::PopAndDestroy( locationIndicator );

            }
        }
    CleanupStack::PopAndDestroy( locationText );
    }

// ---------------------------------------------------------------------------
// Creates description visual.
// ---------------------------------------------------------------------------
//
void CBCPreviewPopup::CreateDescriptionVisualL(
                                const CCalEntry& aEntry,
                                CAlfLayout* aLayout )
    {
    HBufC* description = aEntry.DescriptionL().AllocLC();
    if ( description && description->Length() )
        {
        TPtr ptr = description->Des();
        
        //TODO:- displace
        //CCalenCommonUI::TrimAndCleanText( ptr );
        //TODO:- displace with CCalenCommonUI::TrimAndCleanText(  )
        // after nokia provide same function        
        TrimAndCleanText( ptr );
        
        if ( ptr.Length() )
            {
            iDescriptionVisual = CAlfTextVisual::AddNewL( *iOwner, aLayout );
            iDescriptionVisual->SetTextL( ptr );
            iDescriptionVisualSize = SetTextVisualStyleL( iDescriptionVisual );
            }
        }
    CleanupStack::PopAndDestroy( description );
    }

// ---------------------------------------------------------------------------
// Deletes the preview popup by freeing resources.
// ---------------------------------------------------------------------------
//
void CBCPreviewPopup::DeletePreviewPopup()
    {
    if ( iPreviewPopup )
        {
        if ( iOwner )
            {
            // Cancel animate out event in case preview popup is deleted before
            // the animation is done.
            iOwner->Env().CancelCustomCommands( this );
            }
        iPreviewPopup->RemoveAndDestroyAllD();
        iPreviewPopup = NULL;

        iSummaryVisual = NULL;
        iTimeVisual = NULL;
        iLocationVisual = NULL;
        iDescriptionVisual = NULL;
        iDateVisual = NULL;

        iPopupState = EPopupDeleted;
        }
    }

// ---------------------------------------------------------------------------
// Animates the preview popup in of the display.
// ---------------------------------------------------------------------------
//
void CBCPreviewPopup::AnimateIn()
    {
    TAlfTimedValue opacity;

    opacity.SetValueNow(0.0);
    opacity.SetTarget(1.0, KBCPrevPopupFadeInTime);

    // Text should be brought in after popup is animated in.
    iPopupLines->SetOpacity(opacity);

    iPopup->SetOpacity(opacity);
    iPopupShadow->SetOpacity(opacity);

    iPreviewPopup->SetPos( iAnimInfo.iSourceRealPos, 0 );
    iPreviewPopup->SetPos( iAnimInfo.iTargetRealPos, KBCPrevPopupFadeInTime );

    iPreviewPopup->SetSize( TAlfRealSize(0,0), 0 );
    iPreviewPopup->SetSize( iAnimInfo.iTargetSize, KBCPrevPopupFadeInTime );

    iPopup->SetSize( TAlfRealSize(0,0), 0 );
    iPopup->SetSize( iAnimInfo.iTargetSize, KBCPrevPopupFadeInTime );

    iPopupShadow->SetSize( TAlfRealSize(0,0), 0 );

    TSize compensate(iAnimInfo.iTargetSize);
    compensate.iHeight -= KShadowCompensation;
    compensate.iWidth -= KShadowCompensation;

    iPopupShadow->SetSize( compensate, KBCPrevPopupFadeInTime );
    }

// ---------------------------------------------------------------------------
// Animates the preview popup out of the display.
// ---------------------------------------------------------------------------
//
void CBCPreviewPopup::AnimateOut()
    {

    if ( !iAnimInfo.iIsAllDayEvent )
        {
        // Since the original event may have scrolled, but the popup is floating
        // on top, recalculate the end pos (stored in iSourceRealPos)

        TRect eventRect = iAnimInfo.iFocusedVisual->DisplayRectTarget();
        TRect viewportRect = iViewportLayout->DisplayRectTarget();

        // Crop the event rect to the part visible on the screen currently
        eventRect.Intersection( viewportRect );

        // The start pos (now used as end pos) is the center of the event
        iAnimInfo.iSourceRealPos = eventRect.Center();
        }

    TAlfTimedValue opacity;
    opacity.SetTarget(0.0, KBCPrevPopupFadeOutTime);
    iPopup->SetOpacity(opacity);
    iPopupShadow->SetOpacity(opacity);

    iPreviewPopup->SetSize( TAlfRealSize(0,0), KBCPrevPopupFadeOutTime );
    iPreviewPopup->SetPos( iAnimInfo.iSourceRealPos, KBCPrevPopupFadeOutTime );

    //TODO shadow out
    iPopupShadow->SetSize( TAlfRealSize(0,0), KBCPrevPopupFadeOutTime );
    iPopupShadow->SetPos( iAnimInfo.iSourceRealPos, KBCPrevPopupFadeOutTime );

    iPopup->SetSize( TAlfRealSize(0,0), KBCPrevPopupFadeOutTime );
    iPopup->SetPos( iAnimInfo.iSourceRealPos, KBCPrevPopupFadeOutTime );
    }

// ---------------------------------------------------------------------------
// Calculates the size and lays out the preview popup layout. Returns the size
// ---------------------------------------------------------------------------
//
TSize CBCPreviewPopup::CalculateLayout()
    {
    TInt totalHeight(0);

    TInt padding = iResManager.LayoutHandler().PreviewPopupInnerPadding();
    iPreviewPopup->SetPadding( padding );
    totalHeight += 2*padding;
    TSize visualSize;

    if ( iSummaryVisual )
        {
        totalHeight += iSummaryVisualSize.iHeight;
        iSummaryVisual->SetSize( iSummaryVisualSize );
        }
    if ( iTimeVisual )
        {
        totalHeight += iTimeVisualSize.iHeight;
        // Width of iTimeVisualSize does not matter.
        // Due to EModeFitPerpendicular, iTimeAndIconsLayout is resized to
        // fit the parent layout
        iTimeAndIconsLayout->SetSize( iTimeVisualSize );
        }
    if ( iDateVisual )
        {
        totalHeight += iDateVisualSize.iHeight;
        iDateVisual->SetSize( iDateVisualSize );
        }
    if ( iLocationVisual )
        {
        totalHeight += iLocationVisualSize.iHeight;
        iLocationVisual->SetSize( iLocationVisualSize );
        }
    if ( iDescriptionVisual )
        {
        totalHeight += iDescriptionVisualSize.iHeight;
        iDescriptionVisual->SetSize( iDescriptionVisualSize );
        }
    // Set size. Visual is streched to cover almost the whole width of
    // client rect (= whole display width).
    // Width = width of ClientRect - 2 * the margin between popup
    // Height = Sum of heights of all visuals inside popup.
    TInt newWidth = iViewportLayout->DisplayRectTarget().Width() - 2*KBCPrevPopupHMarginOutsidePopup;

    //Height should be an even number to animate it correctly from the central
    //position using height/2 value (if height is an odd number one pixel will be lost)
    if ( totalHeight%2 != 0 )
        {
        totalHeight++;
        }

    return TSize( newWidth, totalHeight );
    }

// ---------------------------------------------------------------------------
// Determines if the event is an all-day event. It is an all-day event if
// it starts from midnight and ends at midnight. Duration is 24 hours.
// ---------------------------------------------------------------------------
//
TBool CBCPreviewPopup::IsAllDayEvent( const TTime& aStartTime,
                                           const TTime& aEndTime ) const
    {
    TTimeIntervalMinutes durationInMin;
    aEndTime.MinutesFrom(aStartTime, durationInMin);

    TTime beginOfDay = CalenDateUtils::BeginningOfDay(aStartTime);

    return ( aStartTime == beginOfDay && !
            (durationInMin.Int()%KBCPrevPopupOneDayInMinutes) &&
            durationInMin.Int() >= KBCPrevPopupOneDayInMinutes );
    }

// ---------------------------------------------------------------------------
// Sets style parameter for the specified visual. Styles are common for all
// text visuals in the preview popup.
// ---------------------------------------------------------------------------
//
TSize CBCPreviewPopup::SetTextVisualStyleL( CAlfTextVisual* aVisual )
    {
    aVisual->SetWrapping(CAlfTextVisual::ELineWrapTruncate);
    aVisual->SetAlign( EAlfAlignHLocale, EAlfAlignVCenter );
    aVisual->SetMaxLineCount( KBCPrevPopupMaxLineCount );
    
    //TODO:- Using hard code for temp.
    TAlfMetric KBCPrevPopupNoMargin(0);
    TAlfBoxMetric KBCPrevPopupTextVisualMargins(
            KBCPrevPopupNoMargin,
            KBCPrevPopupNoMargin,
            KBCPrevPopupNoMargin,
            KBCPrevPopupNoMargin );
    aVisual->SetPadding(KBCPrevPopupTextVisualMargins);
    aVisual->EnableShadow( EFalse );

    // Setup text style for visual.
    iResManager.TextStyleManager().SetVisualStyleL( EMeetingText, aVisual );
    return aVisual->TextExtents();
    }

// ---------------------------------------------------------------------------
// Checks both RRULE and RDATE information in case of occurence. Return ETrue
// if either of them indicates a recurring event.
// ---------------------------------------------------------------------------
//
TBool CBCPreviewPopup::IsRecurringEventL( const CCalEntry& aEntry )
    {
    TBool isRecurring( EFalse );

    // First check RRULE, if there is occurence information.
    TCalRRule rule;
    isRecurring = aEntry.GetRRuleL( rule );
    if ( !isRecurring )
        {
        // No RRule information --> Check RDATE
        RArray<TCalTime> rDates;
        aEntry.GetRDatesL( rDates );
        if ( rDates.Count() )
            {
            // There is at least one RDATE --> event is recurring
            isRecurring = ETrue;
            rDates.Reset();
            }
        rDates.Close();
        }
    return isRecurring;
    }

// ---------------------------------------------------------------------------
// Checks whether the given entry is an exception to a recurring event
// ---------------------------------------------------------------------------
//
TBool CBCPreviewPopup::IsRecurringExceptionEventL( const CCalEntry& aEntry )
    {
    TBool isException( EFalse );
    if( aEntry.RecurrenceIdL().TimeUtcL() != Time::NullTTime() )
        {
        isException = ETrue;
        }
    return isException;
    }

// End of file
