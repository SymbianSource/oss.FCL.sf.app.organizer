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
* Description:   Callback Timer
*
*/



// INCLUDE FILES
#include <e32base.h>
#include  "searchmanagercallbacktimer.h"

// ================= MEMBER FUNCTIONS =======================

// -----------------------------------------------------------------------------
// CSearchManager::NewL
// Symbian OS 2 phased constructor.
// -----------------------------------------------------------------------------
//
CSearchManagerCallbackTimer* CSearchManagerCallbackTimer::NewL(TCallBack& aCallBack)
    {
    CSearchManagerCallbackTimer* self = new (ELeave) CSearchManagerCallbackTimer(aCallBack);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// -----------------------------------------------------------------------------
// CSearchManager::~CSearchManager
// Destructor.
// -----------------------------------------------------------------------------
//
CSearchManagerCallbackTimer::~CSearchManagerCallbackTimer()
    {
    }

// ---------------------------------------------------------
// CSearchManagerCallbackTimer::StartTimer
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CSearchManagerCallbackTimer::StartTimer( 
                                            const TTimeIntervalMicroSeconds& aTimeOut)
    {    
    Cancel();    

    // MaxInt64 = MaxInt32 * MaxUint32
    // up to MaxUint32 internal rounds by MaxInt32 are SearchManagersible before final 
    // timeout of up to MaxInt32

    iNumInternalRounds = static_cast<TUint32>(aTimeOut.Int64() / KMaxTInt32);
    iTimeOut = static_cast<TInt32>(aTimeOut.Int64() % KMaxTInt32);
    // iTimeOut can be also zero. It is catered for in NextRound

    NextRound();
    }

// ---------------------------------------------------------
// CSearchManagerCallbackTimer::Status
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
TInt CSearchManagerCallbackTimer::Status()
    {
    return iStatus.Int();
    }

// ---------------------------------------------------------
// CSearchManagerCallbackTimer::NextRound
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
TBool CSearchManagerCallbackTimer::NextRound()
    {
    if (iNumInternalRounds > 0)
        {
        After(KMaxTInt32);
        iNumInternalRounds--;
        return ETrue;           // timer not yet completed
        }
    else if (iTimeOut.Int() > 0)
        {
        After(iTimeOut);
        iTimeOut = 0;           // timer will complete on next round
        return ETrue;           // timer not yet completed
        }
    else
        {
        return EFalse;          // timer completed
        }
    }

// -----------------------------------------------------------------------------
// CSearchManagerCallbackTimer::CSearchManagerCallbackTimer
// Performs the first phase of two phase construction.
// -----------------------------------------------------------------------------
CSearchManagerCallbackTimer::CSearchManagerCallbackTimer(TCallBack& aCallBack)
:   CTimer(EPriorityStandard),
    iTimeoutCallBack(aCallBack)    
    {
    CActiveScheduler::Add(this);
    }

// -----------------------------------------------------------------------------
// CSearchManagerCallbackTimer::ConstructL
// ConstructL.
// -----------------------------------------------------------------------------
//	
void CSearchManagerCallbackTimer::ConstructL()
    {
    CTimer::ConstructL();
    }



// ---------------------------------------------------------
// CSearchManagerCallbackTimer::RunL
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CSearchManagerCallbackTimer::RunL()
    {    
    if (!NextRound())           // timer completed?
        {
        if (iStatus.Int() == KErrNone)
            {
            iStatus = KErrTimedOut;
            }
        iTimeoutCallBack.CallBack();
        }    
    }

// ---------------------------------------------------------
// CSearchManagerCallbackTimer::RunError
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
TInt CSearchManagerCallbackTimer::RunError(TInt /*aError*/)
    {
    return KErrNone;
    }
    

//  End of File  
