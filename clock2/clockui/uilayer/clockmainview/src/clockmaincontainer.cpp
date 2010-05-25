/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   This is the source file for the CClockMainContainer class.
*
*/

// System includes
#include <AknsBasicBackgroundControlContext.h>
#include <AknsSkinInstance.h>
#include <AknsUtils.h>
#include <AknsDrawUtils.h>
#include <AknSkinnableClock.h>
#include <csxhelp/clk.hlp.hrh>
#include <aknlayoutscalable_apps.cdl.h>
#include <aknlayoutscalable_avkon.cdl.h>
#include <layoutmetadata.cdl.h>
#include <clock.rsg>
#include <StringLoader.h>
#include <aknlists.h>
#include <gulicon.h>
#include <AknIconArray.h>
#include <AknsConstants.h>
#include <AknDef.h>
#include <clockapp.mbg>
#include <avkon.mbg>
#include <tz.h>
#include <tzlocalizer.h>
#include <tzlocalizationdatatypes.h>
#include <vtzrules.h>
#include <aknbutton.h>

// User includes
#include "clockmaincontainer.h"
#include "clockmainview.h"
#include "clockalarmarray.h"
#include "clockserverclt.h"
#include "clockmarqueelabel.h"
#include "clkcommon.h"
#include "clock.hrh"
#include "clock.h"
#include "clock_debug.h"
#include "clockappui.h"

// Constants
const TInt KNumOfIcons( 3 );
const TInt KHour( 60 );
const TInt KZerothRule( 0 );
const TInt KOffsetStringLength( 64 );
const TInt KLocationNameLength( 100 );
const TInt KDayNames[] =
    {
    R_QTN_WEEK_LONG_MONDAY,
    R_QTN_WEEK_LONG_TUESDAY,
    R_QTN_WEEK_LONG_WEDNESDAY,
    R_QTN_WEEK_LONG_THURSDAY,
    R_QTN_WEEK_LONG_FRIDAY,
    R_QTN_WEEK_LONG_SATURDAY,
    R_QTN_WEEK_LONG_SUNDAY
    };

TInt const KSwitchDelay = 200*1000;

// Literals
_LIT( KIconFileName, "\\resource\\apps\\clockapp.mif" );
_LIT( KPlusGMT, " \x202A+" );
_LIT( KMinusGMT, " \x202A" );
_LIT( KPDF, " \x202C" );
_LIT( KZero, "0" );

// ---------------------------------------------------------
// CClockMainContainer::NewL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
CClockMainContainer* CClockMainContainer::NewL( CClockMainView* aView, const TRect& aRect, CClockAlarmArray* aAlarmArray )
    {
	__PRINTS( "CClockMainContainer::NewL - Entry" );
	
    CClockMainContainer* self = new ( ELeave ) CClockMainContainer;
    CleanupStack::PushL( self );
    
    self->ConstructL( aView, aRect, aAlarmArray );
    
    CleanupStack::Pop( self );
	
	__PRINTS( "CClockMainContainer::NewL - Exit" );
	
    return self;
    }

// ---------------------------------------------------------
// CClockMainContainer::~CClockMainContainer
// rest of the details are commented in the header
// ---------------------------------------------------------
//
CClockMainContainer::~CClockMainContainer()
    {
	__PRINTS( "CClockMainContainer::~CClockMainContainer - Entry" );
	
	if( iBgContext )
	    {
	    delete iBgContext;
	    iBgContext = NULL;
	    }
	if( iSkinnableClock )
	    {
	    delete iSkinnableClock;
	    iSkinnableClock = NULL;
	    }
	if( iListBox )
	    {
	    delete iListBox;
	    iListBox = NULL;
	    }
	if( iDateString )
	    {
	    delete iDateString;
	    iDateString = NULL;
	    }
	if( iDstIndicator )
	    {
	    delete iDstIndicator;
	    iDstIndicator = NULL;
	    }
	if( iDstIndicatorMask )
	    {
	    delete iDstIndicatorMask;
	    iDstIndicatorMask = NULL;
	    }
	if( iDayNameString )
        {
        delete iDayNameString;
        iDayNameString = NULL;
        }
	if( iLocationString )
	    {
	    delete iLocationString;
	    iLocationString = NULL;
	    }
	if( iLocationLabel )
	    {
	    delete iLocationLabel;
	    iLocationLabel = NULL;
	    }
	if( iTimeZoneString )
	    {
	    delete iTimeZoneString;
	    iTimeZoneString = NULL;
	    }

	delete iNewAlarmButton;
	iNewAlarmButton = NULL;
	__PRINTS( "CClockMainContainer::~CClockMainContainer - Exit" );
    }

// ---------------------------------------------------------
// CClockMainContainer::OfferKeyEventL
// rest of the details are commented in the header
// ---------------------------------------------------------
//    
TKeyResponse CClockMainContainer::OfferKeyEventL( const TKeyEvent& aKeyEvent, 
                                                  TEventCode aType )
    {
	__PRINTS( "CClockMainContainer::OfferKeyEventL - Entry" );
	
    TKeyResponse retVal( EKeyWasNotConsumed );

    if( EEventKey == aType )
        {
        if( EStdKeyNo == aKeyEvent.iScanCode )
            {
            // This indicates that Call End key was pressed.
            // The timer is stopped and app is hidden in the background.
            iView->ClockApplicationUi()->HandleCommandL( EClockMainViewExit );

            retVal = EKeyWasConsumed;

            __PRINTS( "CClockMainContainer::OfferKeyEventL - Exit" );

            return retVal;
            }
        
        if( EKeyUpArrow == aKeyEvent.iCode ||
            EKeyDownArrow == aKeyEvent.iCode )
            {

            TInt itemCount = iListBox->Model()->ItemTextArray()->MdcaCount();
            // if neither has focus just then select button to be focused
            if( !iNewAlarmButton->IsFocused() && !iListBox->IsFocused() )
                {
                iListBox->View()->ItemDrawer()->SetFlags( CListItemDrawer::EDisableHighlight );
                iNewAlarmButton->SetFocus( ETrue );
                DrawDeferred();
                retVal = EKeyWasConsumed;
                }
            // button not focused and no list items -> set focus to button
            else if( itemCount == 0 && !iNewAlarmButton->IsFocused() )
                {
                iListBox->View()->ItemDrawer()->SetFlags( CListItemDrawer::EDisableHighlight );
                iListBox->SetFocus( EFalse );
                iNewAlarmButton->SetFocus( ETrue );
                DrawDeferred();
                retVal = EKeyWasConsumed;
                }
            // button focused and listbox has items
            else if( iNewAlarmButton->IsFocused() && itemCount > 0 )
                {
                iListBox->View()->ItemDrawer()->ClearFlags( CListItemDrawer::EDisableHighlight );
                iNewAlarmButton->SetFocus( EFalse );
                iListBox->SetFocus( ETrue );
                if( EKeyDownArrow == aKeyEvent.iCode )
                    {
                    iListBox->SetCurrentItemIndex( itemCount - 1 );
                    }
                else
                    {
                    iListBox->SetCurrentItemIndex( 0 );
                    }
                retVal = iListBox->OfferKeyEventL( aKeyEvent, aType );
                }
            // listbox focused
            else if( iListBox->IsFocused() && itemCount > 0 )
                {
                // first item when only one item in the list
                if( iListBox->CurrentItemIndex() == 0 && itemCount == 1 )
                    {
                    iListBox->View()->ItemDrawer()->SetFlags( CListItemDrawer::EDisableHighlight );
                    iListBox->SetFocus( EFalse );
                    iNewAlarmButton->SetFocus( ETrue );
                    DrawDeferred();
                    retVal = EKeyWasConsumed;
                    }
                // first item
                else if( iListBox->CurrentItemIndex() == 0 && EKeyUpArrow == aKeyEvent.iCode )
                    {
                    iListBox->View()->ItemDrawer()->SetFlags( CListItemDrawer::EDisableHighlight );
                    iListBox->SetFocus( EFalse );
                    iNewAlarmButton->SetFocus( ETrue );
                    DrawDeferred();
                    retVal = EKeyWasConsumed;
                    }
                //last item
                else if( iListBox->CurrentItemIndex() == ( itemCount - 1 )
                        && EKeyDownArrow == aKeyEvent.iCode )
                    {
                    iListBox->View()->ItemDrawer()->SetFlags( CListItemDrawer::EDisableHighlight );
                    iListBox->SetFocus( EFalse);
                    iNewAlarmButton->SetFocus( ETrue );
                    DrawDeferred();
                    retVal = EKeyWasConsumed;
                    }
                // Send the event to the listbox to handle the event.
                else
                    {
                    iListBox->View()->ItemDrawer()->ClearFlags( CListItemDrawer::EDisableHighlight );
                    retVal = iListBox->OfferKeyEventL( aKeyEvent, aType );
                    }
                }
            }

        if( ( EKeyBackspace == aKeyEvent.iCode || EKeyDelete == aKeyEvent.iCode ) &&
            ( KZeroAlarms < iListBox->Model()->ItemTextArray()->MdcaCount() ) )
            {
            // Handle backspace command. Deletes the selected alarm.
			//Single click integration
            if ( iListBox->IsFocused() && iView->MenuBar()->ItemSpecificCommandsEnabled() )
                {
                // if highlighted
                iView->HandleCommandL( EClockRemoveAlarm );
                retVal = EKeyWasConsumed;
                }
            // otherwise do nothing and don't consume key

            }
		//Single click integration
        if ( aKeyEvent.iCode == EKeyOK || aKeyEvent.iCode == EKeyEnter )
            {
            // Send the event to the listbox to handle the event
            // if listbox is focused
            if( iListBox->IsFocused() && iListBox->Model()->ItemTextArray()->MdcaCount() > 0 )
                {
                iListBox->OfferKeyEventL( aKeyEvent, aType );
                retVal = EKeyWasConsumed;
                }
            else
                {
                // default action for the enter is button press
                TKeyEvent keyEvent;
                keyEvent.iCode = EKeyOK;  
                keyEvent.iScanCode = EStdKeyDevice3;
                keyEvent.iModifiers = 0;
                keyEvent.iRepeats = 0;
                iNewAlarmButton->OfferKeyEventL( keyEvent, EEventKeyDown );
                iNewAlarmButton->OfferKeyEventL( keyEvent, EEventKey );
                iNewAlarmButton->OfferKeyEventL( keyEvent, EEventKeyUp );
                retVal = EKeyWasConsumed;
                
                }
            }

        }
		
	__PRINTS( "CClockMainContainer::OfferKeyEventL - Exit" );
		
    return retVal;
    }

