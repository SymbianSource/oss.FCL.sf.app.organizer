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
* Description:   This is the source file for the CAdtUpdaterQueryDialog class.
*
*/

// System includes

// User includes
#include "adtupdaterquerydialog.h"
#include "clock_debug.h"

// Constants

// Literals

// -----------------------------------------------------
// CAdtUpdaterQueryDialog::CAdtUpdaterQueryDialog
// rest of the details are commented in the header 
// -----------------------------------------------------
//
CAdtUpdaterQueryDialog::CAdtUpdaterQueryDialog( TTime& aTime ) : CAknTimeQueryDialog( aTime )
	{
	__PRINTS( "CAdtUpdaterPopupList::CAdtUpdaterPopupList - Entry" );
	
	// No implementation yet.
	
	__PRINTS( "CAdtUpdaterPopupList::CAdtUpdaterPopupList - Entry" );
	}

// -----------------------------------------------------
// CAdtUpdaterQueryDialog::~CAdtUpdaterQueryDialog
// rest of the details are commented in the header 
// -----------------------------------------------------
// 
CAdtUpdaterQueryDialog::~CAdtUpdaterQueryDialog()
	{
	__PRINTS( "CAdtUpdaterQueryDialog::~CAdtUpdaterQueryDialog - Entry" );
	
	// No implementation yet.
	
	__PRINTS( "CAdtUpdaterQueryDialog::~CAdtUpdaterQueryDialog - Exit" );
	}

// -----------------------------------------------------
// CAdtUpdaterQueryDialog::OfferKeyEventL
// rest of the details are commented in the header 
// -----------------------------------------------------
// 
TKeyResponse CAdtUpdaterQueryDialog::OfferKeyEventL( const TKeyEvent& aKeyEvent,
													 TEventCode aType )
	{
	__PRINTS( "CAdtUpdaterQueryDialog::OfferKeyEventL - Entry" );
	
    if( aType != EEventKey )
    	{
    	 __PRINTS( "CAdtUpdaterQueryDialog::OfferKeyEventL - Exit : Key is not consumed for EEvenKey." );
    	 
    	return EKeyWasNotConsumed;    	
    	}

    if( NeedToDismissQueryL( aKeyEvent ) )
    	{
    	 __PRINTS( "CAdtUpdaterQueryDialog::OfferKeyEventL - Exit : Key is not consumed for aKeyEvent." );
    	 
    	return EKeyWasConsumed;
    	}
    
    __PRINTS( "CAdtUpdaterQueryDialog::OfferKeyEventL - Exit" );
        
    return CAknDialog::OfferKeyEventL( aKeyEvent, aType );	
	}

// -----------------------------------------------------
// CAdtUpdaterQueryDialog::NeedToDismissQueryL
// rest of the details are commented in the header 
// -----------------------------------------------------
//
TBool CAdtUpdaterQueryDialog::NeedToDismissQueryL( const TKeyEvent& aKeyEvent )
	{
	__PRINTS( "CAdtUpdaterQueryDialog::NeedToDismissQueryL - Entry" );
	
    if( EKeyPhoneSend == aKeyEvent.iCode )
        {
        DismissQueryL();
        
        __PRINTS( "CAdtUpdaterQueryDialog::NeedToDismissQueryL - Exit : Dismissing query." );
        
        return ETrue;
        }
    
    __PRINTS( "CAdtUpdaterQueryDialog::NeedToDismissQueryL - Exit" );
    
    return EFalse;
    }
		
// -----------------------------------------------------
// CAdtUpdaterQueryDialog::DismissQueryL
// rest of the details are commented in the header 
// -----------------------------------------------------
//
void CAdtUpdaterQueryDialog::DismissQueryL()
	{
	__PRINTS( "CAdtUpdaterQueryDialog::DismissQueryL - Entry" );
	
    if( IsLeftSoftkeyVisible() )
        {
        __PRINTS( "CAdtUpdaterQueryDialog::DismissQueryL - Exit" );
        
        TryExitL( EEikBidOk );
        }
    
    __PRINTS( "CAdtUpdaterQueryDialog::DismissQueryL - Exit" );
	}

// -----------------------------------------------------
// CAdtUpdaterQueryDialog::IsLeftSoftkeyVisible
// rest of the details are commented in the header 
// -----------------------------------------------------
//
TBool CAdtUpdaterQueryDialog::IsLeftSoftkeyVisible()
	{
	__PRINTS( "CAdtUpdaterQueryDialog::IsLeftSoftkeyVisible - Entry" );
	
	__PRINTS( "CAdtUpdaterQueryDialog::IsLeftSoftkeyVisible - Exit" );
	
    return ButtonGroupContainer().ButtonGroup()->IsCommandVisible( ButtonGroupContainer().ButtonGroup()->CommandId( 0 ) );	
	}

// End of file
