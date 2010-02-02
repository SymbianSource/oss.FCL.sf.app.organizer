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
* Description:   This is the source file for the CClockWorldView class.
*
*/

// System includes
#include <aknViewAppUi.h>
#include <StringLoader.h>
#include <AknUtils.h>
#include <clock.rsg>
#include <aknnotewrappers.h>
#include <aknlists.h>
#include <tzlocalizer.h>
#include <tzlocalizationdatatypes.h>
#include <AknQueryDialog.h>
#include <MGFetch.h>
#include <hlplch.h>

// User includes
#include "clockworldview.h"
#include "clockworldcontainer.h"
#include "clockworldarray.h"
#include "clockcityselectionlist.h"
#include "clockdocument.h"
#include "clockappui.h"
#include "clock.h"
#include "clock.hrh"
#include "clockserverclt.h"
#include "clock_debug.h"
#include "clockworlditemprocessor.h"

// Constants
const TInt KSizeMaxCitiesText( 100 );
const TInt KNoCities( 0 );
const TInt KIntervalTime( 60000000 );

// Literals
_LIT( KClockCitySelectionDll, "clockcityselectionlist.dll" );

// ---------------------------------------------------------
// CClockWorldView::NewL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
CClockWorldView* CClockWorldView::NewL()
    {
    __PRINTS( "CClockWorldView::NewL - Entry" );
    
    CClockWorldView* self = new ( ELeave ) CClockWorldView;
    CleanupStack::PushL( self );
    
    self->ConstructL();
    
    CleanupStack::Pop( self );
    
    __PRINTS( "CClockWorldView::NewL - Exit" );
    
    return self;
    }

// ---------------------------------------------------------
// CClockWorldView::~CClockWorldView
// rest of the details are commented in the header
// ---------------------------------------------------------
//
CClockWorldView::~CClockWorldView()
	{
	__PRINTS( "CClockWorldView::~CClockWorldView - Entry" );
	
	if( iWorldArray )
	    {
	    delete iWorldArray;
	    iWorldArray = NULL;
	    }
	if( iContainer )
	    {
	    ( AppUi() )->RemoveFromStack( iContainer );
	    delete iContainer;
	    iContainer = NULL;
	    }
	
	iClockCitySelector.Close();
	
	if( iMaxCitiesAdded )
		{
		delete iMaxCitiesAdded;
		iMaxCitiesAdded = NULL;
		}
	if( iTimer )
        {
        delete iTimer;
        iTimer = NULL;
        }
    if( iItemProcessor )
        {
        delete iItemProcessor;
        iItemProcessor = NULL;
        }
	
	__PRINTS( "CClockWorldView::~CClockWorldView - Exit" );
	}

// ---------------------------------------------------------
// CClockWorldView::Id
// rest of the details are commented in the header
// ---------------------------------------------------------
//
TUid CClockWorldView::Id() const
	{
	__PRINTS( "CClockWorldView::Id - Entry" );
	
	__PRINTS( "CClockWorldView::Id - Entry" );
	
	return KClockAppWorldViewId;
	}

// ---------------------------------------------------------
// CClockWorldView::HandleCommandL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClockWorldView::HandleCommandL( TInt aCommand )
	{
	__PRINTS( "CClockWorldView::HandleCommandL - Entry" );
	
	switch( aCommand )
		{
		case EClockWorldAddRegion:
		
		    {
		    // Add a city to the list.
			AddLocationL();
		    }
		    break;
		    
		case EClockWorldMyRegion:
		    {
			//Single click integration
            // If current item is other than home location set it as new home
		    // location
		    if ( iContainer->ListBox()->CurrentItemIndex() > KZerothIndex )
                {
                SetHomeLocationL();
                }

		    }
		    break;
		    
		case EClockWorldRemoveRegion:
		    {
		    // Remove the selected city from the list.
			RemoveLocationL();
		    }
		    break;
		
		case EClockWorldAddImage:
			{
			// Change the image.
			AddImageL();
			}
			break;
		    
		case EClockWorldSettings:
		    {
		    // Let the appui handle the command.
		    AppUi()->HandleCommandL( EClockSettings );
		    }
		    break;
		    
		case EAknCmdHelp:  //EClockWorldHelp:
		    {
		    // Launch world clock help. 
		    HlpLauncher::LaunchHelpApplicationL( iCoeEnv->WsSession(), iAvkonAppUi->AppHelpContextL() );
		    }
		    break;
		    
		case EAknSoftkeyExit:
		    {
		    AppUi()->HandleCommandL( aCommand );
		    }
		    break;
		    
		case EClockWorldExit:
		    {
		    AppUi()->HandleCommandL( aCommand );
		    }
		    break;
		    
		case EAknCmdHideInBackground:
		    {
		    // Close the gallary, if open already.
		    if( iCanceler )
		        {
		        iCanceler->CancelFetcherL();
		        }
		    }
		    break;
		    
		default:
			{
			// No impelementation yet.
			}
			break;
		}
	
	__PRINTS( "CClockWorldView::HandleCommandL - Exit" );
	}
	
