/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  ?Description
*
*/

// user includes
#include "CalenDefaultEditors.h"
#include "CalendarVariant.hrh"
#include "CalenUids.hrh"

// system includes
#include <e32std.h>
#include <ecom/implementationproxy.h>

// debug
#include "calendarui_debug.h"


// -----------------------------------------------------------------------------
// Provides a key value pair table, this is used to identify
// the correct construction function for the requested interface.
// -----------------------------------------------------------------------------
//
const TImplementationProxy ImplementationTable[] =
    {
    // Hide warning about cast implemented by IMPLEMENTATION_PROXY_ENTRY macro
    //lint -e{611}  
    IMPLEMENTATION_PROXY_ENTRY(KCalenEditorsImplementation, CCalenDefaultEditors::NewL)
    };

// -----------------------------------------------------------------------------
// ImplementationGroupProxy()
// Function used to return an instance of the proxy table.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
    {
    TRACE_ENTRY_POINT;

    aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);

    TRACE_EXIT_POINT;
    return ImplementationTable;
    }

// End of file