// ---------------------------------------------------------
// CClockMainContainer::Draw
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClockMainContainer::Draw( const TRect& aRect ) const
    {
	__PRINTS( "CClockMainContainer::Draw - Entry" );
	
    CWindowGc& windowGc = SystemGc();
    windowGc.Clear( aRect );
    
    if( iBgContext )
        {
        MAknsSkinInstance* skinInstance = AknsUtils::SkinInstance();
        AknsDrawUtils::Background( skinInstance, iBgContext, this, windowGc, aRect );
        }
    
    // Draw the date and day text.
    DrawMainViewText( windowGc );
    
    // TODO: Change the LCT when the new LCT data is available.
    // Check if the DST is ON. If so, display the DST indicator icon.
    if( iDSTOn )
        {
        TAknLayoutRect dstIndicatorRect;
        
        if( Layout_Meta_Data::IsLandscapeOrientation() )
            {
            dstIndicatorRect.LayoutRect( Rect(),
                                         AknLayoutScalable_Apps::main_clock2_pane_g1( KVGALandscapeVariant ).LayoutLine() );
            }
        else
            {
            dstIndicatorRect.LayoutRect( Rect(),
                                         AknLayoutScalable_Apps::main_clock2_pane_g1( KVGAPortraitVariant ).LayoutLine() );
            }
       
        AknIconUtils::SetSize( iDstIndicator, dstIndicatorRect.Rect().Size() );
        dstIndicatorRect.DrawImage( windowGc, iDstIndicator, iDstIndicatorMask );
        }

	__PRINTS( "CClockMainContainer::Draw - Exit" );
    }

// ---------------------------------------------------------
// CClockMainContainer::FocusChanged
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClockMainContainer::FocusChanged( TDrawNow /*aDrawNow*/ )
    {
	__PRINTS( "CClockMainContainer::FocusChanged - Entry" );
	
	__PRINTS( "CClockMainContainer::FocusChanged - Exit" );
    }

// ---------------------------------------------------------
// CClockMainContainer::HandlePointerEventL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClockMainContainer::HandlePointerEventL( const TPointerEvent& aPointerEvent )
    {
	__PRINTS( "CClockMainContainer::HandlePointerEventL - Entry" );
	
    // Check first wheather touch is supported or not.
    if( !AknLayoutUtils::PenEnabled() )
        {
		__PRINTS( "CClockMainContainer::HandlePointerEventL - Exit" );
		
        return;
        }
    
    // to get the vibes let's handle skinnable clock as a special case
    // no need to to delegate pointer events to it
    if( iSkinnableClock && iSkinnableClock->Rect().Contains( aPointerEvent.iPosition ) )
        {
        if( aPointerEvent.iType == TPointerEvent::EButton1Down  )
            {
            TTime now;
            now.HomeTime();
            TInt64 launchFreq = now.MicroSecondsFrom( iPreviousClockTypeSwitch ).Int64();
            if( launchFreq > MAKE_TINT64( 0, KSwitchDelay )  )
                {
                iView->GiveVibes();
                iView->HandleCommandL( EClockSwitchClockType ); 
                iPreviousClockTypeSwitch.HomeTime();
                }   
            }
        } 
    else
        {
        CCoeControl::HandlePointerEventL( aPointerEvent );
        }
	__PRINTS( "CClockMainContainer::HandlePointerEventL - Exit" );
    }

// ---------------------------------------------------------
// CClockMainContainer::HandleListBoxEventL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClockMainContainer::HandleListBoxEventL( CEikListBox* aListBox, TListBoxEvent aEventType )
    {
    __PRINTS( "CClockMainContainer::HandleListBoxEventL - Entry" );
    if( aListBox == iListBox &&
        ( aEventType == EEventItemSingleClicked || aEventType == EEventEnterKeyPressed ) )
        {
        // Check if the user has double clicked on the list item.
        if( !iView->IsAlarmEditorOpen() )
            {
            iView->HandleCommandL( EClockResetAlarm ); 
            }
        }
    
    __PRINTS( "CClockMainContainer::HandleListBoxEventL - Entry" );
    }

// ---------------------------------------------------------
// CClockMainContainer::UpdateAlarmListL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClockMainContainer::UpdateAlarmListL( SClkAlarmInfo& /*aAlarmInfo*/, TAlarmId aAlarmId )
    {
	__PRINTS( "CClockMainContainer::UpdateAlarmListL - Entry" );
	
    // Reinitialize the alarm id array.
    iAlarmArray->InitIdList();
    iListBox->HandleItemAdditionL();
    
    // Get the index of the new alarm added.
    TInt itemIndex( iAlarmArray->ListBoxIndex( aAlarmId ) );
    
    // Update the listbox.
    iListBox->ScrollToMakeItemVisible( itemIndex );
    iListBox->SetCurrentItemIndexAndDraw( itemIndex );
    SetCorrectRectForNewAlarmButton();
    iNewAlarmButton->DrawDeferred();
	__PRINTS( "CClockMainContainer::UpdateAlarmListL - Exit" );
    }

