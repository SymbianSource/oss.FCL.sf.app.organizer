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
* Description:   
 *
*/



//debug
#include "calendarui_debug.h"

#include "CalSvrInitializer.h"


CCalSvrInitializer::CCalSvrInitializer(RCalSvrSession& aSession)
:CActive( 0 ), iSession( aSession )
	{
    CActiveScheduler::Add( this );
    }

CCalSvrInitializer::~CCalSvrInitializer()
	{
    Cancel();
    }

void CCalSvrInitializer::Initialize(MCalSvrAgendaUpdateListener* aHandler)
    {
    __ASSERT_ALWAYS(!IsActive(), User::Panic(_L("CCalSvrInitializer::Initialize"), 1));
    iHandler = aHandler;
    iSession.Initialize(iStatus);
    SetActive();
    }

void CCalSvrInitializer::RunL()
    {
    if(iStatus != KErrCancel)
  	    {
        iHandler->CalendarServerInitialized();
        }
    }

void CCalSvrInitializer::DoCancel()
	{
    iSession.CancelInitialize();
    }

// EOF
