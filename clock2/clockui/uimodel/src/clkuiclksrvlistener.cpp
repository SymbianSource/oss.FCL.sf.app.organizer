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
* Description:  This is the source file for the CClkUiClkSrvListener class.
*
*/

// System includes

// User includes
#include "clkuiclksrvlistener.h"
#include "clockserverclt.h"
#include "clkuiclksrvmodel.h"
#include "clkuilistenerbase.h"

// Constants

// ---------------------------------------------------------
// CClkUiClkSrvListener::CClkUiClkSrvListener
// rest of the details are commented in the header
// ---------------------------------------------------------
//
CClkUiClkSrvListener::CClkUiClkSrvListener( RClkSrvInterface& aClkSrvSes,
                                            CClkUiClkSrvModel& aClkSrvModel,
                                            TInt& aWhatChanged,
                                            TInt& aWhoChanged,
                                            TInt aListenerPriority )
                                            : CClkUiMdlListenerBase( aClkSrvModel, aListenerPriority )
    {
    iClkSrvSes = aClkSrvSes;
    iWhatChanged = aWhatChanged;
    iWhoChanged = aWhoChanged;
    }

// ---------------------------------------------------------
// CClkUiClkSrvListener::~CClkUiClkSrvListener
// rest of the details are commented in the header
// ---------------------------------------------------------
//
CClkUiClkSrvListener::~CClkUiClkSrvListener()
    {
    Cancel();
    }

// ---------------------------------------------------------
// CClkUiClkSrvListener::DoStart
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClkUiClkSrvListener::DoStart()
    {
    iClkSrvSes.NotifyOnChange( iWhatChanged, iWhoChanged, iStatus );
    }

// ---------------------------------------------------------
// CClkUiClkSrvListener::DoStart
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClkUiClkSrvListener::DoCancel()
    {
    iClkSrvSes.NotifyOnChangeCancel();
    }
    
// End of file
