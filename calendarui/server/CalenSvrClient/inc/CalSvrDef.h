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
* Description:   Constants used in calendar server part.
*
*/



#ifndef CALSVRDEF_H
#define CALSVRDEF_H

enum TCalSvrRequests
    {
    ECalSvrInitialize = 0,
    ECalSvrCancelInitialize,
    ECalSvrUninitialize
    };

enum TCalSvrPanic // both client-side and server-side - see category to distinguish
    {
    ECannotStartServer,
    EBadRequest,
    EBadDescriptor,
    EBadProtocolSpec,
    ESendFromZeroPort,
    EErrorFromNonClientObject,
    };

void PanicServer(TInt aPanic);

//FIXME cleanup two def filesb
#include "CalenSvrDef.h" 

_LIT(KCalSvrServerExe, "CalenSvr.exe");

#endif // CALSVRDEF_H


// End of File