// ---------------------------------------------------------
// CClockMainContainer::RemoveAlarmL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClockMainContainer::RemoveAlarmL( TInt aIndex )
    {
	__PRINTS( "CClockMainContainer::RemoveAlarmL - Entry" );
	
    // Reinitialize the alarm list.
    iAlarmArray->InitIdList();
    TInt count = iListBox->Model()->ItemTextArray()->MdcaCount();
    
    //SetCorrectRectForNewAlarmButton();
    
    // if only one item
    if( ( KZeroAlarms == aIndex ) && ( aIndex == count ) )
        {
        iListBox->SetCurrentItemIndex( KZeroAlarms );
        SetCorrectRectForNewAlarmButton();
        }  
    // last item when more than one exists
    else if( ( KZeroAlarms < aIndex ) && ( aIndex == count ) )
        {
        // Last alarm in the list is deleted hence set the previous alarm.
        iListBox->ScrollToMakeItemVisible( aIndex - 1 );
        iListBox->SetCurrentItemIndexAndDraw( aIndex - 1 );
        iListBox->HandleItemRemovalL();
        DrawNow();
        }
    // not last item
    else if( ( KZeroAlarms <= aIndex ) && ( aIndex < count ) )
        {
        // Highlight the alarm item replacing the deleted element
        iListBox->ScrollToMakeItemVisible( aIndex );
        iListBox->SetCurrentItemIndexAndDraw( aIndex );
        iListBox->HandleItemRemovalL();
        DrawNow();
        }
	
	__PRINTS( "CClockMainContainer::RemoveAlarmL - Exit" );
    }

// ---------------------------------------------------------
// CClockMainContainer::ListBox
// rest of the details are commented in the header
// ---------------------------------------------------------
//
//Single click integration
CAknDoubleStyleListBox* CClockMainContainer::ListBox()
    {
	__PRINTS( "CClockMainContainer::ListBox - Entry" );
	
	__PRINTS( "CClockMainContainer::ListBox - Exit" );
	
    return iListBox;
    }

// ---------------------------------------------------------
// CClockMainContainer::SetDayInfoL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClockMainContainer::SetDayInfoL()
    {
	__PRINTS( "CClockMainContainer::SetDayInfoL - Entry" );
	
    // First get the current time.
    TTime homeTime;
    homeTime.HomeTime();
	
	TDateString dateBuffer;
    
    // Construct the date string.
    HBufC* formatString = StringLoader::LoadLC( R_QTN_DATE_USUAL_WITH_ZERO, iCoeEnv );
    // Format the time to get the date.
    homeTime.FormatL( dateBuffer, *formatString );
    // Cleanup.
    CleanupStack::PopAndDestroy( formatString );
    
    // Get the date string.
    if( iDateString )
    	{
    	delete iDateString;
    	iDateString = NULL;
    	}
    iDateString = HBufC::NewL( dateBuffer.Length() );
    TPtr dateStringPtr = iDateString->Des();
    dateStringPtr.Append( dateBuffer );
    
    // Get the day name string.
    if( iDayNameString )
        {
        delete iDayNameString;
        iDayNameString = NULL;
        }
    iDayNameString = StringLoader::LoadL( KDayNames[ homeTime.DayNoInWeek() ], iCoeEnv );
    
	__PRINTS( "CClockMainContainer::SetDayInfoL - Exit" );
    }

// ---------------------------------------------------------
// CClockMainContainer::UpdateDSTStateL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClockMainContainer::UpdateDSTStateL()
    {
	__PRINTS( "CClockMainContainer::UpdateDSTStateL - Entry" );
	
    RTz tzHandle;
    User::LeaveIfError( tzHandle.Connect() );
    CleanupClosePushL( tzHandle );

    CTzId* timezoneId = tzHandle.GetTimeZoneIdL();
    CleanupStack::PushL( timezoneId );

    if( tzHandle.IsDaylightSavingOnL( *timezoneId ) )
        {
        iDSTOn = ETrue;
        }
    else 
        {
        iDSTOn = EFalse;
        }

    CleanupStack::PopAndDestroy( timezoneId );
    CleanupStack::PopAndDestroy( &tzHandle );
	
	__PRINTS( "CClockMainContainer::UpdateDSTStateL - Exit" );
    }

