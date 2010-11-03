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
* Description:  Active object class that makes asycn request to launch the map
 *
*/


// INCLUDES
#include <calencontext.h>
#include <StringLoader.h>
#include <calentry.h>
#include <aknnotewrappers.h>
#include <mnmapview.h>
#include <lbsposition.h>
#include <EPos_CPosLmDatabaseManager.h>
#include <EPos_CPosLandmarkDatabase.h>  
#include <EPos_HPosLmDatabaseInfo.h> 
#include <EPos_Landmarks.h>
#include <e32math.h>
#include <caleninstanceid.h>            // TCalenInstanceId

//debug
#include "calendarui_debug.h"
#include "calenlocationactiveselector.h"
#include "calenglobaldata.h"

// ----------------------------------------------------------------------------
// CCalenLocationActiveSelector::NewL
// NewL function
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
CCalenLocationActiveSelector* CCalenLocationActiveSelector::NewL(MCalenSelectionObserver& aObserver,
																	MAknServerAppExitObserver& aExitObserver,
																	CCalenGlobalData& aGlobalData)
	{
	TRACE_ENTRY_POINT;

    CCalenLocationActiveSelector* self = new(ELeave) CCalenLocationActiveSelector( aObserver,  
    																				aExitObserver, 
    																				aGlobalData);
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    TRACE_EXIT_POINT;
    return self;	
	}
	
// ----------------------------------------------------------------------------
// CCalenLocationActiveSelector::CCalenLocationActiveSelector
// C++ constructor
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
CCalenLocationActiveSelector::CCalenLocationActiveSelector( MCalenSelectionObserver& aObserver,
															MAknServerAppExitObserver& aExitObserver,
															CCalenGlobalData& aGlobalData) 
