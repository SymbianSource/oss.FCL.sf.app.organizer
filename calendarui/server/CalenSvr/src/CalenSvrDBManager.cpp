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
* Description:   Provides methods for handling client interaction with server and 
*								 contains state machine for handling connection with agenda server.  
*
*/



//debug
#include "calendarui_debug.h"

// INCLUDES
#include "CalenSvrDBManager.h"

#include <calenglobaldata.h>

#include <e32base.h>
#include <sbdefs.h>
#include <e32property.h>

// LOCAL CONSTANTS AND MACROS
// FIXME: change timeout to e.g. 1:30 seconds
const TInt KClosingTimeoutMicroSecs(30 /*seconds*/ * 1000000);

enum TCalenSvrDBManagerPanic
    {
    EPanicInvalidState = 1,
    EPanicIllegalBootReady,
    EPanicIllegalOpenCompleted,
    EPanicIllegalUnregister,
    EPanicIllegalClosingTimeOut,
    };

// -----------------------------------------------------------------------------
// ?classname::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
static void Panic(TCalenSvrDBManagerPanic aPanic)
    {
    TRACE_ENTRY_POINT;

    _LIT(KCategory, "CCalenSvrDBManager");
    User::Panic(KCategory, aPanic);

    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// ?classname::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CCalenSvrDBManager* CCalenSvrDBManager::NewL()
    {
    TRACE_ENTRY_POINT;

    CCalenSvrDBManager* self = new (ELeave) CCalenSvrDBManager();
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
CCalenSvrDBManager::CCalenSvrDBManager()
    : iState(EStateBooting)
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
void CCalenSvrDBManager::ConstructL()
    {
    TRACE_ENTRY_POINT;

    iClosingTimer = CPeriodic::NewL(0);

    // The first instance of global data has
    // to be created using NewL() which takes a
    // MCalProgressCallBack callback. We don't need a context change
    // observer so this is NULL.
    iGlobalData = CCalenGlobalData::NewL( *this, NULL );
    iBackupObserver = CPropertyObserver::NewL( *this, KUidSystemCategory, conn::KUidBackupRestoreKey );

    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenSvrDBManager::Completed
// Called when the construction of the instance view
// has completed.
// -----------------------------------------------------------------------------
//
void CCalenSvrDBManager::Completed(TInt /*aError*/)
    {
    TRACE_ENTRY_POINT;
    // The global data is informed that the instance view
    // has been created
    iGlobalData->HandleNotification( ECalenNotifyEntryInstanceViewCreated );

    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenSvrDBManager::NotifyProgress()
// Returns if progress notifications are
// required. As there is no interest in
// the progress returns EFalse.
// -----------------------------------------------------------------------------
//
TBool CCalenSvrDBManager::NotifyProgress()
    {
    TRACE_ENTRY_POINT;

    return EFalse;

    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenSvrDBManager::Progress
// Intentionally empty as there is no interest in the
// progress of the instance view creation
// -----------------------------------------------------------------------------
//
void CCalenSvrDBManager::Progress(TInt /*aPercentageCompleted*/)
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
CCalenSvrDBManager::~CCalenSvrDBManager()
    {
    TRACE_ENTRY_POINT;

    delete iBackupObserver;
    if( iGlobalData )
        {
        iGlobalData->Release();
        iGlobalData = NULL;
        }
    delete iClosingTimer;
    iUsers.Reset();

    TRACE_EXIT_POINT;
    }

// This method is called by server, when booting is ready.
// We need to keep count of registered db users also during booting
void CCalenSvrDBManager::BootReadyL()
    {
    TRACE_ENTRY_POINT;

    switch(iState)
        {
        case EStateBooting:
            if (iUsers.Count() > 0)
                {
                iState = EStateOpeningDB;
                if( !iGlobalData )
                    {
                    iGlobalData = CCalenGlobalData::Instance();
                    if ( !iGlobalData )
                        {
                        // Services and context change notifiers are NULL.
                        iGlobalData = CCalenGlobalData::NewL( *this, NULL );
                        }
                    }
                iGlobalData->InstanceViewL();
                OpenDatabaseCompletedL();
                }
            else
                {
                iState = EStateGoingToCloseDB;
                StartClosingTimer();
                }
            break;
        case EStateRestoring:
            // ignore
            break;
        case EStateOpeningDB:
        case EStateDBOpen:
        case EStateGoingToCloseDB:
        case EStateDBClosed:
            Panic(EPanicIllegalBootReady);
            break;
        default:
            Panic(EPanicInvalidState);
        }

    TRACE_EXIT_POINT;
    }

// RegisterUserL is called either when Calendar Server client session
// registers as db user, or when server itself needs to queue alarms
// -----------------------------------------------------------------------------
// ?classname::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCalenSvrDBManager::RegisterUserL(CCalenSvrDBManager::MCalenDBUser& aUser)
    {
    TRACE_ENTRY_POINT;

    User::LeaveIfError(iUsers.Append(&aUser));

    switch(iState)
        {
        case EStateBooting:
        case EStateOpeningDB:
        case EStateRestoring:
            // do nothing, keep state, users will be notified when db is open
            break;

        case EStateDBOpen:
            // notify user immediately, keep state
            aUser.DatabaseOpened();
            break;

        case EStateGoingToCloseDB:
            iClosingTimer->Cancel();
            iState = EStateDBOpen;
            aUser.DatabaseOpened();
            break;

        case EStateDBClosed:
            iState = EStateOpeningDB;
            if( !iGlobalData )
                {
                iGlobalData = CCalenGlobalData::Instance();
                if ( !iGlobalData )
                    {
                    // Context change notifier is NULL.
                    iGlobalData = CCalenGlobalData::NewL( *this, NULL );
                    }
                }
            iGlobalData->InstanceViewL();
            OpenDatabaseCompletedL();
            break;

        default:
            Panic(EPanicInvalidState);
        }

    TRACE_EXIT_POINT;
    }

// RegisterUserL is called either when Calendar Server client session
// unregisters as db user, or when server has itself has queued alarms
void CCalenSvrDBManager::UnregisterUserL(CCalenSvrDBManager::MCalenDBUser& aUser)
    {
    TRACE_ENTRY_POINT;

    TInt pos = iUsers.Find(&aUser);
    User::LeaveIfError(pos);
    iUsers.Remove(pos);

    switch(iState)
        {
        case EStateBooting:
        case EStateOpeningDB:
        case EStateRestoring:
            // do nothing, keep state;
            break;

        case EStateDBOpen:
            if (iUsers.Count() > 0)
                {
                // do nothing, keep state, there are still users
                }
            else
                {
                iState = EStateGoingToCloseDB;
                StartClosingTimer();
                }
            break;

        case EStateGoingToCloseDB:
        case EStateDBClosed:
            Panic(EPanicIllegalUnregister); // invalid state;
            break;

        default:
            Panic(EPanicInvalidState);
        }

    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// ?classname::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
// From PropertyObserver
void CCalenSvrDBManager::HandlePropertyChange(const TUid& /*aCategory*/, const TUint& /*aKey*/, const TInt& aValue)
    {
    TRACE_ENTRY_POINT;

    const TUint backupMask( conn::KBURPartTypeMask ^ conn::EBURNormal );

    if( aValue & backupMask )
        {
        RestoreStarted();
        }
    else // aValue == conn::TBURPartType::EBURUnset || aValue & conn::TBURPartType::EBURNormal
        {
        // Resume lock
        PIM_TRAPD_HANDLE( RestoreFinishedL() );
        }

    TRACE_EXIT_POINT;
    }


////////////////////
// PRIVATE METHODS
////////////////////

// -----------------------------------------------------------------------------
// CCalenSvrDBManager::OpenDatabaseCompletedL
// OpenDatabaseCompletedL is called back, when engine is ready
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCalenSvrDBManager::OpenDatabaseCompletedL()
    {
    TRACE_ENTRY_POINT;
    switch(iState)
        {
        case EStateOpeningDB:
            {
            if (iUsers.Count() > 0)
                {
                NotifyUsersL();
                iState = EStateDBOpen;
                }
            else
                {
                iState = EStateGoingToCloseDB;
                StartClosingTimer();
                }
            }
            break;

        case EStateRestoring:
        case EStateBooting:
        case EStateDBOpen:
        case EStateGoingToCloseDB:
        case EStateDBClosed:
            Panic(EPanicIllegalOpenCompleted); // invalid state;
            break;

        default:
            Panic(EPanicInvalidState);
        }

    TRACE_EXIT_POINT;
    }

// Closing Timeout is called when closing timer fires
// -----------------------------------------------------------------------------
// ?classname::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCalenSvrDBManager::ClosingTimeOut()
    {
    TRACE_ENTRY_POINT;

    iClosingTimer->Cancel();

    switch(iState)
        {
        case EStateGoingToCloseDB:
            if( iGlobalData )
                {
                iGlobalData->Release();
                iGlobalData = NULL;
                }
            iState = EStateDBClosed;
            break;

        case EStateRestoring:
        case EStateBooting:
        case EStateOpeningDB:
        case EStateDBOpen:
        case EStateDBClosed:
            Panic(EPanicIllegalClosingTimeOut); // invalid state;
            break;

        default:
            Panic(EPanicInvalidState);
        }

    TRACE_EXIT_POINT;
    }

// ClosingTimeOutCallback is just static callback function to wrap actual
// object method
// -----------------------------------------------------------------------------
// ?classname::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CCalenSvrDBManager::ClosingTimeOutCallback(TAny* aManager)
    {
    TRACE_ENTRY_POINT;

    static_cast<CCalenSvrDBManager*>(aManager)->ClosingTimeOut();

    TRACE_EXIT_POINT;
    return 0;
    }

// Helper function
// -----------------------------------------------------------------------------
// ?classname::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCalenSvrDBManager::StartClosingTimer()
    {
    TRACE_ENTRY_POINT;

    TTimeIntervalMicroSeconds32 delay(KClosingTimeoutMicroSecs);
    iClosingTimer->Start(
    delay, delay,
    TCallBack(&CCalenSvrDBManager::ClosingTimeOutCallback, this));

    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// ?classname::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCalenSvrDBManager::RestoreStarted()
    {
    TRACE_ENTRY_POINT;

    switch(iState)
        {
        case EStateDBOpen:
            {
            if( iGlobalData )
                {
                iGlobalData->Release();
                iGlobalData = NULL;
                }
            // Notify users of temporarily closing
            for(TInt i=0; i<iUsers.Count(); ++i)
                {
                iUsers[i]->DatabaseTemporarilyClosed();
                }
            iState = EStateRestoring;
            }
            break;

        case EStateOpeningDB:
            if( iGlobalData )
                {
                iGlobalData->Release();
                iGlobalData = NULL;
                }
            iState = EStateRestoring;
            break;

        case EStateGoingToCloseDB:
            iClosingTimer->Cancel();
            if( iGlobalData )
                {
                iGlobalData->Release();
                iGlobalData = NULL;
                }
            iState = EStateDBClosed;
            break;

        case EStateRestoring:
        case EStateBooting:
        case EStateDBClosed:
            break;

        default:
            Panic(EPanicInvalidState);
        }

    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// ?classname::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCalenSvrDBManager::RestoreFinishedL()
    {
    TRACE_ENTRY_POINT;

    switch(iState)
        {
        case EStateRestoring:
            {
            if(iUsers.Count())
                {
                iState = EStateOpeningDB;
                if( !iGlobalData )
                    {
                    iGlobalData = CCalenGlobalData::Instance();
                    if ( !iGlobalData )
                        {
                        // Context change notifier is NULL.
                        iGlobalData = CCalenGlobalData::NewL( *this, NULL );
                        }
                    }
                iGlobalData->InstanceViewL();
                OpenDatabaseCompletedL();
                }
            else
                {
                iState = EStateDBClosed;
                }
            break;
            }
        case EStateDBOpen:
        case EStateOpeningDB:
        case EStateGoingToCloseDB:
        case EStateBooting:
        case EStateDBClosed:
            break;
        default:
            Panic(EPanicInvalidState);
        }
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenSvrDBManager::NotifyUsersL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCalenSvrDBManager::NotifyUsersL(void)
    {
    TRACE_ENTRY_POINT;

    RPointerArray<MCalenDBUser> users( iUsers.Count() );
    CleanupClosePushL( users ); // content not owned

    for(TInt i(0); i<iUsers.Count(); i++)
        {
        users.Append( iUsers[i] );
        }

    for( TInt i(0); i < users.Count(); ++i )
        {
        users[i]->DatabaseOpened();
        }

    CleanupStack::PopAndDestroy( &users );

    TRACE_EXIT_POINT;
    }


// End of File