// ---------------------------------------------------------
// CClockMainContainer::SetZoneInfoL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClockMainContainer::SetZoneInfoL()
    {
	__PRINTS( "CClockMainContainer::SetZoneInfoL - Entry" );
	
    TBuf< KOffsetStringLength > offsetString;
    TBuf< KLocationNameLength > locationName;
    TInt gmtOffset( KErrNone );

    RArray< TInt > timeZones;
    RArray< TInt > timeZoneOffsets;

    // Connect to the timezone server.
    RTz tzHandle;
    User::LeaveIfError( tzHandle.Connect() );
    CleanupClosePushL( tzHandle );

    // Get the current timezone ID.
    CTzId* timeZoneId;
    timeZoneId = tzHandle.GetTimeZoneIdL();
    CleanupStack::PushL( timeZoneId );

    // Get all the timezone offsets corresponding to the current timezone ID.
    timeZones.Append( timeZoneId->TimeZoneNumericID() );
    tzHandle.GetOffsetsForTimeZoneIdsL( timeZones, timeZoneOffsets );

    // The offset of the current zone.
    TInt currentOffset( timeZoneOffsets[ KErrNone ] );
    gmtOffset = currentOffset;

    // Check if the DST is on for the current timezone.
    if( IsDstOnL( timeZones[ 0 ] ) )
        {
        TInt32 dstOffset;

        // Get the offset
        GetOffSetWithDstL( dstOffset, *timeZoneId );

        // Update the gmt offset with DST offset.
        gmtOffset = dstOffset;
        }
    
    // Cleanup.
    timeZones.Close();
    timeZoneOffsets.Close();

    // Get all the localized timezones for the current timezone ID.
    CTzLocalizedTimeZone* localizedTimeZone( NULL );
    CTzLocalizer* tzLocalizer( NULL ); 
    TRAPD( errVal, tzLocalizer = CTzLocalizer::NewL() );
    if( tzLocalizer && ( !errVal ) )
        {
        // Get the currently set localized timezone.
        CleanupStack::PushL( tzLocalizer );
        TRAPD( errVal, localizedTimeZone = tzLocalizer->GetLocalizedTimeZoneL( timeZoneId->TimeZoneNumericID() ) );

        if( ( localizedTimeZone ) && ( !errVal ) )
            {
            CleanupStack::PushL( localizedTimeZone );
    
            // Get the frequently used localized city.
            CTzLocalizedCity* localizedCity = tzLocalizer->GetFrequentlyUsedZoneCityL( CTzLocalizedTimeZone::ECurrentZone );
            CleanupStack::PushL( localizedCity );

            // Get all the city groups.
            CTzLocalizedCityGroupArray* cityGroupArray = tzLocalizer->GetAllCityGroupsL( CTzLocalizer::ETzAlphaNameAscending );
            CleanupStack::PushL( cityGroupArray );

            // Get the index of the country corresponding to the city group ID.
            TInt countryIndex( 1 );
    
            for( TInt index( KErrNone ); index < cityGroupArray->Count(); index++ )
                {       
                if( localizedCity->GroupId() == cityGroupArray->At( index ).Id() )
                    {
                    countryIndex = index;
                    }
                }

            // Get all the cities within the currently set country.
            CTzLocalizedCityArray* cityList = tzLocalizer->GetCitiesInGroupL( ( cityGroupArray->At( countryIndex ) ).Id(), 
                                                                              CTzLocalizer::ETzAlphaNameAscending );
            CleanupStack::PushL( cityList );
            
            // Check if automatic time update is enabled.
            TBool timeUpdateOn( EFalse );       
            RClkSrvInterface clkSrvIf;  

            // Connect to clockserver
            TInt errorVal( clkSrvIf.Connect() );

            if( KErrNone == errorVal )
                {
                // First check if automatic time update is enabled.   
                errorVal = clkSrvIf.IsAutoTimeUpdateOn( timeUpdateOn );
                }

            // Check if the country contains only one city or if automatic time update is on.
            if( 1 == cityList->Count() || timeUpdateOn )
                {
                // If yes, then display only the country name.
                locationName.Copy( cityGroupArray->At( countryIndex ).Name() );
                }
            else
                {
                // Automatic time update is disabled. Display the city name.
                locationName.Copy( localizedCity->Name() );
                }
            
            // Close the handle to the clockserver.
            clkSrvIf.Close();

            // Cleanup.
            CleanupStack::PopAndDestroy( cityList );
            CleanupStack::PopAndDestroy( cityGroupArray );
            CleanupStack::PopAndDestroy( localizedCity );
            CleanupStack::PopAndDestroy( localizedTimeZone );
            }
        // Cleanup.
        CleanupStack::PopAndDestroy( tzLocalizer );
        }
    
    // Cleanup.
    CleanupStack::PopAndDestroy( timeZoneId );
    CleanupStack::PopAndDestroy( &tzHandle );

    // Now construct the GMT +/-X string.
    TInt tempOffset( gmtOffset );
    TInt offsetHours( tempOffset / KHour );
    TInt offsetMinutes( KErrNone );
    TGmtOffsetText offsetText;

    if( KErrNone < tempOffset )
        {
        // We have a positive offset. Append the + character.
        offsetMinutes = gmtOffset - offsetHours * KHour;
        offsetString += KPlusGMT;
        }
    else if( KErrNone > tempOffset )
        {
        // Negative offset. Append the - character.
        offsetMinutes = -( gmtOffset - offsetHours * KHour );
        offsetString += KMinusGMT;
        }
    else
        {
        // We dont have an offset. We are at GMT zone.
        offsetString += KSingleSpace;
        }
    // Append the hour component.
    offsetText.Num( offsetHours );
    offsetString += offsetText;
    
    // Append the time separator.
    const TInt KSeparIndex( 1 );
    offsetString.Append( TLocale().TimeSeparator( KSeparIndex ) );
    
    // If the minute component is less than 10, append a '0'
    if ( KErrNone <= offsetMinutes && offsetMinutes < 10 )
        {
        offsetString += KZero;
        }
    // Append the minute component.
    offsetText.Num( offsetMinutes );
    offsetString += offsetText;
    offsetString += KPDF;

    // These will hold the complete zone information displayed on the main view.
    if( iLocationString )
        {
        delete iLocationString;
        iLocationString = NULL;
        }
    if( iTimeZoneString )
        {
        delete iTimeZoneString;
        iTimeZoneString = NULL;
        }
    
    // The GMT prefix which is prepended before the offset and zone information.
    HBufC* gmtPrefixText = StringLoader::LoadLC( R_QTN_CLK_NITZ_GMT, iCoeEnv );
    
    // Construct the strings to the displayed.
    iTimeZoneString = HBufC::NewL( gmtPrefixText->Length() + offsetString.Length() );
    iLocationString = HBufC::NewL( locationName.Length() );
    // First the timezone string.    
    TPtr timeZonePtr = iTimeZoneString->Des();
    timeZonePtr += *gmtPrefixText;
    timeZonePtr += offsetString;
    
    // Then the location name.
    TPtr zoneInfoPtr = iLocationString->Des();
    if( locationName.Length() )
        {
        zoneInfoPtr += locationName;
        }
    
    // Cleanup.
    CleanupStack::PopAndDestroy( gmtPrefixText );
	
	__PRINTS( "CClockMainContainer::SetZoneInfoL - Exit" );
    }

// ---------------------------------------------------------
// CClockMainContainer::ResetMarqueeLabelL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClockMainContainer::ResetMarqueeLabelL()
    {
    __PRINTS( "CClockMainContainer::ResetMarqueeLabelL - Entry" );
    
    // Reset the label so that it marquees again.
    if( iLocationLabel )
        {
        iLocationLabel->ResetMarquee();
        }
    
    __PRINTS( "CClockMainContainer::ResetMarqueeLabelL - Exit" );
    }

// ---------------------------------------------------------
// CClockMainContainer::SizeChanged
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClockMainContainer::SizeChanged()
    {
	__PRINTS( "CClockMainContainer::SizeChanged - Entry" );
	
    TRect paneRect = this->Rect();
    // Update the background context.
    iBgContext->SetRect( paneRect );
    
    // The clock main rect.
    TAknLayoutRect mainRect;
    // The rect for the skinnable clock.
    TAknLayoutRect skinClockRect;
    // The rect for the listbox.
    TAknLayoutRect listBoxRect;
    
    // Get the layout for the main rect.
    mainRect.LayoutRect( paneRect, AknLayoutScalable_Apps::main_clock2_pane().LayoutLine() );
    
    if( Layout_Meta_Data::IsLandscapeOrientation() )
        {
        listBoxRect.LayoutRect( mainRect.Rect(), AknLayoutScalable_Apps::listscroll_gen_pane_cp06( 1 ).LayoutLine() );
        skinClockRect.LayoutRect( mainRect.Rect(),
                                  AknLayoutScalable_Apps::popup_clock_analogue_window_cp03( KVGALandscapeVariant ).LayoutLine() );
        }
    else
        {
        listBoxRect.LayoutRect( mainRect.Rect(), AknLayoutScalable_Apps::listscroll_gen_pane_cp06( 0 ).LayoutLine() );
        skinClockRect.LayoutRect( mainRect.Rect(),
                                  AknLayoutScalable_Apps::popup_clock_analogue_window_cp03( KVGAPortraitVariant ).LayoutLine() );
        }
    
    SetCorrectRectForNewAlarmButton();
    
    // Update the skinnable clock.
    iSkinnableClock->SetExtent( skinClockRect.Rect().iTl, skinClockRect.Rect().Size() );
    iSkinnableClock->UpdateDisplay();
    
    // Update the listbox.
    iListBox->SetExtent( listBoxRect.Rect().iTl, listBoxRect.Rect().Size() );
	
	__PRINTS( "CClockMainContainer::SizeChanged - Exit" );
    }

// ---------------------------------------------------------
// CClockMainContainer::CountComponentControls
// rest of the details are commented in the header
// ---------------------------------------------------------
//
TInt CClockMainContainer::CountComponentControls() const
    {
	__PRINTS( "CClockMainContainer::CountComponentControls - Entry" );
	
	__PRINTS( "CClockMainContainer::CountComponentControls - Exit" );
	
    return 3;
    }

// ---------------------------------------------------------
// CClockMainContainer::ComponentControl
// rest of the details are commented in the header
// ---------------------------------------------------------
//  
CCoeControl* CClockMainContainer::ComponentControl( TInt aIndex ) const
    {
	__PRINTS( "CClockMainContainer::ComponentControl - Entry" );
	
    switch( aIndex )
        {
        case 0:
            {
			__PRINTS( "CClockMainContainer::ComponentControl - Exit" );
			
            // Return the skinnable clock.
            return iSkinnableClock;
            }
            
        case 1:
            {
            __PRINTS( "CClockMainContainer::ComponentControl - Exit" );
            
            // Return the new alarm button.
            return iNewAlarmButton;
            }            
            
        case 2:
            {
			__PRINTS( "CClockMainContainer::ComponentControl - Exit" );
			
            // Return the listbox.
            return iListBox;
            }
        
        default:
            {
			__PRINTS( "CClockMainContainer::ComponentControl - Exit" );
			
            return NULL;
            }
        }
    }

