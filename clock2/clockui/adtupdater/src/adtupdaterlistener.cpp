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
* Description:   This is the source file for the CAdtUpdaterListener class.
*
*/

// System includes

// User includes
#include "adtupdatercontainer.h"
#include "adtupdaterlistener.h"
#include "clocktimezoneresolver.h"
#include "clocktimesourceinterface.hrh"
#include "clock_debug.h"

// Constants
const TInt KNitzPlugin( 0x200159A5 );
const TInt KInvalidTzId( -1 );
const TInt KMaxMobileCountryCode( 4 );

// Literals
_LIT( KMobileCountryCode, "%d" );

// ---------------------------------------------------------
// CAdtUpdaterListener::NewL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
CAdtUpdaterListener* CAdtUpdaterListener::NewL( CAdtUpdaterContainer* aObserver )
    {
    __PRINTS( "CAdtUpdaterListener::NewL - Entry" );
    
    CAdtUpdaterListener* self = new ( ELeave ) CAdtUpdaterListener( aObserver );
    CleanupStack::PushL( self );
    
    self->ConstructL();
    
    CleanupStack::Pop( self );
    
    __PRINTS( "CAdtUpdaterListener::NewL - Exit" );
    
    return self;
    }

// ---------------------------------------------------------
// CAdtUpdaterListener::CAdtUpdaterListener
// rest of the details are commented in the header
// ---------------------------------------------------------
//
CAdtUpdaterListener::CAdtUpdaterListener( CAdtUpdaterContainer* aObserver ) : CActive( EPriorityStandard ),
																			  iObserver( aObserver ),
																			  iTimeAvailable( EFalse ),
																			  iMccAvailable( EFalse ),
																			  iCurrentMcc( NULL )
																			  
    {
    __PRINTS( "CAdtUpdaterListener::CAdtUpdaterListener - Entry" );
    
    CActiveScheduler::Add( this );
    
    __PRINTS( "CAdtUpdaterListener::CAdtUpdaterListener - Exit" );
    }

// ---------------------------------------------------------
// CAdtUpdaterListener::~CAdtUpdaterListener
// rest of the details are commented in the header
// ---------------------------------------------------------
//
CAdtUpdaterListener::~CAdtUpdaterListener()
	{
	__PRINTS( "CAdtUpdaterListener::~CAdtUpdaterListener - Entry" );
	
	if( IsActive() )
	    {
	    Cancel();
	    }
	
	iClkSrvIf.Close();
	
	__PRINTS( "CAdtUpdaterListener::~CAdtUpdaterListener - Exit" );
	}

// ---------------------------------------------------------
// CAdtUpdaterListener::ConstructL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CAdtUpdaterListener::ConstructL()
	{
	__PRINTS( "CAdtUpdaterListener::ConstructL - Entry" );
	
	// No implementation yet.
	
	__PRINTS( "CAdtUpdaterListener::ConstructL - Exit" );
	}

// ---------------------------------------------------------
// CAdtUpdaterListener::WaitForNitzInfoL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CAdtUpdaterListener::WaitForNitzInfoL()
	{
	__PRINTS( "CAdtUpdaterListener::WaitForNitzInfoL - Entry" );
	
	if( !IsActive() )
		{
		// We don't have Nitz info yet.
		iInfoAvailable = EFalse;
		
		// Connect to clockserver
		TInt errorVal = iClkSrvIf.Connect();
		
		// If the connection doesn't get established 
		if( KErrNone != errorVal )
			{
			// Notify that Nitz info is not available.
			iObserver->NitzInfoNotAvailableL();
			
			__PRINTS( "CAdtUpdaterListener::WaitForNitzInfoL - Exit: Server is not available." );		
			
			return;
			}
				
		// First check if Nitz is active.
		TBool protocolActive( EFalse );
		TInt retVal = iClkSrvIf.IsProtocolActive( KNitzPlugin, protocolActive );
		
		// If not, activate it.
		if( !protocolActive && KErrNone == retVal )
			{
			__PRINTS( "CAdtUpdaterListener::WaitForNitzInfoL : Plugin was not activated by clockserver." );
			
			retVal = iClkSrvIf.ActivateProtocol( KNitzPlugin );
			
			if( KErrNone != retVal )
				{
				// Notify that Nitz info is not available.
				iObserver->NitzInfoNotAvailableL();
							
				__PRINTS( "CAdtUpdaterListener::WaitForNitzInfoL - Exit: Plugin was not activated." );
			
				return;
				}
			}
		
		// Subscribe for notification when time changes.
		iClkSrvIf.NotifyOnChange( iWhatChanged, iWhoChanged, iStatus );
		
		// Set the active object as active.
		SetActive();
		}
	
	__PRINTS( "CAdtUpdaterListener::WaitForNitzInfoL - Exit" );
	}

