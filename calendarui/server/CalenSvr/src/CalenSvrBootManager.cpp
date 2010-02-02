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
* Description:  Provides asynchronus methods for the multistep boot sequence.
 *
*/



//debug
#include "calendarui_debug.h"

#include "CalenSvrBootManager.h"

#include "CalenServer.h"
#include <calenglobaldata.h>

#include <featmgr.h>

// LOCAL CONSTANTS AND MACROS

enum TCalenSvrBootManagerPanic
    {
    EPanicInvalidState,
    EPanicRunError,
    EPanicCorruptedObserverDll,
    EPanicIncorrectObserverUID
    };

static void Panic(TInt aReason)
    {
    TRACE_ENTRY_POINT;
    
    _LIT(KCategory, "CCalenServerBootHandler");
    User::Panic(KCategory, aReason);
    
    TRACE_EXIT_POINT;
    };


// CLASS METHOD DECLARATIONS

// -----------------------------------------------------------------------------
// ?classname::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CCalenSvrBootManager* CCalenSvrBootManager::NewL(CCalenServer& aServer)
    {
    TRACE_ENTRY_POINT;
    
    CCalenSvrBootManager* self = new( ELeave )CCalenSvrBootManager( aServer );
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    
    TRACE_EXIT_POINT;
    return self;
    }

// -----------------------------------------------------------------------------
// ?classname::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CCalenSvrBootManager::CCalenSvrBootManager(CCalenServer& aServer)
    : CActive(0), iState(EStateInit), iServer(aServer)
    {
    TRACE_ENTRY_POINT;
    
    CActiveScheduler::Add(this);
    
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// ?classname::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCalenSvrBootManager::ConstructL(void)
    {
    TRACE_ENTRY_POINT;
    
    Start();
    
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// ?classname::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCalenSvrBootManager::Start()
    {
    TRACE_ENTRY_POINT;
    
    __ASSERT_ALWAYS(!IsActive(), User::Invariant());
    iState = EStateCreateAgendaFile;
    CompleteSelf();
    
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// ?classname::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CCalenSvrBootManager::~CCalenSvrBootManager()
    {
    TRACE_ENTRY_POINT;
    
    Cancel(); // DoCancel destroy everything
    
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// ?classname::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCalenSvrBootManager::DoCancel()
    {
    TRACE_ENTRY_POINT;
    
    iState = EStateInit;
    
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// ?classname::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CCalenSvrBootManager::RunError(TInt /*aError*/)
    {
    TRACE_ENTRY_POINT;

    // if database is corrupted or we run in to other troubles, we ignore them.
    iState = EStateReady;
    CompleteSelf();
    
    TRACE_EXIT_POINT;
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// ?classname::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCalenSvrBootManager::RunL()
    {
    TRACE_ENTRY_POINT;
    
    switch (iState)
        {
        case EStateCreateAgendaFile:
            {
            CCalenGlobalData* gData  = CCalenGlobalData::Instance();
            if ( !gData  )
                {
                // Server doesn't need context change observer.
                gData = CCalenGlobalData::NewL( *this, NULL );
                }
                        
            CleanupReleasePushL( *gData );
            gData->CalSessionL();
            CleanupStack::PopAndDestroy( gData );
            iState = EStateReady;
            CompleteSelf();
            }
            break;

        case EStateReady:
            iServer.BootReadyL();
            break;

        default:
            Panic(EPanicInvalidState);
            break;
        }
    
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// ?classname::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCalenSvrBootManager::CompleteSelf()
    {
    TRACE_ENTRY_POINT;

    TRequestStatus* pStat = &iStatus;
    User::RequestComplete(pStat, KErrNone);
    SetActive();
    
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenSvrBootManager::Completed
// Unused but needed as CCalenSvrBootManager needs
// be derived from MCalProgressCallBack to call
// CCalenGlobalData::NewL()
// -----------------------------------------------------------------------------
//    
void CCalenSvrBootManager::Completed(TInt /*aError*/)
    {
    }

// -----------------------------------------------------------------------------
// CCalenSvrBootManager::NotifyProgress()
// Unused but needed as CCalenSvrBootManager needs
// be derived from MCalProgressCallBack to call
// CCalenGlobalData::NewL()
// -----------------------------------------------------------------------------
//
TBool CCalenSvrBootManager::NotifyProgress()
    {
    TRACE_ENTRY_POINT;
    
    return EFalse;

    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenSvrBootManager::Progress
// Unused but needed as CCalenSvrBootManager needs
// be derived from MCalProgressCallBack to call
// CCalenGlobalData::NewL()
// -----------------------------------------------------------------------------
//
void CCalenSvrBootManager::Progress(TInt /*aPercentageCompleted*/)
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    }

// End of File