// ---------------------------------------------------------
// CClockMainContainer::MopSupplyObject
// rest of the details are commented in the header
// ---------------------------------------------------------
//
TTypeUid::Ptr CClockMainContainer::MopSupplyObject( TTypeUid aId )
    {
	__PRINTS( "CClockMainContainer::MopSupplyObject - Entry" );
	
    if( MAknsControlContext::ETypeId == aId.iUid )
        {
		__PRINTS( "CClockMainContainer::MopSupplyObject - Exit" );
		
        return MAknsControlContext::SupplyMopObject( aId, iBgContext );
        }
		
	__PRINTS( "CClockMainContainer::MopSupplyObject - Exit" );
    
    return CCoeControl::MopSupplyObject( aId );
    }

// ---------------------------------------------------------
// CClockMainContainer::HandleResourceChange
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClockMainContainer::HandleResourceChange( TInt aType )
    {
	__PRINTS( "CClockMainContainer::HandleResourceChange - Entry" );
	
    // If skinnable clock is present, let it handle the resource change.
    if( iSkinnableClock )
        {
        iSkinnableClock->HandleResourceChange( aType );
        }

    // Allow the listbox to handle the resource change.
    if( iNewAlarmButton )
        {
        iNewAlarmButton->HandleResourceChange( aType );
        }

    // Allow the listbox to handle the resource change.
    if( iListBox )
        {
        iListBox->HandleResourceChange( aType );
        }
    // Then let the controlenvironment handle the change.
    CCoeControl::HandleResourceChange( aType );
    
    switch( aType )
        {
		case KEikMessageWindowsFadeChange:
        case KEikDynamicLayoutVariantSwitch:
            {
            SetRect( iView->ClientRect() );
			// Destroy and reconstruct the label again so that it marquees whenever the orientation changes.
            ResetMarqueeLabelL();
            
          
            }
            break;
            
        case KAknsMessageSkinChange:
            {
            // First get the icon array from listbox item drawer.
            CArrayPtr< CGulIcon >* iconArray = iListBox->ItemDrawer()->FormattedCellData()->IconArray();
                     
            if( iconArray )
                {
                // Reset the iconArray.
                iconArray->ResetAndDestroy();
                delete iconArray;
                iconArray = NULL;            
                iListBox->ItemDrawer()->FormattedCellData()->SetIconArray( NULL );
                }
            
            // Reload the icons into the array.
            iconArray =  new( ELeave ) CAknIconArray( KNumOfIcons );
            CleanupStack::PushL( iconArray );

            CFbsBitmap* iconBitmap( NULL );
            CFbsBitmap* iconMask( NULL );
            CGulIcon* finalIcon( NULL );

            // First read the alarm icon.
            AknsUtils::CreateColorIconL( AknsUtils::SkinInstance(),
                    KAknsIIDQgnIndiAlarmAdd,
                    KAknsIIDQsnIconColors,
                    EAknsCIQsnIconColorsCG13,
                    iconBitmap,
                    iconMask,
                    AknIconUtils::AvkonIconFileName(),
                    EMbmAvkonQgn_indi_alarm_add,
                    EMbmAvkonQgn_indi_alarm_add_mask,
                    TRgb( NULL, NULL, 255 ) );
            CleanupStack::PushL( iconBitmap );
            CleanupStack::PushL( iconMask );
            // Construct the icon.
            finalIcon = CGulIcon::NewL( iconBitmap, iconMask );
            CleanupStack::PushL( finalIcon );
            // Append the above icon to the array.
            iconArray->AppendL( finalIcon );
            // Cleanup.
            CleanupStack::Pop( finalIcon );
            CleanupStack::Pop( iconMask );
            CleanupStack::Pop( iconBitmap );

            // Now load the repeat icon.
            iconBitmap = NULL;
            iconMask = NULL;

            AknsUtils::CreateColorIconL( AknsUtils::SkinInstance(),
                    KAknsIIDQgnIndiRepeatAdd,
                    KAknsIIDQsnIconColors,
                    EAknsCIQsnIconColorsCG13,
                    iconBitmap,
                    iconMask,
                    KIconFileName,
                    EMbmClockappQgn_indi_repeat_add,
                    EMbmClockappQgn_indi_repeat_add_mask,
                    TRgb( NULL, NULL, 255 ) );
            CleanupStack::PushL( iconBitmap );
            CleanupStack::PushL( iconMask );
            // Construct the icon.
            finalIcon = NULL;
            finalIcon = CGulIcon::NewL( iconBitmap, iconMask );
            CleanupStack::PushL( finalIcon );
            // Append the above icon to the array.
            iconArray->AppendL( finalIcon );
            // Cleanup.
            CleanupStack::Pop( finalIcon );
            CleanupStack::Pop( iconMask );
            CleanupStack::Pop( iconBitmap );
            
            // Read the inactive icon.
            iconBitmap = NULL;
            iconMask = NULL;
            
            // Create the color icon for the alarm inactive icon.
            AknsUtils::CreateColorIconL( AknsUtils::SkinInstance(),
                                         KAknsIIDQgnIndiAlarmInactiveAdd,
                                         KAknsIIDQsnIconColors,
                                         EAknsCIQsnIconColorsCG13,
                                         iconBitmap,
                                         iconMask,
                                         KIconFileName,
                                         EMbmClockappQgn_indi_alarm_inactive_add,
                                         EMbmClockappQgn_indi_alarm_inactive_add_mask,
                                         TRgb( NULL, NULL, 255 ) );
            CleanupStack::PushL( iconBitmap );
            CleanupStack::PushL( iconMask );
            
            // Construct the icon.
            finalIcon = CGulIcon::NewL( iconBitmap, iconMask );
            CleanupStack::PushL( finalIcon );
            // Append the above icon to the array.
            iconArray->AppendL( finalIcon );
            // Cleanup.
            CleanupStack::Pop( finalIcon );
            CleanupStack::Pop( iconMask );
            CleanupStack::Pop( iconBitmap );

            // Now load a blank icon.
            iconBitmap = new( ELeave ) CFbsBitmap();
            CleanupStack::PushL( iconBitmap );
            iconMask = new( ELeave ) CFbsBitmap();
            CleanupStack::PushL( iconMask );

            iconBitmap->Create( TSize( NULL, NULL ), ENone );
            iconMask->Create( TSize( NULL, NULL ), ENone );

            // Construct the icon.
            finalIcon = NULL;
            finalIcon = CGulIcon::NewL( iconBitmap, iconMask );
            CleanupStack::PushL( finalIcon );
            // Append it to the icon array.
            iconArray->AppendL( finalIcon );
            // Cleanup.
            CleanupStack::Pop( finalIcon );
            CleanupStack::Pop( iconMask );
            CleanupStack::Pop( iconBitmap );
            
            // Set the iconarray to the itemdrawer of the listbox.
            iListBox->ItemDrawer()->FormattedCellData()->SetIconArray( iconArray );
            CleanupStack::Pop( iconArray );
            iconArray = NULL;
            
            // Delete the DST icon and refresh it again.
            if( iDstIndicator )
                {
                delete iDstIndicator;
                iDstIndicator = NULL;
                }
            if( iDstIndicatorMask )
                {
                delete iDstIndicatorMask;
                iDstIndicatorMask = NULL;
                }
            AknsUtils::CreateColorIconL( AknsUtils::SkinInstance(),
                                         KAknsIIDQgnIndiDstAdd,
                                         KAknsIIDQsnIconColors,
                                         EAknsCIQsnIconColorsCG13,
                                         iDstIndicator,
                                         iDstIndicatorMask,
                                         AknIconUtils::AvkonIconFileName(),
                                         EMbmAvkonQgn_indi_dst_add,
                                         EMbmAvkonQgn_indi_dst_add_mask,
                                         TRgb( NULL, NULL, 255 ) );
            }
        }
		
	__PRINTS( "CClockMainContainer::HandleResourceChange - Exit" );
    }

