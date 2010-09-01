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
* Description:  This is the source file for the CClkUiAlarmIdleObserver class.
*
*/

// System includes

// User includes
#include "clkuialarmidleobserver.h"
#include "clkuiidleobserver.h"

// Constants

// ---------------------------------------------------------
// CClkUiIdleObserver::CClkUiIdleObserver
// rest of the details are commented in the header
// ---------------------------------------------------------
//
EXPORT_C CClkUiAlarmIdleObserver* CClkUiAlarmIdleObserver::NewL( MClkModelObserver* aObserver,
                                                                 TInt aListenerPriority )
    {
    CClkUiAlarmIdleObserver* alarmIdleObserver = new( ELeave ) CClkUiAlarmIdleObserver;
    
    CleanupStack::PushL( alarmIdleObserver );
    alarmIdleObserver->ConstructL( aObserver, aListenerPriority );
    CleanupStack::Pop( alarmIdleObserver );
    
    return alarmIdleObserver;    
    }

// End of file

