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
* Description:   This is the source file for the CAdtUpdaterAppUi class.
*
*/

// System includes
#include <apgtask.h>
#include <w32std.h>

// User includes
#include "adtupdaterappui.h"
#include "adtupdatercontainer.h"
#include "clock_debug.h"

// Constants
const TInt KOrdinalPosition( 0 );
const TInt KOrdinalPriority( 0 );

// Literals

// ---------------------------------------------------------
// CAdtUpdaterAppUi::CAdtUpdaterAppUi
// rest of the details are commented in the header
// ---------------------------------------------------------
//
CAdtUpdaterAppUi::CAdtUpdaterAppUi()
    {
	__PRINTS( "CAdtUpdaterAppUi::CAdtUpdaterAppUi - Entry" );
	
    // No implementation yet.
	
	__PRINTS( "CAdtUpdaterAppUi::CAdtUpdaterAppUi - Exit" );
    }

// ---------------------------------------------------------
// CAdtUpdaterAppUi::~CAdtUpdaterAppUi
// rest of the details are commented in the header
// ---------------------------------------------------------
//
CAdtUpdaterAppUi::~CAdtUpdaterAppUi()
    {
	__PRINTS( "CAdtUpdaterAppUi::~CAdtUpdaterAppUi - Entry" );
	
	if( iContainer )
		{	
        RemoveFromStack(iContainer);
		delete iContainer;
		iContainer = NULL;
		}
	
	__PRINTS( "CAdtUpdaterAppUi::~CAdtUpdaterAppUi - Exit" );
    }

// ---------------------------------------------------------
// CAdtUpdaterAppUi::ConstructL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CAdtUpdaterAppUi::ConstructL()
	{
	__PRINTS( "CAdtUpdaterAppUi::ConstructL - Entry" );
	
	//Single click integration
    BaseConstructL( EStandardApp | EAknEnableSkin | EAknEnableMSK |
            EAknSingleClickCompatible );

    iAdtWgId = iCoeEnv->RootWin().Identifier();

	// Sets the priority for window group
	CEikonEnv &eikEnv = *CEikonEnv::Static();	
	eikEnv.RootWin().SetOrdinalPosition( KOrdinalPosition, KOrdinalPriority );
	
    // Disable priority changes of window server.
	eikEnv.WsSession().ComputeMode( RWsSession::EPriorityControlDisabled );
	
	if( !iContainer )
	    {	    	
	    // Container is not present. Construct and add to the control stack.
	    iContainer = CAdtUpdaterContainer::NewL( ApplicationRect() );
	    	
	    AddToStackL( iContainer );
		// Get the Nitz information.
    	iContainer->InformAboutNwUpdateL();		    	  
	    }         
		
	__PRINTS( "CAdtUpdaterAppUi::ConstructL - Exit" );
	}

// ---------------------------------------------------------
// CAdtUpdaterAppUi::HandleCommandL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CAdtUpdaterAppUi::HandleCommandL( TInt aCommandId )
    {
	__PRINTS( "CAdtUpdaterAppUi::HandleCommandL - Entry" );
	
    switch( aCommandId )
    	{
	    case EEikCmdExit:
	    case EAknCmdExit:
	    	{
	    	__PRINTS( "CAdtUpdaterAppUi::HandleCommandL - Exit" );
	    	
	    	Exit(); 	
	    	}
	    	break;
	    	
    	default:
    		{
    		// No implementation yet.
    		}
    		break;    	
    	}
    
	__PRINTS( "CAdtUpdaterAppUi::HandleCommandL - Exit" );
    }

// ---------------------------------------------------------
// CAdtUpdaterAppUi::HandleForegroundEventL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CAdtUpdaterAppUi::HandleForegroundEventL( TBool aForeground )
	{
	__PRINTS( "CAdtUpdaterAppUi::HandleForegroundEventL - Entry" );
	
	if( !aForeground && iContainer->QueryDialogsInDisplay() )
		{
		ToggleAppViewL( ETrue );
		}
	
	__PRINTS( "CAdtUpdaterAppUi::HandleForegroundEventL - Exit" );
	}

// ---------------------------------------------------------
// CAdtUpdaterAppUi::IsAppInBackground
// rest of the details are commented in the header
// ---------------------------------------------------------
//
TBool CAdtUpdaterAppUi::IsAppInBackground()
    {
	__PRINTS( "CAdtUpdaterAppUi::IsAppInBackground - Entry" );
    // Gets the window group id of the app coming in foreground
    TInt activeWgId = iCoeEnv->WsSession().GetFocusWindowGroup();
    __PRINTS( "CAdtUpdaterAppUi::IsAppInBackground - Exit" );
    if(activeWgId != iAdtWgId)
        {
        return ETrue;
        }
    return EFalse;
    }

// ---------------------------------------------------------
// CAdtUpdaterAppUi::ToggleAppViewL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CAdtUpdaterAppUi::ToggleAppViewL( TBool aForeground )
	{
	__PRINTS( "CCAdtUpdaterAppUi::ToggleAppViewL - Entry" );
	
	// Construct en empty TApaTask object.
	// Giving it a reference to the Window Server session.
	TApaTask task( iEikonEnv->WsSession( ) );
	               
	// Initialise the object with the window group id of
	// our application (so that it represent our app).
	task.SetWgId( CEikonEnv::Static()->RootWin().Identifier() );

	if( aForeground )
		{
		__PRINTS( "Bringing application to foreground" );
		
		// Request window server to bring our application to foreground.
		task.BringToForeground(); 		
		}
	else
		{
		__PRINTS( "Sending application to background" );
		
		// Request window server to send our application to background.
		task.SendToBackground(); 	
		}
	
	__PRINTS( "CAdtUpdaterAppUi::ToggleAppViewL - Exit" );
	}

// ---------------------------------------------------------
// CAdtUpdaterAppView::HideStatusPane
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CAdtUpdaterAppUi::HideStatusPane( TBool aHide )
	{
	__PRINTS( "CAdtUpdaterAppUi::HideStatusPane - Entry" );
	
	if( aHide )
		{
		// Hide the status pane.
		CEikStatusPane* statusPane = StatusPane();
		statusPane->MakeVisible( EFalse );
		}
	
	__PRINTS( "CAdtUpdaterAppUi::HideStatusPane - Exit" );
	}

// End of file
