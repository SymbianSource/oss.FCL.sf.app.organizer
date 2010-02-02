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
* Description:  This is the source file for the CClkUiMdlIntermediateObserver class.
*
*/

// System includes
#include <e32def.h>

// User includes
#include "clkuiintermediateobserver.h"
#include "clkuimdlpanic.h"

// Constants

// ---------------------------------------------------------
// CClkUiMdlIntermediateObserver::CClkUiMdlIntermediateObserver
// rest of the details are commented in the header
// ---------------------------------------------------------
//
CClkUiMdlIntermediateObserver::CClkUiMdlIntermediateObserver()
    {
    // No implementation yet.
    }

// ---------------------------------------------------------
// CClkUiMdlIntermediateObserver::~CClkUiMdlIntermediateObserver
// rest of the details are commented in the header
// ---------------------------------------------------------
//
CClkUiMdlIntermediateObserver::~CClkUiMdlIntermediateObserver()
    {
    // No implementation yet.
    }

// ---------------------------------------------------------
// CClkUiMdlIntermediateObserver::HandleUpdateL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
EXPORT_C void CClkUiMdlIntermediateObserver::HandleUpdateL( TInt aReason )
    {
    // We must have an observer, for the notification to be given.
    __ASSERT_ALWAYS( iObserver, Panic( EClkUiMdlObserver ) );
    NotifyL( aReason );
    }

// ---------------------------------------------------------
// CClkUiMdlIntermediateObserver::SetModelObserver
// rest of the details are commented in the header
// ---------------------------------------------------------
//
EXPORT_C void CClkUiMdlIntermediateObserver::SetModelObserver( MClkModelObserver* aObserver )
    {
    // The observer must not be null.
    __ASSERT_ALWAYS( aObserver, Panic( EClkUiMdlObserver ) );
    iObserver = aObserver;
    }

// ---------------------------------------------------------
// CClkUiMdlIntermediateObserver::NotifyL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
EXPORT_C void CClkUiMdlIntermediateObserver::NotifyL( TInt aReason )
    {
    // We must have an observer, for the notification to be given.
    __ASSERT_ALWAYS( iObserver, Panic( EClkUiMdlObserver ) );
    iObserver->HandleUpdateL( aReason );
    }

// End of file
