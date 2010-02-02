/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implementation of Notepad Application UI class.
*
*/



// INCLUDE FILES
#include <avkon.hrh>
#include <NpdLib.rsg>
#include <NpdApi.h>
#include "NpdListDialog.h"
#include "NpdAppUi.h"

//UID of PinbMdl
const TInt KUidApaMessagePinboardValue(0x1000590C);

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CNotepadAppUi::ConstructL
// Initialize iApi and iDialog.
// non-waiting CNotepadListDialog starts.
// -----------------------------------------------------------------------------
//
void CNotepadAppUi::ConstructL()
    {

    BaseConstructL(EAknEnableSkin | EAknEnableMSK | EAknSingleClickCompatible);

    iApi = CNotepadApi::NewL();

    iDialog = CNotepadListDialog::NewL(R_NOTEPAD_LIST_DIALOG,
        REINTERPRET_CAST( CEikDialog**, &iDialog ) );
    iDialog->SetMopParent(this);
    iDialog->ExecuteLD(); // non waiting    
    }

// -----------------------------------------------------------------------------
// CNotepadAppUi::~CNotepadAppUi
// -----------------------------------------------------------------------------
//
CNotepadAppUi::~CNotepadAppUi()
    {
    delete iDialog;
    delete iApi;
    }

// -----------------------------------------------------------------------------
// CNotepadAppUi::HandleCommandL
// -----------------------------------------------------------------------------
//
void CNotepadAppUi::HandleCommandL(TInt aCommand)
    {
    if ( aCommand == EEikCmdExit )
        {
        TRAPD( err, iDialog->SaveL(); );
        if ( err )
            {
            iEikonEnv->HandleError(err);
            }
        Exit();
        }
    else
        {
        CAknAppUi::HandleCommandL( aCommand );
        }
    }

// -----------------------------------------------------------------------------
// CNotepadAppUi::ProcessCommandParametersL
// From CEikAppUi
// -----------------------------------------------------------------------------
//
TBool CNotepadAppUi::ProcessCommandParametersL(
    TApaCommand aCommand,
    TFileName& aDocumentName,
    const TDesC8& aTail )
    {
    if ( aTail.Length() )
        {
        TLex8 lex(aTail);
        TInt key;
        if ( iDialog && lex.Val(key) == KErrNone)
            {
            // force change to edit mode
            iDialog->EditByKeyL(key);
            }
        }
    return CEikAppUi::ProcessCommandParametersL(aCommand, aDocumentName, 
        aTail);
    }

// -----------------------------------------------------------------------------
// CNotepadAppUi::HandleMessageL
// from MCoeMessageObserver
// -----------------------------------------------------------------------------
//
MCoeMessageObserver::TMessageResponse CNotepadAppUi::HandleMessageL(
    TUint32 aClientHandleOfTargetWindowGroup, 
    TUid aMessageUid, 
    const TDesC8& aMessageParameters )
    {
    MCoeMessageObserver::TMessageResponse messageResponse(EMessageHandled);
    if ( aMessageUid.iUid  == KUidApaMessagePinboardValue )
        {
        TLex8 lex(aMessageParameters);
        TInt key;
        if (lex.Val(key) == KErrNone)
            {
            iDialog->EditByKeyL(key);
            }
        }
    else
        {
        messageResponse = CAknAppUi::HandleMessageL(
            aClientHandleOfTargetWindowGroup, 
            aMessageUid,
            aMessageParameters);
        }
    return messageResponse;
    }

// -----------------------------------------------------------------------------
// CNotepadAppUi::PrepareToExit()
// from CCoeAppUiBase
// -----------------------------------------------------------------------------
//
void CNotepadAppUi::PrepareToExit()
    {
    delete iDialog;
    iDialog = NULL;
    CAknAppUi::PrepareToExit();
    }

// End of File  