// ---------------------------------------------------------
// CClockWorldView::DynInitMenuPaneL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClockWorldView::DynInitMenuPaneL( TInt aResourceId, CEikMenuPane* aMenuPane )
    {
    __PRINTS( "CClockWorldView::DynInitMenuPaneL - Entry" );
    
	//Single click integration
    if( R_CLOCK_WORLD_VIEW_MENUPANE != aResourceId )

        {
        __PRINTS( "CClockWorldView::DynInitMenuPaneL - Exit" );
        
		// We're not bother about other menupanes.
        return;
        }
	// TODO: For help.
    /*if( !FeatureManager::FeatureSupported( KFeatureIdHelp ) )
        {       
        aMenuPane->DeleteMenuItem( EWorldClockHelp );      
        }*/
    
    if( KNoCities == iWorldArray->MdcaCount() )
        {
		//Single click integration
       	aMenuPane->SetItemDimmed( EClockWorldRemoveRegion, ETrue );  
        aMenuPane->SetItemDimmed( EClockWorldAddImage, ETrue );      	
       	aMenuPane->SetItemDimmed( EClockWorldMyRegion, ETrue );
        }
   
	// Add item is displayed only if the selection list is available.
	if( !iSelectionListAvailable )
        { 
        // Hide the Add menu option if loading DLL failed.
       	aMenuPane->SetItemDimmed( EClockWorldAddRegion, ETrue );
        }
	
	// Check if automatic-time update is On.
	RClkSrvInterface clockServerClt;
    // Connect to clockserver
    TInt errorVal( clockServerClt.Connect() );
    // Check if timeupdate is On.
    TBool timeUpdateOn( EFalse );
    if( KErrNone == errorVal )
        {
        errorVal = clockServerClt.IsAutoTimeUpdateOn( timeUpdateOn );
        }
    // Close the session with the server.
    clockServerClt.Close();
	
	if( KZerothIndex == iContainer->ListBox()->CurrentItemIndex() && !timeUpdateOn )
	    {
	    aMenuPane->DeleteMenuItem( EClockWorldMyRegion );
	    aMenuPane->DeleteMenuItem( EClockWorldAddImage );
	    aMenuPane->SetItemDimmed( EClockWorldRemoveRegion, ETrue);
	    }
	
   
    
    __PRINTS( "CClockWorldView::DynInitMenuPaneL - Exit" );
    }

// ---------------------------------------------------------
// CClockWorldView::DynInitMenuBarL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
//single click integration
void CClockWorldView::DynInitMenuBarL( TInt /*aResourceId*/, CEikMenuBar* aMenuBar )

    {
    __PRINTS( "CClockWorldView::DynInitMenuBarL - Entry" );
    
	//single click integration
    if( aMenuBar && ( IsSelectionListOpen() || IsGalleryOpen() ) )
        {
        // If the selection list open, we should not display the menupane.
        aMenuBar->StopDisplayingMenuBar();
        }


    __PRINTS( "CClockWorldView::DynInitMenuBarL - Exit" );
    }

// ---------------------------------------------------------
// CClockWorldView::GetApplicationRect
// rest of the details are commented in the header
// ---------------------------------------------------------
//
TRect CClockWorldView::GetApplicationRect()
    {
    __PRINTS( "CClockWorldView::GetApplicationRect - Entry" );
    
    __PRINTS( "CClockWorldView::GetApplicationRect - Exit" );
    
    // Return the applicationrect from appui.
    return AppUi()->ApplicationRect();
    }



// ---------------------------------------------------------
// CClockWorldView::HandleEnvChangeL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClockWorldView::HandleEnvChangeL( TClockEnvChanged aEnvChanged )
    {
    __PRINTS( "CClockWorldView::HandleEnvChangeL - Entry" );
    
    CClockDocument* clockDocument = static_cast< CClockDocument* > ( AppUi()->Document() );
    
    // Here we check if the environment change was in the system time/zone,
    // we check if the current zone is different from the homecity item in the list
    // and if it is, then we update it with the new one.
    if( EEnvChanged == aEnvChanged && clockDocument->IsHomeCityItemAdded() )
        {
        // Remove the previous home city.
        iWorldArray->RemoveCity( KZerothIndex );

        // Add the current home city to the world clock view. 
        iWorldArray->InsertHomeCityItem();
        
        if( iContainer )
            {
            // We update the first item in the list.
            iContainer->ListBox()->HandleItemAdditionL();
            }
        }
    
    if( iContainer )
        {
        // Update the container.

        iContainer->Refresh();
        }
    
    __PRINTS( "CClockWorldView::HandleEnvChangeL - Exit" );
    }

// ---------------------------------------------------------
// CClockWorldView::IsSelectionListOpen
// rest of the details are commented in the header
// ---------------------------------------------------------
//
TBool CClockWorldView::IsSelectionListOpen()
    {
    __PRINTS( "CClockWorldView::IsSelectionListOpen - Entry" );
    
    __PRINTS( "CClockWorldView::IsSelectionListOpen - Exit" );
    
    return iSelectionListOpen;
    }

// ---------------------------------------------------------
// CClockWorldView::IsGalleryOpen
// rest of the details are commented in the header
// ---------------------------------------------------------
//
TBool CClockWorldView::IsGalleryOpen()
    {
    __PRINTS( "CClockWorldView::IsGalleryOpen - Entry" );
    
    __PRINTS( "CClockWorldView::IsGalleryOpen - Exit" );
    
    return iGalleryOpen;
    }