:   CActive( CActive::EPriorityStandard ),
    iObserver( aObserver ),
    iExitObserver(aExitObserver),
    iGlobalData(aGlobalData)
    {
    TRACE_ENTRY_POINT;
    
    CActiveScheduler::Add( this );
    
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenLocationActiveSelector::ConstructL
// Second Phace constructor
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenLocationActiveSelector::ConstructL()
	{
	TRACE_ENTRY_POINT;


	TRACE_EXIT_POINT;	
	}
// ----------------------------------------------------------------------------
// CCalenLocationActiveSelector::~CCalenLocationActiveSelector
// Destructor
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
CCalenLocationActiveSelector::~CCalenLocationActiveSelector()
    {
    Cancel();
    ReleaseLandmarkResources();
    }

// ----------------------------------------------------------------------------
// CCalenLocationActiveSelector::SelectL
// Initiates selection operation
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenLocationActiveSelector::SelectL( CMnProvider& aProvider,
											TPosition& aPosition,
											TBool /*isLaunchedFromEditors*/)
    {
    if ( IsActive() )
        {
        // avoid two simultaneous selection requests
        User::Leave( KErrInUse );
        }
    
    // create connection
    iMapView = CMnMapView::NewChainedL( aProvider );
    _LIT(text, "Select Meeting Location");
    iMapView->SetCustomRequestTextL(text);
    
    iMapView->SetExitObserverL(iExitObserver);
    
    // Set the current location coordinates
    TReal latitude = aPosition.Latitude();
	TReal longitude = aPosition.Longitude();
	TCoordinate currentPoint(latitude, longitude);
	iMapView->SetMapAreaL(currentPoint,100000);
    iMapView->SetCurrentLocationOption(CMnMapView::ECurrentLocationShowAlways);
    
    // Start the request
    iMapView->SelectFromMapL( iStatus );
    SetActive();
    }

// ----------------------------------------------------------------------------
// CCalenLocationActiveSelector::SelectL
// Initiates selection operation
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenLocationActiveSelector::SelectL( CMnProvider& aProvider,
                                            CPosLandmark* aLandmarkToShow)
    {
    if ( IsActive() )
        {
        // avoid two simultaneous selection requests
        User::Leave( KErrInUse );
        }
    
    // create connection
    iMapView = CMnMapView::NewChainedL( aProvider );
    _LIT(text, "Select Meeting Location");
    iMapView->SetCustomRequestTextL(text);
    iMapView->SetExitObserverL(iExitObserver);
    if(aLandmarkToShow)
        {
        // Add available landmarks
        RPointerArray<CPosLandmark> landmarks;
        landmarks.Append(aLandmarkToShow);
        iMapView->AddLandmarksToShowL(landmarks);
        landmarks.Reset(); 
        }
    
    // Start the request
    iMapView->SelectFromMapL( iStatus );
    SetActive();
    }

// ----------------------------------------------------------------------------
// CCalenLocationActiveSelector::RetrieveAndAddLandMarksL
// Reads all landmarks from all the available databases in the system
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//    
void CCalenLocationActiveSelector::RetrieveAndAddLandMarksL(CMnMapView* aMapView,
                                                             TBool isLaunchedFromEditors)
	{
	RPointerArray<CPosLandmark> landmarks;
	CPosLmDatabaseManager* dbManager = CPosLmDatabaseManager::NewL();
	CleanupStack::PushL( dbManager );

	// Get a list of databases
	RPointerArray<HPosLmDatabaseInfo> dbInfoList;
	dbManager->ListDatabasesL( dbInfoList );

	for ( TInt i = 0; i < dbInfoList.Count(); ++i )
	    {
	    HPosLmDatabaseInfo* dbInfo = dbInfoList[i];

	    // Get information about the database
	    TPtrC uri = dbInfo->DatabaseUri();
	    CPosLandmarkDatabase* db = CPosLandmarkDatabase::OpenL(uri);
		CleanupStack::PushL( db );

	    if(db->IsInitializingNeeded())
		    {
		    db->InitializeL();	
		    }
		// Create an iterator for iterating the landmarks in the database
		CPosLmItemIterator* iter = db->LandmarkIteratorL();
		CleanupStack::PushL( iter );

		// Read each landmark in the database and do something.
		TPosLmItemId lmID = KPosLmNullItemId;
		while ( ( lmID = iter->NextL() ) != KPosLmNullItemId )
		    {
		    CPosLandmark* lm = db->ReadLandmarkLC( lmID );
		    CleanupStack::Pop();
		    // Add landmark into array
		    landmarks.Append(lm);
		    }
		// Close the iterator and the database handle.
		CleanupStack::PopAndDestroy( iter );
		CleanupStack::PopAndDestroy( db );
	    }

	// Destroy list
	dbInfoList.ResetAndDestroy();

	CleanupStack::PopAndDestroy( dbManager );
	
	// Append the landmark from context if any
    MCalenContext& context = iGlobalData.Context();
    CPosLandmark* landmark = context.GetLandMark();
    //TBool isCtxtLmAdded = EFalse;
    if(landmark)
	    {
	    TPosition position;
	    landmark->GetPosition(position);
	    if(Math::IsNaN(position.Latitude()))
		    {
		    //landmarks.Append(landmark);
		    //isCtxtLmAdded = ETrue;	
		    }	
	    }
    else if( !isLaunchedFromEditors )  // If launched frm day view or event viewer
	    {
	    // Get the entry
		TCalLocalUid instanceId = context.InstanceId().iEntryLocalUid;
		CCalEntry* entry = iGlobalData.EntryViewL(context.InstanceId().iColId)->FetchL(instanceId);
		if(entry)
			{
			CleanupStack::PushL( entry );
			CCalGeoValue* geoValue = entry->GeoValueL();
			if(!geoValue)
				{
				TPtrC location = entry->LocationL();
				if(location.Length())
					{
				/*	CPosLandmark* landmark = CPosLandmark::NewL();
					landmark->SetLandmarkNameL(location);
					landmarks.Append(landmark);*/	
					}
				}
			else
				{
				delete geoValue;
				}
			CleanupStack::PopAndDestroy( entry );
			}
	    }
	
	// Add landmarks to show
	aMapView->AddLandmarksToShowL(landmarks);
	
	// Delete landmark pointers
    context.ResetLandMark();
    landmarks.ResetAndDestroy();	
	}

// ----------------------------------------------------------------------------
// CCalenLocationActiveSelector::RunL
// RunL method to handle the user selection
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//    
void CCalenLocationActiveSelector::RunL()
    {
    // request is completed, inform observer
    iObserver.HandleSelectionCompletedL( *iMapView, iStatus.Int() );
    }

// ----------------------------------------------------------------------------
// CCalenLocationActiveSelector::RunError
// Function to handle any errors in async request
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//    
TInt CCalenLocationActiveSelector::RunError( TInt /*aError*/ )
    {
    Reset();
    return KErrNone;
    }

// ----------------------------------------------------------------------------
// CCalenLocationActiveSelector::DoCancel
// Cancel method to handle the user selection
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenLocationActiveSelector::DoCancel()
    {
    if ( IsActive() && iMapView )
        {
        iMapView->Cancel();
        }
    Reset();
    }

// ----------------------------------------------------------------------------
// CCalenLocationActiveSelector::Reset
// Disconnects from provider, when operation is completed
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenLocationActiveSelector::Reset()
    {
    if(iMapView)
	    {
	    iMapView->ResetLandmarksToShow();
	    delete iMapView;
    	iMapView = NULL;	
	    }
    ReleaseLandmarkResources();
    }

// END OF FILE
