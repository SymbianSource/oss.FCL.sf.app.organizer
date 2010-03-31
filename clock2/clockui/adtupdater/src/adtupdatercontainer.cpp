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
* Description:   This is the source file for the CAdtUpdaterContainer class.
*
*/

// System includes
#include <AknsBasicBackgroundControlContext.h>
#include <AknsSkinInstance.h>
#include <AknsUtils.h>
#include <AknsDrawUtils.h>
#include <AknGlobalNote.h>
#include <StringLoader.h>
#include <adtupdater.rsg>
#include <avkon.rsg>
#include <tzlocalizer.h>
#include <aknmessagequerydialog.h>
#include <tz.h>
#include <AknGlobalMsgQuery.h>
#include <badesca.h>
#include <aknlists.h>
#include <centralrepository.h>
#include <starterdomaincrkeys.h>
#include <startupdomainpskeys.h> 
#include <startupdomaincrkeys.h>
#include <starterclient.h>

// User includes
#include "adtupdatercontainer.h"
#include "adtupdaterappui.h"
#include "adtupdaterlistener.h"
#include "adtupdaterpopuplist.h"
#include "adtupdaterquerydialog.h"
#include "clocktimezoneresolver.h"
#include "clock_debug.h"
#include <uikon/eiksrvui.h>

// Constants
const TInt KNitzPlugin( 0x200159A5 );
const TInt KTimeout( 40000000 );				// 40 seconds
const TInt KInterval( 5000000 );				// 5 seconds
const TInt KMaximumTwoDigitValue( 99 );
const TInt KNitzYearOffset( 2000 );
const TInt KMaxCityLength( 120 );
const TInt KMaxCountryLength( 120 );
const TInt KSingleCity( 1 );
const TInt KZero( 0 );
const TInt KFirstBootDone( 1 );
const TInt KTimeFormatLength( 16 );				 // "20070000:090000."
const TInt KMaxMobileCountryCode( 4 );


// Literals
_LIT( KOneSpace, " " );
_LIT( KMobileCountryCode, "%d" );
_LIT( KDefaultDateTimeValue, "20070000:090000." ); // YYYYMMDD:HHMMSS.
_LIT( KDefault, "default" );

// ---------------------------------------------------------
// CAdtUpdaterContainer::NewL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
CAdtUpdaterContainer* CAdtUpdaterContainer::NewL( const TRect& aRect )
    {
    __PRINTS( "CAdtUpdaterContainer::NewL - Entry" );
    
    CAdtUpdaterContainer* self = new ( ELeave ) CAdtUpdaterContainer;
    CleanupStack::PushL( self );
    
    self->ConstructL( aRect );
    
    CleanupStack::Pop( self );
    
    __PRINTS( "CAdtUpdaterContainer::NewL - Exit" );
    
    return self;
    }

// ---------------------------------------------------------
// CAdtUpdaterContainer::~CAdtUpdaterContainer
// rest of the details are commented in the header
// ---------------------------------------------------------
//
CAdtUpdaterContainer::~CAdtUpdaterContainer()
    {
    __PRINTS( "CAdtUpdaterContainer::~CAdtUpdaterContainer - Entry" );
    
	if( iBgContext )
		{
		delete iBgContext;
		iBgContext = NULL;
		}
	if( iPeriodic )
		{
		delete iPeriodic;
		iPeriodic = NULL;
		}
	if( iListener )
		{
		delete iListener;
		iListener = NULL;
		}
	if( iGMTString )
		{
		delete iGMTString;
		iGMTString = NULL;
		}
	if( iGMTText )
		{
		delete iGMTText;
		iGMTText = NULL;
		}
	
	__PRINTS( "CAdtUpdaterContainer::~CAdtUpdaterContainer - Exit" );
    }

// ---------------------------------------------------------
// CAdtUpdaterContainer::CAdtUpdaterContainer
// rest of the details are commented in the header
// ---------------------------------------------------------
//    
CAdtUpdaterContainer::CAdtUpdaterContainer() : iDataAvailability( EDataNone ), iQueryDialogsInDisplay( EFalse )
    {
    __PRINTS( "CAdtUpdaterContainer::CAdtUpdaterContainer - Entry" );
    
    //No Implementation yet
    
    __PRINTS( "CAdtUpdaterContainer::CAdtUpdaterContainer - Exit" );
    }

// ---------------------------------------------------------
// CAdtUpdaterContainer::ConstructL
// rest of the details are commented in the header
// ---------------------------------------------------------
//    
void CAdtUpdaterContainer::ConstructL( const TRect& aRect )
    {
    __PRINTS( "CAdtUpdaterContainer::ConstructL - Entry" );
    
	CreateWindowL();
	
	iAdtUpdaterAppUi =  static_cast < CAdtUpdaterAppUi* > ( CCoeEnv::Static()->AppUi() );
	
	SetRect( aRect );
	
	// Construct the background control context.
    iBgContext = CAknsBasicBackgroundControlContext::NewL( KAknsIIDQsnBgScreen,
    													   aRect.Size(),
    													   EFalse );
    
    // set the flag for showing startup queries
    iDisplayStartupQueries = DisplayStartupQueriesL();
    
    
    ActivateL();
  
    __PRINTS( "CAdtUpdaterContainer::ConstructL - Exit" );
    }     

// ---------------------------------------------------------
// CAdtUpdaterContainer::OfferKeyEventL
// rest of the details are commented in the header
// ---------------------------------------------------------
//    
TKeyResponse CAdtUpdaterContainer::OfferKeyEventL( const TKeyEvent& /*aKeyEvent*/, 
												   TEventCode /*aType*/ )
    {
    __PRINTS( "CAdtUpdaterContainer::OfferKeyEventL - Entry" );
    
    TKeyResponse ret( EKeyWasConsumed );
    
    __PRINTS( "CAdtUpdaterContainer::OfferKeyEventL - Exit" );
    
    return ret;
    }

// ---------------------------------------------------------
// CAdtUpdaterContainer::CountComponentControls
// rest of the details are commented in the header
// ---------------------------------------------------------
//
TInt CAdtUpdaterContainer::CountComponentControls() const
	{
	__PRINTS( "CAdtUpdaterContainer::CountComponentControls - Entry" );
	
	__PRINTS( "CAdtUpdaterContainer::CountComponentControls - Exit" );
	
	return 0;
	}

// ---------------------------------------------------------
// CAdtUpdaterContainer::ComponentControl
// rest of the details are commented in the header
// ---------------------------------------------------------
//	
CCoeControl* CAdtUpdaterContainer::ComponentControl( TInt /*aIndex*/ ) const
	{
	__PRINTS( "CAdtUpdaterContainer::ComponentControl - Entry" );
	
	__PRINTS( "CAdtUpdaterContainer::ComponentControl - Exit" );
	
	return NULL;
	}