// --------------------------------------------------------
// CClockWorldView::TimerCallback
// rest of the details are commented in the header
// ---------------------------------------------------------
//
TInt CClockWorldView::TimerCallback( TAny* aThis )
    {
    __PRINTS( "CClockWorldView::TimerCallback - Entry" );
    
    CClockWorldView* self( STATIC_CAST( CClockWorldView*, aThis ) );

    // Update the container.
    if( self->iContainer )
        {
        
        self->iContainer->Refresh();
        }
    
    TTime homeTime;
    homeTime.HomeTime();
    TDateTime dateTime( homeTime.DateTime() );
    if( 0 < dateTime.Second() )
        {
        self->iTimer->Cancel();
        self->iTimer->After( KIntervalTime - 1000000 * dateTime.Second() - dateTime.MicroSecond() );
        }
    
    __PRINTS( "CClockWorldView::TimerCallback - Exit" );
    
    return FALSE;
    }

// ---------------------------------------------------------
// CClockWorldView::ClockApplicationUi
// rest of the details are commented in the header
// ---------------------------------------------------------
//
CClockAppUi* CClockWorldView::ClockApplicationUi()
    {
    __PRINTS( "CClockWorldView::ClockApplicationUi - Entry" );
    
    return static_cast< CClockAppUi* > ( AppUi() );
    
    __PRINTS( "CClockWorldView::ClockApplicationUi - Exit" );
    }

// ---------------------------------------------------------
// CClockWorldView::DoActivateL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClockWorldView::DoActivateL( const TVwsViewId& /*aPrevViewId*/,
        					       TUid /*aCustomMessageId*/,
        					       const TDesC8& /*aCustomMessage*/ )
    {
    __PRINTS( "CClockWorldView::DoActivateL - Entry" );
    
    iItemToFocus = KZerothIndex;
    
    // Insert the homecity item in the list. We do this only if Automatic-time update is Off.
    RClkSrvInterface clockServerClt;
    // Connect to clockserver
    TInt errorVal( clockServerClt.Connect() );
    // Check if timeupdate is On.
    TBool timeUpdateOn( EFalse );
    if( KErrNone == errorVal )
        {
        errorVal = clockServerClt.IsAutoTimeUpdateOn( timeUpdateOn );
        }
    // Close the session with the server.
    clockServerClt.Close();
    
    CClockDocument* clockDocument = static_cast< CClockDocument* > ( AppUi()->Document() );

    if( !iContainer )
    	{
    	iContainer = CClockWorldContainer::NewL( this, ClientRect(), iWorldArray );
    	( AppUi() )->AddToStackL( *this, iContainer );
    	iContainer->SetMopParent( this );
    	// Create the CClockWorldItemProcessor object to redraw each listitem
    	iItemProcessor = CClockWorldItemProcessor::NewL( iContainer );
    	
    	iWorldArray->SetItemProcessor( iItemProcessor );
        // If automatic time update is OFF then set the empty text to NULL and insert the home city.
        if( !timeUpdateOn )
            {
            iWorldArray->InsertHomeCityItem();
            clockDocument->SetHomeCityItemAdded( ETrue );
            
            // Update the listbox.
            iContainer->ListBox()->HandleItemAdditionL();
            }
        else 
            {
            clockDocument->SetHomeCityItemAdded( EFalse );
            }
    	}
		
    // This code will get executed, even if the container was constructed before, this code has to 
    // get executed to ensure correct text is displayed when the list is empty.
    // Check if automatic time update is ON and home city is added.
    // then remove the home city.
    if( timeUpdateOn )
        {
        // Check if home city is added.
        if( clockDocument->IsHomeCityItemAdded() )
            {
            iWorldArray->RemoveCity( KZerothIndex );
            clockDocument->SetHomeCityItemAdded( EFalse );
            iContainer->ListBox()->HandleItemRemovalL();
            }
        // Automatic time update is on.In case worldarray list is empty then display the no locations string. 
        if( !iWorldArray->MdcaCount() )
            {
            HBufC* noLocationBuf = StringLoader::LoadLC( R_WRLD_CLK_EMPTY_VIEW, iCoeEnv );
            iContainer->ListBox()->View()->SetListEmptyTextL( noLocationBuf->Des() );
            CleanupStack::PopAndDestroy( noLocationBuf );
            }
        }
    else
        {
        // Check if home city is not inserted.
        if( !clockDocument->IsHomeCityItemAdded() )
            {
            iWorldArray->InsertHomeCityItem();
            clockDocument->SetHomeCityItemAdded( ETrue );
            iContainer->ListBox()->HandleItemAdditionL();
            }
        // If automatic time update is OFF then set the empty text to NULL and insert the home city.
        HBufC* noLocationBuf = HBufC::NewLC( 1 );
        noLocationBuf->Des().Append( KSingleSpace );
        iContainer->ListBox()->View()->SetListEmptyTextL( noLocationBuf->Des() );
        CleanupStack::PopAndDestroy( noLocationBuf );
        }
    
    if( clockDocument->IsHomeCityItemAdded() )
        {
        // If home city is already added,
        // get the current home city and the previous home city.
        // Check for difference
        
        //Get the current home city.
        RTz tzHandle;
        User::LeaveIfError( tzHandle.Connect() );

        // Get the timezone id.
        CTzId* currentTimeZoneId = tzHandle.GetTimeZoneIdL();
        CleanupStack::PushL( currentTimeZoneId );

        // Construct CTzLocalizer object to get the timezone from the id.
        CTzLocalizer* tzLocalizer = CTzLocalizer::NewLC();
        
        // Get the frequently used localized city.
        CTzLocalizedCity* currentLocalizedCity = tzLocalizer->GetFrequentlyUsedZoneCityL( CTzLocalizedTimeZone::ECurrentZone );
        CleanupStack::PushL( currentLocalizedCity );
        
        TCityInfo cityInfo = iWorldArray->GetCity( KZerothIndex );
        
        if( currentLocalizedCity->Name().Compare( cityInfo.iCity ) != 0 )
            {
            iWorldArray->RemoveCity( KZerothIndex );
            iWorldArray->InsertHomeCityItem();
            iContainer->ListBox()->HandleItemAdditionL();
            }
        
        // Cleanup.
        CleanupStack::PopAndDestroy( currentLocalizedCity );
        CleanupStack::PopAndDestroy( tzLocalizer );
        CleanupStack::PopAndDestroy( currentTimeZoneId );
        
        tzHandle.Close();
        }
    
    

    if( iWorldArray->MdcaCount() > 0 )
		{
    	// Always focus the first item.
    	iContainer->ListBox()->SetCurrentItemIndexAndDraw( KZerothIndex );
    	iContainer->ListBox()->ScrollToMakeItemVisible( KZerothIndex );
		}

    // Make the time item synchronize the current time every a minute.
    if( !iTimer )
        {
        TTime time;
        time.HomeTime();
        TDateTime dateTime = time.DateTime();
        TCallBack callBack( TimerCallback, this );
        iTimer = CPeriodic::NewL( CActive::EPriorityHigh );
        iTimer->Start( TTimeIntervalMicroSeconds32( KIntervalTime - 1000000 * dateTime.Second() - dateTime.MicroSecond() ),
                       TTimeIntervalMicroSeconds32( KIntervalTime ), callBack );
        }

    // Activate the container.
    iContainer->ActivateL();
    iContainer->SetRect( ClientRect() );
    
    
    
    // Update the title pane text.
    ClockApplicationUi()->SetTitlePaneTextL( R_TITLE_WRLD_CLK );
    
    // If opened from clock, display the navigation pane again.
    ClockApplicationUi()->MakeNavigationPaneVisible( ETrue, EClockAppWorldViewId );
    
    __PRINTS( "CClockWorldView::DoActivateL - Exit" );
    }

