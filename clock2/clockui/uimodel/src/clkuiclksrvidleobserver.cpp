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
* Description:  This is the source file for the CClkUiClkSrvIdleObserver class
*
*/

// System includes

// User includes
#include "clkuiclksrvidleobserver.h"
#include "clkuiidleobserver.h"

// Constants

// ---------------------------------------------------------
// CClkUiClkSrvIdleObserver::NewL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
EXPORT_C CClkUiClkSrvIdleObserver* CClkUiClkSrvIdleObserver::NewL( MClkModelObserver* aObserver,
                                                                 TInt aListenerPriority )
    {
    CClkUiClkSrvIdleObserver* clkSrvIdleObserver = new( ELeave ) CClkUiClkSrvIdleObserver;
    
    CleanupStack::PushL( clkSrvIdleObserver );
    clkSrvIdleObserver->ConstructL( aObserver, aListenerPriority );
    CleanupStack::Pop( clkSrvIdleObserver );
    
    return clkSrvIdleObserver;    
    }

// End of file