// ---------------------------------------------------------
// CAdtUpdaterContainer::Draw
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CAdtUpdaterContainer::Draw( const TRect& aRect ) const
	{
	__PRINTS( "CAdtUpdaterContainer::Draw - Entry" );
	
	CWindowGc& gc = SystemGc();
	gc.Clear( aRect );
	
	if( iBgContext )
		{
		MAknsSkinInstance* skinInstance = AknsUtils::SkinInstance();
		AknsDrawUtils::Background( skinInstance, iBgContext, this, gc, aRect );
		}
	
	__PRINTS( "CAdtUpdaterContainer::Draw - Exit" );
    }

// ---------------------------------------------------------
// CAdtUpdaterContainer::SizeChanged
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CAdtUpdaterContainer::SizeChanged()
	{
	__PRINTS( "CAdtUpdaterContainer::SizeChanged - Entry" );
	
	if( iBgContext )
	    {
	    iBgContext->SetRect( Rect() );
	    }
	__PRINTS( "CAdtUpdaterContainer::SizeChanged - Exit" );
	}

// ---------------------------------------------------------
// CAdtUpdaterContainer::MopSupplyObject
// rest of the details are commented in the header
// ---------------------------------------------------------
//
TTypeUid::Ptr CAdtUpdaterContainer::MopSupplyObject( TTypeUid aId )
    {
    __PRINTS( "CAdtUpdaterContainer::MopSupplyObject - Entry" );
    
    if( MAknsControlContext::ETypeId == aId.iUid )
        {
        return MAknsControlContext::SupplyMopObject( aId, iBgContext );
        }
    
    __PRINTS( "CAdtUpdaterContainer::MopSupplyObject - Exit" );
    
    return CCoeControl::MopSupplyObject( aId );
    }
    
// ---------------------------------------------------------
// CAdtUpdaterContainer::HandleResourceChange
// rest of the details are commented in the header
// ---------------------------------------------------------
//    
void CAdtUpdaterContainer::HandleResourceChange( TInt aType )
	{
	__PRINTS( "CAdtUpdaterContainer::HandleResourceChange - Entry" );
	
	// Forward the control to CCoeControl.
	CCoeControl::HandleResourceChange( aType );
	
	__PRINTS( "CAdtUpdaterContainer::HandleResourceChange - Exit" );
	}

// ---------------------------------------------------------
// CAdtUpdaterContainer::InformAboutNwUpdateL
// rest of the details are commented in the header
// ---------------------------------------------------------
//	
void CAdtUpdaterContainer::InformAboutNwUpdateL()
	{
	__PRINTS( "CAdtUpdaterContainer::InformAboutNwUpdate - Entry" );

	if( IsFirstBoot() &&  IsSimPresent() && IsNitzPluginActive() && PredictiveTimeEnabled())
		{
		__PRINTS( "First boot, sim is present and auto time update is ON" );
		
		// First bring the application to the foreground.
		iAdtUpdaterAppUi->ToggleAppViewL( EFalse );
		// Hide in FSW.
		iAdtUpdaterAppUi->HideApplicationFromFSW( ETrue );
		// Hide the status pane.
		iAdtUpdaterAppUi->HideStatusPane( ETrue );
		
		// Listener object which listens for a Nitz packet
		iListener = CAdtUpdaterListener::NewL( this );
		iListener->WaitForNitzInfoL();
		
		// A 40 seconds call back timer.
		TCallBack timerCallBack( CallBackL, this );
		iPeriodic = CPeriodic::NewL( EPriorityAbsoluteHigh );
		// Start the timer	
		iPeriodic->Start( KTimeout, KInterval, timerCallBack );
		}
	else if( IsFirstBoot()&& PredictiveTimeEnabled() )
		{	
		__PRINTS( "First boot, normal boot sequence" );
		
		// The normal bootup sequence.		
		ContinueWithNormalBootL();
		
		__PRINTS( "CAdtUpdaterContainer::InformAboutNwUpdate : 2nd condition - Exit" );
		
		// Exit the application.
		iAdtUpdaterAppUi->Exit();
		}
	else if( !RTCStatus() && !HiddenReset() && PredictiveTimeEnabled())
		{
		__PRINTS( "RTC invalid or Hidden Reset" );
		
		// Show in FSW.
	    iAdtUpdaterAppUi->HideApplicationFromFSW( EFalse );
		// Hide the status pane.
	    iAdtUpdaterAppUi->HideStatusPane( ETrue );
		
		// No first boot but RTCStatus is corrupted. Ask time and date"
		
		// Showing Date query to user.
		ShowDateQueryL();
		
		// Showing Time query to user.
		ShowTimeQueryL();
		
		//Deactivate the plug-in as we are setting the date/time manually
		DeActivateNitzPlugin();
		
		__PRINTS( "CAdtUpdaterContainer::InformAboutNwUpdate : 3rd condition - Exit" );
		
		// Exit the application.
		iAdtUpdaterAppUi->Exit();
		}		
	else
		{
		__PRINTS( "Not the first boot, simply exit" );
		
		// No need ot execute further as it's not the first boot. Exit the application.
		User::Exit( KErrNone );
		}
	
	__PRINTS( "CAdtUpdaterContainer::InformAboutNwUpdate - Exit" );
	}

// ---------------------------------------------------------
// CAdtUpdaterContainer::InformAboutNwUpdate
// rest of the details are commented in the header
// ---------------------------------------------------------
//
TInt CAdtUpdaterContainer::CallBackL( TAny* aPtr )
	{
	__PRINTS( "CAdtUpdaterContainer::CallBackL - Entry" );
	
	CAdtUpdaterContainer* selfObject = static_cast< CAdtUpdaterContainer* >( aPtr );
	CAdtUpdaterListener* listenerObject = selfObject->Listener();
	
	// Callback after 40 seconds. If control reaches here, it means Nitz packet has
	// not yet arrived and we can continue with normal bootup sequence.
	
	// Buf before we do that, we can do a final check with the server, just to ensure
	// there weren't any missed notifications.
	TBool dataAvailable = listenerObject->ConfirmDataAvailabilityL();
	
	if( dataAvailable )
	    {
	    __PRINTS( "NITZ info is available" );
	    
	    // Update the status to EDataAvailable
	    selfObject->iDataAvailability = EDataAvailable;
	    
	    // We have data from server.
	    listenerObject->NitzInfoAvailableL();
	    }
	else
	    {
	    __PRINTS( "NITZ info unavailable" );
	    
	    // Update the status to EDataNotAvailable
	    selfObject->iDataAvailability = EDataNotAvailable;
	    
	    // We don't have any data for sure.
	    selfObject->NitzInfoNotAvailableL();
	    }
		
	__PRINTS( "CAdtUpdaterContainer::CallBackL - Exit" );
											
	return KZero;
	}