// ---------------------------------------------------------
// CClockWorldView::DoDeactivate
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClockWorldView::DoDeactivate()
	{
	__PRINTS( "CClockWorldView::DoDeactivate - Entry" );
	
	iItemToFocus = KZerothIndex;
	
	// Set the display text to space to avoid flickering when switching between views.
	TRAP_IGNORE( iContainer->ListBox()->View()->SetListEmptyTextL( KSingleSpace ) );
	
	__PRINTS( "CClockWorldView::DoDeactivate - Exit" );
    }

// ---------------------------------------------------------
// CClockWorldView::HandleForegroundEventL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClockWorldView::HandleForegroundEventL( TBool aForeground )
    {
    
    if( aForeground  && iContainer )
        {
        RestartTimerL();

        RClkSrvInterface clockServerClt;
        // Connect to clockserver
        TInt errorVal( clockServerClt.Connect() );
        // Check if timeupdate is On.
        TBool timeUpdateOn( EFalse );
        if( KErrNone == errorVal )
            {
            errorVal = clockServerClt.IsAutoTimeUpdateOn( timeUpdateOn );
            }
        // Close the session with the server.
        clockServerClt.Close();
        
        CClockDocument* clockDocument = static_cast< CClockDocument* > ( AppUi()->Document() );
        
        // This code will get executed, even if the container was constructed before, this code has to 
        // get executed to ensure correct text is displayed when the list is empty.
        // Check if automatic time update is ON and home city is added.
        // then remove the home city.
        if( timeUpdateOn )
            {
            // Check if home city is added.
            if( clockDocument->IsHomeCityItemAdded() )
                {
                iWorldArray->RemoveCity( KZerothIndex );
                clockDocument->SetHomeCityItemAdded( EFalse );
                iContainer->ListBox()->HandleItemRemovalL();
                if( iItemToFocus >= iWorldArray->MdcaCount() )
                    {
                    iItemToFocus = iWorldArray->MdcaCount() - 1;
                    }
                }
            // Automatic time update is on.In case worldarray list is empty then display the no locations string. 
            if( !iWorldArray->MdcaCount() )
                {
                HBufC* noLocationBuf = StringLoader::LoadLC( R_WRLD_CLK_EMPTY_VIEW, iCoeEnv );
                iContainer->ListBox()->View()->SetListEmptyTextL( noLocationBuf->Des() );
                CleanupStack::PopAndDestroy( noLocationBuf );
                }
            }
        else
            {
            // Check if home city is not inserted.
            if( !clockDocument->IsHomeCityItemAdded() )
                {
                iWorldArray->InsertHomeCityItem();
                clockDocument->SetHomeCityItemAdded( ETrue );
                iContainer->ListBox()->HandleItemAdditionL();
                iItemToFocus = KZerothIndex;
                }
            // If automatic time update is OFF then set the empty text to NULL and insert the home city.
            HBufC* noLocationBuf = HBufC::NewLC( 1 );
            noLocationBuf->Des().Append( KSingleSpace );
            iContainer->ListBox()->View()->SetListEmptyTextL( noLocationBuf->Des() );
            CleanupStack::PopAndDestroy( noLocationBuf );
            }

        if( clockDocument->IsHomeCityItemAdded() )
            {
            // If home city is already added,
            // get the current home city and the previous home city.
            // Check for difference

            //Get the current home city.
            RTz tzHandle;
            User::LeaveIfError( tzHandle.Connect() );

            // Get the timezone id.
            CTzId* currentTimeZoneId = tzHandle.GetTimeZoneIdL();
            CleanupStack::PushL( currentTimeZoneId );

            // Construct CTzLocalizer object to get the timezone from the id.
            CTzLocalizer* tzLocalizer = CTzLocalizer::NewLC();

            // Get the frequently used localized city.
            CTzLocalizedCity* currentLocalizedCity = tzLocalizer->GetFrequentlyUsedZoneCityL( CTzLocalizedTimeZone::ECurrentZone );
            CleanupStack::PushL( currentLocalizedCity );

            TCityInfo cityInfo = iWorldArray->GetCity( KZerothIndex );

            if( currentLocalizedCity->Name().Compare( cityInfo.iCity ) != 0 )
                {
                iWorldArray->RemoveCity( KZerothIndex );
                iWorldArray->InsertHomeCityItem();
                iContainer->ListBox()->HandleItemAdditionL();
                }

            // Cleanup.
            CleanupStack::PopAndDestroy( currentLocalizedCity );
            CleanupStack::PopAndDestroy( tzLocalizer );
            CleanupStack::PopAndDestroy( currentTimeZoneId );

            tzHandle.Close();
            }
			
        if( iItemToFocus < iWorldArray->MdcaCount() && KZerothIndex < iItemToFocus )
            {
            iContainer->ListBox()->SetCurrentItemIndexAndDraw( iItemToFocus );
            iContainer->ListBox()->ScrollToMakeItemVisible( iItemToFocus );
            }
        else
            {
            iContainer->ListBox()->SetCurrentItemIndexAndDraw( KZerothIndex );
            iContainer->ListBox()->ScrollToMakeItemVisible( KZerothIndex );
            }
     
        iContainer->Refresh();
        }
    else
        {
        if( iContainer )
            {
            iItemToFocus = iContainer->ListBox()->CurrentItemIndex();            
            }
        if( iTimer )
            {
            StopTimerL();
            }    
        }
    }