// ---------------------------------------------------------
// CClockMainContainer::ConstructL
// rest of the details are commented in the header
// ---------------------------------------------------------
//    
void CClockMainContainer::ConstructL( CClockMainView* aView, const TRect& aRect, CClockAlarmArray* aAlarmArray )
    {
	__PRINTS( "CClockMainContainer::ConstructL - Entry" );
	
    iView = aView;
    iAlarmArray = aAlarmArray;
    
	CreateWindowL();
    
	// Construct the basic skin context.
	iBgContext = CAknsBasicBackgroundControlContext::NewL( KAknsIIDQsnBgAreaMain,
	                                                       aRect,
	                                                       EFalse );
	
	// Here we load the two icons for alarm and repeated alarm.
	CArrayPtr< CGulIcon >* iconArray =  new( ELeave ) CAknIconArray( KNumOfIcons );
	CleanupStack::PushL( iconArray );

	CFbsBitmap* iconBitmap( NULL );
	CFbsBitmap* iconMask( NULL );
	CGulIcon* finalIcon( NULL );

	// First read the alarm icon.
	AknsUtils::CreateColorIconL( AknsUtils::SkinInstance(),
	                             KAknsIIDQgnIndiAlarmAdd,
	                             KAknsIIDQsnIconColors,
	                             EAknsCIQsnIconColorsCG13,
	                             iconBitmap,
	                             iconMask,
	                             AknIconUtils::AvkonIconFileName(),
	                             EMbmAvkonQgn_indi_alarm_add,
	                             EMbmAvkonQgn_indi_alarm_add_mask,
	                             TRgb( NULL, NULL, 255 ) );
	CleanupStack::PushL( iconBitmap );
	CleanupStack::PushL( iconMask );
	// Construct the icon.
	finalIcon = CGulIcon::NewL( iconBitmap, iconMask );
	CleanupStack::PushL( finalIcon );
	// Append the above icon to the array.
	iconArray->AppendL( finalIcon );
	// Cleanup.
	CleanupStack::Pop( finalIcon );
	CleanupStack::Pop( iconMask );
	CleanupStack::Pop( iconBitmap );
	
	// Now load the repeat icon.
	iconBitmap = NULL;
	iconMask = NULL;
	
	AknsUtils::CreateColorIconL( AknsUtils::SkinInstance(),
	                             KAknsIIDQgnIndiRepeatAdd,
	                             KAknsIIDQsnIconColors,
	                             EAknsCIQsnIconColorsCG13,
	                             iconBitmap,
	                             iconMask,
	                             KIconFileName,
	                             EMbmClockappQgn_indi_repeat_add,
	                             EMbmClockappQgn_indi_repeat_add_mask,
	                             TRgb( NULL, NULL, 255 ) );
	CleanupStack::PushL( iconBitmap );
	CleanupStack::PushL( iconMask );
	// Construct the icon.
	finalIcon = NULL;
	finalIcon = CGulIcon::NewL( iconBitmap, iconMask );
	CleanupStack::PushL( finalIcon );
	// Append the above icon to the array.
	iconArray->AppendL( finalIcon );
	// Cleanup.
	CleanupStack::Pop( finalIcon );
	CleanupStack::Pop( iconMask );
	CleanupStack::Pop( iconBitmap );
	
	// Read the inactive icon.
    iconBitmap = NULL;
    iconMask = NULL;
    
    // Create the color icon for the alarm inactive icon.
    AknsUtils::CreateColorIconL( AknsUtils::SkinInstance(),
                                 KAknsIIDQgnIndiAlarmInactiveAdd,
                                 KAknsIIDQsnIconColors,
                                 EAknsCIQsnIconColorsCG13,
                                 iconBitmap,
                                 iconMask,
                                 KIconFileName,
                                 EMbmClockappQgn_indi_alarm_inactive_add,
                                 EMbmClockappQgn_indi_alarm_inactive_add_mask,
                                 TRgb( NULL, NULL, 255 ) );
    CleanupStack::PushL( iconBitmap );
    CleanupStack::PushL( iconMask );
    
    // Construct the icon.
    finalIcon = CGulIcon::NewL( iconBitmap, iconMask );
    CleanupStack::PushL( finalIcon );
    // Append the above icon to the array.
    iconArray->AppendL( finalIcon );
    // Cleanup.
    CleanupStack::Pop( finalIcon );
    CleanupStack::Pop( iconMask );
    CleanupStack::Pop( iconBitmap );
	
	// Now load a blank icon.
	iconBitmap = new( ELeave ) CFbsBitmap();
	CleanupStack::PushL( iconBitmap );
	iconMask = new( ELeave ) CFbsBitmap();
	CleanupStack::PushL( iconMask );
	
	iconBitmap->Create( TSize( NULL, NULL ), ENone );
	iconMask->Create( TSize( NULL, NULL ), ENone );
	
	// Construct the icon.
	finalIcon = NULL;
	finalIcon = CGulIcon::NewL( iconBitmap, iconMask );
	CleanupStack::PushL( finalIcon );
	// Append it to the icon array.
	iconArray->AppendL( finalIcon );
	// Cleanup.
	CleanupStack::Pop( finalIcon );
	CleanupStack::Pop( iconMask );
	CleanupStack::Pop( iconBitmap );
	
	// Construct the listbox.
	//Single click integration
    iListBox = new( ELeave ) CAknDoubleStyleListBox;

	iListBox->ConstructL( this, NULL );
	iListBox->SetContainerWindowL( *this );
	
	// Set the display properties of the listbox.
	iListBox->CreateScrollBarFrameL( ETrue );
	iListBox->ScrollBarFrame()->SetScrollBarVisibilityL( CEikScrollBarFrame::EOff,
														 CEikScrollBarFrame::EAuto );
	iListBox->Model()->SetItemTextArray( iAlarmArray );
	iListBox->ItemDrawer()->FormattedCellData()->SetIconArray( iconArray );
	iListBox->Model()->SetOwnershipType( ELbmDoesNotOwnItemArray );
	
	// Set the observer of the listbox events.
	iListBox->SetListBoxObserver( this );
	//Single click integration
    // Disable list stretching so alarm items are displayed correctly
    iListBox->EnableStretching( EFalse ); 



	// Set the listbox layout.
	TAknLayoutRect listBoxRect;
	TInt layoutOrientation = Layout_Meta_Data::IsLandscapeOrientation() ? 1 : 0;
	
	TAknLayoutRect listRect;
	// Get the main clock rect.
	listRect.LayoutRect( aRect, AknLayoutScalable_Apps::listscroll_gen_pane_cp06( layoutOrientation ).LayoutLine() );

	// Get the skinnable clock rect.
	listBoxRect.LayoutRect( listRect.Rect(), AknLayoutScalable_Apps::listscroll_gen_pane_cp06( layoutOrientation ).LayoutLine() );
	iListBox->SetRect( listBoxRect.Rect() );//.iTl, listBoxRect.Rect().Size() );
	
    // Set the empty list text.
    iListBox->View()->SetListEmptyTextL( _L("") );
    iListBox->ActivateL();
	
    // Cleanup.
	CleanupStack::Pop( iconArray );

    // construct the "New Alarm" button
    iNewAlarmButton = CAknButton::NewL( NULL, NULL, NULL, NULL,
            iAlarmArray->NewAlarmText()->Des(),iAlarmArray->NewAlarmText()->Des(), 0, 0 );
    iNewAlarmButton->SetParent( this );
    iNewAlarmButton->SetContainerWindowL( *this );
    //TAknLayoutRect newAlarmButtonRect;
    // Get the skinnable clock rect.
    //newAlarmButtonRect.LayoutRect( listRect.Rect(),
    //        AknLayoutScalable_Apps::main_clock2_btn_pane( layoutOrientation  ).LayoutLine() );
    
    //iNewAlarmButton->SetRect( newAlarmButtonRect.Rect() );
    SetCorrectRectForNewAlarmButton();
    iNewAlarmButton->SetObserver( this );
    iNewAlarmButton->ActivateL();
    // Add the icon for DST indicator.
    AknsUtils::CreateColorIconL( AknsUtils::SkinInstance(),
                                 KAknsIIDQgnIndiDstAdd,
                                 KAknsIIDQsnIconColors,
                                 EAknsCIQsnIconColorsCG13,
                                 iDstIndicator,
                                 iDstIndicatorMask,
                                 AknIconUtils::AvkonIconFileName(),
                                 EMbmAvkonQgn_indi_dst_add,
                                 EMbmAvkonQgn_indi_dst_add_mask,
                                 TRgb( NULL, NULL, 255 ) );
    
    iLocationLabel = new ( ELeave ) CClockMarqueeLabel();
    iLocationLabel->SetContainerWindowL( *this );
    iLocationLabel->ConstructL( ETrue );
    iLocationLabel->SetTextL( KNullDesC );
    iLocationLabel->SetBackground( iBgContext );
    iLocationLabel->MakeVisible( ETrue );
    
    TAknLayoutRect mainRect;
    // Get the main clock rect.
    mainRect.LayoutRect( aRect, AknLayoutScalable_Apps::main_clock2_pane().LayoutLine() );

    TAknLayoutRect skinClockRect;
    TInt displayOrientation( Layout_Meta_Data::IsLandscapeOrientation() ? KVGALandscapeVariant : KVGAPortraitVariant );
    // Get the skinnable clock rect.
    skinClockRect.LayoutRect( mainRect.Rect(),
            AknLayoutScalable_Apps::popup_clock_analogue_window_cp03( displayOrientation ).LayoutLine() );

    // Construct the skinnable clock.
    iSkinnableClock = CAknSkinnableClock::NewL( this, ETrue, EFalse );
    // Set the size of the clock.
    iSkinnableClock->SetExtent( skinClockRect.Rect().iTl, skinClockRect.Rect().Size() );
    
    SetRect( aRect );
    
    iSkinnableClock->ActivateL();
    							 
	__PRINTS( "CClockMainContainer::ConstructL - Exit" );
    }

