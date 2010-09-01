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
* Description:  This is the source file for the CClkUiMdlListenerBase class.
*
*/

// System includes

// User includes
#include "clkuilistenerbase.h"
#include "clkuimdlbase.h"

// Constants

// ---------------------------------------------------------
// CClkUiMdlListenerBase::CClkUiMdlListenerBase
// rest of the details are commented in the header
// ---------------------------------------------------------
//
EXPORT_C CClkUiMdlListenerBase::CClkUiMdlListenerBase( CClkUiModelBase& aModel, TInt aPriority )
                                                     : CActive( aPriority ), iModel( &aModel )
    {
    // No implementation yet.
    }

// ---------------------------------------------------------
// CClkUiMdlListenerBase::~CClkUiMdlListenerBase
// rest of the details are commented in the header
// ---------------------------------------------------------
//
CClkUiMdlListenerBase::~CClkUiMdlListenerBase()
    {
    // No implementation yet.
    }

// ---------------------------------------------------------
// CClkUiMdlListenerBase::Start
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClkUiMdlListenerBase::Start()
    {
    if( !IsActive() )
        {
        // If not already active.
        DoStart();
        SetActive();
        }
    }

// ---------------------------------------------------------
// CClkUiMdlListenerBase::RunL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
EXPORT_C void CClkUiMdlListenerBase::RunL()
    {
    // First we set the AO active and then notify the view.
    TInt requestStatus( iStatus.Int() );
    // Make it active again.
    Start();
    // Notify the views.
    NotifyModelL( requestStatus );
    }

// ---------------------------------------------------------
// CClkUiMdlListenerBase::CClkUiMdlListenerBase
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClkUiMdlListenerBase::NotifyModelL( TInt aReason )
    {
    // Notify the views.
    iModel->ProcessListenerL( aReason );
    }

// End of file

