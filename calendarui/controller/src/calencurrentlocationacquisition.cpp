/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:    Active class to reteive the coordiantes for current location
 *
*/



// INCLUDES
#include "calencurrentlocationacquisition.h"
//debug
#include "calendarui_debug.h"


// ----------------------------------------------------------------------------
// CCalenCurrentLocationAcquisition::NewL
// First stage construction
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
CCalenCurrentLocationAcquisition* CCalenCurrentLocationAcquisition::NewL( MCalenCurrentLocationObserver& aObserver )
	{
	CCalenCurrentLocationAcquisition* self = new ( ELeave ) CCalenCurrentLocationAcquisition(aObserver);
	CleanupStack::PushL( self );
	self->ConstructL();
	CleanupStack::Pop(self); // self
	return self;	
	}

// ----------------------------------------------------------------------------
// CCalenCurrentLocationAcquisition::CCalenCurrentLocationAcquisition
// C++ constructor
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
CCalenCurrentLocationAcquisition::CCalenCurrentLocationAcquisition(MCalenCurrentLocationObserver& aObserver)
	:CActive( CActive::EPriorityStandard ),
	 iObserver(aObserver)
	{
	CActiveScheduler::Add( this );	
	}

// ----------------------------------------------------------------------------
// CCalenCurrentLocationAcquisition::ConstructL
// Symbian 2nd phase constructor
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//		
void CCalenCurrentLocationAcquisition::ConstructL()
	{
	_LIT(AppName, "Calendar");
	HBufC* srvName = HBufC::NewL(20);
	srvName->Des().Copy(AppName);
	User::LeaveIfError(iServer.Connect());
	User::LeaveIfError(iPositioner.Open( iServer ));

	User::LeaveIfError( iPositioner.SetRequestor( 
	                    CRequestor::ERequestorService,
	                    CRequestor::EFormatApplication,
	                    *srvName )); 	
	}

// ----------------------------------------------------------------------------
// CCalenCurrentLocationAcquisition::~CCalenCurrentLocationAcquisition
// Destructor
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
CCalenCurrentLocationAcquisition::~CCalenCurrentLocationAcquisition()
	{
	Cancel();
	iPositioner.Close();
	iServer.Close();	
	}

// ----------------------------------------------------------------------------
// CCalenCurrentLocationAcquisition::RequestL
// Requests for current location asynchronously
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//	
void CCalenCurrentLocationAcquisition::RequestL()
	{    
	iPositioner.NotifyPositionUpdate( iPositionInfo, iStatus );        
	SetActive();
	}

// ----------------------------------------------------------------------------
// CCalenCurrentLocationAcquisition::CurrentPosition
// Function to return current position details
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//	
TPosition& CCalenCurrentLocationAcquisition::CurrentPosition()
	{
	return iPosition;
	}

// ----------------------------------------------------------------------------
// CCalenCurrentLocationAcquisition::CancelRequest
// Cancels the ongoing request
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//	
void CCalenCurrentLocationAcquisition::CancelRequest()
	{
	Cancel();
	}

// ----------------------------------------------------------------------------
// CCalenCurrentLocationAcquisition::RunL
// RunL method to handle the user selection
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//	
void CCalenCurrentLocationAcquisition::RunL()
	{
	switch ( iStatus.Int() )
		{
		case KErrNone:
		case KPositionPartialUpdate:        
			{
			iPositionInfo.GetPosition( iPosition );
			TReal32 altitude = iPosition.Altitude();
			if (Math::IsNaN( altitude))
				{
				TRealX nan;
				nan.SetNaN();
				iPosition.SetVerticalAccuracy( nan );	
				}
			iObserver.NotifyCurrentLocationL( );
			break;
			}
		case KErrAccessDenied:
		case KPositionQualityLoss:
		case KErrTimedOut:
		case KErrNotFound: // No PSY selected.
		case KErrUnknown:
		case KErrCancel:
		case KErrArgument:
		default:
			{
			// Notify error to observer
			iObserver.NotifyErrorL(iStatus.Int());
			}
		}
	}	

// ----------------------------------------------------------------------------
// CCalenCurrentLocationAcquisition::DoCancel
// Cancel method to handle the user selection
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenCurrentLocationAcquisition::DoCancel()
    {
    iPositioner.CancelRequest( EPositionerNotifyPositionUpdate );
    }

// ----------------------------------------------------------------------------
// CCalenCurrentLocationAcquisition::RunError
// Function to handle any errors in async request
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
TInt CCalenCurrentLocationAcquisition::RunError(TInt aError)
    {
    PIM_TRAPD_HANDLE(iObserver.NotifyErrorL( aError ));
    return KErrNone;
    }

// End of File