// ---------------------------------------------------------
// CClockWorldView::ConstructL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClockWorldView::ConstructL()
	{
	__PRINTS( "CClockWorldView::ConstructL - Entry" );
	
	// Construct the view first.
	BaseConstructL( R_CLOCK_WORLD_VIEW );

	// Get the initial list from the document.
    CClockDocument* clockDocument = static_cast< CClockDocument* > ( AppUi()->Document() );
    iWorldArray = CClockWorldArray::NewL( clockDocument->GetWorldClockList(), CEikonEnv::Static() );

    // TODO: iClockSrvModel = clockDocument->ClockServerModel();
	
	// Load the city/country selection dialog.
	TInt errorVal( iClockCitySelector.Load( KClockCitySelectionDll ) );
	if( KErrNone != errorVal )
        {
        iSelectionListAvailable = EFalse;
        }
    
	// Load the strings.
	iMaxCitiesAdded = StringLoader::LoadL( R_CLOCK_WORLD_NOTE_CITY_MAX, iCoeEnv );
	// Convert numbers From Latin to Davanagiri/Arabic-indi digits.
    TBuf< KSizeMaxCitiesText > textBuf( iMaxCitiesAdded ->Des() );
    AknTextUtils::LanguageSpecificNumberConversion( textBuf );
    iMaxCitiesAdded->Des() = textBuf;
    
    __PRINTS( "CClockWorldView::ConstructL - Exit" );
	}

// ---------------------------------------------------------
// CClockWorldView::CClockWorldView
// rest of the details are commented in the header
// ---------------------------------------------------------
//
CClockWorldView::CClockWorldView()
	{
	__PRINTS( "CClockWorldView::CClockWorldView - Entry" );
	
	iSelectionListAvailable = ETrue;
	//single click integration
    iSelectionListOpen = EFalse;

	iGalleryOpen = EFalse;
	iItemToFocus = KZerothIndex;
	
	__PRINTS( "CClockWorldView::CClockWorldView - Exit" );
	}

