/*
* Copyright (c) 2006-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Client error values
*
*/


#ifndef __SEARCHRRORS_H__
#define __SEARCHRRORS_H__

#include <e32def.h>

_LIT(KSearchClientFault,"Client Fault");

/**
 * SEARCH Client API panic codes
 * It is used to generate error messages
 * @lin searchclient.lib
 */
enum TSearchClientPanic
    {
    /** An attempt has been made to communicate though a non connected handle. */
    ESearchServerBadHandle					= 0,
    /** A client has tried to use a RSearchIPCSession handle that hasn't
    been properly initialised. */
    ESearchNullPointerHolder					= 1,
    /** A client has called an asynchronous request while it is still outstanding
    on that RSearchServer / RSearchIPCSession. */
    ESearchDuplicateRequest                   = 2,
    /** A session or sub-session was closed with requests still outstanding. */
    ESearchRequestsNotCancelled               = 3,
    /** A unknown IPC request has been made on the server. */
    ESearchBadRequest							= 4,
    /** A bad descriptor has been given to the server by the client. */
    ESearchBadDescriptor						= 5,
    /** An attempt has been made to connect using an already open handle. */
    ESearchServerHandleNotClosed				= 6

    };

/**/

#endif//__SEARCHRRORS_H__
