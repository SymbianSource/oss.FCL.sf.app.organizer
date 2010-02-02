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
* Description:   ECom search interface definition
*
*/


// INCLUDE FILES
#include "searchservershutdown.h"

// -------------------------------------------------------------------------------------------------
// CSearchseverShutdown::NewL
// Symbian OS 2 phased constructor.
// -------------------------------------------------------------------------------------------------
//
CSearchseverShutdown* CSearchseverShutdown :: NewL()
    {
    CSearchseverShutdown* self = new (ELeave) CSearchseverShutdown();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// -------------------------------------------------------------------------------------------------
// CSearchClientSessionImp::~CSearchseverShutdown
// Destructor.
// -------------------------------------------------------------------------------------------------
//
CSearchseverShutdown :: ~CSearchseverShutdown()
    {
    Cancel();
    }

// -------------------------------------------------------------------------------------------------
// CSearchseverShutdown::start
// Starts the timer for Server shutdown delay.
// -------------------------------------------------------------------------------------------------
//
void CSearchseverShutdown :: Start()
    {
    After(KshutdownTime);
    }

// -------------------------------------------------------------------------------------------------
// CSearchseverShutdown::CSearchseverShutdown
// Performs the first phase of two phase construction.
// -------------------------------------------------------------------------------------------------
//
CSearchseverShutdown :: CSearchseverShutdown()
:CTimer(-1)
    {
    CActiveScheduler::Add( this );
    }

// -------------------------------------------------------------------------------------------------
// CSearchseverShutdown::ConstructL
// Performs the second phase construction.
// -------------------------------------------------------------------------------------------------
//
void CSearchseverShutdown :: ConstructL()
    {
    CTimer :: ConstructL();
    }


// -------------------------------------------------------------------------------------------------
// CSearchseverShutdown::RunL
// Handles an active object’s request completion event.
// -------------------------------------------------------------------------------------------------
//
void CSearchseverShutdown :: RunL()
    {
    CActiveScheduler:: Stop();
    }


//End of file