// ---------------------------------------------------------
// CClockWorldView::AddLocationL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClockWorldView::AddLocationL()
	{
	__PRINTS( "CClockWorldView::AddLocationL - Entry" );
	
	// If limit of cities in the list has been reached.
	if( KMaxCitiesInList == iWorldArray->MdcaCount() )
		{
		CAknInformationNote* infoNote = new( ELeave ) CAknInformationNote;
		infoNote->ExecuteLD( *iMaxCitiesAdded );
		
		__PRINTS( "CClockWorldView::NewL - Entry" );
		
		return;
		}

	// Hide the navigation pane.
	static_cast< CClockAppUi* > ( AppUi() )->MakeNavigationPaneVisible( EFalse, EClockAppWorldViewId );
	
	// We've now opened the list. 
	iSelectionListOpen = ETrue;

	// Load the first export in the library.
	TLibraryFunction libraryFunction = iClockCitySelector.Lookup( 1 );
	// Construct the selection list.
	MClockCitySelectionList* selectionList = ( MClockCitySelectionList* ) libraryFunction();
	// Sanity check.
	User::LeaveIfNull( selectionList );
	// This will be filled the dialog, will have what the user has selected.
	TCityInfo cityInformation;
	// We add a dummy value for timezone id in the city info. We check for this value after the 
	// dialog has been destroyed. If it is other than the dummy value, we continue to add the city
	// to the list.
	cityInformation.iTimeZoneId = KErrNotFound;
	// Construct the list.
	selectionList->ConstructL( &cityInformation );
	// Launch the list.
	TInt returnVal( selectionList->ExecuteLD() );

	// The list destroyed now.
	iSelectionListOpen = EFalse;
	
	
	// Update the title pane text.
	TVwsViewId viewId;
	AppUi()->GetActiveViewId( viewId );
	if( viewId.iViewUid == KClockAppWorldViewId )
	    {
	   static_cast< CClockAppUi* > ( AppUi() )->SetTitlePaneTextL( R_TITLE_WRLD_CLK );
	   // Show the navigation pane again.
	   static_cast< CClockAppUi* > ( AppUi() )->MakeNavigationPaneVisible( ETrue, EClockAppWorldViewId );
	    }
	
	if( KClockHideInBackground == returnVal )
	    {
	    AppUi()->HandleCommandL( EClockWorldExit );
	    
	    __PRINTS( "CClockWorldView::AddLocationL - Exit" );
	    
	    return;
	    }

	// We first check if the cityinfo has been filled by the selection list. It will be
	// NULL if the user had pressed cancel.
	if( KErrNotFound == cityInformation.iTimeZoneId )
	    {
	    __PRINTS( "CClockWorldView::AddLocationL - Exit" );
	    
	    return;
	    }
	
	// Check if the city is already added.
	TInt itemIndex( iWorldArray->GetListBoxIndex( cityInformation ) );

	if( KErrNotFound == itemIndex )
		{
		// First update the array.
		iWorldArray->AddCityL( cityInformation );
		// Now the container.
		iContainer->ListBox()->HandleItemAdditionL();
		iContainer->ListBox()->ScrollToMakeItemVisible( iWorldArray->MdcaCount() - 1 );
		iContainer->ListBox()->SetCurrentItemIndexAndDraw( iContainer->ListBox()->BottomItemIndex() );

		// Update the Document.
		CClockDocument *clockDocument = static_cast< CClockDocument* > ( AppUi()->Document() );
		clockDocument->StoreDataL();
		}
	else
		{
		iContainer->ListBox()->SetCurrentItemIndexAndDraw( itemIndex );
		}

	RClkSrvInterface clkSrvInterface;
	User::LeaveIfError( clkSrvInterface.Connect() );
	TBool timeUpdateOn( EFalse );
	// Get the state of the plugin.
	clkSrvInterface.IsAutoTimeUpdateOn( timeUpdateOn );
	clkSrvInterface.Close();
		
	

	__PRINTS( "CClockWorldView::AddLocationL - Exit" );
	}

