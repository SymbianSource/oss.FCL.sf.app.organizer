/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Implements server initiallizer class provides asynchronous service handler in client-side.
*
*/



#ifndef CALSVRINITIALIZER_H
#define CALSVRINITIALIZER_H

#include "CalSvrClient.h"
#include <e32base.h>


///////////////////////////////
// This is asynchronous service handler in client-side
// It hides active object from interface user. Interface
// user just provides callback (MCalSvrAgendaUpdateListener)
///////////////////////////////

NONSHARABLE_CLASS( CCalSvrInitializer ) : public CActive
    {
public:

		CCalSvrInitializer(RCalSvrSession& aSession);
		virtual ~CCalSvrInitializer();
		void Initialize(MCalSvrAgendaUpdateListener* aHandler);	

private:
		void RunL();
		void DoCancel();
		
private:
    RCalSvrSession& iSession;
    MCalSvrAgendaUpdateListener* iHandler;

    };

#endif // CALSVRINITIALIZER_H
// End of File
