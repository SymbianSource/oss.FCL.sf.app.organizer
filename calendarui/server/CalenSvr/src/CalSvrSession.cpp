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
* Description:   Provides methods for calendar session for a client thread on the server-side.
 *
*/



//debug
#include "calendarui_debug.h"

#include "CalSvrSession.h"

#include "CalenServer.h"
#include "CalSvrDef.h"


/**
 * CCalSvrSession
 **/

// construct/destruct

// -----------------------------------------------------------------------------
// ?classname::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CCalSvrSession::CCalSvrSession()
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    }

// FIXME: this is never called ! Correct would have been createL
// -----------------------------------------------------------------------------
// ?classname::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCalSvrSession::ConstructL(CCalenServer& /*aServer*/)
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// ?classname::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CCalSvrSession::~CCalSvrSession()
    {
    TRACE_ENTRY_POINT;
    
    PIM_TRAPD_HANDLE( DoDestructL() );
    
    TRACE_EXIT_POINT;
    }

void CCalSvrSession::DoDestructL()
    {
    TRACE_ENTRY_POINT;
    
    CancelInitializeL();

    if( iRegistered ) // if still registered
        {
        Server()->UnregisterUserL(*this);        
        iRegistered = EFalse;
        }
    
    TRACE_EXIT_POINT;    
    }
// -----------------------------------------------------------------------------
// ?classname::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CCalenServer* CCalSvrSession::Server() const
    {
    TRACE_ENTRY_POINT;
    /**
    * Return the Calendar server.
    * 
    * This deliberately hides the CSession2 version of this
    * function, to prevent lots of ugly casts around the code.
    **/
    CCalenServer* server = const_cast<CCalenServer*>(static_cast<const CCalenServer*>(CSession2::Server()));

    TRACE_EXIT_POINT;
    return server;    
    }

// service dispatcher - from CSession2

// -----------------------------------------------------------------------------
// ?classname::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCalSvrSession::ServiceL(const RMessage2& aMessage)
    {
    TRACE_ENTRY_POINT;
    
    switch (aMessage.Function())
        {
        case ECalSvrInitialize:
            {
            InitializeL(aMessage);
            // Asynchronous service, message completion not done here
            }
            break;

        case ECalSvrUninitialize:
            {
            UninitializeL();
            aMessage.Complete(KErrNone);
            }
            break;

        case ECalSvrCancelInitialize:
            {
            CancelInitializeL();
            aMessage.Complete(KErrNone);
            }
            break;

        default:
            Server()->PanicClient(aMessage, EBadRequest);

        }
    
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// ?classname::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCalSvrSession::CancelInitializeL()
    {
    TRACE_ENTRY_POINT;
    
    if(!iInitActive)
        {
        TRACE_EXIT_POINT;
        return;
        }

    Server()->UnregisterUserL(*this);
    iRegistered = EFalse;

    iInitMessage.Complete(KErrCancel);
    iInitActive = EFalse;
    
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// ?classname::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCalSvrSession::InitializeL(const RMessage2& aMessage)
    {
    TRACE_ENTRY_POINT;
    
    __ASSERT_DEBUG(!iInitActive, User::Panic(_L("CalSvrSession::InitializeL"), 0));
    __ASSERT_DEBUG(!iRegistered, User::Panic(_L("CalSvrSession::InitializeL"), 1));
    iInitMessage = aMessage;
    iInitActive = ETrue;
    iRegistered = ETrue;
    Server()->RegisterUserL(*this);
    
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// ?classname::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCalSvrSession::UninitializeL()
    {
    TRACE_ENTRY_POINT;
    
    __ASSERT_DEBUG(iRegistered, User::Panic(_L("CalSvrSession::UninitializeL"), 0));
    CancelInitializeL(); // this will unregister if init active
    if (iRegistered) // if still registered 
        {
        Server()->UnregisterUserL(*this);
        iRegistered = EFalse;
        }
    
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// ?classname::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCalSvrSession::DatabaseOpened()
    {
    TRACE_ENTRY_POINT;

    HandleInitReady();
    
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// ?classname::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCalSvrSession::DatabaseTemporarilyClosed()
    {
    TRACE_ENTRY_POINT;

    // Do not do anything here.
    // Client doesn't need to know anything about database being temporarily 
    // closed, because initialize/uninitialize mechanism is meant for
    // optimization only. From client perspective, it is not real connection
    // to agenda database, but "a guarantee" that someone is holding open
    // connection to agenda database, so that subsequent connections from 
    // clients are cheaper.
    // 
    // After restoring is completed, we need to be sure that there are 
    // no unnecessary notifications to clients. 
    // When we get DatabaseOpened after DatabaseTemporarilyClosed, 
    // there are two options:
    //   1) Client has been already notified in HandleInitReady.
    //      In this case iInitActive is EFalse, and we don't inform 
    //      client. Everything is ok.
    //   2) Client has been notified yet, i.e. restore started in middle of 
    //      CalenSvrDBManager opening connection to database. In this case
    //      we want client to be notified. And this happens as iInitActive 
    //      is still ETrue.
    TRACE_EXIT_POINT;
    }

/**
 * Called by server when agenda observer notifies of change
 **/
// -----------------------------------------------------------------------------
// ?classname::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCalSvrSession::HandleInitReady()
    {
    TRACE_ENTRY_POINT;
    
    if(iInitActive)
        {
        iInitMessage.Complete(KErrNone);
        iInitActive = EFalse;
        }
    
    TRACE_EXIT_POINT;
    }


// End of File
