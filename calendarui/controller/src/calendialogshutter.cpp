/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  For dialog shutter.
*
*/


#include <eikenv.h>
#include <eikappui.h>

#include "calendialogshutter.h"

const TInt KMaxNumEscKeysToSend = 50;

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CCalenDialogShutter::CCalenDialogShutter
// C++ default constructor
// (other items were commented in a header).
// ---------------------------------------------------------
//
CCalenDialogShutter::CCalenDialogShutter( CEikonEnv* aEikEnv ) 
                     : CActive(EPriorityStandard) // Standard priority
                     , iEikon( aEikEnv )
                     , iStartLevel( StartLevel() )
    {
    }

// ----------------------------------------------------------------------------
// CCalenDialogShutter::NewLC
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
CCalenDialogShutter* CCalenDialogShutter::NewLC( CEikonEnv* aEikEnv )
    {
    CCalenDialogShutter* self = new (ELeave) CCalenDialogShutter( aEikEnv );
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

// ----------------------------------------------------------------------------
// CCalenDialogShutter::NewL
// 1st phase of construction
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
CCalenDialogShutter* CCalenDialogShutter::NewL( CEikonEnv* aEikEnv )
    {
    CCalenDialogShutter* self = CCalenDialogShutter::NewLC( aEikEnv );
    CleanupStack::Pop(); // self;
    return self;
    }

// ----------------------------------------------------------------------------
// CCalenDialogShutter::ConstructL
// Second phase of construction
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenDialogShutter::ConstructL()
    {
    CActiveScheduler::Add(this); // Add to scheduler
    }

// ----------------------------------------------------------------------------
// CCalenDialogShutter::~CCalenDialogShutter
// Destructor
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
CCalenDialogShutter::~CCalenDialogShutter()
    {
    Cancel(); // Cancel any request, if outstanding
    }

// ----------------------------------------------------------------------------
// CCalenDialogShutter::DoCancel
// Cancel method to handle the user selection
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenDialogShutter::DoCancel()
    {
    iCount = 0;
    }

// ----------------------------------------------------------------------------
// CCalenDialogShutter::Queue
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenDialogShutter::Queue()
    {
    if (!IsActive())
        {
        SetActive();
        TRequestStatus* status = &iStatus;
        User::RequestComplete( status, KErrNone );
        }
    }

// ---------------------------------------------------------------------------
// CCalenDialogShutter::RunL
// Handles the asyn requests completion 
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//  
void CCalenDialogShutter::RunL()
    {
    if ( iCount++ >=  KMaxNumEscKeysToSend )
        {        
        return;
        }
    
    CEikAppUi* appUi=iEikon->EikAppUi();
    TInt startLevel=StartLevel();
    if ( startLevel > iStartLevel )
        {        
        return;
        }
    
    iStartLevel = startLevel;
    
    if (appUi->IsDisplayingMenuOrDialog())
        {
        TKeyEvent key;
        key.iCode=EKeyEscape;
        key.iModifiers=0;
        key.iRepeats = 0;
        key.iScanCode = EStdKeyNull;
        iEikon->SimulateKeyEventL(key, EEventKey);
        }
    else
        {        
        return;
        }
    Queue();
    }

// ----------------------------------------------------------------------------
// CCalenDialogShutter::RunError
// Function to handle any errors in async request
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
TInt CCalenDialogShutter::RunError(TInt aError)
    {
    return aError;
    }

// ----------------------------------------------------------------------------
// CCalenDialogShutter::StartLevel
// Function to get the startlevel of the opened dialogs
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
TInt CCalenDialogShutter::StartLevel()
    {
    return ((CKludgeScheduler*)CActiveScheduler::Current())->PublicLevel();
    }

// ----------------------------------------------------------------------------
// CCalenDialogShutter::ShutDialogsL
//  Function for making the initial request 
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenDialogShutter::ShutDialogsL( /*CActiveSchedulerWait* aWait*/ )
    {    
    Queue();
    }
