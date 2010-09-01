/*
 * Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
 * Description : Multiple calendar's list dialog listing all the available calendars.
 * */

// System includes
#include <implementationproxy.h>

// User includes
#include "calenaiwprovider.h"
#include "calenaiwproviderdefines.hrh"

// ---------------------------------------------------------
// Define the interface UIDs
// Map the interface implementation UIDs to implementation factory functions
// ---------------------------------------------------------
//
const TImplementationProxy ImplementationTable[] =
    {
    IMPLEMENTATION_PROXY_ENTRY( KCalenAiwProviderDllUid,CCalenAiwProvider::NewL )
    };

// ---------------------------------------------------------
// ImplementationGroupProxy
// Exported proxy for instantiation method resolution.
// ---------------------------------------------------------
//
EXPORT_C const TImplementationProxy* ImplementationGroupProxy( TInt& aCount )
    {
    aCount = sizeof( ImplementationTable ) / sizeof( TImplementationProxy );
    return ImplementationTable;
    }

// End of file.
