/*
* Copyright (c) 2006-2006 Nokia Corporation and/or its subsidiary(-ies).
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






#include <avkon.hrh>
#include <aknnotewrappers.h> 

#include "srchuiappappui.h"

#include "srchuiapp.hrh"
#include "srchuigeneral.h"

// ConstructL is called by the application framework
void CSrchUiAppAppUi::ConstructL()
    {
    BaseConstructL( EAknEnableSkin );
    
    //Construct the Manager
    iGenSrchUi = CSrchUiGeneral::NewL(this);
    }



CSrchUiAppAppUi::~CSrchUiAppAppUi()
    {
    
    delete iGenSrchUi;
    }

// handle any menu commands
void CSrchUiAppAppUi::HandleCommandL(TInt aCommand)
    {
    switch(aCommand)
        {
        case EEikCmdExit:
             Exit();
             break; 
        case EAknSoftkeyExit:
             Exit();
             break;

        case ESearchApplicationCommand1:
            {
            _LIT(message,"Hello!");
            CAknInformationNote* informationNote = new (ELeave) CAknInformationNote;
            informationNote->ExecuteLD(message);
            }
            break;

        default:
            Panic(ESearchApplicationBasicUi);
            break;
        }
    }