// ---------------------------------------------------------
// CAdtUpdaterContainer::NitzInfoNotAvailableL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CAdtUpdaterContainer::NitzInfoNotAvailableL()
	{
	__PRINTS( "CAdtUpdaterContainer::NitzInfoNotAvailableL - Entry" );
	
	iTimeOut = ETrue;
	// First cancel all requests.
	CancelAllRequests();
	
	// The normal bootup sequence.
	ContinueWithNormalBootL();

	__PRINTS( "CAdtUpdaterContainer::NitzInfoNotAvailableL - Exit" );
	
	// We're done. Exit the application.
	iAdtUpdaterAppUi->Exit();
	}

// ---------------------------------------------------------
// CAdtUpdaterContainer::NitzInfoAvailableL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CAdtUpdaterContainer::NitzInfoAvailableL( STimeAttributes aTimeAttributes, TInt aTimeZoneId )
	{
	__PRINTS( "CAdtUpdaterContainer::NitzInfoAvailableL - Entry" );
	
	__PRINTS( "!iTimeOut" );
	
	// Check if the Nitz packet has arrived before timeout. 
	// This is just a sanity check. Since this will function will be called by a different thread.
	if( !iTimeOut )	
		{
		__PRINT( "Received timezoneid: %d", aTimeZoneId );
		
		// First store the values.
		iTimeAttributes = aTimeAttributes;
		iTimeZoneId = aTimeZoneId;
		
		// Cancel all requests and timers.
		CancelAllRequests();
		
		// Display the Nitz info.
		DisplayNitzInfoL();
		
		// We're done. Exit the application.
		iAdtUpdaterAppUi->Exit();
		}
	
	__PRINTS( "CAdtUpdaterContainer::NitzInfoAvailableL - Exit" );
	}

// ---------------------------------------------------------
// CAdtUpdaterContainer::CancelAllRequests
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CAdtUpdaterContainer::CancelAllRequests()
	{
	__PRINTS( "CAdtUpdaterContainer::CancelAllRequests - Entry" );
	
	// Cancel the timer.
	if( iPeriodic )
		{
		iPeriodic->Cancel();
		}
	
	// Cancel the listener
	if( iListener )
		{
		iListener->Cancel();
		}
		
	__PRINTS( "CAdtUpdaterContainer::CancelAllRequests - Exit" );
	}

// ---------------------------------------------------------
// CAdtUpdaterContainer::DisplayNitzInfoL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CAdtUpdaterContainer::DisplayNitzInfoL()
	{
	__PRINTS( "CAdtUpdaterContainer::DisplayNitzInfoL - Entry" );
	
	// Construct CTzLocalizer object to get the name of the zone.
	CTzLocalizer* tzLocalizer = CTzLocalizer::NewL();
	CTzLocalizedCity* localizedCity( NULL );
	CTzLocalizedTimeZone* localizedTimeZone( NULL );

	TRAPD( errorVal, localizedTimeZone = tzLocalizer->GetLocalizedTimeZoneL( iTimeZoneId ) );
	
	__PRINT( "Received timezoneid: %d errorVal: %d ", iTimeZoneId, errorVal );

	if( KErrNone == errorVal )
		{
		if( localizedTimeZone ) 
			{
			__PRINTS( "CAdtUpdaterContainer::DisplayNitzInfoL - Localized TimeZone" );
			
			// Match found!! Get the city corresponding to this timezone.
			localizedCity = tzLocalizer->GetDefaultCityL( localizedTimeZone->TimeZoneId() );
			}
		}

	// If no matching city has been found, continue with normal bootup
	if( !localizedCity ) 
		{
		__PRINTS( "CAdtUpdaterContainer::DisplayNitzInfoL - No Localized default city" );
		
		NitzInfoNotAvailableL();
		
		// Cleanup
		delete localizedTimeZone;
		delete tzLocalizer;
		
		__PRINTS( "CAdtUpdaterContainer::DisplayNitzInfoL - Exit: No Matching city found" );
		
		return;
		}        
    
	// We have the timezone. Get the date and time information.
	// Check if we have 2 digits instead of 4 digits for the year value.
	TInt fourDigitYear( iTimeAttributes.iUtcDateTime.Year() );

	if( fourDigitYear <= KMaximumTwoDigitValue ) 
		{
		// The year received from TSY is 2 digits we make it 4 digits
		fourDigitYear = fourDigitYear + KNitzYearOffset;  // 2000   
		}

	TDateTime dateTime( fourDigitYear,
						TMonth( iTimeAttributes.iUtcDateTime.Month() ),
						iTimeAttributes.iUtcDateTime.Day(),
						iTimeAttributes.iUtcDateTime.Hour(),
						iTimeAttributes.iUtcDateTime.Minute(),
						iTimeAttributes.iUtcDateTime.Second(),
						iTimeAttributes.iUtcDateTime.MicroSecond() );

	// Time sent by networks is the UTC time.
	TTime nwUtcTime( dateTime );
	
	// Set the timezone and UTC time before displaying the info to the user.
	// First set the timezone.
	tzLocalizer->SetTimeZoneL( iTimeZoneId );
	tzLocalizer->SetFrequentlyUsedZoneL( *localizedCity, CTzLocalizedTimeZone::ECurrentZone );
	
	// Now set the UTC time.
	RTz tzHandle;
	User::LeaveIfError( tzHandle.Connect() );	
	User::LeaveIfError( User::SetUTCTime( nwUtcTime ) );	
	// Cleanup
	tzHandle.Close();

	// Marking this boot as first boot.
	MarkFirstBoot();

    delete localizedTimeZone;
	localizedTimeZone = NULL;
	
	delete localizedCity; 
	localizedCity = NULL;
	
	delete tzLocalizer;  
	tzLocalizer = NULL;
	
	__PRINTS( "CAdtUpdaterContainer::DisplayNitzInfoL - Exit" );
	}

// ---------------------------------------------------------
// CAdtUpdaterContainer::ContinueWithNormalBootL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CAdtUpdaterContainer::ContinueWithNormalBootL()
	{
	__PRINTS( "CAdtUpdaterContainer::ContinueWithNormalBootL - Entry" );
	
	// First bring the application to the foreground.
	iAdtUpdaterAppUi->ToggleAppViewL( ETrue );	
	// Show in FSW.
    iAdtUpdaterAppUi->HideApplicationFromFSW( EFalse );
	// Hide the status pane.
    iAdtUpdaterAppUi->HideStatusPane( ETrue );
	
	//Deactivating Nitz
	DeActivateNitzPlugin();
	
    TBool timeSaved;
    TBool dateSaved;
    
    iQueryDialogsInDisplay = ETrue;
    
	// First the country/city list.
	ShowCountryAndCityListsL(); 
	// Then query date.
	timeSaved = ShowDateQueryL();
    // Then query time.
	dateSaved = ShowTimeQueryL();
	
	iQueryDialogsInDisplay = EFalse;
	
	// Modify the FirstBoot flag.
	if( timeSaved && dateSaved )
		{
		MarkFirstBoot();
		}	
	
    __PRINTS( "CAdtUpdaterContainer::ContinueWithNormalBootL - Exit" );
	}