// ---------------------------------------------------------
// CClockWorldView::RemoveLocationL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClockWorldView::RemoveLocationL()
	{
	__PRINTS( "CClockWorldView::RemoveLocationL - Entry" );
	
	// Sanity check.
    if( KNoCities >= iWorldArray->MdcaCount() )
        {
        __PRINTS( "CClockWorldView::NewL - Entry" );
        
        return;
        }
        
	HBufC* queryText( NULL );
    TInt currentListItem( iContainer->ListBox()->CurrentItemIndex() );
    
    // Get information about the city selected.
    TCityInfo cityInformation( iWorldArray->GetCity( currentListItem ) );

    // Construct the localizer.
	CTzLocalizer* tzLocalizer = CTzLocalizer::NewL();
	CleanupStack::PushL( tzLocalizer );	
	CTzLocalizedCityArray* localizedCityList = tzLocalizer->GetCitiesInGroupL( cityInformation.iCityGroupId,
																	           CTzLocalizer::ETzAlphaNameAscending );
	CleanupStack::PushL( localizedCityList );


	
	
	// If user says OK, remove the item from the list as well as from the MDesCArray.
	// Also update the Data with the CClockDocument
  
        TInt imageIndex( iWorldArray->GetImageIndex( currentListItem ) );
        
        // Remove the image icon in case it is not loaded by the other listitems      
        iContainer->RemoveImage( cityInformation.iImagePath, imageIndex, currentListItem  );

        // Update the world array.
        iWorldArray->RemoveCity( currentListItem );
                                    
        if( currentListItem >= iWorldArray->MdcaCount() )
            {
            currentListItem = iWorldArray->MdcaCount() - 1;
            }
        if( KNoCities <= currentListItem )
            {
            // Make the list to display the previous item.
            iContainer->ListBox()->SetCurrentItemIndex( currentListItem );
            }

        // Update the document.
        CClockDocument* clockDocument = static_cast< CClockDocument* > ( AppUi()->Document() );
        clockDocument->StoreDataL();
        // Update the container.
        iContainer->ListBox()->HandleItemRemovalL();
        // If there are no more cities present, update the empty list text.
        if( KNoCities == iWorldArray->MdcaCount() )
            {
            HBufC* noLocationBuf = StringLoader::LoadLC( R_WRLD_CLK_EMPTY_VIEW, iCoeEnv );
            iContainer->ListBox()->View()->SetListEmptyTextL( noLocationBuf->Des() );
            CleanupStack::PopAndDestroy( noLocationBuf );
            }
   
    
    // Cleanup. 
 
	CleanupStack::PopAndDestroy( localizedCityList );
	CleanupStack::PopAndDestroy( tzLocalizer );
    
	RClkSrvInterface clkSrvInterface;
	User::LeaveIfError( clkSrvInterface.Connect() );
	TBool timeUpdateOn( EFalse );
	// Get the state of the plugin.
	clkSrvInterface.IsAutoTimeUpdateOn( timeUpdateOn );
	clkSrvInterface.Close();
	
   
	
	// Redraw the container
   
    iContainer->Refresh();
    
    __PRINTS( "CClockWorldView::RemoveLocationL - Exit" );
	}

// ---------------------------------------------------------
// CClockWorldView::SetHomeLocationL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClockWorldView::SetHomeLocationL()
	{
	__PRINTS( "CClockWorldView::SetHomeLocationL - Entry" );
	
	// Here first we check if Network time update is switched on. If so we ask the user if it needs to switched off
	// before we reset the home location.
	// First connect to the clockserver.
	RClkSrvInterface clkSrvInterface;
	User::LeaveIfError( clkSrvInterface.Connect() );

	TBool timeUpdateOn( EFalse );
	
	// Get the state of the plugin.
	clkSrvInterface.IsAutoTimeUpdateOn( timeUpdateOn );

	if( timeUpdateOn )
		{
		CAknQueryDialog* queryDialog = CAknQueryDialog::NewL();
		// TODO: Resource.
		if( !queryDialog->ExecuteLD( R_CLOCK_DIALOG_QUERY_NW_TIME_OFF ) )
			{
			// User pressed 'No', we don't reset the home zone.
			clkSrvInterface.Close();
			
			__PRINTS( "CClockWorldView::NewL - Entry" );
			
			return;
			}
		}

	// Now we confirm again with the user whether he really wants to update the home zone.
	CAknQueryDialog* queryDialog = CAknQueryDialog::NewL();
	// TODO: Resource.
	if( queryDialog->ExecuteLD( R_CLOCK_DIALOG_QUERY_REGION_AF_TIME ) )
		{
		// First store the info of the currently focused item.
		TInt cityIndex( iContainer->ListBox()->CurrentItemIndex() );
		TCityInfo cityInformation( iWorldArray->GetCity( cityIndex ) );
		
		// The document.
		CClockDocument* clockDocument = static_cast< CClockDocument* > ( AppUi()->Document() );
		
		if( timeUpdateOn )
			{
			// First switch automatic time update off.
			clkSrvInterface.DeActivateAllProtocols();
			
			// Here, the home city wasn't displayed previously, because automatic-time update
			// was on. We've switched it Off and hence the home city has to be displayed.
			
			// Two things to be done here.
			// First is to delete the currently selected item from its position.
			// and then insert it at the zeroth position.
			
			// We ask the document object to reset the homezone.
			clockDocument->SetHomeCityL( cityInformation );
			
			// Now delete the item at that location and update the list.
			TInt imageIndex( iWorldArray->GetImageIndex( cityIndex ) );
			
			// Remove the image icon in case it is not loaded by the other listitems      
            iContainer->RemoveImage( cityInformation.iImagePath, imageIndex, cityIndex );

			// Update the world array.
			iWorldArray->RemoveCity( cityIndex );
			
			// Update the container.
			iContainer->ListBox()->HandleItemRemovalL();
			}
		else
			{
			// Here we already have the home city being displayed as the first item.
			// The things to be done here are these.
			
			// The current and the first list items need to be swapped.
			// We ask the document object to reset the homezone.
			clockDocument->SetHomeCityL( cityInformation );
			
			// Now delete the item at the current location and update the list.
			TInt imageIndex( iWorldArray->GetImageIndex( cityIndex ) );
			
			// Remove the image icon in case it is not loaded by the other listitems      
            iContainer->RemoveImage( cityInformation.iImagePath, imageIndex, cityIndex );

			// Update the world array.
			iWorldArray->RemoveCity( cityIndex );
			
			// Update the container.
			iContainer->ListBox()->HandleItemRemovalL();
			
			// Store the old city info.
			TCityInfo oldHomeCityInfo( iWorldArray->GetCity( KZerothIndex ) );
			
			// Now delete the first item and update the list.
			iWorldArray->RemoveCity( KZerothIndex );

			// Update the container.
			iContainer->ListBox()->HandleItemRemovalL();
			
			// Insert the old home city at the location where the new home city was.
			iWorldArray->InsertOldHomeCityItem( oldHomeCityInfo, cityIndex - 1 );
			// Update the container.
			iContainer->ListBox()->HandleItemAdditionL();
			}
		// Insert the homecity at the top of the list and update it.
		iWorldArray->InsertHomeCityItem();
		// Update the container.
		iContainer->ListBox()->HandleItemAdditionL();
		iContainer->ListBox()->ScrollToMakeItemVisible( KZerothIndex );
		iContainer->ListBox()->SetCurrentItemIndexAndDraw( KZerothIndex );

		// Update the Document.
		clockDocument->StoreDataL();
        clockDocument->SetHomeCityItemAdded( ETrue );
		 // Update the CBA( the MSK text )
	
					
		// Display confirmation to the user.
		// Location name is country name if there is only one city in the list.
		// else the city name.
		CTzLocalizer* tzLocalizer = CTzLocalizer::NewL();
		CleanupStack::PushL( tzLocalizer );

		// Get the city list for the country.
		CTzLocalizedCityArray* cityList = tzLocalizer->GetCitiesInGroupL( cityInformation.iCityGroupId, CTzLocalizer::ETzAlphaNameAscending );
		CleanupStack::PushL( cityList );
			
		// We choose what to display depending on the number of cities in the country.
		HBufC* noteText( NULL );
		if( 1 == cityList->Count() )
			{
			// TODO: Resource.
			noteText = StringLoader::LoadLC( R_CLOCK_NOTE_TIME_CHANGED_COUNTRY, cityInformation.iCountry, iCoeEnv );
			}
		else
			{
			// TODO: Resource.
			noteText = StringLoader::LoadLC( R_CLOCK_NOTE_TIME_CHANGED_COUNTRY, cityInformation.iCity, iCoeEnv );
			}
	
		// Refresh the view.
	
		iContainer->Refresh();
			
		// Display the Information note.
		CAknInformationNote* informationNote = new( ELeave ) CAknInformationNote;
		informationNote->ExecuteLD( *noteText );

		// Cleanup.
		CleanupStack::PopAndDestroy( noteText );
		CleanupStack::PopAndDestroy( cityList );
		CleanupStack::PopAndDestroy( tzLocalizer );
		}

	// Cleanup.
	clkSrvInterface.Close();
	
	__PRINTS( "CClockWorldView::SetHomeLocationL - Exit" );
	}
	
