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
* Description:   Create server
*
*/



// INCLUDE FILES

#include <searchplugin.h>
#include <e32def.h>
#include <ecom/ecom.h>
#include "searchserver.h"
#include "searchserversession.h"
#include "searchserverdefines.h"
// ====================================== MEMBER FUNCTIONS =========================================

// -------------------------------------------------------------------------------------------------
// CSearchServer::NewL
// Symbian OS 2 phased constructor.
// -------------------------------------------------------------------------------------------------
//
CSearchServer* CSearchServer::NewL()
    {
    CSearchServer* self = CSearchServer::NewLC();
    CleanupStack::Pop( self );
    return self;
    }

// -------------------------------------------------------------------------------------------------
// CSearchServer::NewLC
// Symbian OS 2 phased constructor.
// -------------------------------------------------------------------------------------------------
//
CSearchServer* CSearchServer::NewLC()
    {
    CSearchServer* self = new( ELeave ) CSearchServer( EPriority );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// -------------------------------------------------------------------------------------------------
// CSearchServer::~CSearchServer
// Destructor.
// -------------------------------------------------------------------------------------------------
//
CSearchServer::~CSearchServer()
    {
    if ( iSearchPluginInterface )
    {
        delete iSearchPluginInterface;
        iSearchPluginInterface = NULL;
    }
    if (iServertimer) 
        {
        delete iServertimer;
        iServertimer = NULL;
        }
    }

// -------------------------------------------------------------------------------------------------
// CSearchServer::SessionCreated
// Informs the server that a session was created
// -------------------------------------------------------------------------------------------------
//
void CSearchServer::SessionCreatedL( )
    {
    iIncrementCount++;
    if (iServertimer)
        {
        delete iServertimer;
        iServertimer = NULL;
        }
    }
    
// -------------------------------------------------------------------------------------------------
// CSearchServer::SessionDestroyed
// Increments the session count
// -------------------------------------------------------------------------------------------------
//
void CSearchServer::SessionDestroyedL( )
    {
    iIncrementCount--;
    //create sever timer
    iServertimer = CSearchseverShutdown::NewL(); 
    //start the timer	
    iServertimer->Start( );
    }

// -------------------------------------------------------------------------------------------------
// CSearchServer::NewSessionL
// Creates a server-side session object.
// -------------------------------------------------------------------------------------------------
//
CSession2* CSearchServer::NewSessionL( 
    const TVersion& aVersion, 
    const RMessage2& ) const
    {
    if ( !User::QueryVersionSupported( Version(), aVersion ) )
        {
        User::Leave( KErrNotSupported );
        }
    return CSearchServerSession::NewL( *CONST_CAST( CSearchServer*, this ), iSearchPluginInterface );
    }

// -------------------------------------------------------------------------------------------------
// CSearchServer::CSearchServer
// Performs the first phase of two phase construction.
// -------------------------------------------------------------------------------------------------
//
CSearchServer::CSearchServer( TInt aPriority )
    : CServer2( aPriority )
    {

    }

// -------------------------------------------------------------------------------------------------
// CSearchServer::ConstructL
// Performs the second phase construction.
// -------------------------------------------------------------------------------------------------
//
void CSearchServer::ConstructL()
    {

    StartL(KServerName);
    iSearchPluginInterface = CSearchPluginInterface::NewL();
    iSearchPluginInterface->InstantiateAllPlugInsL();

    }

// -------------------------------------------------------------------------------------------------
// CSearchServer::RunError
// Handling Error 
// -------------------------------------------------------------------------------------------------
//
TInt CSearchServer::RunError( TInt aError ) 
    {
	if ( aError != KErrNone )
	    {
       	}
    return KErrNone;
    }


// End of File