// ---------------------------------------------------------
// CAdtUpdaterContainer::ShowCountryAndCityListsL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CAdtUpdaterContainer::ShowCountryAndCityListsL()
	{
	__PRINTS( "CAdtUpdaterContainer::ShowCountryAndCityListsL - Entry" );
	
   // Supress the application  key
   STATIC_CAST( CEikServAppUi*, CCoeEnv::Static()->AppUi() )->SuppressAppSwitching( ETrue ); 
		
	TInt citySelected( EFalse );
	
	if( !iListener )
	    {
		iListener = CAdtUpdaterListener::NewL( this );
	    }
	
	// Get the current Mcc from listener. In case of a valid mcc show the city list directly.
	TInt currentMcc( iListener->GetCurrentMcc() );
		
	if( currentMcc )
	    {	
		CTzLocalizedCity* localizedCity( GetLocalizedCityL( currentMcc ) );
		TInt cityGroupId = localizedCity->GroupId();
		citySelected = ShowCityListL( cityGroupId );
		delete localizedCity;			
		}
		
	while( !citySelected )
	       {        
	       // First the country list.
	       TInt cityGroupId = ShowCountryListL();       
	       if( cityGroupId != KErrCancel )
	           {
	           // Then if needed, i.e., if the country has many cities, then the city list.
	           citySelected = ShowCityListL( cityGroupId );
	           }
	       else
	           {
	           citySelected = ETrue;
	           }
	       }
	
	 // Enable the application  key
	 STATIC_CAST( CEikServAppUi*, CCoeEnv::Static()->AppUi() )->SuppressAppSwitching( EFalse ); 
	 	
	__PRINTS( "CAdtUpdaterContainer::ShowCountryAndCityListsL - Exit" );
	}

// ---------------------------------------------------------
// CAdtUpdaterContainer::ShowCountryListL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
TInt CAdtUpdaterContainer::ShowCountryListL()
	{
	__PRINTS( "CAdtUpdaterContainer::ShowCountryListL - Entry" );
	
	// Listbox to display the country list.
	CAknSinglePopupMenuStyleListBox* countryListBox = new( ELeave ) CAknSinglePopupMenuStyleListBox;
	CleanupStack::PushL( countryListBox );

	
	CAdtUpdaterPopupList* popupListBox = CAdtUpdaterPopupList::NewL( countryListBox,
																  	 R_AVKON_SOFTKEYS_SELECT_CANCEL__SELECT,
																  	 AknPopupLayouts::EMenuGraphicHeadingWindow );
	CleanupStack::PushL( popupListBox );

	// Setup the properties of the popupListBox.
	countryListBox->ConstructL( popupListBox, EAknListBoxSelectionList | EAknListBoxScrollBarSizeExcluded );
	countryListBox->CreateScrollBarFrameL( ETrue );
	countryListBox->ScrollBarFrame()->SetScrollBarVisibilityL( CEikScrollBarFrame::EOff,
															   CEikScrollBarFrame::EAuto );

	// Array to hold the countries.
	CDesCArrayFlat *countryListItems = new( ELeave ) CDesCArrayFlat( 1 );
	CleanupStack::PushL( countryListItems );

	// We get the countries list from the CTzLocalizer.
	CTzLocalizer* tzLocalizer = CTzLocalizer::NewL();
	CleanupStack::PushL( tzLocalizer );

	CTzLocalizedCityGroupArray* countryList( NULL );
	// Get the country list.
	countryList = tzLocalizer->GetAllCityGroupsL( CTzLocalizer::ETzAlphaNameAscending );
	CleanupStack::PushL( countryList );

	for( TInt itemIndex( KZero ); itemIndex < countryList->Count(); itemIndex++ )
		{
		CTzLocalizedCityGroup& cityGroup = countryList->At( itemIndex );

		// Check if the counrty name is blank.
		// If it is blank, ignore it. Empty name shouldn't be shown in the list.
		if( KZero != cityGroup.Name().Compare( KOneSpace ) )
			{
			TBuf< KMaxCountryLength > countryItem;
			countryItem.Insert( KZero, cityGroup.Name() );
			countryListItems->AppendL( countryItem );
			}
		}
	// Set the item array to the listbox through listbox model.
	CTextListBoxModel* listBoxModel = countryListBox->Model();
	listBoxModel->SetItemTextArray( countryListItems );
	listBoxModel->SetOwnershipType( ELbmOwnsItemArray );

	// Set the title text for the popup.
	HBufC* titleText = StringLoader::LoadLC( R_QTN_CLK_QTL_SELECT_COUNTRY );
	popupListBox->SetTitleL( titleText->Des() );
	CleanupStack::PopAndDestroy( titleText );
	
	// Enable find for the listbox.
	popupListBox->EnableFind();
	
	// Select the country which will be focused in the countryselection list
	TInt countryIndex( KZero );
	
	if( EDataRejected == iDataAvailability )
		{
		__PRINTS( "CAdtUpdaterContainer::ShowCountryListL : Data received from nitz has been rejected by user" );
		
		CTzLocalizedCity* localizedCity = tzLocalizer->GetFrequentlyUsedZoneCityL( CTzLocalizedTimeZone::ECurrentZone );
		CleanupStack::PushL( localizedCity );
		
		countryIndex = IndexOfCountry( localizedCity->GroupId(), countryList );
		
		CleanupStack::PopAndDestroy( localizedCity );
		}
	else if( EDataNotAvailable == iDataAvailability )
		{
		__PRINTS( "CAdtUpdaterContainer::ShowCountryListL : Data is not available" );
		
		//Get the current Mcc from listener
		TInt currentMcc ( iListener->GetCurrentMcc());
		
		if( currentMcc )
			{			
			CTzLocalizedCity* localizedCity( GetLocalizedCityL( currentMcc ) );	
			
			//Get the country index from country list
			if( localizedCity )
				{
				countryIndex = IndexOfCountry( localizedCity->GroupId(), countryList );				
				}	
			delete localizedCity;			
			}			
		
		}
		
		
	//Set the index of the time to be 	
	countryListBox->View()->SetCurrentItemIndex( countryIndex );	

	TInt cityGroupId( KErrCancel );
	TInt selectedItem( 0 );

	// check if the startup queries are enabled
	// show the country selection query only if enabled
	if ( iDisplayStartupQueries )
	    {
	    if( popupListBox->ExecuteLD() )
            {
            // Get the selected item index in the list.
            selectedItem = countryListBox->CurrentItemIndex();
            }
	    }
		// Get the text of the selected item.
		TPtrC selectedCountryName = countryListBox->Model()->ItemText( selectedItem );

		// Get the group id of the selected country.
		CTzLocalizedCityGroup* tzLocalizedCityGroup = tzLocalizer->FindCityGroupByNameL( selectedCountryName );
		CleanupStack::PushL( tzLocalizedCityGroup );
		// Save the city group id.
		cityGroupId = tzLocalizedCityGroup->Id();
		
		// Cleanup.
		CleanupStack::PopAndDestroy( tzLocalizedCityGroup );
	
	// Cleanup
	CleanupStack::PopAndDestroy( countryList );
	CleanupStack::PopAndDestroy( tzLocalizer );
	CleanupStack::Pop( countryListItems );
	if( iDisplayStartupQueries )
	    {
	    CleanupStack::Pop( popupListBox ); 
	    }
	else
        {
        CleanupStack::PopAndDestroy( popupListBox );
        }
	
	CleanupStack::PopAndDestroy( countryListBox );
	
	__PRINTS( "CAdtUpdaterContainer::ShowCountryListL - Exit" );

	// Return the cityGroupId.
	return cityGroupId;
	}

