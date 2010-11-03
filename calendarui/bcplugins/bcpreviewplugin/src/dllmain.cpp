/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: Standard Symbian OS DLL entry point function.
*
*/
// Internal includes
#include "bcpreviewplugin.h"
#include "bcpreviewpluginuids.h"
#include "bcpreviewdefines.h"
// System includes
#include <ecom/implementationproxy.h>

/// Implementation table
const TImplementationProxy ImplementationTable[] =
    {
    IMPLEMENTATION_PROXY_ENTRY( KBCPREVIEWPLUGININTERFACEIMPLUID,
            CBCPreViewPlugin::NewL ),
    };

// --------------------------------------------------------------------------
// ImplementationGroupProxy
// The one and only exported function that is the ECom entry point
// --------------------------------------------------------------------------
//
EXPORT_C const TImplementationProxy* ImplementationGroupProxy
        (TInt& aTableCount)
    {
    TRACE_ENTRY_POINT;
    aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);

    return ImplementationTable;
    TRACE_EXIT_POINT;
    }

// End of File