// ---------------------------------------------------------
// CClockMainContainer::CClockMainContainer
// rest of the details are commented in the header
// ---------------------------------------------------------
//    
CClockMainContainer::CClockMainContainer()
    {
	__PRINTS( "CClockMainContainer::CClockMainContainer - Entry" );
	
    // No implementation yet.
	
	__PRINTS( "CClockMainContainer::CClockMainContainer - Exit" );
    }

// ---------------------------------------------------------
// CClockMainContainer::DrawMainViewText
// rest of the details are commented in the header
// ---------------------------------------------------------
//  
void CClockMainContainer::DrawMainViewText( CWindowGc& aWindowGc ) const
    {
	__PRINTS( "CClockMainContainer::DrawMainViewText - Entry" );
	
    TAknLayoutText layoutText;
    MAknsSkinInstance* skinInstance = AknsUtils::SkinInstance();
    TRgb cachedColor;
    TInt displayOrientation( Layout_Meta_Data::IsLandscapeOrientation() ? KVGALandscapeVariant : KVGAPortraitVariant );

    // Get the cached color.
    AknsUtils::GetCachedColor( skinInstance, cachedColor, KAknsIIDQsnTextColors, EAknsCIQsnTextColorsCG6 );

    TAknLayoutRect parentRect;
    parentRect.LayoutRect( Rect(), AknLayoutScalable_Apps::main_clock2_pane().LayoutLine() );
        
    TRect mainClockPane( parentRect.Rect() );
   
    // Get the layout for the day string.
    layoutText.LayoutText( mainClockPane, AknLayoutScalable_Apps::main_clock2_pane_t1( displayOrientation ).LayoutLine() );

    TPtr dayStringPtr = iDayNameString->Des();
    // Day long format.
    layoutText.DrawText( aWindowGc, *iDayNameString, ETrue, cachedColor );
    
    // Get the layout for the datestring.
    layoutText.LayoutText( mainClockPane, AknLayoutScalable_Apps::main_clock2_pane_t2( displayOrientation ).LayoutLine() );

    TPtr dateStringPtr = iDateString->Des();
    // For mirrored languages.
    AknTextUtils::DisplayTextLanguageSpecificNumberConversion( dateStringPtr );
    // Date long format
    layoutText.DrawText( aWindowGc, *iDateString, ETrue, cachedColor );
    
    // Get the layout for the timezone string.
    layoutText.LayoutText( mainClockPane, AknLayoutScalable_Apps::main_clock2_pane_t3( Layout_Meta_Data::IsLandscapeOrientation() ).LayoutLine() );

    TPtr timeZonePtr = iTimeZoneString->Des();
    // This is for mirrored languages.
    AknTextUtils::DisplayTextLanguageSpecificNumberConversion( timeZonePtr );
    // The timezone string.
    layoutText.DrawText( aWindowGc, *iTimeZoneString, ETrue, cachedColor );
    
    // Get the layout for the country string.
    TAknTextComponentLayout labelLayout;
    labelLayout = AknLayoutScalable_Apps::main_clock2_pane_t4( Layout_Meta_Data::IsLandscapeOrientation() );
    AknLayoutUtils::LayoutLabel( iLocationLabel, parentRect.Rect(), labelLayout.LayoutLine() );
    
    // Set the label for the text and start marqueeing.
    iLocationLabel->SetLabelTextL( *iLocationString );
    iLocationLabel->StartMarquee();
    iLocationLabel->RedrawRect( iLocationLabel->Rect() );

	// We display empty text only if there are no alarms set.
	// if( !iAlarmArray->MdcaCount() )
		// {
		// Set the listbox layout.
		// TAknLayoutRect listRect;
		// Get the main clock rect.
		// listRect.LayoutRect( mainClockPane, AknLayoutScalable_Apps::listscroll_gen_pane_cp06( Layout_Meta_Data::IsLandscapeOrientation() ).LayoutLine() );
		
		// Get the rect for the empty text.
		// layoutText.LayoutText( listRect.Rect(), AknLayoutScalable_Apps::listscroll_gen_pane_cp06_t1( Layout_Meta_Data::IsLandscapeOrientation() ).LayoutLine() );
		// Draw the text.
		// layoutText.DrawText( aWindowGc, *iNoAlarmsText, ETrue, cachedColor );
		//}
    
	__PRINTS( "CClockMainContainer::DrawMainViewText - Exit" );
    }

// ---------------------------------------------------------
// CClockMainContainer::IsDstOnL
// rest of the details are commented in the header
// ---------------------------------------------------------
// 
TBool CClockMainContainer::IsDstOnL( TInt aTimeZoneId )
    {
	__PRINTS( "CClockMainContainer::IsDstOnL - Entry" );
	
    TBool returnVal( EFalse );
    CTzId* timeZoneId = CTzId::NewL( aTimeZoneId );
    CleanupStack::PushL( timeZoneId );
    
    RTz tzHandle;
    User::LeaveIfError( tzHandle.Connect() );
    CleanupClosePushL( tzHandle );
    
    returnVal = tzHandle.IsDaylightSavingOnL( *timeZoneId );

    tzHandle.Close();
    CleanupStack::PopAndDestroy( &tzHandle );
    CleanupStack::PopAndDestroy( timeZoneId );
	
	__PRINTS( "CClockMainContainer::IsDstOnL - Exit" );
    
    return returnVal;
    }