// ---------------------------------------------------------
// CAdtUpdaterContainer::ShowCityListL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
TInt CAdtUpdaterContainer::ShowCityListL( TInt aGroupId )
	{
	__PRINTS( "CAdtUpdaterContainer::ShowCityListL - Entry" );
	
	TInt returnVal( ETrue );
    TInt cityIndex( KErrNotFound );
	// We get the city list from the group id from CTzLocalizer.
	// If the city list has only one city, no need to popup the list,
	// select the city directly. Else the list is popped. 
	CTzLocalizer* tzLocalizer = CTzLocalizer::NewLC();
	CTzLocalizedCityArray* cityList( NULL );

	// Get the city list.
	cityList = tzLocalizer->GetCitiesInGroupL( aGroupId,
											   CTzLocalizer::ETzAlphaNameAscending );
	CleanupStack::PushL( cityList );

	// We have only one city.
	if( KSingleCity == cityList->Count() )
		{        
		CTzLocalizedCity& localizedCity = cityList->At( KZero );
		
		// Get the city name
		CTzLocalizedCity* tzLocalizedCity = tzLocalizer->FindCityByNameL( localizedCity.Name() );
		CleanupStack::PushL( tzLocalizedCity );

		// Get the timezone Id of the city.
		TInt timeZoneId = tzLocalizedCity->TimeZoneId();

		// Set this id as the current zone.
		tzLocalizer->SetTimeZoneL( timeZoneId );
		tzLocalizer->SetFrequentlyUsedZoneL( *tzLocalizedCity, CTzLocalizedTimeZone::ECurrentZone );

		// Cleanup.
		CleanupStack::PopAndDestroy( tzLocalizedCity );
		CleanupStack::PopAndDestroy( cityList );
		CleanupStack::PopAndDestroy( tzLocalizer );
		
		__PRINTS( "CAdtUpdaterContainer::ShowCityListL - Exit: In case of only one city" );

		return returnVal;
		}
	
	// Listbox to display the city list.
	CAknSinglePopupMenuStyleListBox* cityListBox = new( ELeave ) CAknSinglePopupMenuStyleListBox;
	CleanupStack::PushL( cityListBox );

	CAdtUpdaterPopupList* popupListBox = CAdtUpdaterPopupList::NewL( cityListBox,
																	 R_AVKON_SOFTKEYS_SELECT_BACK__SELECT,
																	 AknPopupLayouts::EMenuGraphicHeadingWindow );
	CleanupStack::PushL( popupListBox );
	
	// Setup the properties of the popupListBox.
	cityListBox->ConstructL( popupListBox, EAknListBoxSelectionList | EAknListBoxScrollBarSizeExcluded );
	cityListBox->CreateScrollBarFrameL( ETrue );
	cityListBox->ScrollBarFrame()->SetScrollBarVisibilityL( CEikScrollBarFrame::EOff,
															CEikScrollBarFrame::EAuto );
	
	// Array to hold the cities.
	CDesCArrayFlat *cityListItems = new( ELeave ) CDesCArrayFlat( 1 );
	CleanupStack::PushL( cityListItems );

	for( TInt cityItem( 0 ); cityItem < cityList->Count(); cityItem++ )
		{
		CTzLocalizedCity& localizedCity = cityList->At( cityItem );

		// Check if the city name is blank.
		// If it is blank, ignore it. Empty name shouldn't be shown in the list.
		if( 0 != localizedCity.Name().Compare( KOneSpace ) )
			{
			TBuf< KMaxCityLength > cityListItem;
			cityListItem.Insert( 0, localizedCity.Name() );
			cityListItems->AppendL( cityListItem );
	        
	        if( KErrNotFound == cityIndex )
	            {
			    CTzLocalizedTimeZone* localizedTimeZone( NULL );
                localizedTimeZone = tzLocalizer->GetLocalizedTimeZoneL( localizedCity.TimeZoneId() );
	            
	            // Check whether it is the default city and set cityIndex.
			    if( KZero == localizedTimeZone->StandardName().Compare( KDefault ) )
			        {
			        cityIndex = cityItem;	
			        }
			    delete localizedTimeZone;        
			    }
			}
		}
	
	// Set the item array to the listbox through listbox model.
	CTextListBoxModel* listBoxModel = cityListBox->Model();
	listBoxModel->SetItemTextArray( cityListItems );
	listBoxModel->SetOwnershipType( ELbmOwnsItemArray );

	// Set the title text for the popup.
	HBufC* titleText = StringLoader::LoadLC( R_QTN_CLK_QTL_SELECT_CITY );
	popupListBox->SetTitleL( titleText->Des() );
	CleanupStack::PopAndDestroy( titleText );
	
	// Enable find in the listbox.
	popupListBox->EnableFind();
	
	// Focus the default city as the current item index.
	// In case default city is not found set the first listitem  as the current focus.
	if( KErrNotFound == cityIndex )
	    {
	    cityIndex = KZero;	
    	}
	cityListBox->SetCurrentItemIndex( cityIndex );
	
	TInt selectedItemIndex( 0 );
	
	// check if the startup queries are enabled
	// show the city selection query only if enabled
	if ( iDisplayStartupQueries )
	    {
	    // Execute the dialog.
	        if( popupListBox->ExecuteLD() )
	            {
	            selectedItemIndex =  cityListBox->CurrentItemIndex();
	            }
	        else
	            {        
	            returnVal = EFalse;
	            }
	    }
	
		TPtrC cityName = cityListBox->Model()->ItemText( selectedItemIndex );

		// Find the timezone id by the name.
		CTzLocalizedCity* tzLocalizedCity = tzLocalizer->FindCityByNameL( cityName );
		CleanupStack::PushL( tzLocalizedCity );

		TInt timeZoneId = tzLocalizedCity->TimeZoneId();

		// Set the found timezone id as teh current zone.
		tzLocalizer->SetTimeZoneL( timeZoneId );
		tzLocalizer->SetFrequentlyUsedZoneL( *tzLocalizedCity,
											 CTzLocalizedTimeZone::ECurrentZone );

		// Cleanup.
		CleanupStack::PopAndDestroy( tzLocalizedCity );
	// Cleanup.
	CleanupStack::Pop( cityListItems );
	
	if( iDisplayStartupQueries )
        {
        CleanupStack::Pop( popupListBox ); 
        }
    else
        {
        CleanupStack::PopAndDestroy( popupListBox );
        }
	CleanupStack::PopAndDestroy( cityListBox );
	CleanupStack::PopAndDestroy( cityList );
	CleanupStack::PopAndDestroy( tzLocalizer );
	
	__PRINTS( "CAdtUpdaterContainer::ShowCityListL - Exit" );

	// Return.
	return returnVal;
	}

