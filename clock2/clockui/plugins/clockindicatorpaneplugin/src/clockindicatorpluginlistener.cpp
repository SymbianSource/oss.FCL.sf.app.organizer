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
* Description:   Implementation for CClockIndicatorPluginListener class.
*
*/

// System includes

// User includes
#include "clockindicatorpluginlistener.h"
#include "clockindicatorpaneplugin.h"

// ---------------------------------------------------------
// CClockIndicatorPluginListener::NewL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
CClockIndicatorPluginListener* CClockIndicatorPluginListener::NewL( CClockIndicatorPanePlugin* aPanePlugin )
    {
    CClockIndicatorPluginListener* self = new( ELeave ) CClockIndicatorPluginListener( aPanePlugin );
    CleanupStack::PushL( self );

    self->ConstructL();
    
    CleanupStack::Pop( self );
    
    return self;
    }

// ---------------------------------------------------------
// CClockIndicatorPluginListener::ConstructL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClockIndicatorPluginListener::ConstructL()
	{
	// No implementation yet.
	}

// ---------------------------------------------------------
// CClockIndicatorPluginListener::CClockIndicatorPluginListener
// rest of the details are commented in the header
// ---------------------------------------------------------
//
CClockIndicatorPluginListener::CClockIndicatorPluginListener( CClockIndicatorPanePlugin* aPanePlugin )
														: CActive( CActive::EPriorityStandard ),
														  iPanePlugin( aPanePlugin )
	{
	CActiveScheduler::Add( this );
	}
	
// ---------------------------------------------------------
// CClockIndicatorPluginListener::~CClockIndicatorPluginListener
// rest of the details are commented in the header
// ---------------------------------------------------------
//
CClockIndicatorPluginListener::~CClockIndicatorPluginListener()
	{
	if( IsActive() )
		{
		Cancel();
		}
	}

// ---------------------------------------------------------
// CClockIndicatorPluginListener::RunL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClockIndicatorPluginListener::RunL()
	{
	if( iStatus != KRequestPending )
		{
		// There's a change. Cancel any pending request.
		NotifyChangeCancel();
		
		// Call update of the plugin.
		iPanePlugin->UpdatePluginDataL();
		}
	}

// ---------------------------------------------------------
// CClockIndicatorPluginListener::DoCancel
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClockIndicatorPluginListener::DoCancel()
	{
	// No implementation yet.
	}

// ---------------------------------------------------------
// CClockIndicatorPluginListener::NotifyChange
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClockIndicatorPluginListener::NotifyChange()
	{
	// Return if its already active.
	if( IsActive() )
		{
		return;
		}
		
	// We're not specifically interested in which alarm changes.
	TAlarmId dummyAlarmId;
	
	// Register for notification		
	iPanePlugin->NotifyChange( iStatus, dummyAlarmId );
	
	// Set the object active.
	SetActive();
	}
	
// ---------------------------------------------------------
// CClockIndicatorPluginListener::NotifyChangeCancel
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClockIndicatorPluginListener::NotifyChangeCancel()
	{
	// First cancel all requests
	Cancel();
	
	iPanePlugin->NotifyChangeCancel();
	}
	
// End of file
