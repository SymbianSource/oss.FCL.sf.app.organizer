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
* Description:  This is the source file for the CClkUiModelBase class.
*
*/

// System includes
#include <e32def.h>

// User includes
#include "clkuimdlbase.h"
#include "clkuilistenerbase.h"
#include "clkuimdlpanic.h"
#include "clkmdlobserver.h"
// Constants

// ---------------------------------------------------------
// CClkUiModelBase::CClkUiModelBase
// rest of the details are commented in the header
// ---------------------------------------------------------
//
EXPORT_C CClkUiModelBase::CClkUiModelBase() : iNotifying( EFalse )
    {
    // No implementation yet.
    }

// ---------------------------------------------------------
// CClkUiModelBase::~CClkUiModelBase
// rest of the details are commented in the header
// ---------------------------------------------------------
//
EXPORT_C CClkUiModelBase::~CClkUiModelBase()
    {
    if( iListener )
        {
        delete iListener;
        iListener = NULL;
        }
    }

// ---------------------------------------------------------
// CClkUiModelBase::Start
// rest of the details are commented in the header
// ---------------------------------------------------------
//
EXPORT_C void CClkUiModelBase::Start()
    {
    iListener->Start();
    }

// ---------------------------------------------------------
// CClkUiModelBase::Stop
// rest of the details are commented in the header
// ---------------------------------------------------------
//
EXPORT_C void CClkUiModelBase::Stop()
    {
    if( iListener )
        {
        iListener->Cancel();
        }
    }

// ---------------------------------------------------------
// CClkUiModelBase::SetModelObserver
// rest of the details are commented in the header
// ---------------------------------------------------------
//
EXPORT_C void CClkUiModelBase::SetModelObserver( MClkModelObserver* aObserver )
    {
    // Save the observer.
    iObserver = aObserver;
    }

// ---------------------------------------------------------
// CClkUiModelBase::NotifyL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
EXPORT_C void CClkUiModelBase::NotifyL( TInt aReason )
    {
    // If already notifying, we don't notify.
    if( !iNotifying && iObserver )
        {
        DoObserverNotifyL( aReason );
        }
    }

// ---------------------------------------------------------
// CClkUiModelBase::SetListenerActive
// rest of the details are commented in the header
// ---------------------------------------------------------
//
EXPORT_C void CClkUiModelBase::SetListenerActive( CClkUiMdlListenerBase* aListener )
    {
    // Save the listener first.
    iListener = aListener;
    
    if( iListener )
        {
        CActiveScheduler::Add( iListener );
        }
    }

// ---------------------------------------------------------
// CClkUiModelBase::DoObserverNotifyL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClkUiModelBase::DoObserverNotifyL( TInt aReason )
    {
    __ASSERT_DEBUG( iObserver, Panic( EClkUiMdlObserver ) );
    
    iNotifying = ETrue;
    
    iObserver->HandleUpdateL( aReason );
    
    iNotifying = EFalse;
    }

// End of file
