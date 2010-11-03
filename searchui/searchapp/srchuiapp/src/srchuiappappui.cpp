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
* Description:     Responsible for starting the search application. It is used to create UI for the search application. 
* It contains the main application logic and implementation of user commands. 
*
*/






//System Includes
#include <s32mem.h>
#include <avkon.hrh>
#include <aknnotewrappers.h>

// User Includes 
#include "srchuiappappui.h"
#include "srchuiapp.hrh"
#include "srchuigeneral.h"

//-----------------------------------------------------------
// Constructor
//-----------------------------------------------------------
CSrchUiAppAppUi::CSrchUiAppAppUi()
    {
	}


//-----------------------------------------------------------
// ConstructL is called by the application framework
//-----------------------------------------------------------
void CSrchUiAppAppUi::ConstructL()
    {
    BaseConstructL( EAknEnableSkin | EAknEnableMSK );
    if( !IsEmbedded() )
        {
        iGenSrchUi = CSrchUiGeneral::NewL(this);
        }
    }
    

//-----------------------------------------------------------
// Destructor
//-----------------------------------------------------------
CSrchUiAppAppUi::~CSrchUiAppAppUi()
    {
    
    
    }

//-----------------------------------------------------------
// handle any menu commands
//-----------------------------------------------------------
void CSrchUiAppAppUi::HandleCommandL(TInt aCommand)
    {
    switch(aCommand)
        {
        case EEikCmdExit:
        	if( iGenSrchUi )
	        	{
	        	delete iGenSrchUi;
	        	iGenSrchUi = NULL;
	          }
            
             break; 
        case EAknSoftkeyExit:
        case EAknSoftkeyBack:
        if( iGenSrchUi )
	        {
	        delete iGenSrchUi;
	        iGenSrchUi = NULL;
	        }
          Exit();
          break;

        default:
            break;
        }
         Exit();
    }

//-----------------------------------------------------------
// tests whether searchappui is embedded or stand alone
//-----------------------------------------------------------
TBool CSrchUiAppAppUi::IsEmbedded()
    {
    TBool embedded = iEikonEnv->StartedAsServerApp();
    
    return embedded;
    }

//-----------------------------------------------------------    
// creates general search ui
//-----------------------------------------------------------
void CSrchUiAppAppUi::CreateGenSrchUiL(TPtr8 desc )
    {
    RDesReadStream readStream;
	CleanupClosePushL( readStream );
	readStream.Open( desc );
    iGenSrchUi = CSrchUiGeneral::NewL(this, readStream);
    CleanupStack::PopAndDestroy( &readStream );
    }