// ---------------------------------------------------------
// CClockWorldView::AddImageL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClockWorldView::AddImageL()
    {
	__PRINTS( "CClockWorldView::AddImageL - Entry" );
	    
    CDesCArray* selectedFile = new( ELeave ) CDesCArrayFlat( 1 );
    CleanupStack::PushL( selectedFile );
	TBool result( EFalse );
        TInt currentIndex = iContainer->ListBox()->CurrentItemIndex();
	
	// Hide the navigation pane.
	CClockAppUi* appUi = static_cast< CClockAppUi* > ( AppUi() );
	appUi->MakeNavigationPaneVisible( EFalse, EClockAppWorldViewId );
	
	// Launch and Fetch image from media gallery.
	iGalleryOpen = ETrue;
	
	iCanceler = NULL;
	result = MGFetch::RunL( *selectedFile, EImageFile, EFalse,KNullDesC(),KNullDesC(),NULL,iCanceler,NULL);
	
	iGalleryOpen = EFalse;
	
	// Show the navigation pane.
	appUi->MakeNavigationPaneVisible( ETrue, EClockAppWorldViewId );

	// If an image is selected by the user
	if( result )
	    {	
        TPtrC fileName = ( *selectedFile )[ KErrNone ];
        
	    
	    // Add the image for the currently focussed list item. 
	    iContainer->UpdateImageL( fileName, currentIndex );
            iContainer->ListBox()->SetCurrentItemIndexAndDraw( currentIndex );
	
	    // Update the Document.
	    CClockDocument *clockDocument = static_cast< CClockDocument* > ( AppUi()->Document() );
	    clockDocument->StoreDataL();  
	    }
	    
	CleanupStack::PopAndDestroy( selectedFile );
	
    __PRINTS( "CClockWorldView::AddImageL - Exit" );
    }

// ---------------------------------------------------------
// CClockWorldView::RestartTimerL
// rest of the details are commented in the header
// ---------------------------------------------------------
//    
void CClockWorldView::RestartTimerL()
    {
    if( !iTimer->IsActive() )
        {
        TTime currentTime;
        currentTime.HomeTime();
        
        TDateTime currentDateTime( currentTime.DateTime() );
        iTimer->After(KIntervalTime - 1000000 * currentDateTime.Second() - currentDateTime.MicroSecond());
        }
    }

// ---------------------------------------------------------
// CClockWorldView::StopTimerL
// rest of the details are commented in the header
// ---------------------------------------------------------
//    
void CClockWorldView::StopTimerL()
    {
    if( iTimer->IsActive() )
        {
        iTimer->Cancel();
        }
    }

// End of file