// ---------------------------------------------------------
// CAdtUpdaterContainer::ShowDateQueryL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
TBool CAdtUpdaterContainer::ShowDateQueryL()
	{
	__PRINTS( "CAdtUpdaterContainer::ShowDateQueryL - Entry" );
	
	 //Supress the application  key
	 STATIC_CAST( CEikServAppUi*, CCoeEnv::Static()->AppUi() )->SuppressAppSwitching( ETrue );   	 
	
    TTime displayTime;
    if( EDataRejected == iDataAvailability  )
    	{
    	//Get the home time as it is updated by data received by NITZ
    	displayTime.HomeTime();
    	}
    else
    	{
    	//Get the default value from cenrep
    	GetDefaultTimeAndDate( displayTime );
    	}        
    
    // check if the startup queries are enabled
    // show the date query only if enabled
    if ( iDisplayStartupQueries )
        {
    // The Date query dialog.
    CAdtUpdaterQueryDialog* dateQueryDialog = new ( ELeave ) CAdtUpdaterQueryDialog( displayTime );    	
        
    if( dateQueryDialog->ExecuteLD( R_CLK_DATE_SETTING_QUERY_DIALOG ) )
        {
        TTime homeTime;
        homeTime.HomeTime();
        TDateTime homeDateTime = homeTime.DateTime();
        TDateTime newDateTime = displayTime.DateTime();
        newDateTime.SetHour( homeDateTime.Hour() );
        newDateTime.SetMinute( homeDateTime.Minute() );
        newDateTime.SetSecond( homeDateTime.Second() );
        newDateTime.SetMicroSecond( homeDateTime.MicroSecond() );
        displayTime = newDateTime;
        
        // Set the date entered by the user as the hometime.
        RTz tzHandle;
        User::LeaveIfError( tzHandle.Connect() );
        User::LeaveIfError( tzHandle.SetHomeTime( displayTime ) );
        tzHandle.Close();
        
        __PRINTS( "CAdtUpdaterContainer::ShowDateQueryL - Exit: User has entered a date and pressed OK." );
        
        return ETrue;
        }
    else
        {
        __PRINTS( "CAdtUpdaterContainer::ShowDateQueryL - Exit: User had pressed Cancel." );         
       
        // In case of poweroff key was pressed and shutdown is occuring.
        return EFalse;
            }
        }
    else
        {
        // return true, if queries has to be disabled for NTM
        return ETrue;
        }
	}

// ---------------------------------------------------------
// CAdtUpdaterContainer::ShowTimeQueryL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
TBool CAdtUpdaterContainer::ShowTimeQueryL()
	{
	__PRINTS( "CAdtUpdaterContainer::ShowTimeQueryL - Entry" );
	
    TTime displayTime;
    if( EDataRejected == iDataAvailability  )
    	{
    	//Get the home time as it is updated by data received by NITZ
    	displayTime.HomeTime();
    	}
    else
    	{
    	//Get the default value from cenrep
    	GetDefaultTimeAndDate( displayTime );
    	}        
  	
    // check if the startup queries are enabled
    // show the time query only if enabled
    if( iDisplayStartupQueries )
        {
	// The Date query dialog.    	
   	CAdtUpdaterQueryDialog* timeQueryDialog = new ( ELeave ) CAdtUpdaterQueryDialog( displayTime );    	
	
    if( timeQueryDialog->ExecuteLD( R_CLK_TIME_SETTING_QUERY_DIALOG ) )
        {
        TTime homeTime;
        homeTime.HomeTime();
        TDateTime homeDateTime = homeTime.DateTime();
        TDateTime newDateTime = displayTime.DateTime();
        newDateTime.SetYear( homeDateTime.Year() );
        newDateTime.SetMonth( homeDateTime.Month() );
        newDateTime.SetDay( homeDateTime.Day() );
        displayTime = newDateTime;
        
        // Set the date entered by the user as the hometime.
        RTz tzHandle;
        User::LeaveIfError( tzHandle.Connect() );
        User::LeaveIfError( tzHandle.SetHomeTime( displayTime ) );
        tzHandle.Close();
        
        __PRINTS( "CAdtUpdaterContainer::ShowTimeQueryL - Exit: User has entered a time and pressed OK." );
        
        //Enable the application  key
        STATIC_CAST( CEikServAppUi*, CCoeEnv::Static()->AppUi() )->SuppressAppSwitching( EFalse );
        
        return ETrue;
        }
    else
        {
        __PRINTS( "CAdtUpdaterContainer::ShowTimeQueryL - Exit: User has pressed Cancel." );
        
        // In case of poweroff key was pressed and shutdown is occuring.
        return EFalse;
        } 
        }
    else
        {
        // return true, if queries has to be disabled for NTM
        return ETrue;
        }
  	
	}

// ---------------------------------------------------------------------------
// CAdtUpdaterContainer::GetDefaultTimeAndDate
// rest of the details are commented in headers
// ---------------------------------------------------------------------------
void CAdtUpdaterContainer::GetDefaultTimeAndDate( TTime& aTime )
    {
    __PRINTS( "CAdtUpdaterContainer::GetDefaultTimeAndDate(): Entry" );

    CRepository* repository( NULL );
    aTime.Set( KDefaultDateTimeValue );

    TRAPD( errVal, repository = CRepository::NewL( KCRUidStartupConf ) );
    
    // Get the KStartupDefaultTime key value from cenrep
    if ( KErrNone == errVal )
        {
        TBuf< KTimeFormatLength > defaultTime;
        errVal = repository->Get( KStartupDefaultTime, defaultTime );
        if( KErrNone == errVal )
            {
            // returns error if cenrep time format not valid
            errVal = aTime.Set( defaultTime ); 
            }
        }   
    delete repository;
    
    __PRINTS( "CStartupAppUi::GetDefaultTimeAndDate(): Exit" );
    }

