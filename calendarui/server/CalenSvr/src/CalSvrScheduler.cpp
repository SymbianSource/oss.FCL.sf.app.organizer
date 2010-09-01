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
* Description:   Provides class methods for creating the server and starting up the 
 *
*/



//debug
#include "calendarui_debug.h"

// INCLUDES 

#include "CalSvrScheduler.h"
#include "CalenServer.h"
#include "CalSvrDef.h"
#include "CalServerSignaller.h"

// LOCAL CONSTANTS
_LIT( KNameCalenServerThread, "CalendarServerThread" );
_LIT( KNameCalenServerClient, "CalendarServerClient" );


// -----------------------------------------------------------------------------
// ?classname::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void PanicServer(TInt aPanic)
    {
    TRACE_ENTRY_POINT;
    
    _LIT(KPanicCategory,"Calendar Server");
    User::Panic(KPanicCategory, aPanic);
    
    TRACE_EXIT_POINT;
    }


// Process of creating and starting server is copied from
// Richard Harrison's "Symbian OS C++ for Mobile Phones" book.
// Chapter 19 The GSDP Server, 19.3.6 Startup and Shutdown
// 
// Some differences exists: as Calendar Server is meant to be 
// running all the time, shutdown procedure is not implemented. 
// 
// Calendar server can be started two ways, whichever come firsts: 
// 1) Client connects it through client interface, in which case 
// LaunchFromClient is called.
// 2) Starter starts EXE-file, in which case process starts from E32Main 
// function (or E32Dll function in WINS)


/**
 * class CCalSvrScheduler
 **/
// -----------------------------------------------------------------------------
// ?classname::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CCalSvrScheduler::ThreadStart(void)
    {
    TRACE_ENTRY_POINT;
    
    __UHEAP_MARK;

    TInt err = KErrNoMemory;

    // get cleanup stack
    CTrapCleanup* cleanup = CTrapCleanup::New();

    // initialize all up to and including starting scheduler
    if( cleanup )
        {
        TRAP( err, CreateAndRunServerL() );
        delete cleanup;
        }

    __UHEAP_MARKEND;
    
    TRACE_EXIT_POINT;
    return err;
    }

// -----------------------------------------------------------------------------
// ?classname::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCalSvrScheduler::CreateAndRunServerL(void)
    {
    TRACE_ENTRY_POINT;
    
    // Rename thread
    TInt err = User::RenameThread( KNameCalenServerThread );
    ASSERT( !err );

    // construct active scheduler
    CCalSvrScheduler* self = new( ELeave )CCalSvrScheduler;
    CleanupStack::PushL( self );
    CActiveScheduler::Install( self );

    // construct server
    self->iServer = CCalenServer::NewL();
    RProcess process;
	process.SetPriority( EPriorityBackground );
	process.Close();

    //
    // Initialisation complete, now signal the client
    RProcess::Rendezvous( KErrNone );

    CActiveScheduler::Start();

    // Destroy the scheduler
    CleanupStack::PopAndDestroy( self );
    
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// ?classname::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CCalSvrScheduler::~CCalSvrScheduler()
    {
    TRACE_ENTRY_POINT;
    
    delete iServer;
    
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// ?classname::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCalSvrScheduler::Error(TInt aError) const
    {
    TRACE_ENTRY_POINT;

    if (iServer->IsActive())
        {
        __DEBUGGER()
        }
    else
        {
        if (aError==KErrBadDescriptor)
            {
            iServer->ServerMessage().Panic(KNameCalenServerClient,EBadDescriptor);
            }
        // Otherwise relay the error code to the client and restart the server
        iServer->ServerMessage().Complete( aError );
        iServer->ReStart();
        }
    
    TRACE_EXIT_POINT;
    }

// End of File
