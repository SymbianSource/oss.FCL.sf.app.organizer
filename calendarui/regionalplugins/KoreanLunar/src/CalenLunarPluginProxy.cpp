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
*  Description : Class looking after alarm fields for forms.
*
*/

//debug
#include "calendarui_debug.h"

//user includes
#include "CalenLunarPluginUids.hrh"
#include "CalenKoreanLunarPlugin.h"

//system includes
#include <e32std.h>
#include <ecom/implementationproxy.h>

// ---------------------------------------------------------------------------
// ImplementationTable()
// ---------------------------------------------------------------------------
//
const TImplementationProxy ImplementationTable[] =
    {
    IMPLEMENTATION_PROXY_ENTRY( CALENKOREANLUNARPLUGIN_IMPLEMENTATION_UID,
                                CCalenKoreanLunarPlugin::CreateKoreanPluginL )
    };

// ---------------------------------------------------------------------------
// ImplementationGroupProxy()
// This should be only exported function of ECom plugin DLL
// ---------------------------------------------------------------------------
//
EXPORT_C const TImplementationProxy* ImplementationGroupProxy( TInt& aTableCount )
    {
    TRACE_ENTRY_POINT;

    aTableCount = sizeof( ImplementationTable) / sizeof(TImplementationProxy);

    TRACE_EXIT_POINT;
    return ImplementationTable;
    }

// End of file
