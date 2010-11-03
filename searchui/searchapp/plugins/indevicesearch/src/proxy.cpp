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
* Description:      proxy for indevicesearch
*
*/






// System Includes
#include <e32std.h>
#include <implementationproxy.h>

// User includes
#include "srchuiindeviceserviceplugin.h"

// Constants
const TImplementationProxy KSrchUiInDeviceImplementationTable[] = 
	{
    #ifdef __EABI__ 
        IMPLEMENTATION_PROXY_ENTRY(0x10282409,CSrchUiInDeviceServicePlugin::NewL )  // ---for armv5
    #else
        { { 0x10282409 },  CSrchUiInDeviceServicePlugin::NewL}                      //---for wins
    #endif
	};

// ---------------------------------------------------------------------------
// ImplementationGroupProxy
// Gate/factory function
//
// ---------------------------------------------------------------------------
//
EXPORT_C const TImplementationProxy* ImplementationGroupProxy(  TInt& aTableCount )
	{
	aTableCount = sizeof( KSrchUiInDeviceImplementationTable ) 
                / sizeof( TImplementationProxy );
	return KSrchUiInDeviceImplementationTable;
	}