// ---------------------------------------------------------
// CAdtUpdaterListener::RunL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CAdtUpdaterListener::RunL()
	{
	__PRINTS( "CAdtUpdaterListener::RunL - Entry" );
	
	__PRINT( "iStatus: %d", iStatus.Int() );
	
	if( iStatus == KErrNone )
		{	
		__PRINTS( "iStatus = KErrNone" );
		
		__PRINT( "iWhatChanged: %d", iWhatChanged );
		
		__PRINT( "iWhoChanged: %d", iWhoChanged );

		// We need time information only from Nitz.
		if( ( EComponentTime == iWhatChanged ) && ( KNitzPlugin == iWhoChanged ) )
			{
			// First get the time information.
			TInt returnVal = iClkSrvIf.GetProtocolInfo( KNitzPlugin, iTimeAttributes );
			
			if( KErrNone == returnVal )
				{
				__PRINTS( "EComponentTime Recieved" );
				
				// Now we have the Nitz information.
				iTimeAvailable = ETrue;
				}
			}
		
		else if( EComponentMcc == iWhatChanged )
			{
			__PRINTS( "EComponentMcc Recieved" );
			
			// Get the mcc from the clockserver.
			TInt returnVal = iClkSrvIf.GetCurrentMcc( iCurrentMcc );
			
			if( KErrNone == returnVal )
				{
				// We've got Mcc.
				iMccAvailable = ETrue;
				
				__PRINT( "Mcc received from clockserver: %d", iCurrentMcc );
				}
			}
		
		if( iTimeAvailable && iMccAvailable )
			{
			// The tzResolver object to get the timezone id.
			CClockTimeZoneResolver* tzResolver = CClockTimeZoneResolver::NewL();
		    CleanupStack::PushL(tzResolver);

			// Get the string form of mcc.
			TBuf< KMaxMobileCountryCode > mobileCountryCodeBuf;
			mobileCountryCodeBuf.Format( KMobileCountryCode, iCurrentMcc );
			
			// Resolve the data to get the timezone id.
			TInt returnVal = tzResolver->GetTimeZoneL( iTimeAttributes, mobileCountryCodeBuf, iTimeZoneId );
			
			__PRINT( "newTimeZoneId in RunL: %d", iTimeZoneId );
			
			//Cleanup
			CleanupStack::PopAndDestroy( tzResolver );
			
			// Notify the observer if there were no errors.
			if( ( KErrNone == returnVal ) && ( KInvalidTzId != iTimeZoneId ) )
				{
				iObserver->NitzInfoAvailableL( iTimeAttributes, iTimeZoneId );
				
				__PRINTS( "CAdtUpdaterListener::RunL - Exit: (iTimeAvailable && iMccAvailable )" );
				
				return;
				}
			}
		
		// Resubscribe for notification.
		iClkSrvIf.NotifyOnChange( iWhatChanged, iWhoChanged, iStatus );
		
		// Set the active object as active again.
		SetActive();
		}
	else if( iStatus == KErrCancel )
		{
		// If the control comes here, it means that the subscription was cancelled.
		// So we don't issue another request.
		}
	else 
		{
		// if we have received istatus  as other than KErrNone and KErrCancel, and we have still time  
		
		// Resubscribe for notification.
		iClkSrvIf.NotifyOnChange( iWhatChanged, iWhoChanged, iStatus );
		
		// Set the active object as active again.
		SetActive();
		}

	__PRINTS( "CAdtUpdaterListener::RunL - Exit" );
	}

