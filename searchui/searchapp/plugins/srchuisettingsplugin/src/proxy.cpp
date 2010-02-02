/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:      This class creates the Implementation table
*
*/






#include <e32std.h>
#include <implementationproxy.h>

// User includes
#include "srchuisettingsview.h"

// ---------------------------------------------------------------------------
// KSettingsUIViewImplementationTable
// Provides a key value pair table, this is used to identify
// the correct construction function for the requested interface.
//
// ---------------------------------------------------------------------------
//
const TImplementationProxy KSettingsUIViewImplementationTable[] = 
	{
    IMPLEMENTATION_PROXY_ENTRY( 0x10282E06,	CSrchUiSettingsView::NewSettingsUIViewPluginL ) // used by Clock app
	};
 
// ---------------------------------------------------------------------------
// ImplementationGroupProxy
// Function used to return an instance of the proxy table.
//
// ---------------------------------------------------------------------------
//
EXPORT_C const TImplementationProxy* ImplementationGroupProxy(  TInt& aTableCount )
	{
	aTableCount = sizeof( KSettingsUIViewImplementationTable ) 
                / sizeof( TImplementationProxy );
	return KSettingsUIViewImplementationTable;
	}

// ---------------------------------------------------------------------------
// E32Dll
// EKA1 entry point
//
// ---------------------------------------------------------------------------
//
#ifndef EKA2
GLDEF_C TInt E32Dll( TDllReason /*aReason*/ )
	{
	return( KErrNone );
	}
#endif // EKA2
// End of File