// ---------------------------------------------------------------------------
// CAdtUpdaterContainer::GetLocalizedCityL
// rest of the details are commented in headers
// ---------------------------------------------------------------------------
CTzLocalizedCity* CAdtUpdaterContainer::GetLocalizedCityL( TInt& aMcc )
	{
	__PRINTS( "CAdtUpdaterContainer::GetLocalizedCityL : Entry" );
				
	// The tzResolver object to get the timezone id.
	CClockTimeZoneResolver* tzResolver = CClockTimeZoneResolver::NewL();

	// Get the string from of mcc.
	TBuf< KMaxMobileCountryCode > mobileCountryCodeBuf;
	mobileCountryCodeBuf.Format( KMobileCountryCode, aMcc );

	// Resolve the data to get the timezone id.
	RArray< CTzId > aTzIdArray;
	tzResolver->TzIdFromMccL( mobileCountryCodeBuf, aTzIdArray );		
	
	// Construct CTzLocalizer object to get the name of the zone.
	CTzLocalizer* tzLocalizer = CTzLocalizer::NewL();
	CleanupStack::PushL( tzLocalizer );
	
	CTzLocalizedCity* localizedCity( NULL );
	CTzLocalizedTimeZone* localizedTimeZone( NULL );
	
	TInt anyTimeZoneId = aTzIdArray[ KZero ].TimeZoneNumericID();
	
	if( anyTimeZoneId )
		{
		__PRINTS( "CAdtUpdaterContainer::GetLocalizedCityL : we have valid timezoneids for the received mcc" );
		
		TRAPD( errorVal, localizedTimeZone = tzLocalizer->GetLocalizedTimeZoneL( anyTimeZoneId ) );
		CleanupStack::PushL( localizedTimeZone );					

		if( KErrNone == errorVal )
			{
			if( localizedTimeZone ) 
				{
				__PRINTS( "CAdtUpdaterContainer::GetLocalizedCityL - Localized TimeZone" );
				
				// Match found!! Get the city corresponding to this timezone.
				localizedCity = tzLocalizer->GetDefaultCityL( localizedTimeZone->TimeZoneId() );
				}			
			}
		
		CleanupStack::PopAndDestroy( localizedTimeZone );
		CleanupStack::PopAndDestroy( tzLocalizer );		
		}
	aTzIdArray.Close();
	delete tzResolver;
	
	__PRINTS( "CAdtUpdaterContainer::GetLocalizedCityL : Exit" );		

	return localizedCity;
	}

// -----------------------------------------------------
// CAdtUpdaterContainer::IndexOfCountry
// rest of the details are commented in the header
// -----------------------------------------------------
//
TInt CAdtUpdaterContainer::IndexOfCountry( TInt aGroupId, CTzLocalizedCityGroupArray* aCountryList )
	{
	__PRINTS( "CAdtUpdaterContainer::IndexOfCountry - Entry" );	
	
	TInt countrylistCount = aCountryList->Count();
		
	for( TInt countryIndex = KZero; countryIndex < countrylistCount; countryIndex++ )
		{		
		if( aGroupId == aCountryList->At( countryIndex ).Id() )
		    {
		    __PRINTS( "CAdtUpdaterContainer::IndexOfCountry : match found - Exit" );

			return countryIndex;
		    }
		}
	
	__PRINTS( "CAdtUpdaterContainer::IndexOfCountry - Exit" );
	
	return KZero;
	}

// -----------------------------------------------------
// CAdtUpdaterContainer::IsDstOnL
// rest of the details are commented in the header
// -----------------------------------------------------
//
TBool CAdtUpdaterContainer::IsDstOnL( TInt aTimeZoneId )
	{
	TBool retVal( EFalse );
	CTzId* tzId = CTzId::NewL( aTimeZoneId );
	CleanupStack::PushL( tzId );
	
	RTz tz;
	User::LeaveIfError( tz.Connect() );
	CleanupClosePushL( tz );
	
	retVal = tz.IsDaylightSavingOnL( *tzId );

	tz.Close();
	CleanupStack::PopAndDestroy( &tz );
	CleanupStack::PopAndDestroy( tzId );
	
	return retVal;
	}

// -----------------------------------------------------
// CAdtUpdaterContainer::IsFirstBoot
// rest of the details are commented in the header
// -----------------------------------------------------
//
TBool CAdtUpdaterContainer::IsFirstBoot()
    {
    __PRINTS( "CAdtUpdaterContainer::FirstBoot - Entry" );
    
    TInt firstBoot( NULL );
    
    CRepository* repository(NULL);

    TRAPD( errVal, repository = CRepository::NewL( KCRUidStartup ) );
    
    // Get the KStartupFirstBoot key value from cenrep
    if (  KErrNone == errVal )
        {
        errVal = repository->Get( KStartupFirstBoot, firstBoot );
        }
    
    __PRINT( "CAdtUpdaterContainer::IsFirstBoot : firstboot value : %d", firstBoot );
    
    delete repository;
    
    __PRINTS( "CAdtUpdaterContainer::FirstBoot - Exit" );
    
    return  !( (TBool) firstBoot );   
    }

// -----------------------------------------------------
// CAdtUpdaterContainer::IsSimPresent
// rest of the details are commented in the header
// -----------------------------------------------------
//
TBool CAdtUpdaterContainer::IsSimPresent()
	{
	 __PRINTS( "CAdtUpdaterContainer::IsSimPresent - Entry" );	
	 
	//Property has been defined by startup app
	//Retrieving the property value ,the property has been defined by startup application
	TInt simStatus;
	RProperty::Get( KPSUidStartup, KPSSimStatus, simStatus );
	
	__PRINT( "CAdtUpdaterContainer::IsSimPresent :Sim status in phone : %d", simStatus );

	if ( simStatus == ESimNotPresent )
		{
		 __PRINTS( "CAdtUpdaterContainer::IsSimPresent:Sim is not present - Exit" );
		 
		return EFalse;
		}
	else
		{
		__PRINTS( "CAdtUpdaterContainer::IsSimPresent:Sim is present - Exit" );
		
		return ETrue;
		}
	}

