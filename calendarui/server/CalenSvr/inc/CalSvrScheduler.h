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
* Description:   Implements the active Scheduler for calendar server thread.
*                Manages all the active objects used calendar server.
*/



#ifndef CALSVRSCHEDULER_H
#define CALSVRSCHEDULER_H

// INCLUDES
#include <e32base.h>

// FORWARD DECLARATIONS
class CCalenServer;

// CLASS DECLARATION
NONSHARABLE_CLASS( CCalSvrScheduler ) : public CActiveScheduler
    {
public:
    static TInt ThreadStart(void);
    static void CreateAndRunServerL(void);
    ~CCalSvrScheduler();
    void Error(TInt aError) const; // from CActiveScheduler

private:
    CCalenServer* iServer;

    };

#endif // CALSVRSCHEDULER_H


// End of File
