/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:     This class is the InDevice Search base and the menu service provider.
*
*/






// System Includes
#include <srchaiwprovidermenu.rsg>
#include <AiwCommon.h>
#include <AiwMenu.h>

// User Includes
#include "srchaiwprovider.h"
#include "srchaiwproviderpanics.h"
#include "srchaiwproviderdefines.h"

//constants
_LIT( KIDSAiwProviderResFile, "\\resource\\plugins\\srchaiwprovidermenu.rsc");

// ============================ MEMBER FUNCTIONS ===============================

// -------------------------------------------------------------------------------------------------
// Symbian OS 2 phased constructor.
// -------------------------------------------------------------------------------------------------
//
CSrchAiwProvider* CSrchAiwProvider::NewL()
    {
	CSrchAiwProvider* self = CSrchAiwProvider::NewLC();
	CleanupStack::Pop( self );
	return self;
    }
    
// -------------------------------------------------------------------------------------------------
// Symbian OS 2 phased constructor.
// -------------------------------------------------------------------------------------------------
//  
CSrchAiwProvider* CSrchAiwProvider::NewLC()
    {
	CSrchAiwProvider* self = new( ELeave ) CSrchAiwProvider();
	CleanupStack::PushL( self );
	self->ConstructL();
	return self;
    }
    
// -------------------------------------------------------------------------------------------------
// Destructor
// -------------------------------------------------------------------------------------------------
//     
CSrchAiwProvider::~CSrchAiwProvider()
    {
    delete iSearchLauncher;
    }
  
// -------------------------------------------------------------------------------------------------
// Called by the Handler framework to initialise provider with
// necessary information from Handler.
// -------------------------------------------------------------------------------------------------
//           
void CSrchAiwProvider::InitialiseL(MAiwNotifyCallback& /*aFrameworkCallback*/,
    const RCriteriaArray& /*aInterest*/)
    {
    
    }

// -------------------------------------------------------------------------------------------------
// Executes generic service commands included in criteria.
// -------------------------------------------------------------------------------------------------
//   
void CSrchAiwProvider::HandleServiceCmdL( const TInt& aCmdId, 
                                        const CAiwGenericParamList& aInParamList,
                                        CAiwGenericParamList& /*aOutParamList*/,
                                        TUint /*aCmdOptions*/,
                                        const MAiwNotifyCallback* /*aCallback*/ )
    {
	switch( aCmdId )
	    {
	    case KAiwCmdContentSearch:
	        {
	    	iSearchLauncher->SearchL(1, aInParamList);
	        }
	    break;
	    
	    default:
	        break;	    
	    }
    }
    
// -------------------------------------------------------------------------------------------------
// Initialises menu pane by adding provider specific menu items.
// -------------------------------------------------------------------------------------------------
//
void CSrchAiwProvider::InitializeMenuPaneL( CAiwMenuPane& aMenuPane,
                                          TInt aIndex,
                                          TInt /*aCascadeId*/,
                                          const CAiwGenericParamList& /*aInParamList*/ ) 
    {
	TFileName resFile( KIDSAiwProviderResFile );
    TFileName dllName;
    Dll::FileName( dllName );
    TBuf<2> drive = dllName.Left(2);
    resFile.Insert(0, drive);
    aMenuPane.AddMenuItemsL(resFile, R_AIWSEARCHPROVIDER_MENU, KAiwCmdContentSearch, aIndex);
    }

// -------------------------------------------------------------------------------------------------
// Handle a menu command invoked by the Handler.
// -------------------------------------------------------------------------------------------------
//                                          
void CSrchAiwProvider::HandleMenuCmdL( TInt aMenuCmdId,
                                     const CAiwGenericParamList& aInParamList,
                                     CAiwGenericParamList& aOutParamList,
                                     TUint aCmdOptions,
                                     const MAiwNotifyCallback* aCallback )
    {
    // Route to HandleServiceCmdL
    HandleServiceCmdL( aMenuCmdId, aInParamList, aOutParamList, aCmdOptions, aCallback );
    }
    
// -------------------------------------------------------------------------------------------------
// Performs the first phase construction.
// -------------------------------------------------------------------------------------------------
//     
CSrchAiwProvider::CSrchAiwProvider()
    {
	
    }


// -------------------------------------------------------------------------------------------------
// Performs the second phase construction.
// -------------------------------------------------------------------------------------------------
//     
void CSrchAiwProvider::ConstructL()
    {
    iSearchLauncher = CSearchServiceLauncher::NewL();
    }

// End of file
