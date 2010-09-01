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
* Description:  Implementation of Notepad Viewer Application UI class.
*
*/


// INCLUDE FILES
#include <coeutils.h>
#include <apparc.h>
#include <avkon.hrh>
#include <NpdApi.h>
#include "NpdViewerAppUi.h"
#include "NpdViewerDialog.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CNotepadViewerAppUi::ConstructL
// -----------------------------------------------------------------------------
//
void CNotepadViewerAppUi::ConstructL()
    {
    iCurrentFileOpened = EFalse;
    BaseConstructL(EAknEnableSkin | EAknEnableMSK | EAknSingleClickCompatible);
    iApi = CNotepadApi::NewL(); // AddResouceFile NpdLib.rsc
    iApi->CreateViewerDialogL(EFalse); // create non-wait viewerdialog
    iApi->Dialog()->SetMopParent(this);
    STATIC_CAST(CNotepadViewerDialog*, iApi->Dialog())->ExecuteLD(); //non-wait
    }

// -----------------------------------------------------------------------------
// CNotepadViewerAppUi::~CNotepadViewerAppUi
// Destructor.
// -----------------------------------------------------------------------------
//
CNotepadViewerAppUi::~CNotepadViewerAppUi()
    {
    delete iApi; // Release Resource, delete dialog
    if ( iDoorObserver )
        {
        iDoorObserver->NotifyExit(MApaEmbeddedDocObserver::EEmpty);
        }
    }

// -----------------------------------------------------------------------------
// CNotepadViewerAppUi::HandleCommandL
// -----------------------------------------------------------------------------
//
void CNotepadViewerAppUi::HandleCommandL(TInt aCommand)
    {
    switch ( aCommand )
        {
        case EEikCmdExit:
            {
            Exit();
            }
            break;
        default:
            break;      
        }
    }

// -----------------------------------------------------------------------------
// CNotepadViewerAppUi::ProcessCommandParametersL
// From CEikAppUi
// -----------------------------------------------------------------------------
//
TBool CNotepadViewerAppUi::ProcessCommandParametersL(
    TApaCommand /*aCommand*/,
    TFileName& aDocumentName,
    const TDesC8& /*aTail*/ )
    {
    // never call CEikAppUi::ProcessCommandParametersL(..) because 
    // it is so harmfull to overwrite and destroy aDocumentName.
    //
    return ConeUtils::FileExists(aDocumentName);
    }

// -----------------------------------------------------------------------------
// CNotepadViewerAppUi::OpenFileL
// -----------------------------------------------------------------------------
//
void CNotepadViewerAppUi::OpenFileL(const TDesC& aFilename)
    {
      if( !iCurrentFileOpened )
        {
        iCurrentFileOpened = ETrue;
    STATIC_CAST(CNotepadViewerDialog*, iApi->Dialog())->OpenFileL(
        aFilename, NULL, ETrue ); // auto-detect encoding
        }
    }

// -----------------------------------------------------------------------------
// CNotepadViewerAppUi::OpenFileL(RFile& aFile)
// -----------------------------------------------------------------------------
//
void CNotepadViewerAppUi::OpenFileL(RFile& aFile)
    {
    if( !iCurrentFileOpened )
        {
        iCurrentFileOpened = ETrue;
    STATIC_CAST(CNotepadViewerDialog*, iApi->Dialog())->OpenFileL(
        aFile, NULL, ETrue ); // auto-detect encoding
        }
    }

// End of File  
