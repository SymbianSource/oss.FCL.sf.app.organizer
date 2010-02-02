/*
* Copyright (c) 2002, 2003 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: Calendar timer class.  
 *
*/



// ============================ MEMBER FUNCTIONS ===============================


// ---------------------------------------------------------
// CCalenTimer::ConstructL
// Symbian 2nd phase constructor can leave.
// ---------------------------------------------------------
//
inline void CCalenTimer::ConstructL(void)
{
    CTimer::ConstructL();
    CActiveScheduler::Add( this );
}

// ---------------------------------------------------------
// CCalenTimer::NewL
// Two-phased constructor.
// ---------------------------------------------------------
//
inline CCalenTimer* CCalenTimer::NewL(TCallBack& aCallBack)
{
    CCalenTimer* self = new( ELeave )CCalenTimer( aCallBack);
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
}

// ---------------------------------------------------------
// CCalenTimer::CCalenTimer
// ?implementation_description
// (other items were commented in a header).
// ---------------------------------------------------------
//
inline CCalenTimer::CCalenTimer(TCallBack& aCallBack)
 : CTimer( CActive::EPriorityStandard )
{
    iCallBack= aCallBack;
}

// ---------------------------------------------------------
// CCalenTimer::~CCalenTimer
// ?implementation_description
// (other items were commented in a header).
// ---------------------------------------------------------
//
inline CCalenTimer::~CCalenTimer(void)
{
    Deque();
}

// ---------------------------------------------------------
// CCalenTimer::RunL
// ?implementation_description
// (other items were commented in a header).
// ---------------------------------------------------------
//
inline void CCalenTimer::RunL(void)
{
    iCallBack.CallBack();
}


// End of File
