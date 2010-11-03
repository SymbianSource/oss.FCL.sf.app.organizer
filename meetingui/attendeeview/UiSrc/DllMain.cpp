/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Dll entry point
*
*/



// INCLUDE FILES
#include "AttendeeViewUID.h"
#include "CAttendeeViewImpl.h"
#include <e32std.h>
#include <ecom/implementationproxy.h>


// Map the interface UIDs to implementation factory functions
const TImplementationProxy ImplementationTable[] =
	{
    IMPLEMENTATION_PROXY_ENTRY( KAttendeeViewImplementationUID,
    							CAttendeeViewImpl::NewL )
	};

// Exported proxy for instantiation method resolution
EXPORT_C const TImplementationProxy* ImplementationGroupProxy( 
                                           TInt& aTableCount )
	{
	aTableCount = sizeof( ImplementationTable ) / 
                    sizeof( TImplementationProxy );

	return ImplementationTable;
	}
// End of File
