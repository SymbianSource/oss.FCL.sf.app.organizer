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
* Description:   This file is source file for seachserverglobal
*
*/



// INCLUDE FILES
#include <ecom/ecom.h>
#include <searchcscommon.h>
#include "searchserver.h"


LOCAL_C TInt StartServer();
LOCAL_C void StartServerL(CSearchServer**,CActiveScheduler **activeScheduler);
// -------------------------------------------------------------------------------------------------
// Main function called by E32.
// -------------------------------------------------------------------------------------------------
//
GLDEF_C TInt E32Main()
    {
    return StartServer();
    }

// -------------------------------------------------------------------------------------------------
// Starts the server.
// -------------------------------------------------------------------------------------------------
//
LOCAL_C TInt StartServer()
    {
    CTrapCleanup* cleanup = CTrapCleanup::New();	// Create clean-up stack
    TInt error = KErrNoMemory;
   CSearchServer *searchServer = NULL; 
   CActiveScheduler* activeScheduler= NULL;
    if( cleanup )
        {
        TRAP( error, StartServerL(&searchServer,&activeScheduler) );
        delete cleanup;
        }

    // So that no false memory leaks are reported
    delete searchServer;
    searchServer = NULL;
    
    delete activeScheduler;
    activeScheduler = NULL;
    
    REComSession::FinalClose() ;
    if( error != KErrNone )
        {
        // Signal the client that server creation failed
        RProcess::Rendezvous( error );
        }
    return error;
    }

// -------------------------------------------------------------------------------------------------
// Starts the server.
// -------------------------------------------------------------------------------------------------
//
LOCAL_C void StartServerL(CSearchServer** searchServer, CActiveScheduler **activeScheduler)
    {
    // Create and install the active scheduler
    *activeScheduler = new( ELeave )CActiveScheduler;
    //CleanupStack::PushL( activeScheduler );
    CActiveScheduler::Install( *activeScheduler );

    // Create server
     *searchServer = CSearchServer::NewL();

    // Initialisation complete, now signal the client
    User::LeaveIfError( RThread().RenameMe( KSearchServerName ) );
    RProcess::Rendezvous( KErrNone );

    // Ready to run
    CActiveScheduler::Start();
    }

// -------------------------------------------------------------------------------------------------
// Function to panic the server.
// -------------------------------------------------------------------------------------------------
//
GLDEF_C void PanicServer( /*TSearchServerPanics aPanic*/ )
    {
    _LIT(KSearchServerPanic,"Panic From Server");
    User::Panic(KSearchServerPanic , 5 );
    }

// End of File