// ---------------------------------------------------------
// CClockMainContainer::GetOffSetWithDstL
// rest of the details are commented in the header
// ---------------------------------------------------------
// 
void CClockMainContainer::GetOffSetWithDstL( TInt32& aDstOffset, const CTzId& aTzId )
    {
	__PRINTS( "CClockMainContainer::GetOffSetWithDstL - Entry" );
	
    RTz rTz;
    User::LeaveIfError( rTz.Connect() );
    CleanupClosePushL( rTz );
    
    // Local time
    TTime homeTime;
    homeTime.HomeTime();    
    
    TDateTime dateTime;
    dateTime = homeTime.DateTime();
    
    // Get the CTzRules for the current year and for the given time zone id.
    CTzRules* tzRules = rTz.GetTimeZoneRulesL( aTzId, 
                                               dateTime.Year(), 
                                               dateTime.Year(), 
                                               ETzWallTimeReference );
    CleanupStack::PushL( tzRules );
    
    // Get the Actualised rules for the same year. These are the DST rules from which we get the iNewOffset.
    CVTzActualisedRules *vActualisedRules = CVTzActualisedRules::NewL(
                                                    homeTime.DateTime().Year(), 
                                                    homeTime.DateTime().Year() );
    CleanupStack::PushL( vActualisedRules );
    tzRules->GetActualisedRulesL( *vActualisedRules );
    
    // This way of fetching initial offset is being used rather than tzRules->InitialStdTimeOffset()
    // because in some cases, the offset returned is incorrect. (For ex: Argentina and Canada/Iqaluit)
    RArray< TInt > timeZones;
    RArray< TInt > zoneOffsets;

    // Append the current timezone ID and request for the standard offset
    timeZones.Append( aTzId.TimeZoneNumericID() );
    rTz.GetOffsetsForTimeZoneIdsL( timeZones, zoneOffsets );
    
    // The initial offset or the standard offset (w/o DST)
    TInt initialTimeZoneOffset = zoneOffsets[ 0 ];
    
    // Close the resource handles
    timeZones.Close();
    zoneOffsets.Close();
    
    // The number of actualised rules
    TInt ruleCount = vActualisedRules->Count();
    
    for( TInt ruleIndex( KZerothRule ); ruleIndex < ruleCount; ruleIndex++ )
        {
        const TVTzActualisedRule& tVTzactRule = ( *vActualisedRules )[ ruleIndex ];
        
        // If the standard offset and the new offset do not match then we have a dst offset.
        // Technically if a timezone has DST then it can have a max of two offsets. One is the standard which doesn't show the 
        // DST usage, and the other is the DST offset which is standard offset + the amount of DST
        if( initialTimeZoneOffset != tVTzactRule.iNewOffset )
            {
            aDstOffset = tVTzactRule.iNewOffset;
            CleanupStack::PopAndDestroy( vActualisedRules );
            CleanupStack::PopAndDestroy( tzRules );
            CleanupStack::PopAndDestroy( &rTz );
			
			__PRINTS( "CClockMainContainer::GetOffSetWithDstL - Exit" );
			
            return;
            }
        }
    CleanupStack::PopAndDestroy( vActualisedRules );
    CleanupStack::PopAndDestroy( tzRules );
    CleanupStack::PopAndDestroy( &rTz );
	
	__PRINTS( "CClockMainContainer::GetOffSetWithDstL - Exit" );
    }

// ---------------------------------------------------------
// CClockMainContainer::GetHelpContext
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClockMainContainer::GetHelpContext( TCoeHelpContext& aContext ) const
{
    aContext.iMajor = KClockAppUid;
    if( GetAutoTimeUpdateState () )
        {
        aContext.iContext = KCLK_HLP_REAL_TIME_CLOCK_AN;
        }
    else
        {
        aContext.iContext = KCLK_HLP_REAL_TIME_CLOCK_IN;
        }
}

// ---------------------------------------------------------
// CClockMainContainer::GetAutoTimeUpdateState
// rest of the details are commented in the header
// ---------------------------------------------------------
//
TBool CClockMainContainer::GetAutoTimeUpdateState() const
{
        // Check if automatic time update is enabled.
        TBool timeUpdateOn( EFalse );       
        RClkSrvInterface clkSrvIf;  

        // Connect to clockserver
        TInt errorVal( clkSrvIf.Connect() );

        if( KErrNone == errorVal )
            {
            // First check if automatic time update is enabled.   
            errorVal = clkSrvIf.IsAutoTimeUpdateOn( timeUpdateOn );
            }
        return timeUpdateOn;
}

void CClockMainContainer::HandleControlEventL( CCoeControl* aControl,
            TCoeEvent aEventType )
    {
    if ( aControl == iNewAlarmButton && aEventType == MCoeControlObserver::EEventStateChanged )
        {
        iView->HandleCommandL( EClockNewAlarm );
        }
    }

void CClockMainContainer::SetCorrectRectForNewAlarmButton()
    {
    if( iNewAlarmButton )
        {
        TRect paneRect = this->Rect();
        TAknLayoutRect newAlarmButtonRect;
        TAknLayoutRect mainRect;
        TInt displayOrientation( Layout_Meta_Data::IsLandscapeOrientation() ? 1 : 0 );
        // Get the layout for the main rect.
        mainRect.LayoutRect( paneRect,
                AknLayoutScalable_Apps::main_clock2_pane().LayoutLine() );
        if( displayOrientation )
            {
            // landscape
            TInt alarmCount( 0 );
            if( iListBox )
                {
                alarmCount = iListBox->Model()->ItemTextArray()->MdcaCount();
                }
            if( alarmCount > 0 )
                {
            if( iListBox )
                {
                iListBox->MakeVisible( ETrue );
                }
                // landscape if not empty alarm list
                newAlarmButtonRect.LayoutRect( mainRect.Rect(),
                        AknLayoutScalable_Apps::main_clock2_btn_pane( displayOrientation ).LayoutLine() );
                }
            else
                {
                // landscape if empty alarm list
                if( iListBox )
                    {
                    iListBox->MakeVisible( EFalse );
                    }
                newAlarmButtonRect.LayoutRect( mainRect.Rect(),
                        AknLayoutScalable_Apps::main_clock2_btn_pane_cp01( displayOrientation ).LayoutLine() );
                }
            }
        else
            {
            // portrait
            if( iListBox )
                {
                iListBox->MakeVisible( ETrue );
                }
            newAlarmButtonRect.LayoutRect( mainRect.Rect(),
                    AknLayoutScalable_Apps::main_clock2_btn_pane( displayOrientation ).LayoutLine() );
            }
        iNewAlarmButton->SetRect( newAlarmButtonRect.Rect() );
        }

    }

void CClockMainContainer::SwitchClockTypeL()
    {
    TRect skinClockRect = iSkinnableClock->Rect();
    delete iSkinnableClock;
    iSkinnableClock = NULL;
    // Construct the skinnable clock.
    iSkinnableClock = CAknSkinnableClock::NewL( this, ETrue, EFalse );
    // Set the size of the clock.
    iSkinnableClock->SetExtent( skinClockRect.iTl, skinClockRect.Size() );
    iSkinnableClock->ActivateL();
    iSkinnableClock->DrawDeferred();
    }
// End of file
