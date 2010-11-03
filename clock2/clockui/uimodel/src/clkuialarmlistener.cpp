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
* Description:  This is the source file for the CClkUiMdlAlarmListener class.
*
*/

// System includes

// User includes
#include "clkuialarmlistener.h"
#include "clkuilistenerbase.h"
#include "clkuialarmmodel.h"

// Constants

// ---------------------------------------------------------
// CClkUiMdlAlarmListener::CClkUiMdlAlarmListener
// rest of the details are commented in the header
// ---------------------------------------------------------
//
CClkUiMdlAlarmListener::CClkUiMdlAlarmListener( RASCliSession& aAlarmSrvSes,
                                                CClkUiAlarmModel& aAlarmModel,
                                                TInt aListenerPriority )
                                                : CClkUiMdlListenerBase( aAlarmModel, aListenerPriority ),
                                                  iAlarmSrvSes( aAlarmSrvSes )
    {
    // No implementation yet.
    }

// ---------------------------------------------------------
// CClkUiMdlAlarmListener::~CClkUiMdlAlarmListener
// rest of the details are commented in the header
// ---------------------------------------------------------
//
CClkUiMdlAlarmListener::~CClkUiMdlAlarmListener()
    {
    Cancel();
    }

// ---------------------------------------------------------
// CClkUiMdlAlarmListener::DoStart
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClkUiMdlAlarmListener::DoStart()
    {
    iAlarmSrvSes.NotifyChange( iStatus, iAlarmId );
    }

// ---------------------------------------------------------
// CClkUiMdlAlarmListener::DoCancel
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClkUiMdlAlarmListener::DoCancel()
    {
    iAlarmSrvSes.NotifyChangeCancel();
    }

// End of file
