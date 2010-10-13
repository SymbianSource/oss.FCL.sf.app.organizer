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
* Description:   Controller class for location UI behavior
 *
*/



//debug
#include "calendarui_debug.h"

#include "calenlocationui.h"
#include "calencontroller.h"
#include <calencommands.hrh>
#include "calenlocationactiveselector.h"
#include "calenglobaldata.h"
#include "calenlocationutil.h"
#include <Calendar.rsg>
#include <calencommonui.rsg>
#include <calentry.h>
#include <CalenInterimUtils2.h>
#include <aknnotewrappers.h>
#include <aknselectionlist.h>
#include <aknPopup.h>
#include <aknlists.h>
#include <StringLoader.h>
#include <sysutil.h>
#include <eikenv.h>
#include <mnmapview.h>
#include <mnproviderfinder.h>
#include <EPos_CPosLandmark.h>
#include <lbsposition.h>
#include <e32math.h>
#include <calencontext.h>
#include <AknUtils.h>

// CONSTANTS and MACROS
_LIT(KComma, ",");
_LIT(KNokiaVendorName, "Nokia gate5 GmbH");
const TInt KCalenMaxTextEditorLength(160);
_LIT( KReplaceWhitespaceChars, "\x0009\x000A\x000B\x000C\x000D\x2028\x2029" );

// ----------------------------------------------------------------------------
// CCalenLocationUi::NewL
// First stage construction
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
CCalenLocationUi* CCalenLocationUi::NewL( CCalenController& aController )
    {
    TRACE_ENTRY_POINT;

    CCalenLocationUi* self = new(ELeave) CCalenLocationUi( aController );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    TRACE_EXIT_POINT;
    return self;
    }

