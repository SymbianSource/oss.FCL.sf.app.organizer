/*
* Copyright (c) 2010 Sun Microsystems, Inc. and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributor:
* Maximilian Odendahl
*
* Contributors:
* 
* Description: calav ecom plugin proxy implementation
*
*/ 

#include <implementationproxy.h>

#include "CalDavUids.h"
#include "calencaldavplugin.h"


// ----------------------------------------------------------------------------
// ImplementationTable
// ----------------------------------------------------------------------------
const TImplementationProxy ImplementationTable[] =
    {
    IMPLEMENTATION_PROXY_ENTRY( CALEN_CALDAV_IMPLEMENTATION_UID,
                                CCalenCalDavPlugin::NewL )
    
    };


// ----------------------------------------------------------------------------
// ImplementationGroupProxy
// ----------------------------------------------------------------------------
EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
    {
    aTableCount = sizeof(ImplementationTable) / sizeof (TImplementationProxy);
    return ImplementationTable;
    }

// ----------------------------------------------------------------------------
// End of file
// ----------------------------------------------------------------------------

