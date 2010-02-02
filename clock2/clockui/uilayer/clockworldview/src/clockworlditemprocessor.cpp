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
* Description:   This is the source file for the CClockWorldItemProcessor class.
*
*/

// System includes
#include <e32base.h>

// User includes
#include "clockworlditemprocessor.h"
#include "clockworldcontainer.h"
#include "clock_debug.h"

// Literals

// Constants 
const TInt KEvenNumber( 2 );

// ---------------------------------------------------------
// CClockWorldItemProcessor::NewL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
CClockWorldItemProcessor* CClockWorldItemProcessor::NewL( CClockWorldContainer* aContainer )
    {
    __PRINTS( "CClockWorldItemProcessor::NewL - Entry" );
    
    CClockWorldItemProcessor* self = new( ELeave ) CClockWorldItemProcessor();
    
    CleanupStack::PushL( self );
    self->ConstructL( aContainer );
    
    CleanupStack::Pop( self );
    
    __PRINTS( "CClockWorldItemProcessor::NewL - Exit" );
    
    return self;
    	
    }

// ---------------------------------------------------------
// CClockWorldItemProcessor::~CClockWorldItemProcessor
// rest of the details are commented in the header
// ---------------------------------------------------------
//
CClockWorldItemProcessor::~CClockWorldItemProcessor()
    {
    __PRINTS( "CClockWorldItemProcessor::~CClockWorldItemProcessor - Entry" );
    
    Cancel();
    
    __PRINTS( "CClockWorldItemProcessor::~CClockWorldItemProcessor - Exit" );
    }

// ---------------------------------------------------------
// CClockWorldItemProcessor::ProcessEachItem
// rest of the details are commented in the header
// ---------------------------------------------------------
//
TInt CClockWorldItemProcessor::ProcessEachItem( TInt aIndex )
    {
    __PRINTS( "CClockWorldItemProcessor::ProcessEachItem - Entry" );
    
    TInt retValue( KErrNotFound );
    
    if( iContainer )
        {
        retValue = iContainer->RedrawItem( aIndex );
        }
    
    __PRINTS( "CClockWorldItemProcessor::ProcessEachItem - Exit" );
    
    return retValue;
    }

// ---------------------------------------------------------
// CClockWorldItemProcessor::RunL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClockWorldItemProcessor::RunL()
    {
    __PRINTS( "CClockWorldItemProcessor::RunL - Entry" );

    // Call the redraw for even number lisitems first and then for the odd number lisitems
    // This gives a faster loading effect on the hardware 	
  
    TInt returnValue( KErrNone );
 
    returnValue = ProcessEachItem( iDrawnListItem );
  
    if( returnValue )
        {
        iDrawnListItem = iDrawnListItem + KEvenNumber;	
        }  
    else if( KErrNone == iDrawnListItem % KEvenNumber )
        {             
        iDrawnListItem = KEvenNumber - 1;	             
        }
    else
        {
        Cancel();
        
        __PRINTS( "CClockWorldItemProcessor::RunL - Exit" );
        
        return;	
        }
    
    DoAsyncCallback();	

    __PRINTS( "CClockWorldItemProcessor::RunL - Exit" );
    }
    
// ---------------------------------------------------------
// CClockWorldItemProcessor::DoCancel
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClockWorldItemProcessor::DoCancel()
    {
    __PRINTS( "CClockWorldItemProcessor::DoCancel - Entry" );
    
	// No Implementation Yet.
    
    __PRINTS( "CClockWorldItemProcessor::DoCancel - Exit" );
    }


// ---------------------------------------------------------
// CClockWorldItemProcessor::CClockWorldItemProcessor
// rest of the details are commented in the header
// ---------------------------------------------------------
//
CClockWorldItemProcessor::CClockWorldItemProcessor() : CActive( CActive::EPriorityStandard )
    {
    __PRINTS( "CClockWorldItemProcessor::CClockWorldItemProcessor - Entry" );

    CActiveScheduler::Add( this );
    DoAsyncCallback();
	
	__PRINTS( "CClockWorldItemProcessor::CClockWorldItemProcessor - Exit" );
    }
    
// ---------------------------------------------------------
// CClockWorldItemProcessor::ConstructL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClockWorldItemProcessor::ConstructL( CClockWorldContainer* aContainer )
    {
	__PRINTS( "CClockWorldItemProcessor::ConstructL - Entry" );

	iContainer = aContainer;
    iDrawnListItem = 0;	

    __PRINTS( "CClockWorldItemProcessor::ConstructL - Exit" );
    }

// ---------------------------------------------------------
// CClockWorldItemProcessor::DoAsyncCallback
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClockWorldItemProcessor::DoAsyncCallback()
    {
    __PRINTS( "CClockWorldItemProcessor::DoAsyncCallback - Entry" );
    
    if( !IsActive() )
        {
        if( IsAdded() )
            {
            // Make sure this is the last Active Object in the list
            Deque();
            CActiveScheduler::Add( this );
            }
        TRequestStatus* status = &iStatus;
        User::RequestComplete( status, KErrNone );
        SetActive();       
        }
    
    __PRINTS( "CClockWorldItemProcessor::DoAsyncCallback - Exit" );
    }

// End of file