// -----------------------------------------------------
// CAdtUpdaterContainer::IsNitzPluginActive
// rest of the details are commented in the header
// -----------------------------------------------------
//
TBool CAdtUpdaterContainer::IsNitzPluginActive()
	{
	__PRINTS( "CAdtUpdaterListener::IsPluginActive : Entry" );
	
	TBool pluginActive( EFalse );		
	RClkSrvInterface clkSrvIf;	
	
	// Connect to clockserver
	TInt errorVal( clkSrvIf.Connect() );
	
	// If the connection doesn't get established 
	if( KErrNone != errorVal )
		{
		__PRINTS( "CAdtUpdaterListener::IsPluginActive : Couldn't able to connect to server." );			
		
		return pluginActive;
		}
			
	// First check if Nitz is active.	
	errorVal = clkSrvIf.IsProtocolActive( KNitzPlugin, pluginActive );
	
	// Protocol is not active.
	if( pluginActive && KErrNone == errorVal )
		{
		__PRINTS( "CAdtUpdaterListener::IsPluginActive : Plugin has been  activated by clockserver." );		
		
		pluginActive = ETrue;		
		}
	else
		{
		__PRINTS( "CAdtUpdaterListener::IsPluginActive : Plugin has not been activated by clockserver. " );		
		
		pluginActive = EFalse;
		}
	// Closing the session
	clkSrvIf.Close();
	
	__PRINTS( "CAdtUpdaterListener::IsPluginActive : Exit" );
	
	return 	pluginActive;	
	}

// ---------------------------------------------------------------------------
// CAdtUpdaterContainer::HiddenReset
// rest of the details are commented in header.
// ---------------------------------------------------------------------------
TBool CAdtUpdaterContainer::HiddenReset()
    {
    __PRINTS( "CAdtUpdaterContainer::HiddenReset : Entry" );

    TBool retVal( EFalse );
    if( IsFirstBoot() )
        {
        TInt startupReason( ENormalStartup );
        //Get the property value to know the reason of boot up like language switch, RFS, etc. 
        TInt errVal = RProperty::Get( KPSUidStartup, KPSStartupReason, startupReason ); 
        if( KErrNone != errVal )
            {
            __PRINT( "CAdtUpdaterContainer::HiddenReset(): ENormalStartup get err %d", errVal );            
            }
            
        __PRINT( "CAdtUpdaterContainer::HiddenReset(): ENormalStartup get err %d", startupReason );
        
        retVal = ( startupReason != ENormalStartup );
        }
    
    __PRINTS( "CAdtUpdaterContainer::HiddenReset : Exit" );
    
    return retVal;
    }

// ---------------------------------------------------------------------------
// CAdtUpdaterContainer::RTCStatus
// rest of the details are commented in header
// ---------------------------------------------------------------------------
TBool CAdtUpdaterContainer::RTCStatus()
    {
    __PRINTS( "CAdtUpdaterContainer::RTCStatus : Entry" );
    
    TBool retVal( EFalse );

    RStarterSession starterSession;
    if(  KErrNone == starterSession.Connect() )
        {
        __PRINTS( "CAdtUpdaterContainer::RTCStatus(): Connected to Starter " );
        
        retVal = starterSession.IsRTCTimeValid();
        starterSession.Close();
        }
    
   __PRINT( "CAdtUpdaterContainer::RTCStatus(): Exit, return %d ", retVal );
    
    return retVal;
    }

// -----------------------------------------------------
// CAdtUpdaterContainer::MarkFirstBoot
// rest of the details are commented in the header
// -----------------------------------------------------
//
void CAdtUpdaterContainer::MarkFirstBoot()
    {
    __PRINTS( "CAdtUpdaterContainer::MarkFirstBoot - Entry" );
    
    CRepository* cenRep( NULL );

    TRAPD( errValue, cenRep = CRepository::NewL( KCRUidStartup ) );
    if ( KErrNone == errValue )
        {
        errValue = cenRep->Set( KStartupFirstBoot, KFirstBootDone );
        }
    
    // Cleanup.
    delete cenRep;
    cenRep = NULL;
    
    __PRINTS( "CAdtUpdaterContainer::MarkFirstBoot - Exit" );   
    }

// ---------------------------------------------------------
// CAdtUpdaterListener::DeActivateNitzPlugin
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CAdtUpdaterContainer::DeActivateNitzPlugin()
	{
	__PRINTS( "CAdtUpdaterContainer::DeActivateNitzPlugin - Entry" );
	
	RClkSrvInterface  clkSrvIf;	
	// Connect to clockserver
	TInt errorVal( clkSrvIf.Connect() );
				
	// If the connection  gets established 
	if( KErrNone == errorVal )
		{				
		clkSrvIf.DeActivateProtocol( KNitzPlugin );

		//Closing the session
		clkSrvIf.Close();	
		}
	
	__PRINTS( "CAdtUpdaterContainer::DeActivateNitzPlugin - Exit" );
	}

// -----------------------------------------------------
// CAdtUpdaterContainer::Listener
// rest of the details are commented in the header
// -----------------------------------------------------
//
CAdtUpdaterListener* CAdtUpdaterContainer::Listener()
    {
    __PRINTS( "CAdtUpdaterContainer::Listener - Entry" );
    
    if( iListener )
        {
        __PRINTS( "CAdtUpdaterContainer::Listener - Exit" );
        
        return iListener;
        }
    
    __PRINTS( "CAdtUpdaterContainer::Listener - Exit" );
    
    return NULL;
    }

// -----------------------------------------------------
// CAdtUpdaterContainer::QueryDialogsInDisplay
// rest of the details are commented in the header
// -----------------------------------------------------
//
TBool CAdtUpdaterContainer::QueryDialogsInDisplay()
	{
	__PRINTS( "CAdtUpdaterContainer::QueryDialogsInDisplay - Entry" );
	
	__PRINTS( "CAdtUpdaterContainer::QueryDialogsInDisplay - Exit" );

	return iQueryDialogsInDisplay;
	}

// -----------------------------------------------------
// CAdtUpdaterContainer::DisplayStartupQueriesL
// rest of the details are commented in the header
// -----------------------------------------------------
//
TBool CAdtUpdaterContainer::DisplayStartupQueriesL()
    {
    __PRINTS( "CAdtUpdaterContainer::StartupQueriesEnabledL - Entry" );
    
    TInt showQueries( 1 );
    
    __PRINTS( "Before cenrep constrction" );
    CRepository *repository( NULL );
    
    TRAPD( errorVal, repository = CRepository::NewL( KCRUidStartupConf ) );
    
    __PRINT( "After construction, error value: %d", errorVal );
    
    if( KErrNone == errorVal )
        {
        TInt errorCode = repository->Get( KStartupQueries, showQueries );
        delete repository;
        }
        
    __PRINTS( "CAdtUpdaterContainer::StartupQueriesEnabledL - Exit" );
    
    return TBool( showQueries ); 
    }

// ---------------------------------------------------------------------------
// CAdtUpdaterContainer::PredictiveTimeEnabled()
// Rest of the details are commented in headers.
// ---------------------------------------------------------------------------
TBool CAdtUpdaterContainer::PredictiveTimeEnabled()
     {
     TInt value( EPredictiveTimeEnabled );
     CRepository* repository(NULL);
     
     TRAPD( err, repository = CRepository::NewL( KCRUidStartupConf ) );

     if ( err == KErrNone )
         {
         err = repository->Get( KPredictiveTimeAndCountrySelection, value );
         }
     delete repository;
 
     return value;
     } 

// End of file