// ----------------------------------------------------------------------------
// CCalenLocationUi::~CCalenLocationUi
// Destructor
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
CCalenLocationUi::~CCalenLocationUi()
    {
    TRACE_ENTRY_POINT;
    if(iMapView)
       {
       iMapView->Cancel();
       iMapView->ResetLandmarksToShow();
       delete iMapView;
       iMapView = NULL;    
       }
    ReleaseLandmarkResources();
    
    iController.CancelNotifications( this );
    
    if(iLocationSelector)
	    {
	    delete iLocationSelector;	
	    }
	    
	if(iCurrLocAcquisition)
		{
		delete iCurrLocAcquisition;	
		}
		
	if( iGlobalData )
        {
        iGlobalData->Release();
        }
        
    if(iLocationText)
    	{
    	delete iLocationText;
    	iLocationText = NULL;
    	}

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenLocationUi::CCalenLocationUi
// Constructor
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
CCalenLocationUi::CCalenLocationUi( CCalenController& aController )
    : iController( aController )
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenLocationUi::ConstructL
// Second stage construction
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenLocationUi::ConstructL()
    {
    TRACE_ENTRY_POINT;
    
    isCurrLocAvailable = EFalse;
    iGlobalData = CCalenGlobalData::InstanceL();
	iLocationSelector = CCalenLocationActiveSelector::NewL( *this, *this, *iGlobalData);
	              
	RArray<TCalenNotification> notificationArray;
	notificationArray.Append( ECalenNotifyEntryClosed );
  	notificationArray.Append( ECalenNotifyInstanceSaved );
    notificationArray.Append( ECalenNotifyDialogClosed );
    notificationArray.Append( ECalenNotifyMapClosed );
    notificationArray.Append( ECalenNotifyCancelMapLaunch );
    notificationArray.Append( ECalenNotifyEntrySaved );
    notificationArray.Append( ECalenNotifyEntryDeleted );

	iController.RegisterForNotificationsL(this, notificationArray);
	notificationArray.Reset();
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenLocationUi::HandleActionUiCommandL
// Handles action ui commands
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
TBool CCalenLocationUi::HandleCommandL( const TCalenCommand& aCommand )
    {
    TRACE_ENTRY_POINT;
    
    if(isRequestPending)
	    {
	    // Already one request is being processed,
	    // Ignore this new request
	    return EFalse;	
	    }
	    
    TBool continueCommand(EFalse);
    TInt command = aCommand.Command();
    switch ( command )
	    {
	    case ECalenGetLocationAndSave:
	    	isGetLocationAndSave = ETrue;
		    LaunchMapL();
		    break;
	    case ECalenGetLocation:
		    LaunchMapL();
		    break;
	    case ECalenShowLocation:
	    	ShowLocationOnMapL();
	    	break;
	    case ECalenGetLocationAndReplace:
	        isGetLocationAndSave = ETrue;
	        isReplaceLocation = ETrue;
            LaunchMapL();
            break;
	    case ECalenShowLocationQuery:
	        {
	        TInt userResponse = ShowDefineLocationQuery();
	        if(userResponse)
	            {
	            isGetLocationAndSave = ETrue;
	            LaunchMapL();
	            }
	        else
	            {
	            iController.BroadcastNotification(ECalenNotifyMapClosed);
	            }
	        }
	        break;
	    default:
	    	break;
	    }
	    
	TRACE_EXIT_POINT;
    return continueCommand;
    }

// ----------------------------------------------------------------------------
// CCalenLocationUi::CalenCommandHandlerExtensionL
//
// ----------------------------------------------------------------------------
//
TAny* CCalenLocationUi::CalenCommandHandlerExtensionL( TUid /*aExtensionUid*/ )
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    return NULL;
    }

// ----------------------------------------------------------------------------
// CCalenLocationUi::HandleNotification
// Handle notifications
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenLocationUi::HandleNotification(const TCalenNotification aNotification )
    {
    TRACE_ENTRY_POINT;
    MCalenContext& context = iGlobalData->Context();

    switch( aNotification )
	    {
	    case ECalenNotifyEntryClosed:  // issued when new entry dialog is cancelled
        case ECalenNotifyEntrySaved:
        case ECalenNotifyInstanceSaved:
        case ECalenNotifyDialogClosed:
        case ECalenNotifyEntryDeleted:
        	{
        	if(isRequestPending)
	        	{
	        	iCurrLocAcquisition->CancelRequest();
	        	delete 	iCurrLocAcquisition;
	        	iController.BroadcastNotification(ECalenNotifyMapClosed);
	        	}
        	// Delete the landmark object from context
        	context.ResetLandMark();
        	}
        	break;
	    case ECalenNotifyMapClosed:
	    	// Repopulate the current view
	    	if(isGetLocationAndSave)
		    	{
		    	isGetLocationAndSave = EFalse;
	    		PIM_TRAPD_HANDLE(iController.IssueCommandL( ECalenStartActiveStep ));	
		    	}
	    	break;
	    case ECalenNotifyCancelMapLaunch:
	    	{
	    	// Cancel any pending async requests
	    	
	    	if(iMapView)
               {
               iMapView->Cancel();
               iMapView->ResetLandmarksToShow();
               delete iMapView;
               iMapView = NULL;    
               }
            ReleaseLandmarkResources();
	    	            
	    	iLocationSelector->DoCancel();
	    	if(iProvider)
	    	    {
    	    	delete iProvider;
    			iProvider = NULL;
	    	    }
	    	}
	    	break;
	    }
		    
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenLocationUi::RequestCurrenLocationL
// It issues asyn request to get the coordinates for current location
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCalenLocationUi::RequestCurrenLocationL()
    {
    TRACE_ENTRY_POINT;
     
    iCurrLocAcquisition = CCalenCurrentLocationAcquisition::NewL( *this);                
    isRequestPending = ETrue;
    iCurrLocAcquisition->RequestL();
    
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenLocationUi::NotifyCurrentLocationL
// Callback which gets called when async request is complete.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCalenLocationUi::NotifyCurrentLocationL()
	{
	TRACE_ENTRY_POINT;
	
	// Asyn request complete, now get the coordinates of current location
	TPosition currPosition = iCurrLocAcquisition->CurrentPosition();	
	delete iCurrLocAcquisition;
	iCurrPosition = currPosition;
	isCurrLocAvailable = ETrue;
	isRequestPending = EFalse;	
	LaunchMapL(currPosition);
	
	TRACE_EXIT_POINT;
	}

// -----------------------------------------------------------------------------
// CCalenLocationUi::NotifyCurrentLocationL
// Callback function that gets called if there is any error
// while fetching coordinates for current location
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCalenLocationUi::NotifyErrorL(const TInt /* errorCode */)
	{
	TRACE_ENTRY_POINT;
	
	delete iCurrLocAcquisition;
	isRequestPending = EFalse;
	
	// Error occurred, launch the map with default values
	LaunchMapL();
	
	TRACE_EXIT_POINT;	
	}
	
// ----------------------------------------------------------------------------
// CCalenLocationUi::LaunchMapL
// Launches Map with coords of current location 
// for the user to select the event location
// (other items were commented in a header).
// ----------------------------------------------------------------------------
// 
void CCalenLocationUi::LaunchMapL(TPosition& aPosition)
	{
	TRACE_ENTRY_POINT;
	
    const CMnProvider::TServices neededServices = CMnProvider::EServiceNavigation |
    											  CMnProvider::EServiceMapView ; 									  
	iProvider = GetFirstProviderL(neededServices);
	iLocationSelector->SelectL( *iProvider, aPosition, !isGetLocationAndSave );
	
	TRACE_EXIT_POINT;
	}

// ----------------------------------------------------------------------------
// CCalenLocationUi::LaunchMapL
// Launches Map with default coords for the user to select the event location
// (other items were commented in a header).
// ----------------------------------------------------------------------------
// 
void CCalenLocationUi::LaunchMapL()
	{
	TRACE_ENTRY_POINT;
	
	const CMnProvider::TServices neededServices = CMnProvider::EServiceNavigation |
	                                                  CMnProvider::EServiceMapView ;                                      
	iProvider = GetFirstProviderL(neededServices);
	MCalenContext& context = iGlobalData->Context();
    CPosLandmark* landmark = context.GetLandMark();
	if(landmark)
	    {
	        {
	        // entry has proepr geovalues, launch the map with that location at the centre
	        iLocationSelector->SelectL( *iProvider, landmark );
	        }
		
	    context.ResetLandMark();
	    }
	else if(isGetLocationAndSave) // If issued from Day view or event viewer
	    {
	    // Get the entry
		TCalLocalUid instanceId = context.InstanceId().iEntryLocalUid;
		CCalEntry* entry = iGlobalData->EntryViewL(context.InstanceId().iColId)->FetchL(instanceId);
		if(entry)
			{
			CleanupStack::PushL( entry );
			CCalGeoValue* geoValue = entry->GeoValueL();
			TPtrC location = entry->LocationL();
			if(geoValue)
				{
				// entry has proepr geovalues, launch the map with that location at the centre
				TReal latitude;
                TReal longitude;
                geoValue->GetLatLong(latitude, longitude);
                TPosition position;
                position.SetCoordinate(latitude, longitude);
                CPosLandmark* landmarkToShow = CPosLandmark::NewL();
                //TPtrC location = entry->LocationL();
                landmarkToShow->SetLandmarkNameL(location);
                landmarkToShow->SetPositionL(position);
				delete geoValue;	
				iLocationSelector->SelectL( *iProvider, landmarkToShow );
				delete landmarkToShow;
				}
			else if(location.Length())
			    {
                TBuf<KCalenMaxTextEditorLength> locationBuf;
                locationBuf.Copy(location);
                AknTextUtils::ReplaceCharacters(locationBuf, KReplaceWhitespaceChars, TChar(' '));
			    // Add dummy landmark, so that Maps search box will get filled
			    CPosLandmark* landmarkToShow = CPosLandmark::NewL();
                landmarkToShow->SetLandmarkNameL(locationBuf);
                iLocationSelector->SelectL( *iProvider, landmarkToShow );
                delete landmarkToShow;
			    }
			else{
			    iLocationSelector->SelectL( *iProvider, NULL );
			    }
			CleanupStack::PopAndDestroy( entry );
			}
		}
	else
	    {
	    iLocationSelector->SelectL( *iProvider, NULL );
	    }
	
	TRACE_EXIT_POINT;
	}

// ----------------------------------------------------------------------------
// CCalenLocationUi::ShowLocationOnMapL
// Launches Map for the user to show the event location
// (other items were commented in a header).
// ----------------------------------------------------------------------------
// 
void CCalenLocationUi::ShowLocationOnMapL()
	{
	TRACE_ENTRY_POINT;
	
	const CMnProvider::TServices neededServices = CMnProvider::EServiceMapView ; 
	iProvider = GetFirstProviderL(neededServices);
	
	iMapView = CMnMapView::NewChainedL( *iProvider );
	iMapView->SetExitObserverL(*this);
	
    // Get the entry
    MCalenContext& context = iGlobalData->Context();
	CPosLandmark* landmark = context.GetLandMark();
	RPointerArray<CPosLandmark> landmarks;
	
	    if(landmark)
	        {
	        landmarks.Append(landmark);
	        iMapView->AddLandmarksToShowL(landmarks);
	        context.ResetLandMark();
	        }
	    else
	        {
	        TCalLocalUid instanceId = context.InstanceId().iEntryLocalUid;
            CCalEntry* entry = iGlobalData->EntryViewL(context.InstanceId().iColId)->FetchL(instanceId);
            if(entry)
                {
                CleanupStack::PushL( entry );
                CCalGeoValue* geoValue = entry->GeoValueL();
                if(geoValue)
                    {
                    TReal latitude;
                    TReal longitude;
                    geoValue->GetLatLong(latitude, longitude);
                    TPosition position;
                    position.SetCoordinate(latitude, longitude);
                    CPosLandmark* landmarkToShow = CPosLandmark::NewL();
                    TPtrC location = entry->LocationL();
                    landmarkToShow->SetLandmarkNameL(location);
                    landmarkToShow->SetPositionL(position);
                    landmarks.Append(landmarkToShow);
                    iMapView->AddLandmarksToShowL(landmarks);
                    delete landmarkToShow;
                    delete geoValue;    
                    }
                CleanupStack::PopAndDestroy( entry );
                }
	        } 
	_LIT(text, "Meeting Location");
	
    // start request
    iMapView->SetCustomRequestTextL(text);
	iMapView->ShowMapL(); 
	
	TRACE_EXIT_POINT;
	}

// ----------------------------------------------------------------------------
// CCalenLocationUi::HandleSelectionCompletedL
// Handles user selection for the event location on the map
// (other items were commented in a header).
// ----------------------------------------------------------------------------
// 	
void CCalenLocationUi::HandleSelectionCompletedL( CMnMapView& aMapView, TInt aError )
    {
    TRACE_ENTRY_POINT;
    
    // Forcefully broadcast app foreground notification, so that
    // ECalenMapState would be current state
    if(!iController.IsFasterAppFlagEnabled())
        {
    	iController.BroadcastNotification(ECalenNotifyAppForegrounded);	
        }
    // selection is done, analyze error code first...
    if ( !aError )
        {
        // retrieve result
        switch ( aMapView.SelectionResultType() )
            {
            case CMnMapView::ESelectionFreeLandmark:
                {
                // retrieve result landmark as free-location landmark
                CPosLandmark* landmark = aMapView.RetrieveSelectionResultL();
                TPtrC name;
                TBuf<3 * KCalenMaxTextEditorLength> landmarkname;
                // Get the location name if available
                TPtrC locationName;
                if( landmark->IsPositionFieldAvailable( EPositionFieldLocationName ) )
                    {
                    landmark->GetPositionField( EPositionFieldLocationName, locationName );
                    landmarkname.Append(locationName);
                    }
                // Get the street name if available
                TPtrC street;
                if( landmark->IsPositionFieldAvailable( EPositionFieldStreet ) )
                    {
                    if(landmarkname.Length())
                        {
                        // Append comma before we add this field
                        landmarkname.Append(KComma);
                        }
                    landmark->GetPositionField( EPositionFieldStreet, street );
                    landmarkname.Append(street);
                    }
                // Get the city name if available
                TPtrC city;
                if( landmark->IsPositionFieldAvailable( EPositionFieldCity ) )
                    {
                    if(landmarkname.Length())
                        {
                        // Append comma before we add this field
                        landmarkname.Append(KComma);
                        }
                    landmark->GetPositionField( EPositionFieldCity, city );
                    landmarkname.Append(city);
                    } 
                // Get the state name if available
                TPtrC state;
                if( landmark->IsPositionFieldAvailable( EPositionFieldState ) )
                    {
                    if(landmarkname.Length())
                        {
                        // Append comma before we add this field
                        landmarkname.Append(KComma);
                        }
                    landmark->GetPositionField( EPositionFieldState, state );
                    landmarkname.Append(state);
                    } 
                // Get the country name if available                    
                TPtrC country;
                if( landmark->IsPositionFieldAvailable( EPositionFieldCountry ) )
                    {
                    if(landmarkname.Length())
                        {
                        // Append comma before we add this field
                        landmarkname.Append(KComma);
                        }
                    landmark->GetPositionField( EPositionFieldCountry, country );
                    landmarkname.Append(country);
                    }
 				
               if(landmarkname.Length() > KCalenMaxTextEditorLength )
                     {
                     // Truncate the location details to fit to its max length
                     TBuf<KCalenMaxTextEditorLength> truncLocationInfo;
                     
                     truncLocationInfo.Append(landmarkname.Left(KCalenMaxTextEditorLength));
                     landmark->SetLandmarkNameL(truncLocationInfo);
                     }
               else
                     {
                     landmark->SetLandmarkNameL(landmarkname);
                     }
               	
               	iLocationSelector->Reset();
			    delete iProvider;
				iProvider = NULL;
				if(!landmarkname.Length())
                    {
                    // User has selected a landmark that does'nt have any name identified
                    // hence, return without saving the landmark
                    delete landmark;
                    iController.BroadcastNotification(ECalenNotifyMapClosed);
                    return;
                    }
               	if(isGetLocationAndSave)
	               	{
	               	StoreLocationInfoL(landmark);
	               	delete landmark;	
	               	}
	            else
	            	{
	            	// Get the context
	            	MCalenContext& context = iGlobalData->Context();
	            	context.SetLandMark(landmark);	
	            	}
               	iController.BroadcastNotification(ECalenNotifyMapClosed);
                }
                break;
                
             default:
             	break;
            }
        }
    else if( aError == -3 ) // If user has pressed  back without selecting any locaiton on map, then
        {                   // cleanup everything. Not doing here is causing maps crash when coming back without selecting any location
        iLocationSelector->Reset();
        delete iProvider;
        iProvider = NULL;
        // Issue Map Closed notification
        iController.BroadcastNotification(ECalenNotifyMapClosed);
        }
	
	TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenLocationUi::HandleServerAppExit
// Handle the exit of a connected server app.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
// 
void CCalenLocationUi::HandleServerAppExit(TInt  aReason )
	{
	TRACE_ENTRY_POINT;
	
	if(!iMapView)
	    {
	    iLocationSelector->Reset();	
	    delete iProvider;
		iProvider = NULL;      
		// Issue Map Closed notification    
		iController.BroadcastNotification(ECalenNotifyMapClosed);
		}
	else{
        
        // Remove the observer
        iMapView->RemoveExitObserver(); 
        iMapView->ResetLandmarksToShow();
        
        // Cleanup
        delete iMapView;
        iMapView = NULL;
        delete iProvider;
        iProvider = NULL;
        // Issue Map Closed notification
        iController.BroadcastNotification(ECalenNotifyMapClosed);
        }
	    
	if((aReason == EAknCmdExit)  || (aReason == EAknSoftkeyExit)|| (aReason == -15)) /* if user presses END key, then terminate calendar application
	     * -15 is checked because aReason will be -15 when END key is pressed while maps is being launched and launching is above 60% complete. 
	     * If END key is pressed before 60% of completion, Calendar is not getting any notification from map. Hence, Calendar will
	     * be sent to background but not exited */
	    {                                             
		// User has pressed red button on maps, fake exit calendar
		TRAP_IGNORE(iController.IssueCommandL(EAknSoftkeyExit));
		}
	
	TRACE_EXIT_POINT;	
	}

// ----------------------------------------------------------------------------
// CCalenLocationUi::GetFirstProviderL
// Returns the Map provider available in the system
// (other items were commented in a header).
// ----------------------------------------------------------------------------
// 	
CMnProvider* CCalenLocationUi::GetFirstProviderL( CMnProvider::TServices aServicesNeeded )
	{
	TRACE_ENTRY_POINT;
	
	RPointerArray<CMnProvider> providers;
    MnProviderFinder::FindProvidersL( providers, aServicesNeeded );
	
    TInt count = providers.Count();
    CMnProvider* nokiaProvider = NULL;

    for(TInt i = 0; i < count; i++)
	    {
	    TPtrC vendorName;
        providers[i]->GetVendorName(vendorName);
        if(vendorName.Compare(KNokiaVendorName) == 0)
	        {
	        // Found provider for Nokia maps
	        nokiaProvider = providers[i];
	        providers.Remove(i);
	        break;	
	        }
	    }
    providers.ResetAndDestroy();
    return nokiaProvider;
    
    TRACE_EXIT_POINT;	
	}

// ----------------------------------------------------------------------------
// CCalenLocationUi::ShowLocationReplaceNotice
// Function to query user whether to replace or keep existing location details
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
TInt CCalenLocationUi::ShowLocationReplaceNoticeL(TPtrC aLocaitonName)
	{
	TRACE_ENTRY_POINT;
	
	CAknQueryDialog *dialog = CAknQueryDialog::NewL( );
    CleanupStack::PushL( dialog );
    TInt resID = R_CALEN_QTN_REPLACE_LOCATION;
    
    TBuf<KCalenMaxTextEditorLength> location;
    TChar quote = '"';
    location.Append(quote);
    location.Append(aLocaitonName);
    location.Append(quote);
    
    HBufC* prompt = StringLoader::LoadL( resID, location, CEikonEnv::Static() );
    CleanupStack::PushL( prompt );
	dialog->SetPromptL( *prompt );
    CleanupStack::PopAndDestroy( prompt );

    CleanupStack::Pop( dialog );
	
	return dialog->ExecuteLD( R_CALEN_LOCATION_REPLACE_NOTE );
	
	TRACE_EXIT_POINT;
	}

// ----------------------------------------------------------------------------
// CCalenLocationUi::ShowAddressUpdatedNoticeL
// Displays "Address Updated" transient notice
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//  
void CCalenLocationUi::ShowAddressUpdatedNoticeL()
    {
    HBufC* buf = StringLoader::LoadLC( R_CALEN_QTN_ADDRESS_UPDATED, CEikonEnv::Static() );
    CAknConfirmationNote* dialog = new( ELeave ) CAknConfirmationNote();
    dialog->ExecuteLD(*buf);
    CleanupStack::PopAndDestroy( buf );     
    }

// -----------------------------------------------------------------------------
//  CCalenLocationUi::ShowDefineLocationQuery
//  Queries user to validate the location frm maps or not
// -----------------------------------------------------------------------------
//  
TInt CCalenLocationUi::ShowDefineLocationQuery()
    {
    TRACE_ENTRY_POINT;
    
    CAknQueryDialog *dialog = CAknQueryDialog::NewL( );
    CleanupStack::PushL( dialog );
    TInt resID = R_CALEN_QTN_DEFINE_LOCATION_NOTE;
    
    HBufC* prompt;
    prompt = StringLoader::LoadLC( resID, CEikonEnv::Static() );
    
    dialog->SetPromptL( *prompt );
    CleanupStack::PopAndDestroy( prompt );

    CleanupStack::Pop( dialog );
    return dialog->ExecuteLD( R_CALEN_DEFINE_LOCATION_NOTE ); 
    
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenLocationUi::ShowLocationReplaceNotice
// Stores the locaiton details of currently focusses entry in Day/Event Viewer 
// into Agenda Server
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//	
void CCalenLocationUi::StoreLocationInfoL(CPosLandmark* landmark)
	{
	TRACE_ENTRY_POINT;
	
	MCalenContext& context = iGlobalData->Context();
	// Get the entry
	TCalLocalUid instanceId = context.InstanceId().iEntryLocalUid;
	CCalEntry* entry = iGlobalData->EntryViewL(context.InstanceId().iColId)->FetchL(instanceId);

	if(entry)
	    {
        TPtrC existingLocationInfo = entry->LocationL();
        TBuf<2*KCalenMaxTextEditorLength> locationInfo;
        TPtrC landmarkname;
        landmark->GetLandmarkName(landmarkname);
        CCalGeoValue* entryGeoValue = entry->GeoValueL();
        if(entryGeoValue || isReplaceLocation)
        {
        isReplaceLocation = EFalse;
        // Query user to replace
        TInt userResponse = ShowLocationReplaceNoticeL(landmarkname);
        if(!userResponse)
            {
            delete entryGeoValue;
            delete entry;
            return;	
            }
        else
            {
            locationInfo.Append(landmarkname);
            delete entryGeoValue;
            }
        }
        else if(existingLocationInfo.Length() && !isReplaceLocation)
        {
        RPointerArray<HBufC> locationStrings;
        HBufC* oldLocation = HBufC::NewL(KCalenMaxTextEditorLength);
        oldLocation->Des().Copy(existingLocationInfo);
        
        HBufC* oldNewLocation = HBufC::NewL(KCalenMaxTextEditorLength);
        TBuf<2*KCalenMaxTextEditorLength> combLocation;
        combLocation.Append(existingLocationInfo);
        combLocation.Append(KComma);
        combLocation.Append(landmarkname);
        oldNewLocation->Des().Copy(combLocation.Left(KCalenMaxTextEditorLength));
        
        HBufC* newLocation = HBufC::NewL(KCalenMaxTextEditorLength);
        newLocation->Des().Copy(landmarkname);
        
        locationStrings.Append(oldNewLocation);
        locationStrings.Append(newLocation);
        locationStrings.Append(oldLocation);
        TInt userResponse = CCalenLocationUtil::ShowLocationAppendOrReplaceL(locationStrings);
        locationStrings.ResetAndDestroy();
        if(userResponse == KErrCancel)
            {
            delete entry;
            return;
            }
        switch(userResponse)
            {
            case 0:
                {
                locationInfo.Append(combLocation.Left(KCalenMaxTextEditorLength));
                }
                break;
            case 1:
                {
                locationInfo.Append(landmarkname);
                }
                break;
            case 2:
                {
                locationInfo.Append(existingLocationInfo);
                }
                break;
            default:
                break;
            }
        }
        else // for isReplaceLocation
            {
            locationInfo.Append(landmarkname);
            }
        TPtrC landmarkDesc;
        landmark->GetLandmarkDescription(landmarkDesc);
        if(landmarkDesc.Size())
            {
            locationInfo.Append(KComma);
            locationInfo.Append(landmarkDesc);
            }
        
        // Get the geo coordinates	
        TLocality position;
        landmark->GetPosition(position);
        CCalGeoValue* geoValue = CCalGeoValue::NewL();
        geoValue->SetLatLongL(position.Latitude(), position.Longitude());
        
        // Get the context
        entry->SetLocationL(locationInfo);
        entry->SetGeoValueL(*geoValue);
        delete geoValue;
        
        if(existingLocationInfo.Length())
            {
            ShowAddressUpdatedNoticeL();
            }
        // Save entry into Agenda server
        CCalenInterimUtils2::StoreL( *(iGlobalData->EntryViewL(context.InstanceId().iColId)), *entry, ETrue );
        delete entry;
	    }
	
	
	TRACE_EXIT_POINT;	
	}
	
// End Of File    
