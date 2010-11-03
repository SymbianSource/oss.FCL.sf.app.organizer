/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  
 *
*/

 
//debug
#include "calendarui_debug.h"

#include <e32std.h>
#include <ecom/implementationproxy.h>
#include "AIAgendaPluginEngine.h"
#include "aiagendapluginengineuids.hrh"

// Provide a key pair value table for ECOM.
// Used to identify the correct construction function for the requested interface.
const TImplementationProxy ImplementationTable[] = 
    {
    IMPLEMENTATION_PROXY_ENTRY( KAIAgendaPluginEngineImplUid, 
                                CAIAgendaPluginEngine::NewL )
    };


// Return an instance of the proxy table.
EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
    {
    TRACE_ENTRY_POINT;
    
    aTableCount = sizeof( ImplementationTable ) / sizeof( TImplementationProxy );
    
    TRACE_EXIT_POINT;
    return ImplementationTable;
    }


// End of File
