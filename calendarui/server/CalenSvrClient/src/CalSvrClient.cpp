/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: Provides methods for client session implementation of calendar server. 
 *
*/



//debug
#include "calendarui_debug.h"

#include "CalSvrClient.h"
#include "CalSvrInitializer.h"

#include "CalSvrDef.h"
#include "CalServerSignaller.h"

#include <e32base.h>

// -----------------------------------------------------------------------------
// ?classname::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
static TInt StartCalendarServer()
    {
    TRACE_ENTRY_POINT;
    
    TRequestStatus started;
    TCalServerStart start( started );

    const TUidType serverUid( KNullUid, KNullUid, KNullUid );

    //
    // EPOC is easy, we just create a new server process. Simultaneous
    // launching of two such processes should be detected when the
    // second one attempts to create the server object, failing with
    // KErrAlreadyExists.
    //
    RProcess server;
    const TInt ret=server.Create( KCalSvrServerExe, start.AsCommand(), serverUid );

    // Did we manage to create the thread/process?
    if(ret != KErrNone)
        {
        TRACE_EXIT_POINT;
        return ret;
        }

    // Wait to see if the thread/process died during construction
    TRequestStatus serverDiedRequestStatus;
    server.Rendezvous(serverDiedRequestStatus);

    // do we have to abort?
    if(serverDiedRequestStatus != KRequestPending)
        {
        server.Kill(0); // abort the sartup here
        }
    else
        {
        server.Resume(); // start server
        }

    User::WaitForRequest(serverDiedRequestStatus);

    // determine the reason for the server exit
    const TInt exitReason =(server.ExitType()==EExitPanic) ? KErrGeneral : serverDiedRequestStatus.Int();
    server.Close();
    
    TRACE_EXIT_POINT;
    return exitReason;
    }

// -----------------------------------------------------------------------------
// ?classname::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C RCalSvrSession::RCalSvrSession()
:RSessionBase(), iListenerCallback(NULL), iInitializer(NULL)
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    }

// open/close

// -----------------------------------------------------------------------------
// ?classname::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void RCalSvrSession::ConnectL()
    {
    TRACE_ENTRY_POINT;
    
    // connect to server
    TInt err( KErrNone );

    for( TInt tries(0); tries < 2; tries++ )
        {
        err = CreateSession( KCalendarServerName, TVersion(0,0,0) );

        if( !err )
            {
            // connected to existing server - ok
            break;
            }

        if( err != KErrNotFound && err != KErrServerTerminated )
            {
            // problems other than server not here - propagate error
            break;
            }

        err = StartCalendarServer();

        if( !err || (err==KErrAlreadyExists) )
            {
            // if server launched ok, try again to connect 
            // OR
            // if someone else got there first, try again to connect
            continue;
            }

        break; // server not launched: don't cycle round again
        }

    User::LeaveIfError( err );

    CleanupClosePushL( *this ); // Close if next operation leaves

    iInitializer = new( ELeave )CCalSvrInitializer( *this );

    CleanupStack::Pop( this );
    
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// ?classname::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void RCalSvrSession::Close()
    {
    TRACE_ENTRY_POINT;
    
    delete iInitializer;
    iInitializer = NULL;
    RSessionBase::Close();
    
    TRACE_EXIT_POINT;
    }

///////////////////////////////
// These are high-level client interface functions
///////////////////////////////

// -----------------------------------------------------------------------------
// ?classname::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void RCalSvrSession::Initialize(MCalSvrAgendaUpdateListener& aListener)
    {
    TRACE_ENTRY_POINT;
    
    //__ASSERT_ALWAYS(!iInitializer->IsActive(), User::Invariant());
    if(!(iInitializer->IsActive()) && (KRequestPending != iInitializer->iStatus.Int()))
        {
        iInitializer->Initialize(&aListener);
        }
    
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// ?classname::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void RCalSvrSession::StopInitialization()
    {
    TRACE_ENTRY_POINT;
    
    iInitializer->Cancel();
    
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// ?classname::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void RCalSvrSession::Uninitialize()
    {
    TRACE_ENTRY_POINT;

    if(iInitializer->IsActive())
        {
        iInitializer->Cancel();
        }
    SendReceive(ECalSvrUninitialize, TIpcArgs());
    
    TRACE_EXIT_POINT;
    }


///////////////////////////////
// These are low-level functions talking to server
//////////////////////////////

// -----------------------------------------------------------------------------
// ?classname::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void RCalSvrSession::Initialize(TRequestStatus& aStatus)
    {
    TRACE_ENTRY_POINT;
    
    SendReceive(ECalSvrInitialize, TIpcArgs(), aStatus);
    
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// ?classname::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void RCalSvrSession::CancelInitialize()
    {
    TRACE_ENTRY_POINT;

    SendReceive(ECalSvrCancelInitialize, TIpcArgs());
    
    TRACE_EXIT_POINT;
    }


// End of File