// ---------------------------------------------------------
// CAdtUpdaterListener::ConfirmDataAvailability
// rest of the details are commented in the header
// ---------------------------------------------------------
//
TBool CAdtUpdaterListener::ConfirmDataAvailabilityL()
	{
	__PRINTS( "CAdtUpdaterListener::ConfirmDataAvailability - Entry" );
	
	// First check for mcc.
	if( !iMccAvailable )
		{
		TInt returnVal = iClkSrvIf.GetCurrentMcc( iCurrentMcc );
		
		if( iCurrentMcc && ( KErrNone == returnVal ) )
			{
			iMccAvailable = ETrue;
			}
		}
	// Now check for time.
	if( !iTimeAvailable )
		{
		TInt returnVal = iClkSrvIf.GetProtocolInfo( KNitzPlugin, iTimeAttributes );
		
		if( KErrNone == returnVal )
			{
			iTimeAvailable = ETrue;
			}
		}
	
	if( iTimeAvailable && iMccAvailable )
		{
		// The tzResolver object to get the timezone id.
		CClockTimeZoneResolver* tzResolver = CClockTimeZoneResolver::NewL();
		CleanupStack::PushL(tzResolver);

		// Get the string form of mcc.
		TBuf< 4 > mobileCountryCodeBuf;
		mobileCountryCodeBuf.Format( KMobileCountryCode, iCurrentMcc );

		// Resolve the data to get the timezone id.
		TInt returnVal = tzResolver->GetTimeZoneL( iTimeAttributes, mobileCountryCodeBuf, iTimeZoneId );

		__PRINT( "newTimeZoneId in RunL: %d", iTimeZoneId );
		
		//Cleanup 
		CleanupStack::PopAndDestroy( tzResolver );

		// Notify the observer if there were no errors.
		if( ( KErrNone == returnVal ) && ( KInvalidTzId != iTimeZoneId ) )
			{
			__PRINTS( "CAdtUpdaterListener::ConfirmDataAvailability - Exit" );
			
			// We have the time information from server.
			return ETrue; 
			}
		}
	
	__PRINTS( "CAdtUpdaterListener::ConfirmDataAvailability - Exit" );
	
	// We don't have the time information from server.
	return EFalse;
	}

// ---------------------------------------------------------
// CAdtUpdaterListener::NitzInfoAvailable
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CAdtUpdaterListener::NitzInfoAvailableL()
	{
	__PRINTS( "CAdtUpdaterListener::NitzInfoAvailable - Entry" );
	
	// Notify the observer with the data.
	iObserver->NitzInfoAvailableL( iTimeAttributes, iTimeZoneId );
	
	__PRINTS( "CAdtUpdaterListener::NitzInfoAvailable - Exit" );
	}

// ---------------------------------------------------------
// CAdtUpdaterListener::GetCurrentMcc
// rest of the details are commented in the header
// ---------------------------------------------------------
//
TInt CAdtUpdaterListener::GetCurrentMcc()
	{
	__PRINTS( "CAdtUpdaterListener::NitzInfoAvailable - Entry/Exit" );
	
	return iCurrentMcc;		
	}

// ---------------------------------------------------------
// CAdtUpdaterListener::DoCancel
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CAdtUpdaterListener::DoCancel()
	{
	__PRINTS( "CAdtUpdaterListener::DoCancel - Entry" );
	
	iClkSrvIf.NotifyOnChangeCancel();
	
	__PRINTS( "CAdtUpdaterListener::DoCancel - Exit" );
	}

// End of file
