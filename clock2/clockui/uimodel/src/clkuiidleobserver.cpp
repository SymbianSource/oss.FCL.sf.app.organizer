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
* Description:  This is the source file for the CClkUiIdleObserver class.
*
*/

// System includes

// User includes
#include "clkuiidleobserver.h"

// Constants

// ---------------------------------------------------------
// CClkUiIdleObserver::CClkUiIdleObserver
// rest of the details are commented in the header
// ---------------------------------------------------------
//
EXPORT_C CClkUiIdleObserver::CClkUiIdleObserver()
    {
    // No implementation yet.
    }

// ---------------------------------------------------------
// CClkUiIdleObserver::~CClkUiIdleObserver
// rest of the details are commented in the header
// ---------------------------------------------------------
//
EXPORT_C CClkUiIdleObserver::~CClkUiIdleObserver()
    {
    if( iIdle )
        {
        iIdle->Cancel();
        
        delete iIdle;
        iIdle = NULL;
        }
    }

// ---------------------------------------------------------
// CClkUiIdleObserver::ConstructL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
EXPORT_C void CClkUiIdleObserver::ConstructL( MClkModelObserver* aObserver, TInt aPriority )
    {
    // Construct the idle object
    iIdle = CIdle::NewL( aPriority );
    CancelIdleRefresh();
    
    // Set the observer.
    SetModelObserver( aObserver );
    }

// ---------------------------------------------------------
// CClkUiIdleObserver::HandleUpdateL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
EXPORT_C void CClkUiIdleObserver::HandleUpdateL( TInt aReason )
    {
    // Store the reason for notification.
    iReason = aReason;
    
    if( KErrServerTerminated == iReason )
        {
        DoIdleNotifyL();
        }
    else
        {
        StartIdleRefresh();
        }
    }

// ---------------------------------------------------------
// CClkUiIdleObserver::CancelIdleRefresh
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClkUiIdleObserver::CancelIdleRefresh()
    {
    if( iIdle )
        {
        iIdle->Cancel();
        }
    
    DoIdleCancel();
    }

// ---------------------------------------------------------
// CClkUiIdleObserver::DoIdleCancel
// rest of the details are commented in the header
// ---------------------------------------------------------
//
EXPORT_C void CClkUiIdleObserver::DoIdleCancel()
    {
    iReason = KErrCancel;
    }

// ---------------------------------------------------------
// CClkUiIdleObserver::DoIdleNotifyL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
EXPORT_C void CClkUiIdleObserver::DoIdleNotifyL()
    {
    NotifyL( iReason );
    }

// ---------------------------------------------------------
// CClkUiIdleObserver::StartIdleRefresh
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClkUiIdleObserver::StartIdleRefresh()
    {
    if( !iIdle->IsActive() )
        {
        TCallBack callBack( &CClkUiIdleObserver::IdleCallBackL, this );
        iIdle->Start( callBack );
        }
    }

// ---------------------------------------------------------
// CClkUiIdleObserver::IdleCallBackL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
TInt CClkUiIdleObserver::IdleCallBackL( TAny* aPtr )
    {
    CClkUiIdleObserver* selfObj = static_cast< CClkUiIdleObserver* > ( aPtr );
    
    // Notify the observer.
    selfObj->DoIdleNotifyL();
    
    return 0;
    }

// End of file
